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

struct HashTableNode{
   /* The address of the first LinkedListNode. */
   struct LinkedListNode *psFirstNode;
};

/*--------------------------------------------------------------------*/

/* A SymTable is a structure that points to the first LinkedListNode. */
struct SymTable{

   /* The address of the array of HashTableNodes. */
   struct HashTableNode *psHashNodes;
   /* Amount of bindings in the SymTable */
   size_t stBindings;
    /* current Bucket Size */
   size_t stBuckets;
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));

   if (oSymTable == NULL) return NULL;
        
   oSymTable->psHashNodes = calloc(bucketSizes[0]*sizeof(oSymTable->psHashNodes));
   oSymTable->stBindings = 0;
   oSymTable->stBuckets = bucketSizes[0];
   
   return oSymTable;
}

size_t SymTable_getLength(SymTable_T oSymTable){
    return oSymTable->stBindings;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   size_t hashValue;
   struct LinkedListNode *psTempNode;
   struct HashTableNode *psHashNode;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,oSymTable->stBuckets);
   psHashNode = &(oSymTable->psHashNodes[hashValue]);
   psTempNode = psHashNode->psFirstNode;

   while(psTempNode){
        if(strcmp(psTempNode->pcKey,pcKey) == 0) return 1;
        else psTempNode = psTempNode->psNextNode;
   }
   return 0;

}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   size_t hashValue;
   struct LinkedListNode *psTempNode, *psLastFirst;
   struct HashTableNode *psHashNode;
   char *pcKeyCopy;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,oSymTable->stBuckets);
   psHashNode = &(oSymTable->psHashNodes[hashValue]);
   psTempNode = psHashNode->psFirstNode;
   
   
   while(psTempNode != NULL){
        if(strcmp(psTempNode->pcKey,pcKey) == 0) return 0;
        else psTempNode = psTempNode->psNextNode;
   }
   if(psHashNode->psFirstNode == NULL){
     pcKeyCopy = (char*)malloc(strlen(pcKey));
     if (pcKeyCopy == NULL) return 0;
     strcpy(pcKeyCopy,(char*)pcKey);

     psTempNode = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
     if (psTempNode == NULL){
      free(pcKeyCopy);
      return 0;
     }
     psTempNode->pcKey = pcKeyCopy;
     psTempNode->pvValue = pvValue;
     psTempNode->psNextNode = NULL;
     psHashNode->psFirstNode = psTempNode;
     oSymTable->stBindings++;
   }
   else{
     pcKeyCopy = (char*)malloc(strlen(pcKey));
     if (pcKeyCopy == NULL) return 0;
     strcpy(pcKeyCopy,(char*)pcKey);

     psTempNode = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
     if (psTempNode == NULL){
      free(pcKeyCopy);
      return 0;
     }
     psTempNode->pcKey = pcKeyCopy;
     psTempNode->pvValue = pvValue;
     psLastFirst = psHashNode->psFirstNode;
     psTempNode->psNextNode = psLastFirst;
     psHashNode->psFirstNode = psTempNode;
     oSymTable->stBindings++;
   }
   return 1;
}

void SymTable_free(SymTable_T oSymTable){
   struct LinkedListNode *psTempNode, *psNextLink;
   struct HashTableNode *psHashNode;
   int i;
   
   assert(oSymTable != NULL);

   for(i=0;i<oSymTable->stBuckets;i++){
        psHashNode = &(oSymTable->psHashNodes[i]);
        psTempNode = psHashNode->psFirstNode;
        while(psTempNode){
          if(psTempNode->psNextNode){
             psNextLink = psTempNode->psNextNode;
             free((char*)psTempNode->pcKey);
             free(psTempNode);
             psHashNode->psFirstNode = psNextLink; 
          }
          else{
            free((char*)psTempNode->pcKey);
            free(psTempNode);  
            psHashNode->psFirstNode = NULL; 
          }
          psTempNode = psHashNode->psFirstNode;
        }
   }
   free(oSymTable->psHashNodes);
   free(oSymTable);
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   size_t hashValue;
   struct LinkedListNode *psTempNode;
   struct HashTableNode *psHashNode;
   const void *pvTempValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,oSymTable->stBuckets);
   psHashNode = &(oSymTable->psHashNodes[hashValue]);
   psTempNode = psHashNode->psFirstNode;

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

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   size_t hashValue;
   struct LinkedListNode *psTempNode;
   struct HashTableNode *psHashNode;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,oSymTable->stBuckets);
   psHashNode = &(oSymTable->psHashNodes[hashValue]);
   psTempNode = psHashNode->psFirstNode;

   while(psTempNode){
        if(strcmp(psTempNode->pcKey,pcKey) == 0) return (void*) psTempNode->pvValue;
        else psTempNode = psTempNode->psNextNode;
   }
   return NULL;      
}

