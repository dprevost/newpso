/*
 * Copyright (C) 2006-2010 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Hash.h"
#include "Nucleus/hash_fnv.h"
#include "Nucleus/SessionContext.h"
#include "Nucleus/MemoryObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The array length (for the hash) will double or shrink by a factor two
 * using precomputed lengths (all the lengths are prime numbers - using
 * prime numbers help reduces the risk of collision.
 *
 * To make it simple, we use the first prime number less than 2^n (except 
 * for 29 to make the jump from 13 to 61 smoother).
 *
 * The list of prime numbers was obtained from http://primes.utm.edu/,
 * specifically the page http://primes.utm.edu/lists/2small/0bit.html
 * (which list the first 10 prime numbers less than 2^n for n up to 400).
 */
#if SIZEOF_VOID_P == 4
size_t g_psonArrayLengths[PSON_PRIME_NUMBER_ARRAY_LENGTH] = 
{
   13,     
   29,
   61,
   127,
   251,
   509,
   1021,
   2039,
   4093,
   8191,
   0x00004000 - 3,  /* 2^14 - 3  */
   0x00008000 - 19, /* 2^15 - 19 */
   0x00010000 - 15, /* etc...    */
   0x00020000 - 1,
   0x00040000 - 5,
   0x00080000 - 1,
   0x00100000 - 3,
   0x00200000 - 9,
   0x00400000 - 3,
   0x00800000 - 15,
   0x01000000 - 3,
   0x02000000 - 39,
   0x04000000 - 5,
   0x08000000 - 39,
   0x10000000 - 57,
   0x20000000 - 3,
   0x40000000 - 35,
   0x80000000 - 1
};
#else
size_t g_psonArrayLengths[PSON_PRIME_NUMBER_ARRAY_LENGTH] = 
{
   13,     
   29,
   61,
   127,
   251,
   509,
   1021,
   2039,
   4093,
   8191,
   0x0000000000004000 - 3,  /* 2^14 - 3 */
   0x0000000000008000 - 19,
   0x0000000000010000 - 15,
   0x0000000000020000 - 1,
   0x0000000000040000 - 5,
   0x0000000000080000 - 1,
   0x0000000000100000 - 3,
   0x0000000000200000 - 9,
   0x0000000000400000 - 3,
   0x0000000000800000 - 15,
   0x0000000001000000 - 3,
   0x0000000002000000 - 39,
   0x0000000004000000 - 5,
   0x0000000008000000 - 39,
   0x0000000010000000 - 57,
   0x0000000020000000 - 3,
   0x0000000040000000 - 35,
   0x0000000080000000 - 1,
   0x0000000100000000 - 5,
   0x0000000200000000 - 9,
   0x0000000400000000 - 41,
   0x0000000800000000 - 31,
   0x0000001000000000 - 5,
   0x0000002000000000 - 25,
   0x0000004000000000 - 45,
   0x0000008000000000 - 7,
   0x0000010000000000 - 87,
   0x0000020000000000 - 21,
   0x0000040000000000 - 11,
   0x0000080000000000 - 57,
   0x0000100000000000 - 17,
   0x0000200000000000 - 55,
   0x0000400000000000 - 21,
   0x0000800000000000 - 115,
   0x0001000000000000 - 59,
   0x0002000000000000 - 81,
   0x0004000000000000 - 27,
   0x0008000000000000 - 129,
   0x0010000000000000 - 47,
   0x0020000000000000 - 111,
   0x0040000000000000 - 33,
   0x0080000000000000 - 55,
   0x0100000000000000 - 5,
   0x0200000000000000 - 13,
   0x0400000000000000 - 27,
   0x0800000000000000 - 55,
   0x1000000000000000 - 93,
   0x2000000000000000 - 1,
   0x4000000000000000 - 57,
   0x8000000000000000 - 25,

};

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define HASH_ARRAY_MIN_SIZE 13   /* a prime number */

/** In % */
static unsigned int g_maxLoadFactor = 300;
static unsigned int g_minLoadFactor = 100;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- 
 *
 * Static inline functions are first.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
