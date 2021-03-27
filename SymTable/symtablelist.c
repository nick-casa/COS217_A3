/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Nickolas Casalinuovo                                       */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

/* Each item is stored in a StackNode.  StackNodes are linked to
   form a list.  */

struct StackNode{
   /* The key. */
   char *pcKey;
   /* The value. */
   const void *pvValue;

   /* The address of the next StackNode. */
   struct StackNode *psNextNode;
};

/*--------------------------------------------------------------------*/

/* A Stack is a structure that points to the first StackNode. */

struct Stack{
   /* The address of the first StackNode. */
   struct StackNode *psFirstNode;

   /* Amount of bindings in the stack */
   size_t iBindings;
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct Stack));

   if (oSymTable == NULL)
      return NULL;

   oSymTable->psFirstNode = NULL;
   oSymTable->iBindings = 0;
   return oSymTable;
}

size_t SymTable_getLength(SymTable_T oSymTable){
   return oSymTable->iBindings;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue){
   struct StackNode *psNewNode, *pcInsNode;
   char* pcKeyCopy;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   assert(pvValue != NULL);
   
   psNewNode = oSymTable->psFirstNode;
   
   pcKeyCopy = (char*)malloc(strlen(pcKey));
   if (pcKeyCopy == NULL)
    return 0;
   strcpy(pcKeyCopy,(char*)pcKey);

   pcInsNode = (struct StackNode*)malloc(sizeof(struct StackNode));
   if (pcInsNode == NULL){
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
   struct StackNode *psCheckNode;
   
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
   struct StackNode *psCheckNode;
   
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
   struct StackNode *psCheckNode;
   
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   assert(pvValue != NULL);
   
   psCheckNode = oSymTable->psFirstNode;

   if(!psCheckNode) return NULL;
   else{
     while(psCheckNode!= NULL){
       if(strcmp(psCheckNode->pcKey,pcKey)==0){
        psCheckNode->pvValue = pvValue;
        return (void*)pvValue;
       }
       else psCheckNode = psCheckNode->psNextNode;   
     }
   }
   return NULL;
}

void SymTable_free(SymTable_T oSymTable){
   struct StackNode *psCurrentNode;
   struct StackNode *psNextNode;

   assert(oSymTable != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
        psCurrentNode != NULL;
        psCurrentNode = psNextNode)
   {
      psNextNode = psCurrentNode->psNextNode;
      free((char*)psCurrentNode->pcKey);
      free(psCurrentNode);
   }

   free(oSymTable);
}

void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), 
    const void *pvExtra){
  
   struct StackNode *psCheckNode;
   
   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   
   psCheckNode = oSymTable->psFirstNode;

   while(psCheckNode!= NULL){
     (*pfApply)(psCheckNode->pcKey, (void*)psCheckNode->pvValue, (void*)pvExtra);
     psCheckNode = psCheckNode->psNextNode;   
   }

}


void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
  struct StackNode *psCheckNode,*psTempNode;
  int comp; 
  void* pvValue;

  assert(oSymTable != NULL);
  assert(pcKey != NULL);
   
  psCheckNode = oSymTable->psFirstNode;
  
  // Case = No nodes in list
  if(!psCheckNode) return NULL;  
  else{
    comp = strcmp(psCheckNode->pcKey,pcKey);
    if(comp == 0 && !psCheckNode->psNextNode){
      pvValue =  (void*) psCheckNode->pvValue;
      free(psCheckNode->pcKey);
      free(psCheckNode);
      oSymTable->psFirstNode = NULL;
      oSymTable->iBindings = 0;
      return pvValue;
    }
    else if(comp == 0 && psCheckNode->psNextNode){
      pvValue = (void*)psCheckNode->pvValue;
      psTempNode = psCheckNode;
      oSymTable->iBindings--;
      oSymTable->psFirstNode = psTempNode->psNextNode;
      free(psTempNode->pcKey);
      free(psTempNode);
      return pvValue; 
    }
    else{
      while(psCheckNode->psNextNode){
        if(strcmp(psCheckNode->psNextNode->pcKey,pcKey)==0){
          if(psCheckNode->psNextNode->psNextNode){
            pvValue = (void*)psCheckNode->psNextNode->pvValue;
            psTempNode = psCheckNode->psNextNode;
            psCheckNode->psNextNode = psCheckNode->psNextNode->psNextNode; 
            oSymTable->iBindings--;
            free(psTempNode->pcKey);
            free(psTempNode);
            return pvValue; 
          }
          else {
            pvValue = (void*)psCheckNode->psNextNode->pvValue;
            psTempNode = psCheckNode->psNextNode;
            psCheckNode->psNextNode = NULL; 
            oSymTable->iBindings--;
            free(psTempNode->pcKey);
            free(psTempNode);
            return pvValue; 
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
    
    SymTable_T oSymTable;
    oSymTable = SymTable_new();
    
    i = SymTable_put(oSymTable, "Ruth", "RF");
    i = SymTable_put(oSymTable, "Gehrig", "1B");
    i = SymTable_put(oSymTable, "Mantle", "CF");
    i = SymTable_put(oSymTable, "Jeter", "SS");
    
    uLength = SymTable_getLength(oSymTable);
    
    pvOldValue = SymTable_get(oSymTable, "Mantle");
    printf("%s\n",(char*)pvOldValue );
    
    pvOldValue = SymTable_replace(oSymTable, "Mantle", "1B");
    printf("%s\n",(char*)pvOldValue );
        
    pvOldValue = SymTable_get(oSymTable, "Mantle");
    printf("%s\n",(char*)pvOldValue );
    
    i = SymTable_contains(oSymTable, "Ruth");
    printf("%i\n",i );
    
    
    pvOldValue = SymTable_remove(oSymTable, "Ruth"); 
    printf("Removed: %s\n",(char*)pvOldValue );
    
    i = SymTable_contains(oSymTable, "Ruth");
    printf("%i\n",i );
  
    SymTable_free(oSymTable);
     
    
}
*/

