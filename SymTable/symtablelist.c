/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Nickolas Casalinuovo                                       */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#ifndef SYMTABLE_INCLUDED
#include "symtable.h"
#endif


/* Each item is stored in a LinkedListNode.  LinkedListNodes are linked to
   form a list.  */

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
   /* The address of the first LinkedListNode. */
   struct LinkedListNode *psFirstNode;

   /* Amount of bindings in the SymTable */
   size_t iBindings;
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));

   if (oSymTable == NULL)
      return NULL;

   oSymTable->psFirstNode = NULL;
   oSymTable->iBindings = 0;
   return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable){
   struct LinkedListNode *psCurrentNode;
   size_t iBindings,iIter;

   assert(oSymTable != NULL);
   for(iBindings = oSymTable->iBindings;
      iBindings>0;
      iBindings--){
      psCurrentNode = oSymTable->psFirstNode;
      for(iIter = 0; iIter<iBindings-1;iIter++){
        psCurrentNode = psCurrentNode->psNextNode;
      }
      free((char*)psCurrentNode->pcKey);
      free(psCurrentNode);
   }
   free(oSymTable);
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable){
   return oSymTable->iBindings;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   struct LinkedListNode *psNewNode, *pcInsNode;
   char* pcKeyCopy;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   psNewNode = oSymTable->psFirstNode;
   
   /* If the oSymTable is empty */
   if(!psNewNode){
     /* Allocate space for defensive key */
     pcKeyCopy = (char*)malloc(strlen(pcKey)+1);
     if (pcKeyCopy == NULL)
      return 0;
     /* Copy key to allocated memory */
     strcpy(pcKeyCopy,(char*)pcKey);

     /* Allocate space for node */
     pcInsNode = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
     if (pcInsNode == NULL){
      /* If there is no space, free key */
      free(pcKeyCopy);
      return 0;
     }
     pcInsNode->pcKey = pcKeyCopy;
     pcInsNode->pvValue = pvValue;
     pcInsNode->psNextNode = NULL;
     oSymTable->psFirstNode = pcInsNode;
     oSymTable->iBindings++;
     return 1;
   }
   /* If the oSymTable is not empty */
   else{
     /* Traverse entire linked list */
     while(psNewNode!= NULL){
       /* Return 0 of key is in linked list*/
       if(strcmp(psNewNode->pcKey,pcKey)==0) return 0;
       else psNewNode = psNewNode->psNextNode;   
     }
   }
   /* Symtable is not empty and key is not in list. */
   /* Allocate space for defensive key */
   pcKeyCopy = (char*)malloc(strlen(pcKey)+1);
   if (pcKeyCopy == NULL)
      return 0;
   /* Copy key to allocated memory */
   strcpy(pcKeyCopy,(char*)pcKey);

   /* Allocate space for node */
   pcInsNode = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
   if (pcInsNode == NULL){
      /* If there is no space, free key */
      free(pcKeyCopy);
      return 0;
   }

   /* Insert values into node */
   pcInsNode->pcKey = pcKeyCopy;
   pcInsNode->pvValue = pvValue;
   /* Insert node into linked list */
   pcInsNode->psNextNode = oSymTable->psFirstNode;
   oSymTable->iBindings++; 
   oSymTable->psFirstNode = pcInsNode;
   return 1;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   struct LinkedListNode *psCheckNode;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   psCheckNode = oSymTable->psFirstNode;

   if(!psCheckNode) return NULL;
   else{
     while(psCheckNode!= NULL){
       if(strcmp(psCheckNode->pcKey,pcKey)==0) return (void*) psCheckNode->pvValue;
       else psCheckNode = psCheckNode->psNextNode;   
     }
   }
   return NULL;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   struct LinkedListNode *psCheckNode;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   psCheckNode = oSymTable->psFirstNode;

   if(!psCheckNode) return 0;
   else{
     while(psCheckNode!= NULL){
       if(strcmp(psCheckNode->pcKey,pcKey)==0) return 1;
       else psCheckNode = psCheckNode->psNextNode;   
     }
   }
   return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   struct LinkedListNode *psCheckNode;
   const void* pvTempValue;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   psCheckNode = oSymTable->psFirstNode;

   if(!psCheckNode) return NULL;
   else{
     while(psCheckNode!= NULL){
       if(strcmp(psCheckNode->pcKey,pcKey)==0){
        pvTempValue = psCheckNode->pvValue;
        psCheckNode->pvValue = pvValue;
        return (void*) pvTempValue;
       }
       else psCheckNode = psCheckNode->psNextNode;   
     }
   }
   return NULL;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), 
    const void *pvExtra){
  
   struct LinkedListNode *psCurrentNode;
   
   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   
   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psCurrentNode->psNextNode)
    (*pfApply)(psCurrentNode->pcKey, (void*)psCurrentNode->pvValue, (void*)pvExtra);
  
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
  
  struct LinkedListNode *psCheckNode,*psTempNode;
  int comp; 
  const void* pvValue;
  
  assert(oSymTable != NULL);
  assert(pcKey != NULL);
  
  psCheckNode = oSymTable->psFirstNode;
  
  if(psCheckNode == NULL) return NULL;  

  /* Check whether the first node is same as removed key*/
  comp = strcmp(psCheckNode->pcKey,pcKey);

  if(comp == 0 && !psCheckNode->psNextNode){
    pvValue = psCheckNode->pvValue;
    free((char*)psCheckNode->pcKey);
    free(psCheckNode);
    oSymTable->psFirstNode = NULL;
    oSymTable->iBindings = 0;
    return (void*)pvValue;
  }
  
  else if(comp == 0 && psCheckNode->psNextNode){
    pvValue = psCheckNode->pvValue;
    psTempNode = psCheckNode;
    oSymTable->iBindings--;
    oSymTable->psFirstNode = psTempNode->psNextNode;
    free((char*)psTempNode->pcKey);
    free(psTempNode);
    return (void*) pvValue; 

  }
  
  while(psCheckNode){
    /* Case 1.0 : psCheckNode->psNextNode exists and is key  */
    if(psCheckNode->psNextNode && strcmp(psCheckNode->psNextNode->pcKey,pcKey) == 0 ){
      /*  Case 1.1 : psCheckNode->psNextNode->psNextNode exists     */
      if(psCheckNode->psNextNode->psNextNode){
        pvValue = psCheckNode->psNextNode->pvValue;
        psTempNode = psCheckNode->psNextNode->psNextNode;
        free((char*)psCheckNode->psNextNode->pcKey);
        free(psCheckNode->psNextNode);
        psCheckNode->psNextNode = psTempNode;
        oSymTable->iBindings--;
        return (void*)pvValue; 
      }
      /*  Case 1.2 : psCheckNode->psNextNode->psNextNode does not exists  */
      else{
        pvValue = psCheckNode->psNextNode->pvValue;
        free((char*)psCheckNode->psNextNode->pcKey);
        free(psCheckNode->psNextNode);
        psCheckNode->psNextNode = NULL;
        oSymTable->iBindings--;
        return (void*)pvValue; 
      }
    }
    else psCheckNode = psCheckNode->psNextNode;   
  }    
  
  return NULL;
}




