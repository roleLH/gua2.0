#pragma once
#include <stdlib.h>
#include <assert.h>
// 一个哈希表 19-9-29
// 参考 jsw_hlib

typedef struct HashTable HashTable;
// 用户自定义 散列函数
typedef unsigned (*hashFuncp) (const void* key);
// 用户自定义 比较函数
typedef int (*cmpFuncp) (const void* keyA, const void* keyB);
// 用户自定义 键拷贝函数
typedef void* (*keyCopyFuncp) (const void* key);
// 用户自定义值 值拷贝函数
typedef void* (*valueCopyFuncp) (const void* value);
// 用户自定义 键删除函数
typedef void (*keyDtorFuncp) (void* key);
// 用户自定义 值删除函数
typedef void (*valueDtorFuncp) (void* value);

HashTable* newHashTable();


typedef struct HNode
{
    void* key;          
    void* value;
    struct HNode* next; // 出现冲突时使用
}HNode;

typedef struct HHead
{
    HNode* first;       
    size_t size;        // 出现冲突时，该链的大小
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
                keyDtorFuncp(hTable->nodeCacheLine[i / LINESIZE][i % LINESIZE].key);
                valueDtorFuncp(hTable->nodeCacheLine[i / LINESIZE][i % LINESIZE].value);
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
        HNode* iter = hTable->buckets[index].first->next;
        for(; iter != NULL; iter = iter->next)
            if(hTable->cmp(key, iter->key) == 0)
                return iter->value;
    }
    return NULL;
}

// 插入
int hInsert(HashTable* hTable, void* key, void* value)
{
    unsigned index = hTable->hash(key) % hTable->capacity;
    if(hFind(hTable, key))  // 不允许插入重复键
        return 0;
    void* keyCopy = hTable->keyCopy(key);
    void* valueCopy = hTable->valueCopy(value);
    HNode* node = newNode(hTable, keyCopy, valueCopy, NULL);
    if(node == NULL)
        return 0;
    node->next = hTable->buckets[index].first->next;
    hTable->buckets[index].first->next = node;
    ++ hTable->buckets[index].size;
    ++ hTable->size;
    return hTable->size;
}

// 当 size >= capacity 时尝试增大buckets，注意，可能会有性能问题
int resize(HashTable* hTable)
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
        hTable->nodeCacheLine[curLine][index].next = hTable->buckets[h].first->next;
        hTable->buckets[h].first->next = &hTable->nodeCacheLine[curLine][index];
        ++ hTable->buckets[h].size;
    }
    return 1;
}





static unsigned strHash(const void* str)
{
    unsigned char* p = (unsigned char*)str;
    unsigned h = 0;
    for(p; *p != '\0'; p++)
    {
        h = 33 * h ^ (*p);
    }
    return h;
}

static unsigned intHash(const void* num)
{
    return *(int*)num;
}

static int strCmp(const void* ap, const void* bp)
{
    char* a = (char*)ap;
    char* b = (char*)bp;
    while(*a != '\0' && *b != '\0')
    {
        if(*a != *b)
            return *a - *b;
        a++;
        b++;
    }
    return *a - *b;
}

static int intCmp(const void* a, const void* b)
{
    return *(int*)a - *(int*)b;
}

static void* intCpy(const void* num)
{
    int* n = (int*)malloc(sizeof(int));
    *n = *(int*)num;
    return (void*)n;
}

static void intDtor(void* num)
{
    if(num)
        free(num);
}