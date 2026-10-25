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
   /* TODO: same 3 fields as in symtablelist.c */
};

/*--------------------------------------------------------------------*/

/* A SymTable is a hash table: an array of buckets, each of which is
   a linked list of Bindings. */

struct SymTable
{
   /* TODO: comment + array of buckets (what type, if its size
      changes at run time?) */

   /* TODO: comment + current number of buckets */

   /* TODO: comment + number of bindings (getLength in O(1)) */
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
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

/*--------------------------------------------------------------------*/

/* TODO: comment. Grow oSymTable to the next bucket count and move
   all bindings. If memory allocation fails, leave oSymTable
   unchanged. */

static void SymTable_expand(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   /* TODO: 1) already at the max (65521)? -> do nothing
            2) allocate the new bucket array (calloc), return on NULL
            3) walk every old bucket and every node:
               rehash with the NEW count, relink the node into the
               new bucket (no malloc, no key copy!)
            4) free the old array, store the new array and count */
}

/*--------------------------------------------------------------------*/

/* TODO: comment */

SymTable_T SymTable_new(void)
{
   /* TODO: allocate struct SymTable, then an array of 509 buckets
      (calloc -> all NULL). If the second allocation fails, free
      the first! */
   return NULL;
}

/*--------------------------------------------------------------------*/

/* TODO: comment */

void SymTable_free(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   /* TODO: for each bucket free its list (keys + nodes), then the
      bucket array, then the table itself */
}

/*--------------------------------------------------------------------*/

/* TODO: comment */

size_t SymTable_getLength(SymTable_T oSymTable)
{
   assert(oSymTable != NULL);
   /* TODO */
   return 0;
}

/*--------------------------------------------------------------------*/

/* TODO: comment */

int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue)
{
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   /* TODO: 1) hash ONCE -> bucket index
            2) walk ONLY that bucket; if the key exists -> return 0
            3) allocate node + key copy (clean up on failure)
            4) insert at the front of the bucket, increment count
            5) if count > bucket count -> SymTable_expand
               (one short if, so it can be commented out for timing)
            6) return 1 */
   return 0;
}

/*--------------------------------------------------------------------*/

/* TODO: comment */

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
