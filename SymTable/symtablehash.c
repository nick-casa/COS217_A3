/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Nickolas Casalinuovo                                       */
/*--------------------------------------------------------------------*/
#include <stdio.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#ifndef SYMTABLE_INCLUDED
#include "symtable.h"
#endif

static size_t bucketSizes[8] = { 509, 1021, 2039, 4093, 8191, 16381, 32749, 65521 }; 

/*--------------------------------------------------------------------*/

/* Return a hash code for pcKey that is between 0 and uBucketCount-1, inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount){
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;
   assert(pcKey != NULL);
   for (u = 0; pcKey[u] != '\0'; u++)
     uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
   return uHash % uBucketCount;
}

/*--------------------------------------------------------------------*/
      
/* Each item is stored in a LinkedListNode.  LinkedListNodes are linked to form a list.  */
struct LinkedListNode{
   /* The key. */
   const char *pcKey;
   /* Pointer to the value. */
   const void *pvValue;

   /* The address of the next LinkedListNode. */
   struct LinkedListNode *psNextNode;
};

/*--------------------------------------------------------------------*/

/* A SymTable is a structure that points to the first LinkedListNode. */
struct SymTable{

   /* The address of the array of LinkedLists. */
   struct LinkedListNode **psFirstNode;
   /* Amount of bindings in the SymTable */
   size_t stBindings;
    /* index Bucket Size */
   size_t stBucketIndex;
};


/*--------------------------------------------------------------------*/
static SymTable_T SymTable_larger(size_t size, size_t bucketIndex){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));

   if (oSymTable == NULL) return NULL;
        
   oSymTable->psFirstNode = calloc(size,sizeof(struct LinkedListNode*));
   oSymTable->stBindings = 0;
   oSymTable->stBucketIndex = bucketIndex;
   
   return oSymTable;
}
/*--------------------------------------------------------------------*/
static void putMap(const char *pcKey, void *pvValue, void *pvExtra){
   SymTable_put(pvExtra, pcKey, pvValue);
}
/*--------------------------------------------------------------------*/

static void SymTable_grow(SymTable_T oSymTable){
   SymTable_T newSymTable;
   struct LinkedListNode **oldHashTable;
   struct LinkedListNode *psNextLink, *psCurrentLink;
   size_t newSize, i;

   assert(oSymTable != NULL);

   newSize = oSymTable->stBucketIndex;
   newSize++;
   newSymTable = SymTable_larger(bucketSizes[newSize], newSize);
   
   SymTable_map(oSymTable, putMap, newSymTable);
   oSymTable->stBucketIndex = newSize;
   oldHashTable = oSymTable->psFirstNode;
   oSymTable->psFirstNode = newSymTable->psFirstNode;
   
   free(newSymTable);
   for(i=0;i<bucketSizes[newSize--];i++){
        psCurrentLink = oldHashTable[i];
        while(psCurrentLink != NULL){
             psNextLink = psCurrentLink->psNextNode;
             free((char*)psCurrentLink->pcKey);
             free(psCurrentLink);
             psCurrentLink = psNextLink;
        }
   }
   free(oldHashTable);
}
/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));

   if (oSymTable == NULL) return NULL;
        
   oSymTable->psFirstNode = calloc(bucketSizes[0],sizeof(struct LinkedListNode*));
   oSymTable->stBindings = 0;
   oSymTable->stBucketIndex = 0;
   
   return oSymTable;
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable){
    return oSymTable->stBindings;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   size_t hashValue;
   struct LinkedListNode *psTempNode;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,bucketSizes[oSymTable->stBucketIndex]);
   if(oSymTable->psFirstNode[hashValue])
        psTempNode = oSymTable->psFirstNode[hashValue];
   else return 0;
   while(psTempNode){
        if(strcmp(psTempNode->pcKey,pcKey) == 0) return 1;
        else psTempNode = psTempNode->psNextNode;
   }
   return 0;

}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   size_t hashValue;
   struct LinkedListNode *psLastFirst, *psNewNode;
   char *pcKeyCopy;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   if(oSymTable->stBindings+1>bucketSizes[oSymTable->stBucketIndex]&&
        oSymTable->stBindings+1<bucketSizes[7]){
        SymTable_grow(oSymTable);
   }
   

   hashValue = SymTable_hash(pcKey,bucketSizes[oSymTable->stBucketIndex]);
   
   if(SymTable_contains(oSymTable, pcKey)) return 0;
      
   pcKeyCopy = (char*)malloc(strlen(pcKey)+1);
   if (pcKeyCopy == NULL) return 0;
   strcpy(pcKeyCopy,(char*)pcKey);

   psNewNode = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
   if (psNewNode == NULL){
    free(pcKeyCopy);
    return 0;
   }

   psNewNode->pcKey = pcKeyCopy;
   psNewNode->pvValue = pvValue;
   psLastFirst = oSymTable->psFirstNode[hashValue];
   psNewNode->psNextNode = psLastFirst;
   oSymTable->psFirstNode[hashValue] = psNewNode;
   oSymTable->stBindings++;
   
   return 1;
}

