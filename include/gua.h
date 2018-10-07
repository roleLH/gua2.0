#pragma once
#include "../util/hashTable.h"
#include "../util/vector.h"

typedef struct GValue GValue;

struct GValue
{
    int c;
};

typedef struct Expression Expression;
typedef struct Statement Statement;

typedef enum 
{

}CompileError;


typedef enum 
{
    NUMBER_EXPRESSION,

} ExpressionType;

typedef struct ArgumentList
{ 
    TinyVector* argList;
} ArgumentList;

typedef struct 
{
    Expression* left;
    Expression* operand;
} AssignExpression;

typedef struct 
{
    Expression  *left;
    Expression  *right;
} BinaryExpression;


typedef struct 
{
    char* funcName;
    ArgumentList* args;
} FunctionCallExpression;

typedef struct
{
    Expression* object;
    Expression* index;
} IndexExpression;

struct Expression
{
    ExpressionType type;
    int lineNumber;
    union 
    {
        int number;
        AssignExpression asgExp;
        BinaryExpression binExp;
        Expression* minusExp;
        Expression* typeofExp;
        Expression* delExp;
        FunctionCallExpression funcCallExp;
        IndexExpression indexExp;
    } exp;
};



typedef struct StatementList
{
    TinyVector* statements;
} StatementList;

typedef struct Block
{
    StatementList* statementList;
} Block;

typedef struct Elsif
{
    Expression* condition;
    Block* block;
} Elsif;

typedef struct ElsifList
{
    TinyVector* elsifs;
} ElsifList;

typedef struct IfStatement
{
    Expression* condition;
    Block* thenBlock;
    ElsifList* elsifList;
    Block* elseBlock;
} IfStatement;

typedef struct WhileStatement
{
    Expression* condition;
    Block* block;
} WhileStatement;

typedef struct ForStatement
{
    Expression* init;
    Expression* condition;
    Expression* post;
    Block* block;
} ForStatement;

typedef struct {
    Expression* returnValue;
} ReturnStatement;


typedef enum
{

} StatementType;


struct Statement
{
    StatementType type;
    int lineNumber;
    union 
    {
        Expression* exp;
        IfStatement ifs;
        WhileStatement whiles;
        ForStatement fors;
        ReturnStatement returns;
    }stmt;
};








typedef struct ParameterList
{
    HashTable* params;
} ParameterList;

typedef struct FunctionDefinition
{
    char* name;
    ParameterList* paramList;
    Block* block;

} FunctionDefinition;


typedef struct FieldInfo
{
    char* name;
    unsigned size;
} FieldInfo;

typedef struct ChunkDefinition
{
    HashTable* fields;
} ChunkDefinition;

typedef enum 
{

} StatementResultType;

typedef struct StatementResult
{
    StatementResultType type;
    GValue value;
} StatementResult;

typedef struct Compiler
{
    HashTable* funcMap;     // <funcName, FuncDef*>
    HashTable* chunkMap;    // <chunkName, ChunkDef*>
    int lineNum;
    Block* curBlock;
    FunctionDefinition* curFunc;
} Compiler;

static Compiler* getCurCompiler();
static FunctionDefinition* searchFunc(char* name);
static int addFuncDef(FunctionDefinition* funcDef);

static FunctionDefinition* searchFunc(char* name)
{
    Compiler* c = getCurCompiler();
    return (FunctionDefinition*)hFind(c->funcMap, name);
}

static int addFuncDef(FunctionDefinition* funcDef)
{
    Compiler* c = getCurCompiler();
    return hInsert(c->funcMap, funcDef->name, funcDef);
}

/************ create ***************/
static int initStructPool();
static void releaseStructPool();

FunctionDefinition* gFunctionDefine(char* id, ParameterList* params, Block* block);

ParameterList* gNewParamList(char* id);
ParameterList* gAddParam(ParameterList* list, char* id);

ArgumentList* gNewArgList(Expression* exp);
ArgumentList* gAddArg(ArgumentList* list, Expression* exp);

StatementList* gNewStmtList(Statement* stmt);
StatementList* gAddStmt(StatementList* list, Statement* stmt);


FieldInfo* gNewFieldInfo(char* id, unsigned size);
ChunkDefinition* gNewChunkDefine(FieldInfo* info);
ChunkDefinition* gAddFieldInfo(ChunkDefinition* chunkDef, FieldInfo* info);

Expression* gNewExp(ExpressionType type);
Expression* gNewAsgExp(Expression* left, Expression* operand);
Expression* gNewBinExp(ExpressionType op, Expression* left, Expression* right);
Expression* gNewMinusExp(Expression* operand);
Expression* gNewDeleteExp(Expression* exp);
Expression* gNewIndexExp(Expression* obj, Expression* index);
Expression* gNewIdExp(char* id);
Expression* gNewFuncCallExp(char* funcName, ArgumentList* args);

Statement* gNewStmt(StatementType type);
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


//
void gCompileError(CompileError id, ...);








