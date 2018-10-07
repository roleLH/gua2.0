#pragma once
// 

#include <stdlib.h>

// 值类型
typedef enum EGValueType
{
    GVALUE_REF,  // 引用类型
    GVALUE_NUM,  // int类型
    GOTHER      // 其他
}EGValueType;

typedef enum EGValueState
{
    GFREE,
    GUSED,
    GGETTING,
    GSETTING
}EGValueState;

typedef struct GValue
{
    EGValueType type;
    EGValueState state;
    union 
    {
        struct 
        {
            ushort nextFree;
            ushort size;
        };
        int value;
    };
}GValue;

typedef struct GValueVector GValueVector;
// 因为实在不想void* void* 的转换，所以就特化一个。。
struct GValueVector
{
    size_t size;
    size_t capacity;
    GValue* array;
};

GValueVector* newgVector(int size)
{
    GValueVector* v = (GValueVector*)malloc(sizeof(GValueVector));
    if(v == NULL)
        return NULL;
    v->size = 0;
    v->capacity = size;
    v->array = (GValue*)malloc(v->capacity * sizeof(GValue));
    if(v->array == NULL)
    {
        free(v);
        return NULL;
    }
    return v;
}

void freegVector(GValueVector* vect)
{
    if(vect)
    {
        free(vect->array);
        free(vect);
    }
}

GValue* gvAt(GValueVector* vect, size_t index)
{
    return &vect->array[index];
}
int gvGrow(GValueVector* vect)
{
    GValue* newArray = (GValue*)malloc((vect->capacity << 1) * sizeof(GValue));
    if(newArray == NULL)
        return 0;
    vect->capacity = vect->capacity << 1;
    size_t i = 0;
    for(; i < vect->size; ++i)
    {
        newArray[i] = vect->array[i];
    }
    free(vect->array);
    vect->array = newArray;
    return 1;
}

int gvPush_back(GValueVector* vect, 
                EGValueType type, 
                EGValueState state, 
                int value)
{
    if(vect->size + 1 > vect->capacity)
    {
        if(gvGrow(vect) == 0)
            return 0;
    }
    vect->array[vect->size].type = type;
    vect->array[vect->size].state = state;
    vect->array[vect->size].value = value;
    return ++ vect->size;
}

int gvPop_back(GValueVector* vect)
{
    if(vect->size <= 0)
        return 0;
    return -- vect->size;
}

GValue* gvTop(GValueVector* vect)
{
    return &vect->array[vect->size -1];
}

GValue* gvPeek(GValueVector* vect, unsigned index)
{
    return &vect->array[vect->size -1 - index];
}