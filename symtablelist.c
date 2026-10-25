/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Peter Strecansky and Sebastian Whiteley                    */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

/* Each binding is stored in a Binding. Bindings are linked to form
   a list. */

struct Binding
{
   /* The key of the binding, a defensive copy owned by the
      SymTable. */
   char *pcKey;

   /* The value of the binding, owned by the client. */
   const void *pvValue;

   /* The address of the next Binding in the list. */
   struct Binding *psNext;
};

/*--------------------------------------------------------------------*/

/* A SymTable is a structure that points to the first Binding and
   keeps track of how many bindings it contains. */

struct SymTable
{
   /* The address of the first Binding in the list. */
   struct Binding *psFirst;

   /* The number of bindings in the SymTable. */
   size_t uLength;
};

/*--------------------------------------------------------------------*/

/* Return a new SymTable_T object that contains no bindings, or NULL
   if insufficient memory is available. */

SymTable_T SymTable_new(void)
{
   /* allocate space for struct and check for null/allocation fail */
   SymTable_T oSymTable = malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;
   oSymTable->psFirst = NULL;
   oSymTable->uLength = 0;
   return oSymTable;
}

/*--------------------------------------------------------------------*/

/* Free all memory occupied by oSymTable, including its keys. The
   values of the bindings are not freed. */

void SymTable_free(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   /* Iterate through each key/node and free each before
      iterating to next, leave values themselves allocated */
   while (oSymTable->psFirst != NULL)
   {
      struct Binding *psCurrent = oSymTable->psFirst;
      oSymTable->psFirst = psCurrent->psNext;
      free(psCurrent->pcKey);
      free(psCurrent);
   }
   /* Free the symbol table */
   free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Return the number of bindings in oSymTable. */

size_t SymTable_getLength(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   return oSymTable->uLength;
}

/*--------------------------------------------------------------------*/

/* If oSymTable does not contain a binding with key pcKey, add a new
   binding consisting of a copy of pcKey and value pvValue to
   oSymTable and return 1. Otherwise, or if insufficient memory is
   available, leave oSymTable unchanged and return 0. */

int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue)
{
   struct Binding *psCurrent;
   struct Binding *psNewBinding;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   for (psCurrent = oSymTable->psFirst; psCurrent != NULL;
        psCurrent = psCurrent->psNext)
   {
      if (strcmp(psCurrent->pcKey, pcKey) == 0)
         return 0;
   }
   psNewBinding = malloc(sizeof(struct Binding));
   if (psNewBinding == NULL)
   {
      return 0;
   }
   psNewBinding->pcKey = malloc(strlen(pcKey) + 1);
   if (psNewBinding->pcKey == NULL)
   {
      free(psNewBinding);
      return 0;
   }
   /* copy key and store value, then update links to next and first
      and increment length */
   strcpy(psNewBinding->pcKey, pcKey);
   psNewBinding->pvValue = pvValue;
   psNewBinding->psNext = oSymTable->psFirst;
   oSymTable->psFirst = psNewBinding;
   oSymTable->uLength++;
   return 1;
}

/*--------------------------------------------------------------------*/

/* If oSymTable contains a binding with key pcKey, replace its value
   with pvValue and return the old value. Otherwise leave oSymTable
   unchanged and return NULL. */

void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue)
{
   struct Binding *psCurrent;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   for (psCurrent = oSymTable->psFirst; psCurrent != NULL; 
      psCurrent = psCurrent->psNext)
   {
      if (strcmp(psCurrent->pcKey, pcKey) == 0)
      {
         void *pvOldValue = (void *)psCurrent->pvValue;
         psCurrent->pvValue = pvValue;
         return pvOldValue;
      }
   }

   return NULL;
}

/*--------------------------------------------------------------------*/

/* Return 1 if oSymTable contains a binding with key pcKey, and 0
   otherwise. */

int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
   struct Binding *psCurrent;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* Traverse the list and search for a node with the given key. */
   for (psCurrent = oSymTable->psFirst; psCurrent != NULL;
      psCurrent = psCurrent->psNext)
   {
      if (strcmp(psCurrent->pcKey, pcKey) == 0)
      {
         return 1;
      }
   }
   /* The key was not found in the list. */
   return 0;
}

/*--------------------------------------------------------------------*/

/* Return the value of the binding in oSymTable whose key is pcKey,
   or NULL if no such binding exists. */

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
   struct Binding *psCurrent;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* Traverse the list and search for a node with the given key. */
   for (psCurrent = oSymTable->psFirst; psCurrent != NULL;
      psCurrent = psCurrent->psNext)
   {
      if (strcmp(psCurrent->pcKey, pcKey) == 0)
      {
         return (void *)psCurrent->pvValue;
      }
   }
   /* The key was not found in the list. */
   return NULL;
}

/*--------------------------------------------------------------------*/

/* If oSymTable contains a binding with key pcKey, remove that binding
   from oSymTable and return its value. Otherwise leave oSymTable
   unchanged and return NULL. */

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
   struct Binding *psCurrent;
   struct Binding *psPrev;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   psCurrent = oSymTable->psFirst;
   psPrev = NULL;

   /* Search for the node with the requested key. */
   while (psCurrent != NULL)
   {
      if (strcmp(psCurrent->pcKey, pcKey) == 0)
      {
         void *pvValue = (void *)psCurrent->pvValue;

         /* Unlink the node, handling the head separately. */
         if (psPrev == NULL)
         {
            oSymTable->psFirst = psCurrent->psNext;
         }
         else
         {
            psPrev->psNext = psCurrent->psNext;
         }
         /* Free the node and its key, but not the value. */
         free(psCurrent->pcKey);
         free(psCurrent);
         oSymTable->uLength--;
         return pvValue;
      }
      /* Move to the next node, keeping track of its predecessor. */
      psPrev = psCurrent;
      psCurrent = psCurrent->psNext;
   }

   /* The requested key was not found. */
   return NULL;
}

/*--------------------------------------------------------------------*/

/* Apply function *pfApply to each binding in oSymTable, passing the
   binding's key and value and pvExtra as arguments. */

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra)
{
   struct Binding *psCurrent;
   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   /* calls pfApply on each binding */
   for (psCurrent = oSymTable->psFirst; psCurrent != NULL;
      psCurrent = psCurrent->psNext)
   {
      pfApply(psCurrent->pcKey, (void *)psCurrent->pvValue,
      (void *)pvExtra);
   }
}