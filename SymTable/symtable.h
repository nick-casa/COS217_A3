/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Nickolas Casalinuovo                                       */
/*--------------------------------------------------------------------*/

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

/*--------------------------------------------------------------------*/
/* A Stack_T object is a last-in-first-out collection of items. */
/*--------------------------------------------------------------------*/
typedef struct Stack *SymTable_T;

/*--------------------------------------------------------------------*/
/*   SymTable_new returns a new SymTable object                       */
/*   that contains no bindings or NULL if                             */
/*   insufficient memory is available                                 */
/*--------------------------------------------------------------------*/
SymTable_T SymTable_new(void);

/*--------------------------------------------------------------------*/
/*   If oSymTable does not contain a binding with key pcKey, then     */
/*   SymTable_put must add a new binding to oSymTable consisting      */ 
/*   of key pcKey and value pvValue and return 1 (TRUE).              */
/*   Otherwise the function must leave oSymTable unchanged            */
/*   and return 0 (FALSE). If insufficient memory is available,       */    
/*   then the function must leave oSymTable unchanged and             */
/*   return 0 (FALSE).                                                */
/*--------------------------------------------------------------------*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);


void SymTable_free(SymTable_T oSymTable);

size_t SymTable_getLength(SymTable_T oSymTable);


void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue);

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);


void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), 
    const void *pvExtra);
/*
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);
*/
#endif