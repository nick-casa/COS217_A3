/*--------------------------------------------------------------------*/
/* stack.c (Version 3)                                                */
/* Author: Bob Dondero                                                */
/*--------------------------------------------------------------------*/

#include "stack.h"
#include <stdlib.h>
#include <assert.h>

/*--------------------------------------------------------------------*/

/* A Stack object consists of an array of items, and related data. */
struct Stack
{
   /* The array in which items are stored. */
   double *pdArray;

   /* The index one beyond the top element. */
   size_t uTop;

   /* The number of elements in the array. */
   size_t uPhysLength;
};

/*--------------------------------------------------------------------*/

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
inclusive. */
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount){
        const size_t HASH_MULTIPLIER = 65599;
        size_t u;
        size_t uHash = 0;
        assert(pcKey != NULL);
        for (u = 0; pcKey[u] != '\0'; u++)
        uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
        
        return uHash % uBucketCount;
}
     
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

/*--------------------------------------------------------------------*/

Stack_T Stack_new(void)
{
   const size_t INITIAL_PHYS_LENGTH = 2;

   Stack_T oStack;

   oStack = (Stack_T)malloc(sizeof(struct Stack));
   if (oStack == NULL)
      return NULL;

   oStack->pdArray =
      (double*)calloc(INITIAL_PHYS_LENGTH, sizeof(double));
   if (oStack->pdArray == NULL)
   {
      free(oStack);
      return NULL;
   }

   oStack->uTop = 0;
   oStack->uPhysLength = INITIAL_PHYS_LENGTH;
   return oStack;
}

/*--------------------------------------------------------------------*/

void Stack_free(Stack_T oStack)
{
   assert(oStack != NULL);
   free(oStack->pdArray);
   free(oStack);
}

/*--------------------------------------------------------------------*/

int Stack_push(Stack_T oStack, double dItem)
{
   assert(oStack != NULL);
   if (oStack->uTop == oStack->uPhysLength)
      if (! Stack_grow(oStack))
         return 0;
   (oStack->pdArray)[oStack->uTop] = dItem;
   (oStack->uTop)++;
   return 1;
}

/*--------------------------------------------------------------------*/

double Stack_pop(Stack_T oStack)
{
   assert(oStack != NULL);
   assert(oStack->uTop > 0);
   (oStack->uTop)--;
   return (oStack->pdArray)[oStack->uTop];
}

/*--------------------------------------------------------------------*/

int Stack_isEmpty(Stack_T oStack)
{
   assert(oStack != NULL);
   return oStack->uTop == 0;
}
 