void SymTable_free(SymTable_T oSymTable){
   struct LinkedListNode *psNextLink, *psCurrentLink;
   size_t i;
   
   assert(oSymTable != NULL);

   for(i=0;i<bucketSizes[oSymTable->stBucketIndex];i++){
        psCurrentLink = oSymTable->psFirstNode[i];
        while(psCurrentLink != NULL){
             psNextLink = psCurrentLink->psNextNode;
             free((char*)psCurrentLink->pcKey);
             free(psCurrentLink);
             psCurrentLink = psNextLink;
        }
   }

   free(oSymTable->psFirstNode);
   free(oSymTable);
}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   size_t hashValue;
   struct LinkedListNode *psTempNode;
   const void *pvTempValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,bucketSizes[oSymTable->stBucketIndex]);
   if(oSymTable->psFirstNode[hashValue])
        psTempNode = oSymTable->psFirstNode[hashValue];
   else psTempNode = NULL;

   while(psTempNode){
        if(strcmp(psTempNode->pcKey,pcKey) == 0){
                pvTempValue = psTempNode->pvValue;
                psTempNode->pvValue = pvValue;
                return (void*) pvTempValue;
        }
        else psTempNode = psTempNode->psNextNode;
   }
   return NULL;    
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   size_t hashValue;
   struct LinkedListNode *psTempNode;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,bucketSizes[oSymTable->stBucketIndex]);
   if(oSymTable->psFirstNode[hashValue])
        psTempNode = oSymTable->psFirstNode[hashValue];
   else return NULL; 

   while(psTempNode){
        if(strcmp(psTempNode->pcKey,pcKey) == 0) return (void*) psTempNode->pvValue;
        else psTempNode = psTempNode->psNextNode;
   }
   return NULL;      
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), 
    const void *pvExtra){
   struct LinkedListNode *psTempNode;
   size_t i;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   
   for(i=0;i<bucketSizes[oSymTable->stBucketIndex];i++){
        if(oSymTable->psFirstNode[i] != NULL)
                psTempNode = oSymTable->psFirstNode[i];
        else psTempNode = NULL;

        while(psTempNode){
          (*pfApply)(psTempNode->pcKey, (void*)psTempNode->pvValue,(void*) pvExtra);
          psTempNode = psTempNode->psNextNode;
        }
   }
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   size_t hashValue;
   struct LinkedListNode *psTempNode, *psLastNode;
   const void* pvValue;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,bucketSizes[oSymTable->stBucketIndex]);
   
   if(oSymTable->psFirstNode[hashValue]){
        psTempNode = oSymTable->psFirstNode[hashValue];
        psLastNode = psTempNode;
   }
   else return NULL;

   
   while(psTempNode){
     if(strcmp(psTempNode->pcKey,pcKey) == 0){
        if(psTempNode->psNextNode){
          pvValue = psTempNode->pvValue;
          if(oSymTable->psFirstNode[hashValue] == psTempNode)
                oSymTable->psFirstNode[hashValue] = psTempNode->psNextNode;
          else psLastNode->psNextNode = psTempNode->psNextNode;
          free((char*)psTempNode->pcKey);
          free(psTempNode);
          oSymTable->stBindings--;
          return (void*)pvValue;
        }
        else{
          pvValue = psTempNode->pvValue;
          psTempNode->pvValue = NULL;
          if(oSymTable->psFirstNode[hashValue] == psTempNode) 
                oSymTable->psFirstNode[hashValue] = NULL;
          else psLastNode->psNextNode = NULL;
          free((char*)psTempNode->pcKey);
          free(psTempNode);
          oSymTable->stBindings--;
          return (void*)pvValue;   
        }
     }
     else{
        psLastNode = psTempNode;
        psTempNode = psTempNode->psNextNode;
     }
   }
   
   return NULL;
}
/*
int main(void){
     SymTable_T oSymTable;
   int iSuccessful;
   char acCenterField[] = "pitcher";
   char acCatcher[] = "catcher";
   char acFirstBase[] = "first base";
   char acRightField[] = "second base";
   char *pcValue;

   
   oSymTable = SymTable_new();
   
   iSuccessful = SymTable_put(oSymTable, "250", acCenterField);
   printf("%zu\n", SymTable_hash("250",bucketSizes[oSymTable->stBucketIndex]));
   iSuccessful = SymTable_put(oSymTable, "469", acCatcher);
   printf("%zu\n", SymTable_hash("469",bucketSizes[oSymTable->stBucketIndex]));
   iSuccessful = SymTable_put(oSymTable, "947", acFirstBase);
   printf("%zu\n", SymTable_hash("947",bucketSizes[oSymTable->stBucketIndex]));
   iSuccessful = SymTable_put(oSymTable, "1303", acRightField);
   printf("%zu\n", SymTable_hash("1303",bucketSizes[oSymTable->stBucketIndex]));
   iSuccessful = SymTable_put(oSymTable, "2016", acRightField);
   printf("%zu\n", SymTable_hash("2016",bucketSizes[oSymTable->stBucketIndex]));
   
   
   pcValue = SymTable_get(oSymTable, "250");
   printf("%s\n", pcValue);
   pcValue = SymTable_get(oSymTable, "469");
   printf("%s\n", pcValue);
   pcValue = SymTable_get(oSymTable, "947");
   printf("%s\n", pcValue);
   pcValue = SymTable_get(oSymTable, "1303");
   printf("%s\n", pcValue);
   pcValue = SymTable_get(oSymTable, "2016");
   printf("%s\n", pcValue);

   pcValue = SymTable_remove(oSymTable, "947");
   printf("Removed: %s\n", pcValue);   
   pcValue = SymTable_remove(oSymTable, "2016");
   printf("Removed: %s\n", pcValue);
   pcValue = SymTable_remove(oSymTable, "250");
   printf("Removed: %s\n", pcValue);
   pcValue = SymTable_get(oSymTable, "469");
   printf("Got: %s\n", pcValue);

   pcValue = SymTable_get(oSymTable, "1303");
   printf("Got: %s\n", pcValue);

   SymTable_free(oSymTable);
  }
)
*/
