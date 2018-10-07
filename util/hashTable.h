#pragma once
#include <stdlib.h>
#include <string.h>
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

HashTable* newHashTable(unsigned size,              // 散列表的初始容量
                        unsigned numOfCacheLine,    // 节点缓存行的数量
                        hashFuncp hash,
                        cmpFuncp cmp,
                        keyCopyFuncp keyCopy,
                        valueCopyFuncp valueCopy,
                        keyDtorFuncp keyDtor,
                        valueDtorFuncp valueDtor);

void freeHashTable(HashTable* hTable);

// 根据键值在散列表中查找，没有找到返回NULL
void* hFind(HashTable* hTable, void* key);

// 根据id在nodeCache中查找，
void* hFindById(HashTable* hTable, size_t id);

// 插入, 0插入失败，非0成功
int hInsert(HashTable* hTable, void* key, void* value);

size_t hSize(HashTable* hTable);



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

static void* strCpy(const void* str)
{
    size_t len = strlen((char*)str);
    char* s = (char*)malloc(len + 1);
    if(s == NULL)
        return NULL;
    strcpy(s, (char*)str);
    s[len] = '\0';
    return s;
}

static void strDtor(void* str)
{
    if(str)
        free(str);
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

static void* pointerCpy(const void* point)
{
    return (void*)point;
}

// 由于可能是复杂类型的指针，所以不能简单的free(point)。
// 我们在这里什么都不做，由用户自己删除相应区域
static void pointerDtor(void* point)
{
    return;
}