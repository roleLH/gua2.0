#include "util/hashTable.h"

#include <stdio.h>






int main()
{
    // <int, int>
    HashTable* htab = newHashTable(5, 5, intHash, intCmp, intCpy, intCpy, intDtor, intDtor);
    
    int arr[] = {1,2,3,4};
    int arr2[] = {4,3,2,1};

    hInsert(htab, &arr[0], &arr2[0]);


    freeHashTable(htab);
    return 0;
}