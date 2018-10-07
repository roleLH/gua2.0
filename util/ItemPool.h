#pragma once
#include <stdlib.h>
#include <stdio.h>

#define VECTOR_SIZE 5
#define ARRAY_LENGTH 8
#define THRESHOLD 32

#define oom() exit(-1)

#define NODE(type)       \
union Node           \
{                        \
    type item;         \
    union Node* nextFree;\
}                   


#define ItemPool(type, name)    \
typedef struct          \
{                       \
    size_t size;        \
    size_t numOfArray;    \
    union   \
    {   \
        type item;  \
        type* nextFree;   \
    }* free, **arrays;  \
} name                


#define insertHead(head, freeNode) do {   \
    ((typeof(head))(freeNode))->nextFree = (head);    \
    (head) = (freeNode);    \
} while(0)                  


#define dumpPool(pool, cb) do {    \
    printf("-- size of Node : %d --\n", sizeof(*(pool)->free));   \
    printf("-- cur size : %d --\n", (pool)->size); \
    printf("-- cur capacity : %d -- \n", (pool)->numOfArray * ARRAY_LENGTH);    \
    int i = 0;  \
    printf("-- allocated items --\n");   \
    for(i; i < (pool)->numOfArray; ++i)    \
    {   \
        if((pool)->arrays[i])  \
        {   \
            int j = 0;  \
            for(j; j < ARRAY_LENGTH; ++j)   \
                cb(&(pool)->arrays[i][j].item);  \
        }   \
    }       \
    printf("\n -- unused items -- \n");  \
    typeof((pool)->free) tp = (pool)->free;    \
    while(tp)   \
    {   \
        cb(&(tp->item));    \
        tp = tp->nextFree;  \
    }   \
} while(0) 

#define grow(pool) do {                                                \
    int curArray = (pool)->size / ARRAY_LENGTH;    \
    if(curArray >= (pool)->numOfArray)     \
    {                                       \
        int newSize = (pool)->numOfArray > THRESHOLD   \
            ? (pool)->numOfArray + VECTOR_SIZE         \
            : (pool)->numOfArray + ((pool)->numOfArray >> 1); \
        typeof((pool)->arrays) newArrays =         \
            (typeof((pool)->arrays))malloc(sizeof(*(pool)->arrays) * newSize);    \
        if(newArrays == NULL)   oom();  \
        int i = 0;  \
        for(i; i < (pool)->numOfArray; ++i) \
            newArrays[i] = (pool)->arrays[i];  \
        for(i; i < newSize; ++i)    \
            newArrays[i] = NULL;    \
        (pool)->numOfArray = newSize;  \
        free((pool)->arrays);  \
        (pool)->arrays = newArrays;    \
    }   \
     (pool)->arrays[curArray] = (typeof(*(pool)->arrays))malloc(sizeof(**(pool)->arrays) * ARRAY_LENGTH); \
    if((pool)->arrays[curArray] == NULL)   oom();  \
    int i = ARRAY_LENGTH - 1;   \
    for(i; i >= 0; --i) \
    {   \
        insertHead((pool)->free, &(pool)->arrays[curArray][i]);   \
    }   \
} while(0) 

#define initItemPool(pool) do{                                           \
    (pool)->size = 0;                                                  \
    (pool)->numOfArray = VECTOR_SIZE;                                    \
    (pool)->free = NULL;                                               \
    (pool)->arrays = (typeof((pool)->arrays))malloc(sizeof(*(pool)->arrays) * (pool)->numOfArray);  \
    if((pool)->arrays == NULL)   oom();                                  \
    int i = 0;                                                          \
    for(i; i < (pool)->numOfArray; ++i)                                  \
        (pool)->arrays[i] = NULL;                                       \
}   while(0)                                                            

#define freeItemPool(pool) do {  \
    int i = 0;      \
    for(i; i < (pool)->numOfArray; ++i)    \
    {       \
        if((pool)->arrays[i])  \
            free((pool)->arrays[i]);   \
    }   \
    free((pool)->arrays);  \
    (pool)->arrays = NULL; \
} while(0)  

#define newItem(pool, item)  do { \
    if((pool)->free == NULL)       \
    {   \
        grow(pool);    \
        if((pool)->free == NULL)   oom();  \
    }   \
    item = (pool)->free;   \
    (pool)->free = (pool)->free->nextFree;    \
    (pool)->size++;        \
} while(0)

#define freeItem(pool, item) do {  \
    insertHead((pool)->free, item);    \
    (pool)->size--;    \
} while(0)  

