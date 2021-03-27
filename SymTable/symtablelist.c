/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Nickolas Casalinuovo                                       */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef SYMTABLE_INCLUDED
#include "symtable.h"
#endif

/*--------------------------------------------------------------------*/

/* Each item is stored in a LinkedListNode.  LinkedListNodes are linked to
   form a list.  */

struct LinkedListNode{
   /* The key. */
   char *pcKey;
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

void SymTable_free(SymTable_T oSymTable){
   struct LinkedListNode *psCurrentNode;
   int iBindings,iIter;

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

size_t SymTable_getLength(SymTable_T oSymTable){
   return oSymTable->iBindings;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   struct LinkedListNode *psNewNode, *pcInsNode;
   char* pcKeyCopy;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   psNewNode = oSymTable->psFirstNode;
   
   pcKeyCopy = (char*)malloc(strlen(pcKey));
   if (!pcKeyCopy)
    return 0;
   strcpy(pcKeyCopy,(char*)pcKey);

   pcInsNode = (struct LinkedListNode*)malloc(sizeof(struct LinkedListNode));
   if (!pcInsNode){
    free(pcKeyCopy);
    return 0;
   }

   pcInsNode->pcKey = pcKeyCopy;
   pcInsNode->pvValue = pvValue;

   if(!psNewNode){
     oSymTable->psFirstNode = pcInsNode;
     oSymTable->iBindings++;
     return 1;
   }
   else{
     while(psNewNode!= NULL){
       if(strcmp(psNewNode->pcKey,pcKeyCopy)==0) return 0;
       else psNewNode = psNewNode->psNextNode;   
     }
   }
   
   pcInsNode->psNextNode = oSymTable->psFirstNode;
   oSymTable->iBindings++; 
   oSymTable->psFirstNode = pcInsNode;
   
   return 1;
}

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

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   struct LinkedListNode *psCheckNode;
   const void* pvTempValue;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   assert(pvValue != NULL);
   
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

void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), 
    const void *pvExtra){
  
   struct LinkedListNode *psCheckNode;
   
   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   
   psCheckNode = oSymTable->psFirstNode;

   while(psCheckNode!= NULL){
     (*pfApply)(psCheckNode->pcKey, (void*)psCheckNode->pvValue, (void*)pvExtra);
     psCheckNode = psCheckNode->psNextNode;   
   }

}


void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
  struct LinkedListNode *psCheckNode,*psTempNode;
  int comp; 
  const void* pvValue;

  assert(oSymTable != NULL);
  assert(pcKey != NULL);
   
  psCheckNode = oSymTable->psFirstNode;
  
  if(!psCheckNode) return NULL;  
  else{
    comp = strcmp(psCheckNode->pcKey,pcKey);
    if(comp == 0 && !psCheckNode->psNextNode){
      pvValue = psCheckNode->pvValue;
      free(psCheckNode->pcKey);
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
      free(psTempNode->pcKey);
      free(psTempNode);
      return (void*) pvValue; 
    }
    else{
      while(psCheckNode->psNextNode){
        if(strcmp(psCheckNode->psNextNode->pcKey,pcKey)==0){
          if(psCheckNode->psNextNode->psNextNode){
            pvValue = psCheckNode->psNextNode->pvValue;
            psTempNode = psCheckNode->psNextNode;
            psCheckNode->psNextNode = psCheckNode->psNextNode->psNextNode; 
            oSymTable->iBindings--;
            free(psTempNode->pcKey);
            free(psTempNode);
            return (void*)pvValue; 
          }
          else {
            pvValue = psCheckNode->psNextNode->pvValue;
            psTempNode = psCheckNode->psNextNode;
            psCheckNode->psNextNode = NULL; 
            oSymTable->iBindings--;
            free(psTempNode->pcKey);
            free(psTempNode);
            return (void*)pvValue; 
          }
        }
        else psCheckNode = psCheckNode->psNextNode;   
      }
      
    }
  }
  return NULL;
}
/*
int main(void){
    int i;
    size_t uLength;
    void *pvOldValue;
    char* ss;

    ss = "2B";

    SymTable_T oSymTable;
    oSymTable = SymTable_new();
    
    i = SymTable_put(oSymTable, "Ruth", "RF");
    i = SymTable_put(oSymTable, "Gehrig", "1B");
    i = SymTable_put(oSymTable, "Mantle", "CF");
    i = SymTable_put(oSymTable, "Jeter", ss);
    
    uLength = SymTable_getLength(oSymTable);
    pvOldValue = SymTable_get(oSymTable, "Mantle");
    pvOldValue = SymTable_replace(oSymTable, "Mantle", "1B");
    i = SymTable_contains(oSymTable, "Jeter");
    
    ss = "LF";

    pvOldValue = SymTable_get(oSymTable, "Jeter");
    printf("ss: %s SymTable_get: %s\n",ss,pvOldValue);
  
    pvOldValue = SymTable_remove(oSymTable, "Jeter"); 
    printf("ss: %s SymTable_remove: %s\n",ss,pvOldValue);
    
    printf("true: %i\n",pvOldValue==ss);
    
    i = SymTable_contains(oSymTable, "Gehrig");
    printf("%i\n", i);
    i = SymTable_contains(oSymTable, "Ruth");
    printf("%i\n", i);
    i = SymTable_contains(oSymTable, "Mantle");
    printf("%i\n", i);
    i = SymTable_contains(oSymTable, "Jeter");
    printf("%i\n", i);
    SymTable_free(oSymTable);
        
}
*/




