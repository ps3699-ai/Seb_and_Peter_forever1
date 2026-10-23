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

SymTable_T SymTable_new(void)
{
   /* allocate space for struct and check for null/allocation fail*/
   SymTable_T oSymTable = malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;
   oSymTable->psFirst = NULL;
   oSymTable->uLength = 0;
   return oSymTable;
}

/*--------------------------------------------------------------------*/

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

size_t SymTable_getLength(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   return oSymTable->uLength;
}

/*--------------------------------------------------------------------*/

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

void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO */
   return NULL;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO */
   return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO */
   return NULL;
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO: jeden prechod, pamätaj si predchádzajúci uzol.
            Pozor na prípad, keď je to prvý uzol. */
   return NULL;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra)
{
   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   /* TODO: zavolaj pfApply na každý binding */
}