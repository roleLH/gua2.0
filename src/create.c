#include "../include/gua.h"
#include "../util/ItemPool.h"
#include <string.h>

ItemPool(Expression, ExpPool);
ItemPool(Statement, StmtPool);
ItemPool(FunctionDefinition, FuncDefPool);

ItemPool(FieldInfo, FieldInfoPool);
ItemPool(ChunkDefinition, ChunkDefPool);
ItemPool(Elsif, ElsifPool);

/*
ItemPool(ElsifList, ElsifListPool);
ItemPool(ArgumentList, ArgListPool);
ItemPool(ParameterList, paramListPool);
ItemPool(StatementList, StmtListPool);
*/

typedef Expression* (*expFuncp)(ExpressionType type);
typedef Statement* (*stmtFuncp)(StatementType type);
typedef FunctionDefinition* (*funcDefFuncp)(char* id, ParameterList* params, Block* block);
typedef FieldInfo* (*fieldFuncp)(char* id, unsigned size);
typedef ChunkDefinition* (*chunkDefFuncp)(FieldInfo* info);
typedef Elsif* (*elsifFuncp)(Expression* exp, Block* block);


typedef 
struct StructPool
{
    ExpPool exps;
    StmtPool stmts;
    FuncDefPool funcs;
    FieldInfo fields;
    ChunkDefPool chunks;
    Elsif elsifs;
    struct
    {
        expFuncp expAlloc;
        stmtFuncp stmtAlloc;
        funcDefFuncp funcDefAlloc;
        fieldFuncp fieldInfoAlloc;
        chunkDefFuncp chunkDefAlloc;
        elsifFuncp elsifAlloc;
    } ops; 
} StructPool;

static StructPool structPool;


FunctionDefinition* gFunctionDefine(char* id, ParameterList* params, Block* block)
{
    if(searchFunc(id))
    {
        ///compile error
        return;
    }
    FunctionDefinition* funcDef = NULL;
    FuncDefPool* funcs = &structPool.funcs;
    newItem(funcs, funcDef);
    if(funcDef == NULL) return NULL;
    funcDef->name = strdup(id);
    funcDef->paramList = params;
    funcDef->block = block;
//    addFuncDef(funcDef);
    return funcDef;
}

ParameterList* gNewParamList(char* id)
{
    ParameterList* l = (ParameterList*)malloc(sizeof(*l));
    if(l == NULL)   return NULL;
    hInsert(l->params, id, NULL);
    return l;
}
//TODO: 没有进行重复参数的检查
ParameterList* gAddParam(ParameterList* list, char* id)
{
    hInsert(list->params, id, NULL);
    return list;
}

ArgumentList* gNewArgList(Expression* exp)
{
    ArgumentList* l = (ArgumentList*)malloc(sizeof(*l));
    if(l == NULL)   return NULL;
    hInsert(l->argList, exp, NULL);
    return l;
}
//TODO: 没有进行重复参数的检查
ArgumentList* gAddArg(ArgumentList* list, Expression* exp)
{
    hInsert(list->argList, exp, NULL);
    return list;
}

StatementList* gNewStmtList(Statement* stmt)
{
    StatementList* l = (StatementList*)malloc(sizeof(*l));
    if(l == NULL)   return NULL;
    tvPush_back(l->statements, stmt);
    return l;
}
StatementList* gAddStmt(StatementList* list, Statement* stmt)
{
    tvPush_back(list->statements, stmt);
    return list;
}


FieldInfo* gNewFieldInfo(char* id, unsigned size)
{
    FieldInfo* info = NULL;
    FieldInfoPool* fields = &structPool.fields;

    newItem(fields, info);
    if(info == NULL)    return NULL;
    info->name = strdup(id);
    info->size = size;
    return info;
}
ChunkDefinition* gNewChunkDefine(FieldInfo* info)
{
    ChunkDefinition* cf = NULL;
    ChunkDefPool* cps = &structPool.chunks;

    newItem(cps, cf);
    if(NULL == cf)  return NULL;
    hInsert(cf->fields, info->name, info);
    return cf;
}
ChunkDefinition* gAddFieldInfo(ChunkDefinition* chunkDef, FieldInfo* info);

Expression* gNewExp(ExpressionType type)
{
    Expression* exp = NULL;
    ExpPool* pool = &structPool.exps;

    newItem(pool, exp);
    if(exp == NULL) return NULL;
    exp->type = type;
    exp->lineNumber = getCurCompiler()->lineNum;
    return exp;
}
Expression* gNewAsgExp(Expression* left, Expression* operand)
{
    Expression* exp = structPool.ops.expAlloc();
    exp->exp.asgExp.left = left;
    exp->exp.asgExp.operand = operand;

    return exp;
}

// TODO:
static Expression* value2Exp(GValue* v)
{
    Expression* exp;
    return exp;
}

Expression* gNewBinExp(ExpressionType op, Expression* left, Expression* right)
{
    if(left->type == NUMBER_EXPRESSION)
    {

    }
    else
    {
        Expression* exp = structPool.ops.expAlloc(op);
        exp->exp.binExp.left = left;
        exp->exp.binExp.right = right;
        return exp;
    }
}
Expression* gNewMinusExp(Expression* operand);
Expression* gNewDeleteExp(Expression* exp);
Expression* gNewIndexExp(Expression* obj, Expression* index);
Expression* gNewIdExp(char* id);
Expression* gNewFuncCallExp(char* funcName, ArgumentList* args);

Statement* gNewStmt(StatementType type)
{
    Statement* stmt = NULL;
    StmtPool* stmts = &structPool.stmts;

    newItem(stmts, stmt);
    if(stmt == NULL)    return NULL;
    stmt->type = type;
    stmt->lineNumber = getCurCompiler()->lineNum;
}
Statement* gNewIfStmt(Expression *condition,
                        Block *thenBlock, ElsifList *elsifList,
                        Block *elseBlock);
Elsif* gNewElsif(Expression* exp, Block* block);
ElsifList* gNewElsifList(Elsif* elsif);
ElsifList* gAddElsifList(ElsifList* list, Elsif* elsif);
Statement* gNewWhileStmt(Expression *condition, Block *block);
Statement* gNewForStmt(Expression *init, Expression *cond,
                        Expression *post, Block *block);
Statement* gNewExpStmt(Expression* exp);
Statement* gNewReturnStmt(Expression* exp);
Statement* gNewBreakStmt();
Statement* gNewContinueStmt();

Block* gNewBlock(StatementList* list);




