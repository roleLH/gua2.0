#pragma once
#include "gValue.h"
#include "util/hashTable.h"

typedef struct FuncDef
{
    char* name;
    HashTable* map;
    GValueVector* stk;
    GValue returnVal;

    size_t curPc;

    struct FuncDef* nextFunc;
    struct FuncDef* prevFunc;

}FuncDef;

///////// 其他想法

