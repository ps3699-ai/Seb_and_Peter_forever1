/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Peter Strecansky and Sebastian Whiteley                    */
/*--------------------------------------------------------------------*/

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <stddef.h>

/* A SymTable_T object is an unordered collection of bindings. Each
   binding consists of a key, which is a string that uniquely
   identifies the binding, and a value, which is a pointer to data
   of any type. */

typedef struct SymTable *SymTable_T;

/*--------------------------------------------------------------------*/

/* Return a new SymTable_T object that contains no bindings, or NULL
   if insufficient memory is available. */

SymTable_T SymTable_new(void);

/*--------------------------------------------------------------------*/

/* Free all the memory associated with oSymTable, including all
   keys, and the table itself. Values are not freed as they are
   owned by the client */

void SymTable_free(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

/* Return the number of bindings in oSymTable. */

size_t SymTable_getLength(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

/* If oSymTable does not contain a binding with key pcKey, add a new
   binding consisting of key pcKey and value pvValue to oSymTable and
   return 1 (TRUE). Otherwise if pcKey already exists, leave oSymTable
   unchanged and return 0 (FALSE). If insufficient memory is available,
   leave oSymTable unchanged and return 0 (FALSE). */

int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue);

/*--------------------------------------------------------------------*/

/* If oSymTable has a binding with key pcKey, replace its value
   with pvValue and return a pointer to the old value. Otherwise,
   leave oSymTable unchanged and return NULL. */

void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue);

/*--------------------------------------------------------------------*/

/* Return 1 (TRUE) if a binding in oSymTable contains the given key,
   pcKey, 0 (FALSE) otherwise. */

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

/* Return a pointer to the value associated with the given key, pcKey,
   in oSymTable or NULL if the key is not found. */

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

/* Remove the binding with the given key, pcKey, if it exists.
   Return a pointer to the value associated with the key, or if the
   key is not found, return NULL and leave oSymTable unchanged. */

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

/* Apply the given function, *pfApply, to each binding in oSymTable
   by calling (*pfApply)(pcKey, pvValue, pvExtra) for each binding. */

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra);

#endif
