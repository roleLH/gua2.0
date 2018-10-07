#pragma once
// 一个简单 chunk 定义 19-09-30
#include "util/hashTable.h"
#include "util/vector.h"
#include  <string.h>
#include <assert.h>

typedef struct Chunk Chunk;


struct ChunkInfo
{
    int size;                // 大小
    HashTable* fieldDescriptor; // 每个域的描述<string, int>从前向后，域的名字，以及其相对块开头的偏移
    char chunkName[1];            // 块名
};

// 创建一个 块信息结构
ChunkInfo* newChunk(const char* chunkName);
void freeChunk(ChunkInfo* info);
// 根据对应域返回其偏移，没有找到返回-1
int fieldOffset(ChunkInfo* info, const char* fieldName);
// 添加新的域记录, 0 表示添加失败，非0 成功
// \param size 这里size表示该域所占的大小
int addfd(ChunkInfo* info, const char* fieldName, int size);


ChunkInfo* newChunk(const char* chunkName)
{
    size_t len = strlen(chunkName);
    ChunkInfo* info = (ChunkInfo*)malloc(sizeof(ChunkInfo) + len);
    if(info == NULL)
        return NULL;
    
    info->size = 0;
    info->fieldDescriptor = 
        newHashTable(5, 5, strHash, strCmp, 
                     strCpy, intCpy, strDtor, intDtor);
    if(info->fieldDescriptor == NULL)
    {
        free(info);
        return NULL;
    }
    strcpy(info->chunkName, chunkName);
    info->chunkName[len] = '\0';
    return info;
}

void freeChunk(ChunkInfo* info)
{
    if(info)
    {
        freeHashTable(info->fieldDescriptor);
        free(info);
    }
}

int fieldOffset(ChunkInfo* info, const char* fieldName)
{
    int* nump = (int*)hFind(info->fieldDescriptor, (void*)fieldName);
    if(nump == NULL)
        return -1;
    return *nump;
}

int addfd(ChunkInfo* info, const char* fieldName, int size)
{
    assert(size > 0);
    int oldSize = info->size;
    info->size += size;
    return hInsert(info->fieldDescriptor, (void*)fieldName, (void*)&oldSize);
}



struct ChunksManager
{
    HashTable* chunkMap;    // <string, ChunkInfo*>
    
};

void releaseChunks(ChunksManager* m)
{
    size_t mapSize = hSize(m->chunkMap);
    size_t i = 0;
    for(; i < mapSize; ++i)
    {
        ChunkInfo* info = (ChunkInfo*)hFindById(m->chunkMap, i);
        if(info)
            freeChunk(info);
    }
}


ChunksManager* newChunksManager()
{
    ChunksManager* m = (ChunksManager*)malloc(sizeof(ChunksManager));
    if(m == NULL)
        return NULL;
    m->chunkMap = newHashTable(5, 5, strHash, strCmp,
                                strCpy, pointerCpy, strDtor, pointerDtor);
    if(m->chunkMap == NULL)
    {
        free(m);
        return NULL;
    }
}



void freeChunkManager(ChunksManager* m)
{
    if(m)
    {
        releaseChunks(m);
        freeHashTable(m->chunkMap);
        free(m);
    }
}

int addChunk(ChunksManager* m, char* chunkName, ChunkInfo* info)
{
    return hInsert(m->chunkMap, (void*)chunkName, (void*)info);
}

ChunkInfo* findChunk(ChunksManager* m, const char* chunkName)
{
    return  (ChunkInfo*)hFind(m->chunkMap, (void*)chunkName);
}

ChunkInfo* findChunkById(ChunksManager* m, size_t id)
{
    return (ChunkInfo*)hFindById(m->chunkMap, id);
}