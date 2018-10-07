#pragma once
#include "../util/hashTable.h"
#include "../util/ItemPool.h"
#include "../include/gValue.h"
#include "../util/vector.h"

typedef unsigned short ushort;

typedef struct 
{
    ushort instruction;
    ushort operand;
}GByte;



typedef struct 
{
    unsigned startPc;
    unsigned pcCnt;
    unsigned lineNumber;
} LineInfo;

typedef struct 
{
    short baseAddr;
    short size;
} AllocInfo;

ItemPool(LineInfo, LineInfoPool);
ItemPool(AllocInfo, AllocInfoPool);
ItemPool(GByte, GBytePool);

typedef struct 
{
    char* name;
    int id;
    int paramCnt;
    HashTable* paramMap;
    HashTable* lineMap;
    TinyVector* code;
    LineInfoPool lineInfos;
    size_t stkLength;
}GFunction;

/*
typedef struct RunTimeFunction
{

} RunTimeFunction;
*/

typedef struct
{
    unsigned bp;
//    unsigned sp;
    GValueVector* stk;
} Stack;

typedef ushort (*allocFuncp)(Heap* heap, ushort size);
typedef void (*freeFuncp)(Heap* heap, ushort baseAddr);

typedef struct 
{
    HashTable* allocLog;    // <base_addr, info*>
    AllocInfoPool infos;
    GValue freeHead;
    allocFuncp alloc;
    freeFuncp free;
    GValueVector* heap;
} Heap;


ushort alloc(Heap* heap, ushort size)
{
    
}


ItemPool(GFunction, FuncPool);

typedef struct
{
    Stack stack;
    Heap heap;
    int pc;
    GFunction* curFunction;
    int functionId;
    HashTable* funcMap;     // <funcName, func*>
    FuncPool functoins;
}GVirtualMachine;



static int check(Heap* h, ushort baseAddr, ushort index)
{
    AllocInfo* info = (AllocInfo*)hFind(h->allocLog, &baseAddr);
    if(info == NULL)
    {
        // no object
    }
    if(index > info->size -1)
    {
        // index too long for this object
    }
    return ;
}


typedef enum 
{
    GPUSH = 1,
    GPUSH_STACK,
    GPOP_STACK,
    GPUSH_CHUNK_INDEX,
    GPOP_CHUNK_INDEX,
    GADD,
    GSUB,
    GMUL,
    GDIV,
    GMOD,
    GMINUS,
    GEQ,
    GNE,
    GGT,
    GGE,
    GLT,
    GLE,
    GAND,
    GOR,
    GNOT,
    GPOP,
    GDUP,
    GJMP,
    GJNZ,
    GJZ,
    GCALL,
    GRET,
    GNEW,
    GDELETE
} OpCode;


int GVMPush(GVirtualMachine* gvm, ushort operand)
{
    return gvPush_back(gvm->stack.stk, GVALUE_NUM, GFREE, operand);
}
int GVMPushStack(GVirtualMachine* gvm, ushort operand)
{
    GValue* v = gvAt(gvm->stack.stk, gvm->stack.bp + operand);
    return gvPush_back(gvm->stack.stk, v->type, v->state, v->value);
}
int GVMPopStack(GVirtualMachine* gvm, ushort operand)
{
    GValue* v = gvAt(gvm->stack.stk, gvm->stack.bp + operand);
    *v = *gvTop(gvm->stack.stk);
    return gvPop_back(gvm->stack.stk);
}
int GVMPushChunkIndex(GVirtualMachine* gvm, ushort operand)
{
    GValue* vo = gvPeek(gvm->stack.stk, 1);
    if(vo->type != GVALUE_REF)
    {
        // runtime error  not a object
    }
    GValue* vi = gvPeek(gvm->stack.stk, 0);
    if(vi->type != GVALUE_NUM)
    {
        // runtime error not a number
    }
    int num = check(&gvm->heap, vo->value, vi->value);
    if(num) // if error 
    {
        // some error message
    }
    GValue* v = gvAt(gvm->heap.heap, vi->value + vo->value);
    *vo = *v;
    gvPop_back(gvm->stack.stk);
    return some;
}
int GVMPopChunkIndex(GVirtualMachine* gvm, ushort operand)
{
    GValue* vo = gvPeek(gvm->stack.stk, 1);
    if(vo->type != GVALUE_REF)
    {
        // runtime error  not a object
    }
    GValue* vi = gvPeek(gvm->stack.stk, 0);
    if(vi->type != GVALUE_NUM)
    {
        // runtime error not a number
    }
    int num = check(&gvm->heap, vo->value, vi->value);
    if(num) // if error 
    {
        // some error message
    }
    GValue* v = gvAt(gvm->heap.heap, vi->value + vo->value);
    GValue* vg = gvPeek(gvm->stack.stk, 2);
    *v = *vg;
    gvPop_back(gvm->stack.stk);
    gvPop_back(gvm->stack.stk);
    gvPop_back(gvm->stack.stk);
    return some;
}

