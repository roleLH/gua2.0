#include "util/hashTable.h"

#include <stdio.h>

int testHashTable()
{
    // <int, int>
    HashTable* htab = newHashTable(5, 5, intHash, intCmp, intCpy, intCpy, intDtor, intDtor);

    int arr[] = {1,2,3,4, 5, 67};
    int arr2[] = {4,3,2,1, 0, -78};

    int i = 0;
    for(; i < 6; ++i)
        hInsert(htab, &arr[i], &arr2[i]);
    i = 0;
    for(;i < 6; ++i)
        printf("num : %d \n", *(int*)hFind(htab, &arr[i]));
    //printf("num : %d \n", *(int*)hFind(htab, &arr[3]));

    freeHashTable(htab);
    return 0;
}
