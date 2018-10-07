#pragma once
// 一个简单的数组 19-09-30
#include <stdlib.h>

typedef struct TinyVector TinyVector;

typedef void* (*itemCpyFuncp)(const void* item);
typedef void (*itemDtorFuncp)(void* item);

typedef struct Item
{
    void* item;
} Item;

struct TinyVector
{
    size_t size;
    size_t capacity;
    Item* items;

    itemCpyFuncp itemCpy;
    itemDtorFuncp itemDtor;
};

TinyVector* newVector(itemCpyFuncp itemCpy,
                    itemDtorFuncp itemDtor)
{
    TinyVector* tv = (TinyVector*)malloc(sizeof(TinyVector));
    if(tv == NULL)
        return NULL;
    tv->size = 0;
    tv->capacity = 5;
    tv->items = (Item*)malloc(sizeof(Item)* tv->capacity);
    if(tv->items == NULL)
    {
        free(tv);
        return NULL;
    }
    tv->itemCpy = itemCpy;
    tv->itemDtor = itemDtor;
    return tv;
}

void freeVector(TinyVector* tv)
{
    if(tv)
    {
        if(tv->items)
        {
            size_t i = 0;
            for(; i < tv->size; ++i)
            {
                tv->itemDtor(tv->items[i].item);
            }
            free(tv->items);
        }
        free(tv);
    }
}

int tvGrow(TinyVector* tv)
{
    size_t ns = tv->capacity  + (tv->capacity >> 1);
    Item* newItems = (Item*)malloc(sizeof(Item) * ns);
    if(newItems == NULL)
        return 0;   // 增长失败
    size_t i = 0;
    for(; i < tv->size; ++i)
    {   // 注意，我们在这 只是拷贝了指针的值
        newItems[i].item = tv->items[i].item;
    }
    free(tv->items);
    tv->items = newItems;
    return 1;
}

int tvPush_back(TinyVector* tv, void* item)
{
    if(tv->size + 1 > tv->capacity)
    {
        if(tvGrow(tv) == 0)
            return 0;       // 插入失败
    }
    tv->items[tv->size].item = tv->itemCpy(item);
    return ++ tv->size;
}

int tvPop_back(TinyVector* tv)
{
    if(tv->size <= 0)
        return 0;
    tv->itemDtor(tv->items[tv->size -1].item);
    return --tv->size;
}

void* tvAt(TinyVector* tv, size_t index)
{
    return tv->items[index].item;
}

size_t tvSize(TinyVector* tv)
{
    return tv->size;
}

/* 算了，以后再说

// 我需要一个真正的vector

*/

