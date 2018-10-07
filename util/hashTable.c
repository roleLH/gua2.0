#include "hashTable.h"
#include <assert.h>

typedef struct HNode
{
    void* key;
    void* value;
    struct HNode* next; // 出现冲突时使用
}HNode;

typedef struct HHead
{
    HNode* first;
    size_t size;        // 该链的大小
}HHead;


#define LINESIZE 5
struct HashTable
{
    size_t size;
    size_t capacity;
    size_t numOfCacheLine;  // 一共多少缓存行
    HNode* freeNode;        // 下一个可以利用的节点
    HNode** nodeCacheLine;  // 每当我们需要节点时，便从这里面获取
    HHead* buckets;           // 散列表
    hashFuncp hash;
    cmpFuncp cmp;
    keyCopyFuncp keyCopy;
    valueCopyFuncp valueCopy;
    keyDtorFuncp keyDtor;
    valueDtorFuncp valueDtor;
};

// 初始化 节点缓存行
int initNodeCacheLine(HashTable* hTable)
{
    assert(hTable);
    hTable->nodeCacheLine = (HNode**)malloc(sizeof(HNode*) * hTable->numOfCacheLine);
    if(hTable->nodeCacheLine == NULL)
        return 0;   // 返回失败
    size_t i = 0;
    for(; i < hTable->numOfCacheLine; ++i)
    {
        hTable->nodeCacheLine[i] = NULL;
    }
    hTable->freeNode = NULL;
    return 1;       // 返回成功
}

int initBuckets(HashTable* hTable)
{
    assert(hTable);
    hTable->buckets = (HHead*)malloc(sizeof(HHead) * hTable->capacity);
    if(hTable->buckets == NULL)
        return 0;
    size_t i = 0;
    for(; i < hTable->capacity; ++i)
    {
        hTable->buckets[i].first = NULL;
        hTable->buckets[i].size = 0;
    }
    return 1;
}

HNode* newNode(HashTable* hTable, void* key, void* value, HNode* next)
{
    if(hTable->freeNode == NULL)
    {   // 如果当前没有可以利用的节点，尝试扩展缓存行
        if(!addCacheLine(hTable))
            return NULL;
    }
    HNode* t = hTable->freeNode;
    t->key = key;
    t->value = value;
    t->next = next;
    // 设置freeNode 为下一个
    int curLineCnt = hTable->size / LINESIZE;
    int index = hTable->size % LINESIZE;
    if(index + 1 >= LINESIZE)
        hTable->freeNode = NULL;
    else
        hTable->freeNode = &hTable->nodeCacheLine[curLineCnt][index + 1];
    return t;
}

// 如果节点数不够，那么就添加一条缓存行
int addCacheLine(HashTable* hTable)
{
    assert(hTable);
    int curLineCnt = hTable->size / LINESIZE;
    if(curLineCnt >= hTable->numOfCacheLine)
        return 0;   // 返回失败
    hTable->nodeCacheLine[curLineCnt] = (HNode*)malloc(sizeof(HNode) * LINESIZE);
    if(hTable->nodeCacheLine[curLineCnt] == NULL)
        return 0;
    hTable->freeNode = &hTable->nodeCacheLine[curLineCnt][0];
    return 1;
}

HashTable* newHashTable(unsigned size,
                        unsigned numOfCacheLine,
                        hashFuncp hash,
                        cmpFuncp cmp,
                        keyCopyFuncp keyCopy,
                        valueCopyFuncp valueCopy,
                        keyDtorFuncp keyDtor,
                        valueDtorFuncp valueDtor)
{
    HashTable* hTable = (HashTable*)malloc(sizeof(HashTable));
    if(hTable == NULL)
        return NULL;
    hTable->size = 0;
    hTable->capacity = size;
    hTable->numOfCacheLine = numOfCacheLine;
    if(!initBuckets(hTable) || !initNodeCacheLine(hTable))
    {
        free(hTable);
        return NULL;
    }
    hTable->hash = hash;
    hTable->cmp = cmp;
    hTable->keyCopy = keyCopy;
    hTable->keyDtor = keyDtor;
    hTable->valueCopy = valueCopy;
    hTable->valueDtor = valueDtor;
    return hTable;
}


void freeHashTable(HashTable* hTable)
{
    if(hTable)
    {
        if(hTable->buckets)
        {
            free(hTable->buckets);
        }
        if(hTable->nodeCacheLine)
        {
            size_t i = 0;
            // 先调用对应的析构函数
            for(i; i < hTable->size; ++i)
            {
                hTable->keyDtor(hTable->nodeCacheLine[i / LINESIZE][i % LINESIZE].key);
                hTable->valueDtor(hTable->nodeCacheLine[i / LINESIZE][i % LINESIZE].value);
            }
            i = 0;
            // 然后释放缓存行
            for(; i < hTable->numOfCacheLine; ++i)
            {
                if(hTable->nodeCacheLine[i])
                {
                    free(hTable->nodeCacheLine[i]);
                }
            }
            free(hTable->nodeCacheLine);
        }
    }
}

// 在散列表中查找，没有找到返回NULL
void* hFind(HashTable* hTable, void* key)
{
    unsigned index = hTable->hash(key) % hTable->capacity;
    if(hTable->buckets[index].first)
    {
        HNode* iter = hTable->buckets[index].first;
        for(; iter != NULL; iter = iter->next)
            if(hTable->cmp(key, iter->key) == 0)
                return iter->value;
    }
    return NULL;
}

void* hFindById(HashTable* hTable, size_t id)
{
    return hTable->nodeCacheLine[id / LINESIZE][id % LINESIZE].value;
}

size_t hSize(HashTable* hTable)
{
    return hTable->size;
}


// 插入
int hInsert(HashTable* hTable, void* key, void* value)
{

    if(hTable->size +1 >= hTable->capacity)
        grow(hTable);

    unsigned index = hTable->hash(key) % hTable->capacity;
    if(hFind(hTable, key))  // 不允许插入重复键
        return 0;


    void* keyCopy = hTable->keyCopy(key);
    void* valueCopy = hTable->valueCopy(value);
    HNode* node = newNode(hTable, keyCopy, valueCopy, NULL);
    if(node == NULL)
        return 0;
    node->next = hTable->buckets[index].first;
    hTable->buckets[index].first = node;
    ++ hTable->buckets[index].size;
    ++ hTable->size;
    return hTable->size;
}

// 当 size >= capacity 时尝试增大buckets，注意，可能会有性能问题
int grow(HashTable* hTable)
{
    HHead* oldBuckets = hTable->buckets;
    size_t oldCapacity = hTable->capacity;
    hTable->capacity = (oldCapacity << 1) + 1;
    if(!initBuckets(hTable))
    {
        hTable->buckets = oldBuckets;
        hTable->capacity = oldCapacity;
        return 0;   // 扩容失败
    }

    // 重散列
    size_t i = 0;
    for(i; i < hTable->size; ++i)
    {
        size_t curLine = i / LINESIZE;
        size_t index = i % LINESIZE;
        unsigned h = hTable->hash(hTable->nodeCacheLine[curLine][index].key) % hTable->capacity;
        hTable->nodeCacheLine[curLine][index].next = hTable->buckets[h].first;
        hTable->buckets[h].first = &hTable->nodeCacheLine[curLine][index];
        ++ hTable->buckets[h].size;
    }
    return 1;
}
