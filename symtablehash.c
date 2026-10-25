/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Peter Strecansky and Sebastian Whiteley                    */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

/* The sequence of bucket counts used as the table expands. */
static const size_t auBucketCounts[] =
   {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};

/* The number of elements in auBucketCounts. */
static const size_t uNumBucketCounts =
   sizeof(auBucketCounts) / sizeof(auBucketCounts[0]);

/*--------------------------------------------------------------------*/

/* Each binding is stored in a Binding. Bindings that hash to the
   same bucket are linked to form a list. */

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

/* A SymTable is a hash table: an array of buckets, each of which is
   a linked list of Bindings. */

struct SymTable
{
   /* An array of pointers to Binding nodes. */
   struct Binding **ppsBuckets;

   /* The current number of buckets in the array. */

   size_t uNumBuckets;

   /* The number of bindings in the table. */
   size_t uLength;
};

/*--------------------------------------------------------------------*/

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;
   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
   {
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
   }
   return uHash % uBucketCount;
}

/*--------------------------------------------------------------------*/

/* TODO: comment. Grow oSymTable to the next bucket count and move
   all bindings. If memory allocation fails, leave oSymTable
   unchanged. */

static void SymTable_expand(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   /* If the current number of buckets is already the largest in
      auBucketCounts, do nothing. Otherwise, find the next larger
      bucket count, allocate a new array of that size, and rehash all
      bindings into the new array. If any allocation fails, free any
      memory allocated for the new array and leave oSymTable
      unchanged. */
   if (oSymTable->uNumBuckets >= auBucketCounts[uNumBucketCounts - 1])
   {
      return;
   }
   size_t uNewBucketCount = auBucketCounts[0];
   for (size_t i = 0; i < uNumBucketCounts; i++)
   {
      if (auBucketCounts[i] > oSymTable->uNumBuckets)
      {
         uNewBucketCount = auBucketCounts[i];
         break;
      }
   }
   struct Binding **ppsNewBuckets = calloc(uNewBucketCount, sizeof(struct Binding *));
   if (ppsNewBuckets == NULL)
   {
      return;
   }
   for (size_t i = 0; i < oSymTable->uNumBuckets; i++)
   {
      struct Binding *psCurrent = oSymTable->ppsBuckets[i];
      while (psCurrent != NULL)
      {
         struct Binding *psNext = psCurrent->psNext;
         size_t uNewIndex = SymTable_hash(psCurrent->pcKey, uNewBucketCount);
         psCurrent->psNext = ppsNewBuckets[uNewIndex];
         ppsNewBuckets[uNewIndex] = psCurrent;
         psCurrent = psNext;
      }
   }
   free(oSymTable->ppsBuckets);
   oSymTable->ppsBuckets = ppsNewBuckets;
   oSymTable->uNumBuckets = uNewBucketCount;
}

/*--------------------------------------------------------------------*/

/* Return a new SymTable_T object that contains no bindings, or NULL
   if insufficient memory is available. */

SymTable_T SymTable_new(void)
{
   struct SymTable *oSymTable = malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
   {
      return NULL;
   }
   oSymTable->uNumBuckets = auBucketCounts[0];
   oSymTable->uLength = 0;
   oSymTable->ppsBuckets = calloc(oSymTable->uNumBuckets, sizeof(struct Binding *));
   if (oSymTable->ppsBuckets == NULL)
   {
      free(oSymTable);
      return NULL;  
   }
}

/*--------------------------------------------------------------------*/

/* Free memory allocated to oSymTable including keys, leave
value's memory allocated */

void SymTable_free(SymTable_T oSymTable)
{
   size_t uBucket;
   struct Binding *psCurrent;
   struct Binding *psNext;
   assert(oSymTable != NULL);
   /* Free each binding key/node from each bucket, 
   leave the values allocated */
   for (uBucket = 0; uBucket < oSymTable->uNumBuckets; uBucket++)
   {
      psCurrent = oSymTable->ppsBuckets[uBucket];
      while (psCurrent != NULL)
      {
         psNext = psCurrent->psNext;
         free(psCurrent->pcKey);
         free(psCurrent);
         psCurrent = psNext;
      }
   }
   /* free buckets and symtable themselves*/
   free(oSymTable->ppsBuckets);
   free(oSymTable);
}

/*--------------------------------------------------------------------*/

/* Return the number of bindings in SymTable */

size_t SymTable_getLength(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   return oSymTable->uLength;
}

/*--------------------------------------------------------------------*/

/* Add a new binding to oSymTable by checking first
if it already exists, before adding a binding
with a copy of the key and value to the table. 
Expand table if bindings exceeds bucket count */

int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue)
{
   size_t uBucket;
   struct Binding *psNewBinding;
   struct Binding *psCurrent;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* Hash the key to get bucket index */
   uBucket = SymTable_hash(pcKey, oSymTable->uNumBuckets);
   /* Search through bucket for matching key, return 0 if 
   it already exists*/
   for (psCurrent = oSymTable->ppsBuckets[uBucket];
      psCurrent != NULL; psCurrent = psCurrent->psNext)
   {
      if (strcmp(psCurrent->pcKey, pcKey) == 0)
      {
         return 0;
      }
   }
   /* Allocate memory for new binding, check for failure*/
   psNewBinding = malloc(sizeof(struct Binding));
   if (psNewBinding == NULL)
   {
      return 0;
   }
   /* Defensive copy, check for failure and free
   if so */
   psNewBinding->pcKey = malloc(strlen(pcKey) + 1);
   if (psNewBinding->pcKey == NULL)
   {
      free(psNewBinding);
      return 0;
   }
   strcpy(psNewBinding->pcKey, pcKey);
   psNewBinding->pvValue = pvValue;

   /* Insert binding to front of bucket*/

   psNewBinding->psNext = oSymTable->ppsBuckets[uBucket];
   oSymTable->ppsBuckets[uBucket] = psNewBinding;
   oSymTable->uLength++;

   /* Expand if binding number > bucket count */
   if (oSymTable->uLength > oSymTable->uNumBuckets)
   {
      SymTable_expand(oSymTable);
   }

   return 1;
}

/*--------------------------------------------------------------------*/

/* Replace the value associated with a key in oSymTable */

void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO: hash, search in one bucket */
   return NULL;
}

/*--------------------------------------------------------------------*/

/* TODO: comment */

int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO */
   return 0;
}

/*--------------------------------------------------------------------*/

/* TODO: comment */

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO */
   return NULL;
}

/*--------------------------------------------------------------------*/

/* TODO: comment */

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO: like the list version, but the "first node" is the head
      of the bucket, not of the whole table */
   return NULL;
}

/*--------------------------------------------------------------------*/

/* TODO: comment */

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra)
{
   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   /* TODO: two nested loops: over buckets, then over nodes in each
      bucket */
}
