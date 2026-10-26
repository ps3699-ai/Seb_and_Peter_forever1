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

/* Grow oSymTable to the next bucket count and move
   all bindings. If memory allocation fails, leave oSymTable
   unchanged. */

static void SymTable_expand(SymTable_T oSymTable)
{
   size_t uNewBucketCount;
   size_t u;
   size_t uNewIndex;
   struct Binding **ppsNewBuckets;
   struct Binding *psCurrent;
   struct Binding *psNext;
   assert(oSymTable != NULL);

   /* If at largest bucket do nothing */
   if (oSymTable->uNumBuckets >= auBucketCounts[uNumBucketCounts - 1])
   {
      return;
   }

   /* Find next larger bucket count */
   uNewBucketCount = auBucketCounts[0];
   for (u = 0; u < uNumBucketCounts; u++)
   {
      if (auBucketCounts[u] > oSymTable->uNumBuckets)
      {
         uNewBucketCount = auBucketCounts[u];
         break;
      }
   }

   /* Allocate for new buckets, if allocation fails
      leave oSymTable as is */
   ppsNewBuckets = calloc(uNewBucketCount,
      sizeof(struct Binding *));
   if (ppsNewBuckets == NULL)
   {
      return;
   }

   /* Move all bindings to the new bucket in new array */
   for (u = 0; u < oSymTable->uNumBuckets; u++)
   {
      psCurrent = oSymTable->ppsBuckets[u];
      while (psCurrent != NULL)
      {
         psNext = psCurrent->psNext;
         uNewIndex = SymTable_hash(psCurrent->pcKey, uNewBucketCount);
         psCurrent->psNext = ppsNewBuckets[uNewIndex];
         ppsNewBuckets[uNewIndex] = psCurrent;
         psCurrent = psNext;
      }
   }

   /* Free old buckets and update buckets pointer/bucket count */
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
   oSymTable->ppsBuckets = calloc(oSymTable->uNumBuckets, 
      sizeof(struct Binding *));
   if (oSymTable->ppsBuckets == NULL)
   {
      free(oSymTable);
      return NULL;
   }
   return oSymTable;
}

/*--------------------------------------------------------------------*/

/* Free memory allocated to oSymTable including keys, leave
   values' memory allocated */

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

   /* Free buckets and symtable themselves */
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
   Expand table if bindings exceeds bucket count,
   return 1 if function succeeds, 0 if not */

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
     it already exists */
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

   /* Insert binding to front of bucket */

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

/* Replace the value (pvValue) associated with a key (pcKey) in 
   oSymTable with a new value, if the key is not found return NULL,
   else return the old value */

void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue)
{
   size_t uBucket;
   struct Binding *psCurrent;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* Hash key to get bucket */
   uBucket = SymTable_hash(pcKey, oSymTable->uNumBuckets);
   /* Search bucket for matching key and replace value if found */
   for (psCurrent = oSymTable->ppsBuckets[uBucket];
      psCurrent != NULL; psCurrent = psCurrent->psNext)
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

/* Search oSymTable for a key (pcKey), return 1 if found, 
   0 if not */

int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
   size_t uBucket;
   struct Binding *psCurrent;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* Hash key to get bucket and search bucket for matching
      key */
   uBucket = SymTable_hash(pcKey, oSymTable->uNumBuckets);
   for (psCurrent = oSymTable->ppsBuckets[uBucket];
      psCurrent != NULL; psCurrent = psCurrent->psNext)
   {
      if (strcmp(psCurrent->pcKey, pcKey) == 0)
      {
         return 1;
      }
   }
   return 0;
}

/*--------------------------------------------------------------------*/

/* Search oSymTable for a key (pcKey), return bound value if found, 
   NULL if not */

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
   size_t uBucket;
   struct Binding *psCurrent;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* Hash key to get bucket and search bucket */
   uBucket = SymTable_hash(pcKey, oSymTable->uNumBuckets);
   for (psCurrent = oSymTable->ppsBuckets[uBucket];
      psCurrent != NULL; psCurrent = psCurrent->psNext)
   {
      if (strcmp(psCurrent->pcKey, pcKey) == 0)
      {
         return (void *)psCurrent->pvValue;
      }
   }
   /* If key is not found, return NULL */
   return NULL;
}

/*--------------------------------------------------------------------*/

/* Search oSymTable for a key (pcKey), remove it if found, return
   the removed value bound to pcKey, or NULL if not found */

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
   size_t uBucket;
   struct Binding *psCurrent;
   struct Binding *psPrev;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* Hash key to get bucket and search bucket for matching key,
      if binding is found, remove and free allocated memory */
   uBucket = SymTable_hash(pcKey, oSymTable->uNumBuckets);
   for (psCurrent = oSymTable->ppsBuckets[uBucket], psPrev = NULL;
      psCurrent != NULL; psPrev = psCurrent, 
      psCurrent = psCurrent->psNext)
   {
      if (strcmp(psCurrent->pcKey, pcKey) == 0)
      {
         void *pvValue = (void *)psCurrent->pvValue;
         /* Remove binding from bucket */
         if (psPrev == NULL)
         {
            oSymTable->ppsBuckets[uBucket] = psCurrent->psNext;
         }
         else
         {
            psPrev->psNext = psCurrent->psNext;
         }
         free(psCurrent->pcKey);
         free(psCurrent);
         oSymTable->uLength--;
         return pvValue;
      }
   }
   /* If key is not found, return NULL */
   return NULL;
}

/*--------------------------------------------------------------------*/

/* Apply the function *pfApply to each binding, using
   the key, value and pvExtra as args */

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra)
{
   size_t uBucket;
   struct Binding *psCurrent;
   assert(oSymTable != NULL);
   assert(pfApply != NULL);
   /* Iterate through every bucket and every binding in each bucket,
      calling pfApply with each */
   for (uBucket = 0; uBucket < oSymTable->uNumBuckets; uBucket++)
   {
      psCurrent = oSymTable->ppsBuckets[uBucket];
      while (psCurrent != NULL)
      {
         pfApply(psCurrent->pcKey, (void *)psCurrent->pvValue,
            (void *)pvExtra);
         psCurrent = psCurrent->psNext;
      }
   }
}