#define BINOP(op) do { \
    GValue* v1 = gvPeek(gvm->stack.stk, 1); \
    GValue* v2 = gvPeek(gvm->stack.stk, 0); \
    if(v1->type != GVALUE_NUM || v2->type != GVALUE_NUM)    \
    {   \
        /* runtime error not a number */ \
    }   \
    v1->value = v1->value op v2->value;  \
    gvPop_back(gvm->stack.stk); \
} while(0)

int GVMADD(GVirtualMachine* gvm, ushort operand)
{
    BINOP(+);
    return some;
}
int GVMSUB(GVirtualMachine* gvm, ushort operand)
{
    BINOP(-);
    return some;
}
int GVMMUL(GVirtualMachine* gvm, ushort operand)
{
    BINOP(*);
    return some;
}
int GVMDIV(GVirtualMachine* gvm, ushort operand)
{
    BINOP(/);
    return some;
}
int GVMMOD(GVirtualMachine* gvm, ushort operand)
{
    BINOP(%);
    return some;
}
int GVMMINUS(GVirtualMachine* gvm, ushort operand)
{
    GValue* v = gvTop(gvm->stack.stk);
    if(v->type != GVALUE_NUM)
    {
        // runtime error not a num
    }
    v->value = -v->value;
    return some;
}
int GVMEQ(GVirtualMachine* gvm, ushort operand)
{
    BINOP(==);
    return some;
}
int GVMNE(GVirtualMachine* gvm, ushort operand)
{
    BINOP(!=);
    return some;
}
int GVMGT(GVirtualMachine* gvm, ushort operand)
{
    BINOP(>);
    return some;
}
int GVMGE(GVirtualMachine* gvm, ushort operand)
{
    BINOP(>=);
    return some;
}
int GVMLT(GVirtualMachine* gvm, ushort operand)
{
    BINOP(<);
    return some;
}
int GVMLE(GVirtualMachine* gvm, ushort operand)
{
    BINOP(<=);
    return some;
}
int GVMAND(GVirtualMachine* gvm, ushort operand)
{
    BINOP(&&);
    return some;
}
int GVMOR(GVirtualMachine* gvm, ushort operand)
{
    BINOP(||);
    return some;
}
int GVMNOT(GVirtualMachine* gvm, ushort operand);
int GVMPop(GVirtualMachine* gvm, ushort operand)
{
    return gvPop_back(gvm->stack.stk);
}
int GVMDup(GVirtualMachine* gvm, ushort operand)
{
    GValue* v = gvTop(gvm->stack.stk);
    return gvPush_back(gvm->stack.stk, v->type, v->state, v->value);
}
int GVMJmp(GVirtualMachine* gvm, ushort operand)
{
    return gvm->pc = operand -1;
}
int GVMJnz(GVirtualMachine* gvm, ushort operand)
{
    GValue* v = gvTop(gvm->stack.stk);
    if(v->value != 0)
    {
        gvm->pc = operand -1;
    }
    return gvPop_back(gvm->stack.stk);
}
int GVMjz(GVirtualMachine* gvm, ushort operand)
{
    GValue* v = gvTop(gvm->stack.stk);
    if(v->value == 0)
    {
        gvm->pc = operand -1;
    }
    return gvPop_back(gvm->stack.stk);
}
// 注意。 我们需要把当前pc functionId bp sp 入栈
// 然后设置当前的pc functionId bp sp
int GVMCall(GVirtualMachine* gvm, ushort operand)
{
    gvPush_back(gvm->stack.stk, GVALUE_NUM, GFREE, gvm->pc);
    gvPush_back(gvm->stack.stk, GVALUE_NUM, GFREE, gvm->functionId);
    gvPush_back(gvm->stack.stk, GVALUE_NUM, GFREE, gvm->stack.bp);
    gvPush_back(gvm->stack.stk, GVALUE_NUM, GFREE, gvm->stack.stk->size);

    gvm->pc = -1;
    gvm->functionId = operand;
    gvm->curFunction = (GFunction*)hFindById(gvm->funcMap, operand);
    gvm->stack.bp = gvm->stack.stk->size;
    gvm->stack.stk->size += gvm->curFunction->stkLength;
    return some;
}
int GVMRet(GVirtualMachine* gvm, ushort operand)
{
   
    GValue* v = gvTop(gvm->stack.stk);
    *gvAt(gvm->stack.stk, gvm->stack.bp) = *v;

    gvm->pc = gvAt(gvm->stack.stk, gvm->stack.bp -4)->value;
    gvm->functionId = gvAt(gvm->stack.stk, gvm->stack.bp -3)->value;
    gvm->stack.bp = gvAt(gvm->stack.stk, gvm->stack.bp -2)->value;
    gvm->stack.stk->size = gvAt(gvm->stack.stk, gvm->stack.bp -1)->value +1;
    gvm->curFunction = (GFunction*)hFindById(gvm->funcMap, gvm->functionId);
    return some;
}
int GVMNew(GVirtualMachine* gvm, ushort operand)
{

}
int GVMDelete(GVirtualMachine* gvm, ushort operand)
{

}



