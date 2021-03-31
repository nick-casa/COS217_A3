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

static const size_t bucketSizes[8] = { 509, 1021, 2039, 4093, 8191, 16381, 32749, 65521 }; 
static const size_t MAX_BUCKET_INDEX = 7; 

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
   void *pvValue;

   /* The address of the next LinkedListNode. */
   struct LinkedListNode *psNextNode;
};

/*--------------------------------------------------------------------*/

/* A SymTable is a structure that points to the first LinkedListNode. */
struct SymTable{

   /* The address of the array of LinkedListNode */
   struct LinkedListNode **psFirstNode;
   /* stBindings holds the amount of bindings in the SymTable */
   size_t stBindings;
    /* stBucketIndex holds the index of the bucket size */
   size_t stBucketIndex;
};


/*--------------------------------------------------------------------*/
static struct LinkedListNode** SymTable_newHash(size_t size){
   struct LinkedListNode** oHashTable;
   
   oHashTable = calloc(size,sizeof(struct LinkedListNode*));
   if(oHashTable == NULL){
        free(oHashTable);
        return NULL;
   }
   return oHashTable;
}
/*--------------------------------------------------------------------*/
static int SymTable_putMap(const char *pcKey, void *pvValue, 
        struct LinkedListNode **pvHashTable, size_t ubucketIndex){
   
   size_t hashValue;
   struct LinkedListNode *psLastFirst, *psNewNode, *psTempNode;
   char *pcKeyCopy;

   hashValue = SymTable_hash(pcKey,bucketSizes[ubucketIndex]);
   
   if(pvHashTable[hashValue]) psTempNode = pvHashTable[hashValue];
   else psTempNode = NULL;

   while(psTempNode){
        if(strcmp(psTempNode->pcKey,pcKey) == 0) return 0;
        else psTempNode = psTempNode->psNextNode;
   }
      
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
   psLastFirst = pvHashTable[hashValue];
   psNewNode->psNextNode = psLastFirst;
   pvHashTable[hashValue] = psNewNode;
   
   return 1;
}
/*--------------------------------------------------------------------*/

static void SymTable_grow(SymTable_T oSymTable){
   struct LinkedListNode **oldHashTable,**newHashTable;
   struct LinkedListNode *psNextLink, *psCurrentLink;
   size_t oldSize, newSize, i;

   assert(oSymTable != NULL);

   oldSize = oSymTable->stBucketIndex;
   newSize = oldSize+1;
   newHashTable = SymTable_newHash(bucketSizes[newSize]);
   if(newHashTable != NULL){
      oldHashTable = oSymTable->psFirstNode;
      for(i=0;i<bucketSizes[oldSize];i++){
           psCurrentLink = oldHashTable[i];
           while(psCurrentLink != NULL){
                   psNextLink = psCurrentLink->psNextNode;
                   SymTable_putMap(psCurrentLink->pcKey, 
                          psCurrentLink->pvValue, 
                          newHashTable, newSize);
                   
                   free((char*)psCurrentLink->pcKey);
                   free(psCurrentLink);
                   psCurrentLink = psNextLink;
           }
      }
      free(oSymTable->psFirstNode);
      oSymTable->psFirstNode = newHashTable;
      oSymTable->stBucketIndex = newSize;
   }

}

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));

   if (oSymTable == NULL) return NULL;
        
   oSymTable->psFirstNode = calloc(bucketSizes[0],sizeof(struct LinkedListNode*));
   if(oSymTable->psFirstNode == NULL){
        free(oSymTable);
        return NULL;
   }
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
        oSymTable->stBindings+1<bucketSizes[MAX_BUCKET_INDEX]){
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
   psNewNode->pvValue = (void*)pvValue;
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
   void *pvTempValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hashValue = SymTable_hash(pcKey,bucketSizes[oSymTable->stBucketIndex]);
   if(oSymTable->psFirstNode[hashValue])
        psTempNode = oSymTable->psFirstNode[hashValue];
   else psTempNode = NULL;

   while(psTempNode){
        if(strcmp(psTempNode->pcKey,pcKey) == 0){
                pvTempValue = psTempNode->pvValue;
                psTempNode->pvValue = (void*)pvValue;
                return pvTempValue;
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
        if(strcmp(psTempNode->pcKey,pcKey) == 0) return psTempNode->pvValue;
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
          (*pfApply)(psTempNode->pcKey,  psTempNode->pvValue,(void*) pvExtra);
          psTempNode = psTempNode->psNextNode;
        }
   }
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   size_t hashValue;
   struct LinkedListNode *psTempNode, *psLastNode;
   void* pvValue;
   
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
          return pvValue;
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
          return pvValue;   
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
   enum {MAX_KEY_LENGTH = 10};

   SymTable_T oSymTable;
   SymTable_T oSymTableSmall;
   char acKey[MAX_KEY_LENGTH];
   char *pcValue;
   int i;
   int iSmall;
   int iLarge;
   int iSuccessful;
   size_t uLength = 0;
   size_t uLength2;

   oSymTableSmall = SymTable_new();
   iSuccessful = SymTable_put(oSymTableSmall, "xxx", "xxx");
   iSuccessful = SymTable_put(oSymTableSmall, "yyy", "yyy");
   
   oSymTable = SymTable_new();
   
    for (i = 0; i < 520; i++){
      sprintf(acKey, "%d", i);
      pcValue = (char*)malloc(sizeof(char) * (strlen(acKey) + 1));
      strcpy(pcValue, acKey);
      iSuccessful = SymTable_put(oSymTable, acKey, pcValue);
      uLength = SymTable_getLength(oSymTable);
   }

   iSmall = 0;
   iLarge = 520 - 1;
   while (iSmall < iLarge)
   {

      sprintf(acKey, "%d", iSmall);
      pcValue = (char*)SymTable_get(oSymTable, acKey);
      iSmall++;

      sprintf(acKey, "%d", iLarge);
      pcValue = (char*)SymTable_get(oSymTable, acKey);
      iLarge--;
   }

   if (iSmall == iLarge)
   {
      sprintf(acKey, "%d", iSmall);
      pcValue = (char*)SymTable_get(oSymTable, acKey);
   }


   iSmall = 0;
   iLarge = 520 - 1;
   while (iSmall < iLarge)
   {

      sprintf(acKey, "%d", iSmall);
      pcValue = (char*)SymTable_remove(oSymTable, acKey);
      printf("%s\n",pcValue );
      
      free(pcValue);
      uLength--;
      uLength2 = SymTable_getLength(oSymTable);
      iSmall++;

      sprintf(acKey, "%d", iLarge);
      pcValue = (char*)SymTable_remove(oSymTable, acKey);
      free(pcValue);
      uLength--;
      uLength2 = SymTable_getLength(oSymTable);
      iLarge--;
   }

   if (iSmall == iLarge)
   {
      sprintf(acKey, "%d", iSmall);
      pcValue = (char*)SymTable_remove(oSymTable, acKey);
      printf("%s\n",pcValue );
      free(pcValue);
      uLength--;
      uLength2 = SymTable_getLength(oSymTable);
   }


   pcValue = (char*)SymTable_get(oSymTableSmall, "xxx");
   pcValue = (char*)SymTable_get(oSymTableSmall, "yyy");
   

   SymTable_free(oSymTable);
   SymTable_free(oSymTableSmall);


}
*/
