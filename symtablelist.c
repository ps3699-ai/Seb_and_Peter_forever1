/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Peter Strecansky and Seb                                   */
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
   /* TODO: alokuj, skontroluj NULL, inicializuj polia */
   return NULL;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   /* TODO: uvoľni každý kľúč a uzol, potom tabuľku.
      Hodnoty NEuvoľňuj. */
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   /* TODO */
   return 0;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO: 1) existuje už pcKey? -> return 0
            2) alokuj uzol, alokuj kópiu kľúča (pri zlyhaní uprať!)
            3) pripoj uzol, zvýš počet, return 1 */
   return 0;
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