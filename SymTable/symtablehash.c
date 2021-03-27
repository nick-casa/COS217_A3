/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Nickolas Casalinuovo                                       */
/*--------------------------------------------------------------------*/

#include "stack.h"
#include <stdlib.h>
#include <assert.h>

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
struct Sy{
   /* The address of the first LinkedListNode. */
   struct LinkedListNode *psFirstNode;

   /* Amount of bindings in the LinkedList */
   size_t iBindings;
};

/*--------------------------------------------------------------------*/

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount){
        const size_t HASH_MULTIPLIER = 65599;
        size_t u;
        size_t uHsash = 0;
        assert(pcKey != NULL);
        for (u = 0; pcKey[u] != '\0'; u++)
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
        
        return uHash % uBucketCount;
}
   

/*--------------------------------------------------------------------*/

/* Double the physical length of oStack.  Return 1 (TRUE) if
   successful, or 0 (FALSE) if insufficient memory is available. */

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

SymTable_T SymTable_new(void);

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

void SymTable_free(SymTable_T oSymTable);

size_t SymTable_getLength(SymTable_T oSymTable);

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), 
    const void *pvExtra);