size_t calculateItemLength( uint32_t keyLength,
                            uint32_t dataLength )
{
   size_t len;
   
   len = offsetof(psonHashItem, key) + keyLength;
   len = ((len-1)/PSOC_ALIGNMENT_STRUCT + 1)*PSOC_ALIGNMENT_STRUCT;
   
   len += dataLength;
   
   return len;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Just in case a new function is used instead, one of these days */
static inline u_long
hash_it (const unsigned char * str, uint32_t len)
{
   return fnv_buf( (void *)str, len, FNV1_INIT );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
psonHashResizeEnum isItTimeToResize( psonHash * pHash )
{
   unsigned int loadFactor = 100 * pHash->numberOfItems / 
      g_psonArrayLengths[pHash->lengthIndex];

  if ( loadFactor >= g_maxLoadFactor ) return PSON_HASH_TIME_TO_GROW;
  if ( pHash->lengthIndex > pHash->lengthIndexMinimum ) {
     if ( loadFactor <= g_minLoadFactor ) return PSON_HASH_TIME_TO_SHRINK;
  }
  
  return PSON_HASH_NO_RESIZE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- 
 *
 * Static non-inline functions.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static bool findKey( psonHash            * pHash,
                     ptrdiff_t           * pArray,
                     const unsigned char * pKey,
                     uint32_t              keyLength,
                     psonHashItem       ** ppItem,
                     psonHashItem       ** ppPreviousItem,
                     size_t              * pBucket,
                     psonSessionContext  * pContext )
{
   ptrdiff_t currentOffset, nextOffset;
   psonHashItem* pItem;

   PSO_TRACE_ENTER_NUCLEUS( pContext );

   *pBucket = hash_it( pKey, keyLength ) % g_psonArrayLengths[pHash->lengthIndex];
   currentOffset = pArray[*pBucket];
   
   *ppPreviousItem = NULL;

   while ( currentOffset != PSON_NULL_OFFSET ) {
      GET_PTR(pContext->pBaseAddress, pItem, currentOffset, psonHashItem );
      nextOffset = pItem->nextItem;
     
      if ( keyLength == pItem->keyLength ) {
         if ( memcmp( pKey, pItem->key, keyLength ) == 0 ) {
            *ppItem = pItem;
            PSO_TRACE_EXIT_NUCLEUS( pContext, true );
            return true;
         }
      }

      /* Move to the next item in our bucket */
      currentOffset = nextOffset;
      *ppPreviousItem = pItem;
   }
   
   /* Nothing was found, return false */
   *ppItem = NULL;
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- 
 *
 * Functions declared in Hash.h (alphabetic order).
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoErrors psonHashCopy( psonHash           * pOldHash,
                             psonHash           * pNewHash,
                             psonSessionContext * pContext )
{
   ptrdiff_t * pOldArray, * pNewArray;
   size_t i;
   ptrdiff_t currentOffset, tempOffset, newBucket;
   psonHashItem * pOldItem, * pNewItem, * previousItem, * tempItem;
   psonMemObject * pMemObject;
   size_t itemLength;
   
   PSO_PRE_CONDITION( pOldHash != NULL );
   PSO_PRE_CONDITION( pNewHash != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_INV_CONDITION( pOldHash->initialized == PSON_HASH_SIGNATURE );
   PSO_INV_CONDITION( pNewHash->initialized == PSON_HASH_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   GET_PTR(pContext->pBaseAddress, pOldArray, pOldHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pOldArray != NULL );
   GET_PTR(pContext->pBaseAddress, pNewArray, pNewHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pNewArray != NULL );

   GET_PTR(pContext->pBaseAddress, pMemObject, pNewHash->memObjOffset, psonMemObject );

   if ( pOldHash->lengthIndex == pNewHash->lengthIndex ) {
      /* Much simpler path... hashing is done for us */
      for ( i = 0; i < g_psonArrayLengths[pOldHash->lengthIndex]; ++i ) {

         currentOffset = pOldArray[i];
         previousItem = NULL;
         while ( currentOffset != PSON_NULL_OFFSET ) {
            GET_PTR(pContext->pBaseAddress, pOldItem, currentOffset, psonHashItem );

            itemLength = calculateItemLength( pOldItem->keyLength,
                                              pOldItem->dataLength );
            pNewItem = (psonHashItem*) psonMalloc( pMemObject, 
                                                   itemLength, 
                                                   pContext );
            if ( pNewItem == NULL ) {
               PSO_TRACE_EXIT_NUCLEUS( pContext, false );
               return PSO_NOT_ENOUGH_PSO_MEMORY;
            }
            
            /*
             * We copy everything and we reset the offset of interest
             * to proper values (the nextSameKey offset should always be 
             * PSON_NULL_OFFSET since the oldHash is for a read-only object).
             */
            memcpy( pNewItem, pOldItem, itemLength );
            pNewItem->dataOffset = SET_OFFSET(pContext->pBaseAddress, pNewItem) + itemLength - 
                                   pOldItem->dataLength;
            /* Set the chain */
            if ( previousItem == NULL ) {
               pNewArray[i] = SET_OFFSET(pContext->pBaseAddress, pNewItem);
            }
            else {
               previousItem->nextItem = SET_OFFSET(pContext->pBaseAddress, pNewItem);
            }
            previousItem = pNewItem;
            
            currentOffset = pOldItem->nextItem;
         }
      }
   }
   else {
      for ( i = 0; i < g_psonArrayLengths[pOldHash->lengthIndex]; ++i ) {
         currentOffset = pOldArray[i];
      
         while ( currentOffset != PSON_NULL_OFFSET ) {
            GET_PTR(pContext->pBaseAddress, pOldItem, currentOffset, psonHashItem );

            itemLength = calculateItemLength( pOldItem->keyLength,
                                              pOldItem->dataLength );
            pNewItem = (psonHashItem*) psonMalloc( pMemObject, 
                                                   itemLength, 
                                                   pContext );
            if ( pNewItem == NULL ) {
               PSO_TRACE_EXIT_NUCLEUS( pContext, false );
               return PSO_NOT_ENOUGH_PSO_MEMORY;
            }
            
            /*
             * We copy everything and we reset the offset of interest
             * to proper values (the nextSameKey offset should always be 
             * PSON_NULL_OFFSET since the oldHash is for a read-only object).
             */
            memcpy( pNewItem, pOldItem, itemLength );
            pNewItem->dataOffset = SET_OFFSET(pContext->pBaseAddress, pNewItem) + itemLength - 
                                   pOldItem->dataLength;

            /* Set the chain */
            newBucket = hash_it( pNewItem->key, pNewItem->keyLength ) % 
                        g_psonArrayLengths[pNewHash->lengthIndex];
            pNewItem->bucket = newBucket;
            if ( pNewArray[newBucket] == PSON_NULL_OFFSET ) {
               pNewArray[newBucket] = SET_OFFSET(pContext->pBaseAddress, pNewItem);
            }
            else {
               tempOffset = pNewArray[newBucket];
               GET_PTR(pContext->pBaseAddress, tempItem, tempOffset, psonHashItem );
               while ( tempItem->nextItem != PSON_NULL_OFFSET ) {
                  tempOffset = tempItem->nextItem;
                  GET_PTR(pContext->pBaseAddress, tempItem, tempOffset, psonHashItem );
               }
               tempItem->nextItem = SET_OFFSET(pContext->pBaseAddress, pNewItem);
            }
            
            /* Move to the next item in our bucket */
            currentOffset = pOldItem->nextItem;
         }
      }
   }

   pNewHash->totalDataSizeInBytes = pOldHash->totalDataSizeInBytes;
   pNewHash->numberOfItems        = pOldHash->numberOfItems;
   pNewHash->enumResize           = pOldHash->enumResize;

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Note: there is no "same key" here since this function is used for doing
 * changes to a temp. copy of read-only objects - no transaction.
 */
bool psonHashDelWithKey( psonHash            * pHash,
                         const unsigned char * pKey, 
                         uint32_t              keyLength,
                         psonSessionContext  * pContext  )
{
   size_t bucket = 0;
   ptrdiff_t * pArray;
   bool keyFound = false;
   psonHashItem * previousItem = NULL, * pItem = NULL;
   ptrdiff_t nextOffset;
   psonMemObject * pMemObject;
  
   PSO_PRE_CONDITION( pHash     != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pKey      != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   GET_PTR(pContext->pBaseAddress, pMemObject, pHash->memObjOffset, psonMemObject );
   
   keyFound = findKey( pHash, 
                       pArray, 
                       pKey, 
                       keyLength, 
                       &pItem, 
                       &previousItem, 
                       &bucket,
                       pContext );
   
   if ( keyFound ) {
      nextOffset  = pItem->nextItem;
      
      pHash->totalDataSizeInBytes -= pItem->dataLength;
      psonFree( pMemObject, 
                (unsigned char*)pItem, 
                calculateItemLength(pItem->keyLength,pItem->dataLength),
                pContext );
                
      if ( previousItem == NULL ) {
         pArray[bucket] = nextOffset;
      }
      else {
         previousItem->nextItem = nextOffset;
      }
      
      pHash->numberOfItems--;
      pHash->enumResize = isItTimeToResize( pHash );

      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return true;
   }

   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonHashDump( psonHash           * pHash,
                   int                  indent,
                   psonSessionContext * pContext )
{
   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonHash (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pHash, SET_OFFSET(pContext->pBaseAddress, pHash) );
   if ( pHash == NULL ) return;
   
   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Offset of the memory object we belong to: "
      PSO_PTRDIFF_T_FORMAT"\n", pHash->memObjOffset );
   
   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Offset to our array "PSO_PTRDIFF_T_FORMAT"\n",
      pHash->arrayOffset ); 
   
   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Number of items: "PSO_SIZE_T_FORMAT"\n", 
      pHash->numberOfItems );
   
   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Total amount of bytes of data: "PSO_SIZE_T_FORMAT"\n",
      pHash->totalDataSizeInBytes );

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Index into the array of lengths : %d\n", 
      pHash->lengthIndex );

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Mimimum index value: %d\n", pHash->lengthIndexMinimum );

   switch( pHash->enumResize ) {
   case PSON_HASH_NO_RESIZE:
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "No need to resize the array\n" );
      break;
      
   case PSON_HASH_TIME_TO_GROW:
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "The array needs to grow\n" );
      break;
      
   case PSON_HASH_TIME_TO_SHRINK:
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "The array needs to shrink\n" );
      break;
      
   default:
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "Unknown value for enumResize - value: %d\n",
         pHash->enumResize );
      break;
   }
   
   if ( pHash->initialized == PSON_HASH_SIGNATURE ) {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "Signature is ok.\n" );
   }
   else {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "Signature is wrong - value is %d, exepted: %d\n",
         pHash->initialized, PSON_HASH_SIGNATURE );
   }

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonHash END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonHashEmpty( psonHash           * pHash,
                    psonSessionContext * pContext )
{
   ptrdiff_t* pArray, currentOffset, nextOffset;
   size_t i;
   psonHashItem* pItem;
   psonMemObject * pMemObject;
   
   PSO_PRE_CONDITION( pHash != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   GET_PTR(pContext->pBaseAddress, pMemObject, pHash->memObjOffset, psonMemObject );

   for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      while ( currentOffset != PSON_NULL_OFFSET ) {
         GET_PTR(pContext->pBaseAddress, pItem, currentOffset, psonHashItem );
         nextOffset = pItem->nextItem;
         
         psonFree( pMemObject, 
                   (unsigned char*) pItem, 
                   calculateItemLength(pItem->keyLength,pItem->dataLength), 
                   pContext );
         
         /* Move to the next item in our bucket */
         currentOffset = nextOffset;
      }
      
      pArray[i] = PSON_NULL_OFFSET;
   }
   
   pHash->numberOfItems = 0;
   pHash->totalDataSizeInBytes = 0;
   pHash->enumResize = isItTimeToResize( pHash );

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonHashFini( psonHash           * pHash,
                   psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHash != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   pHash->initialized = 0;

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashGet( psonHash            * pHash,
                  const unsigned char * pKey,
                  uint32_t              keyLength,
                  psonHashItem       ** ppItem,
                  size_t              * pBucket,
                  psonSessionContext  * pContext )
{
   bool keyFound = false;
   ptrdiff_t* pArray;
   psonHashItem* pItem, *dummy;
   
   PSO_PRE_CONDITION( pHash    != NULL );
   PSO_PRE_CONDITION( pKey     != NULL );
   PSO_PRE_CONDITION( ppItem   != NULL );
   PSO_PRE_CONDITION( pBucket  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength, 
                       &pItem, &dummy, pBucket, pContext );

   if ( keyFound ) *ppItem = pItem;

   PSO_TRACE_EXIT_NUCLEUS( pContext, keyFound );
   return keyFound;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(_WIN32)
/* unreachable code */
//#  pragma warning(disable:4702) 
#endif

bool psonHashGetFirst( psonHash            * pHash,
                       psonHashItem       ** ppItem,
                       psonSessionContext  * pContext )
{
   ptrdiff_t * pArray, currentOffset;
   bool SHOULD_NOT_REACHED_THIS = true;
   size_t i;
   
   PSO_PRE_CONDITION( pHash != NULL );
   PSO_PRE_CONDITION( ppItem != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_SIGNATURE );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   if ( pHash->numberOfItems == 0 ) {
      return false;
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   }
   
   /* 
    * Note: the first item has to be the first non-empty pArray[i],
    * this makes the search easier.
    */
   for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      if (currentOffset != PSON_NULL_OFFSET ) {
         GET_PTR(pContext->pBaseAddress, *ppItem, currentOffset, psonHashItem );
         PSO_TRACE_EXIT_NUCLEUS( pContext, true );
         return true;
      }
   }

   PSO_POST_CONDITION( SHOULD_NOT_REACHED_THIS == false );
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false; /* Should never occur */
}

#if defined (_WIN32)
//#  pragma warning(default:4702) /* unreachable code */
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashGetNext( psonHash           * pHash,
                      psonHashItem       * previousItem,
                      psonHashItem      ** nextItem,
                      psonSessionContext * pContext )
{
   ptrdiff_t* pArray, currentOffset;
   size_t i;
   
   PSO_PRE_CONDITION( pHash        != NULL );
   PSO_PRE_CONDITION( previousItem != NULL );
   PSO_PRE_CONDITION( nextItem     != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   if ( previousItem->nextItem != PSON_NULL_OFFSET ) {
      /* We found the next one in the linked list. */
      GET_PTR(pContext->pBaseAddress, *nextItem, previousItem->nextItem, psonHashItem );
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return true;
   }
   
   /* 
    * Note: the next item has to be the first non-empty pArray[i] beyond
    * the current bucket (previousBucket). 
    */
   for ( i = previousItem->bucket+1; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      if (currentOffset != PSON_NULL_OFFSET ) {
         GET_PTR(pContext->pBaseAddress, *nextItem, currentOffset, psonHashItem );
         PSO_TRACE_EXIT_NUCLEUS( pContext, true );
         return true;
      }
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoErrors psonHashInit( psonHash           * pHash,
                             ptrdiff_t            memObjOffset,
                             size_t               reservedSize, 
                             psonSessionContext * pContext )
{
   size_t len, numBuckets;
   ptrdiff_t* ptr;
   unsigned int i;
   psonMemObject * pMemObject;
   
   PSO_PRE_CONDITION( pHash != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   pHash->numberOfItems = 0;
   pHash->totalDataSizeInBytes = 0;
   pHash->enumResize = PSON_HASH_NO_RESIZE;
   
   GET_PTR(pContext->pBaseAddress, pMemObject, memObjOffset, psonMemObject );
   /*
    * reservedSize... In real life what it means is that we cannot shrink 
    * the array to a point where we would need to increase it in order
    * to hold reservedSize items.
    *
    * Since a ratio (load factor) of 1.5-2 is considered optimal (based on
    * research done on the web...) we will use a load factor of 1.75 to 
    * calculate the number of buckets.
    */
   numBuckets = reservedSize * 100 / 175;
   
   /* Which one of our lengths is closer but larger than numBuckets? */
   pHash->lengthIndex = pHash->lengthIndexMinimum = 0;
   for ( i = 1; i < PSON_PRIME_NUMBER_ARRAY_LENGTH; ++i ) {
      if ( g_psonArrayLengths[i] > numBuckets ) {
         pHash->lengthIndex = i - 1;
         pHash->lengthIndexMinimum = i - 1;
         break;
      }
   }
   
   len = g_psonArrayLengths[pHash->lengthIndex] * sizeof(ptrdiff_t);
   
   ptr = (ptrdiff_t*) psonMalloc( pMemObject, len, pContext );
   if ( ptr != NULL ) {
      for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i) {
         ptr[i] = PSON_NULL_OFFSET;
      }
      
      pHash->arrayOffset = SET_OFFSET(pContext->pBaseAddress, ptr );
      pHash->initialized = PSON_HASH_SIGNATURE;
      pHash->memObjOffset = memObjOffset;
   
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return PSO_OK;
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return PSO_NOT_ENOUGH_PSO_MEMORY;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoErrors psonHashInsert( psonHash            * pHash,
                               const unsigned char * pKey,
                               uint32_t              keyLength,
                               const void          * pData,
                               uint32_t              dataLength,
                               psonHashItem       ** ppHashItem,
                               psonSessionContext  * pContext )
{
   ptrdiff_t* pArray;   
   size_t bucket = 0;
   bool   keyFound = false;
   psonHashItem* pItem, *previousItem = NULL;
   size_t itemLength;
   psonMemObject * pMemObject;
   
   PSO_PRE_CONDITION( pHash      != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pKey       != NULL );
   PSO_PRE_CONDITION( pData      != NULL );
   PSO_PRE_CONDITION( ppHashItem != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( dataLength > 0 );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_SIGNATURE );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength, 
                       &pItem, &previousItem, &bucket, pContext );

   if ( keyFound ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return PSO_ITEM_ALREADY_PRESENT;
   }
   
   GET_PTR(pContext->pBaseAddress, pMemObject, pHash->memObjOffset, psonMemObject );
   
   /* The whole item is allocated in one step, header+data, to minimize */
   /* overheads of the memory allocator */
   itemLength = calculateItemLength( keyLength, dataLength );
   pItem = (psonHashItem*) psonMalloc( pMemObject, itemLength, pContext );
   if ( pItem == NULL ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return PSO_NOT_ENOUGH_PSO_MEMORY;
   }

   pItem->nextItem = PSON_NULL_OFFSET;
   pItem->bucket = bucket;
   
   /* keyLength must be set before calling getData() */   
   pItem->keyLength = keyLength;
   pItem->dataLength = dataLength;
   pItem->dataOffset = SET_OFFSET(pContext->pBaseAddress, pItem) + itemLength - dataLength;
   
   memcpy( pItem->key, pKey, keyLength );
   memcpy( GET_PTR_FAST(pContext->pBaseAddress, pItem->dataOffset, unsigned char), pData, dataLength );

   pHash->totalDataSizeInBytes += dataLength;
   pHash->numberOfItems++;

   pHash->enumResize = isItTimeToResize( pHash );
   
   if ( previousItem == NULL ) {
      pArray[bucket] = SET_OFFSET(pContext->pBaseAddress, pItem);
   }
   else {
      previousItem->nextItem = SET_OFFSET(pContext->pBaseAddress, pItem);
   }
   
   *ppHashItem = pItem;
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoErrors psonHashResize( psonHash           * pHash,
                               psonSessionContext * pContext )
{
   int newIndexLength;
   ptrdiff_t * ptr, * pArray;
   size_t len, i;
   ptrdiff_t currentOffset, nextOffset, newBucket, newOffset;
   psonHashItem * pItem, * pNewItem;
   psonMemObject * pMemObject;
   unsigned int loadFactor;
   
   PSO_PRE_CONDITION( pHash != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   if ( pHash->enumResize == PSON_HASH_NO_RESIZE ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return PSO_OK;
   }
   
   GET_PTR(pContext->pBaseAddress, pMemObject, pHash->memObjOffset, psonMemObject );

   newIndexLength = pHash->lengthIndex;
   if ( pHash->enumResize == PSON_HASH_TIME_TO_GROW ) {
      do {
         newIndexLength++;
         loadFactor = 100 * pHash->numberOfItems/
            g_psonArrayLengths[newIndexLength];
      } while ( loadFactor >= g_maxLoadFactor );
   }
   else {
      do {
         newIndexLength--;
         loadFactor = 100 * pHash->numberOfItems/
            g_psonArrayLengths[newIndexLength];
      } while ( loadFactor < g_minLoadFactor );
   }

   len = g_psonArrayLengths[newIndexLength] * sizeof(ptrdiff_t);
   
   ptr = (ptrdiff_t*) psonMalloc( pMemObject, len, pContext );
   if ( ptr == NULL ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return PSO_NOT_ENOUGH_PSO_MEMORY;
   }
   
   for ( i = 0; i < g_psonArrayLengths[newIndexLength]; ++i) {
      ptr[i] = PSON_NULL_OFFSET;
   }
   
   for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      while ( currentOffset != PSON_NULL_OFFSET ) {
         GET_PTR(pContext->pBaseAddress, pItem, currentOffset, psonHashItem );
         nextOffset = pItem->nextItem;
         pItem->nextItem = PSON_NULL_OFFSET;
         
         newBucket = hash_it( pItem->key, pItem->keyLength ) % 
                     g_psonArrayLengths[newIndexLength];
         pItem->bucket = newBucket;
         if ( ptr[newBucket] == PSON_NULL_OFFSET ) {
            ptr[newBucket] = currentOffset;
         }
         else {
            newOffset = ptr[newBucket];
            GET_PTR(pContext->pBaseAddress, pNewItem, newOffset, psonHashItem );
            while ( pNewItem->nextItem != PSON_NULL_OFFSET ) {
               newOffset = pNewItem->nextItem;
               GET_PTR(pContext->pBaseAddress, pNewItem, newOffset, psonHashItem );
            }
            pNewItem->nextItem = currentOffset;
         }
         
         /* Move to the next item in our bucket */
         currentOffset = nextOffset;
      }
   }
   
   len = g_psonArrayLengths[pHash->lengthIndex]*sizeof(ptrdiff_t);

   pHash->lengthIndex = newIndexLength;
   pHash->arrayOffset = SET_OFFSET(pContext->pBaseAddress, ptr );

   psonFree( pMemObject,
             (unsigned char*)pArray,
             len, 
             pContext );
   
   pHash->enumResize = PSON_HASH_NO_RESIZE;

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return PSO_OK;   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Warning: this function should only be used when working with a temp copy
 * of read-only maps. For read-write containers, do not use as you cannot
 * rollback!
 */

enum psoErrors 
psonHashUpdate( psonHash            * pHash,
                const unsigned char * pKey,
                uint32_t              keyLength,
                const void          * pData,
                uint32_t              dataLength,
                psonHashItem       ** ppHashItem,
                psonSessionContext  * pContext )
{
   size_t bucket = 0;
   bool keyFound = false;
   ptrdiff_t * pArray;
   size_t newItemLength, oldItemLength;
   psonHashItem * pOldItem, * previousItem = NULL, * pNewItem = NULL;
   psonMemObject * pMemObject;

   PSO_PRE_CONDITION( pHash      != NULL );
   PSO_PRE_CONDITION( pKey       != NULL );
   PSO_PRE_CONDITION( pData      != NULL );
   PSO_PRE_CONDITION( ppHashItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( dataLength > 0 );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   keyFound = findKey( pHash, pArray, pKey, keyLength,
                       &pOldItem, &previousItem, &bucket, pContext );

   if ( ! keyFound ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return PSO_NO_SUCH_ITEM;
   }
   
   newItemLength = calculateItemLength( keyLength, dataLength );
   oldItemLength = calculateItemLength( keyLength, pOldItem->dataLength );

   if ( newItemLength == oldItemLength ) {
      /*
       * Less work! We can just copy the data. We do not care about crash 
       * recovery -> this a temp copy and it will be removed if the program
       * crashes.
       */
      memcpy( GET_PTR_FAST(pContext->pBaseAddress, pOldItem->dataOffset, void), pData, dataLength );
      pHash->totalDataSizeInBytes += (dataLength - pOldItem->dataLength);
      pOldItem->dataLength = dataLength;

      *ppHashItem = pOldItem;
   }
   else {
      pMemObject = GET_PTR_FAST(pContext->pBaseAddress, pHash->memObjOffset, psonMemObject );
      pNewItem = (psonHashItem*) psonMalloc( pMemObject,
                                             newItemLength,
                                             pContext );
      if ( pNewItem == NULL ) {
         PSO_TRACE_EXIT_NUCLEUS( pContext, false );
         return PSO_NOT_ENOUGH_PSO_MEMORY;
      }
      
      /* initialize the new record */
      pNewItem->nextItem = pOldItem->nextItem;
      pNewItem->bucket = pOldItem->bucket;
      pNewItem->keyLength = keyLength;
      pNewItem->dataLength = dataLength;
      pNewItem->dataOffset = SET_OFFSET(pContext->pBaseAddress, pNewItem) + newItemLength - dataLength;
   
      memcpy( pNewItem->key, pKey, keyLength );
      memcpy( GET_PTR_FAST(pContext->pBaseAddress, pNewItem->dataOffset, unsigned char), pData, dataLength );

      pHash->totalDataSizeInBytes += (dataLength - pOldItem->dataLength);

      if ( previousItem == NULL ) {
         pArray[bucket] = SET_OFFSET(pContext->pBaseAddress, pNewItem);
      }
      else {
         previousItem->nextItem = SET_OFFSET(pContext->pBaseAddress, pNewItem);
      }

      *ppHashItem = pNewItem;
      
      /* Eliminate the old record */
      psonFree( pMemObject, (unsigned char*)pOldItem, oldItemLength, pContext );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

