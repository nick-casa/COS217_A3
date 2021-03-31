/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Nickolas Casalinuovo                                       */
/*--------------------------------------------------------------------*/

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED
#include <stddef.h>

/*--------------------------------------------------------------------*/
/* A Stack_T object is a last-in-first-out collection of items. */
/*--------------------------------------------------------------------*/
typedef struct SymTable *SymTable_T;

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
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
    const void *pvValue);


/*--------------------------------------------------------------------*/
/* SymTable_free takes a SymTable_T (oSymTable) as an argument and    */
/* clears all associated memory in the heap.                          */
/*--------------------------------------------------------------------*/
void SymTable_free(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/
/* SymTable_getLength takes a SymTable_T (oSymTable) as an argument   */
/* and returns the amount of bindings within the symbol table.        */
/*--------------------------------------------------------------------*/
size_t SymTable_getLength(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/
/* Symtable_replace takes a Symtable_T (oSymTable) and seaches for a  */
/* pcKey. If the pcKey is found, set its value to pvValue and return  */
/* old pvValue. If pcKey is not found, return NULL.                   */
/*--------------------------------------------------------------------*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
 const void *pvValue);

/*--------------------------------------------------------------------*/
/* SymTable_contains takes a Symtable_T (oSymTable) and seaches for a */
/* pcKey. If the pcKey is found, return 1. If pcKey is not found,     */
/* return NULL.                                                       */
/*--------------------------------------------------------------------*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/
/* SymTable_contains takes a Symtable_T (oSymTable) and seaches for a */
/* pcKey. If the pcKey is found, return its pvValue. If pcKey is not  */
/* found, return NULL.                                                */
/*--------------------------------------------------------------------*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/
/* SymTable_contains takes a Symtable_T (oSymTable) and seaches for a */
/* pcKey. If the pcKey is found, remove pcKey and return its pvValue. */
/* If pcKey is not found, return NULL.                                */
/*--------------------------------------------------------------------*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/
/* SymTable_map takes a Symtable_T (oSymTable) and applies a function,*/
/* pfApply, to every element. pfApply takes pcKey, pvValue, and an    */
/* additional keyword, pvExtra                                        */
/*--------------------------------------------------------------------*/
void SymTable_map(SymTable_T oSymTable, 
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), 
    const void *pvExtra);

#endif