void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), 
    const void *pvExtra){
   struct LinkedListNode *psTempNode;
   struct HashTableNode *psHashNode;
   int i;
   
   for(i=0;i<oSymTable->stBuckets;i++){
        psHashNode = &(oSymTable->psHashNodes[i]);
        psTempNode = psHashNode->psFirstNode;
        while(psTempNode){
          (*pfApply)(psTempNode->pcKey, (void*)psTempNode->pvValue,(void*) pvExtra);
          psTempNode = psTempNode->psNextNode;
        }
   }
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   size_t hashValue;
   struct LinkedListNode *psTempNode, *psLastNode;
   struct HashTableNode *psHashNode;
   const void* pvValue;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,oSymTable->stBuckets);
   psHashNode = &(oSymTable->psHashNodes[hashValue]);
   psTempNode = psHashNode->psFirstNode;
   psLastNode = psTempNode;
   if(psTempNode != NULL){
     while(psTempNode){
       if(strcmp(psTempNode->pcKey,pcKey) == 0){
          if(psTempNode->psNextNode){
            pvValue = psTempNode->pvValue;
            if(psHashNode->psFirstNode == psLastNode) psHashNode->psFirstNode = psTempNode->psNextNode;
            else psLastNode->psNextNode = psTempNode->psNextNode;
            free((char*)psTempNode->pcKey);
            free(psTempNode);
            oSymTable->stBindings--;
            return (void*)pvValue;
          }
          else{
            pvValue = psTempNode->pvValue;
            psTempNode->pvValue = NULL;
            free((char*)psTempNode->pcKey);
            free(psTempNode);
            oSymTable->stBindings--;
            psTempNode = NULL;
            return (void*)pvValue;   
          }
       }
       else{
          psLastNode = psTempNode;
          psTempNode = psTempNode->psNextNode;
       }
     }
   }
   return NULL;
   
   
}
/*-------------------------------------------------------------------- 

static int Stack_grow(Stack_T oStack)
{
   const size_t GROWTH_FACTOR = 2;

   size_t uNewPhysLength;
   double *pdNewArray;

   assert(oStack != NULL);

   uNewPhysLength = GROWTH_FACTOR * oStack->uPhysLength;
   pdNewArray = (double*)
      realloc(oStack->pdArray, sizeof(double) * uNewPhysLength);
   if (pdNewArray == NULL)
      return 0;
   oStack->uPhysLength = uNewPhysLength;
   oStack->pdArray = pdNewArray;

   return 1;
}


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
   printf("%i\n",iSuccessful);
   iSuccessful = SymTable_put(oSymTable, "469", acCatcher);
   printf("%i\n",iSuccessful);
   iSuccessful = SymTable_put(oSymTable, "947", acFirstBase);
   printf("%i\n",iSuccessful);
   iSuccessful = SymTable_put(oSymTable, "1303", acRightField);
   printf("%i\n",iSuccessful);
   iSuccessful = SymTable_put(oSymTable, "2016", acRightField);
   printf("%i\n",iSuccessful);
   
   pcValue = SymTable_get(oSymTable, "250");
   printf("%s\n",pcValue);
   pcValue = SymTable_get(oSymTable, "469");
   printf("%s\n",pcValue);
   pcValue = SymTable_get(oSymTable, "947");
   printf("%s\n",pcValue);
   pcValue = SymTable_get(oSymTable, "1303");
   printf("%s\n",pcValue);
   pcValue = SymTable_get(oSymTable, "2016");
   printf("%s\n",pcValue);

   printf("Found 1 %s\n",oSymTable->psHashNodes[123].psFirstNode->pcKey);
   printf("Found 1 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->pcKey);
   printf("Found 1 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->psNextNode->pcKey);
   printf("Found 1 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->psNextNode->psNextNode->pcKey);
   printf("Found 1 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->psNextNode->psNextNode->psNextNode->pcKey);
   
   pcValue = SymTable_remove(oSymTable, "947");
   printf("Removed %s\n",pcValue);
   printf("Found 2 %s\n",oSymTable->psHashNodes[123].psFirstNode->pcKey);
   printf("Found 2 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->pcKey);
   printf("Found 2 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->psNextNode->pcKey);
   printf("Found 2 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->psNextNode->psNextNode->pcKey);
   
   pcValue = SymTable_remove(oSymTable, "2016");
   printf("Removed %s\n",pcValue);
   printf("Found 3 %s\n",oSymTable->psHashNodes[123].psFirstNode->pcKey);
   printf("Found 3 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->pcKey);
   printf("Found 3 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->psNextNode->pcKey);
   
   pcValue = SymTable_remove(oSymTable, "250");
   printf("Removed %s\n",pcValue);
   printf("Found 4 %s\n",oSymTable->psHashNodes[123].psFirstNode->pcKey);
   printf("Found 4 %s\n",oSymTable->psHashNodes[123].psFirstNode->psNextNode->pcKey);
   
   pcValue = SymTable_get(oSymTable, "469");
   
   printf("Get %s\n",pcValue);
   pcValue = SymTable_get(oSymTable, "1303");
   
   printf("Get %s\n",pcValue);
   
   

   SymTable_free(oSymTable);
   return 0;
}

*/

