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

#include "Nucleus/HashTx.h"
#include "Nucleus/hash_fnv.h"
#include "Nucleus/SessionContext.h"
#include "Nucleus/MemoryObject.h"

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

   len = offsetof(psonHashTxItem, key) + keyLength;
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
psonHashResizeEnum isItTimeToResize( psonHashTx * pHash )
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

static bool findKey( psonHashTx          * pHash,
                     ptrdiff_t           * pArray,
                     const unsigned char * pKey,
                     uint32_t              keyLength,
                     psonHashTxItem     ** ppItem,
                     psonHashTxItem     ** ppPreviousItem,
                     size_t              * pBucket,
                     psonSessionContext  * pContext )
{
   ptrdiff_t currentOffset, nextOffset;
   psonHashTxItem* pItem;

   PSO_TRACE_ENTER_NUCLEUS( pContext );

   *pBucket = hash_it( pKey, keyLength ) % g_psonArrayLengths[pHash->lengthIndex];
   currentOffset = pArray[*pBucket];
   
   *ppPreviousItem = NULL;

   while ( currentOffset != PSON_NULL_OFFSET ) {
      GET_PTR(pContext->pBaseAddress, pItem, currentOffset, psonHashTxItem );
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
 * Functions declared in HashTx.h (alphabetic order).
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonHashTxDelete( psonHashTx         * pHash,
                       psonHashTxItem     * pItem,
                       psonSessionContext * pContext )
{
   ptrdiff_t * pArray;
   psonHashTxItem * pNewItem = NULL, * previousItem = NULL;
   ptrdiff_t nextOffset;
   psonMemObject * pMemObject;
   size_t bucket;
   
   PSO_PRE_CONDITION( pHash    != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pItem    != NULL );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_TX_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   GET_PTR(pContext->pBaseAddress, pMemObject, pHash->memObjOffset, psonMemObject );
   bucket = pItem->bucket;
   
   nextOffset = pArray[bucket];
   while ( nextOffset != PSON_NULL_OFFSET ) {
      previousItem = pNewItem;
      GET_PTR(pContext->pBaseAddress, pNewItem, nextOffset, psonHashTxItem );
      if ( pNewItem == pItem ) break;
      nextOffset = pNewItem->nextItem;
   }
   PSO_INV_CONDITION( pNewItem == pItem );

   nextOffset = pItem->nextItem;
      
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
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonHashTxDump( psonHashTx         * pHash,
                     int                  indent,
                     psonSessionContext * pContext )
{
   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonHashTx (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
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
   
   if ( pHash->initialized == PSON_HASH_TX_SIGNATURE ) {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "Signature is ok.\n" );
   }
   else {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "Signature is wrong - value is %d, exepted: %d\n",
         pHash->initialized, PSON_HASH_TX_SIGNATURE );
   }

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonHashTx END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonHashTxFini( psonHashTx         * pHash,
                     psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHash != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_TX_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   pHash->initialized = 0;

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashTxGet( psonHashTx          * pHash,
                    const unsigned char * pKey,
                    uint32_t              keyLength,
                    psonHashTxItem     ** ppItem,
                    size_t              * pBucket,
                    psonSessionContext  * pContext )
{
   bool keyFound = false;
   ptrdiff_t* pArray;
   psonHashTxItem* pItem, *dummy;
   
   PSO_PRE_CONDITION( pHash    != NULL );
   PSO_PRE_CONDITION( pKey     != NULL );
   PSO_PRE_CONDITION( ppItem   != NULL );
   PSO_PRE_CONDITION( pBucket  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_TX_SIGNATURE );
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
#  pragma warning(disable:4702) 
#endif

bool psonHashTxGetFirst( psonHashTx         * pHash,
                         ptrdiff_t          * pFirstItemOffset,
                         psonSessionContext * pContext )
{
   ptrdiff_t* pArray, currentOffset;
   bool SHOULD_NOT_REACHED_THIS = true;
   size_t i;
   
   PSO_PRE_CONDITION( pHash != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pFirstItemOffset != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_TX_SIGNATURE );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   if ( pHash->numberOfItems == 0 ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return false;
   }
   /* 
    * Note: the first item has to be the first non-empty pArray[i],
    * this makes the search easier.
    */
   for ( i = 0; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      if (currentOffset != PSON_NULL_OFFSET ) {
         *pFirstItemOffset = currentOffset;
         PSO_TRACE_EXIT_NUCLEUS( pContext, true );
         return true;
      }
   }

   PSO_POST_CONDITION( SHOULD_NOT_REACHED_THIS == false );
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false; /* Should never occur */
}

#if defined (_WIN32)
#  pragma warning(default:4702) /* unreachable code */
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashTxGetNext( psonHashTx         * pHash,
                        ptrdiff_t            previousOffset,
                        ptrdiff_t          * pNextItemOffset,
                        psonSessionContext * pContext )
{
   ptrdiff_t* pArray, currentOffset;
   size_t i;
   psonHashTxItem* pItem;
   
   PSO_PRE_CONDITION( pHash != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pNextItemOffset != NULL );
   PSO_PRE_CONDITION( previousOffset != PSON_NULL_OFFSET );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_TX_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   GET_PTR(pContext->pBaseAddress, pItem, previousOffset, psonHashTxItem );
   if ( pItem->nextItem != PSON_NULL_OFFSET ) {
      /* We found the next one in the linked list. */
      *pNextItemOffset = pItem->nextItem;
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return true;
   }
   
   /* 
    * Note: the next item has to be the first non-empty pArray[i] beyond
    * the current bucket (previousBucket). 
    */
   for ( i = pItem->bucket+1; i < g_psonArrayLengths[pHash->lengthIndex]; ++i ) {
      currentOffset = pArray[i];
      
      if (currentOffset != PSON_NULL_OFFSET ) {
         *pNextItemOffset = currentOffset;
         PSO_TRACE_EXIT_NUCLEUS( pContext, true );
         return true;
      }
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoErrors psonHashTxInit( psonHashTx         * pHash,
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
      pHash->initialized = PSON_HASH_TX_SIGNATURE;
      pHash->memObjOffset = memObjOffset;
   
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return PSO_OK;
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return PSO_NOT_ENOUGH_PSO_MEMORY;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoErrors psonHashTxInsert( psonHashTx          * pHash,
                                 size_t                bucket,
                                 const unsigned char * pKey,
                                 uint32_t              keyLength,
                                 const void          * pData,
                                 uint32_t              dataLength,
                                 psonHashTxItem     ** ppNewItem,
                                 psonSessionContext  * pContext )
{
   ptrdiff_t* pArray;   
   psonHashTxItem* pItem, *previousItem = NULL;
   size_t itemLength;
   psonMemObject * pMemObject;
   ptrdiff_t currentOffset;
   
   PSO_PRE_CONDITION( pHash     != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pKey      != NULL );
   PSO_PRE_CONDITION( pData     != NULL );
   PSO_PRE_CONDITION( ppNewItem != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( dataLength > 0 );
   PSO_PRE_CONDITION( bucket < g_psonArrayLengths[pHash->lengthIndex] );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_TX_SIGNATURE );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   currentOffset = pArray[bucket];

   while ( currentOffset != PSON_NULL_OFFSET ) {
      GET_PTR(pContext->pBaseAddress, previousItem, currentOffset, psonHashTxItem );
      currentOffset = previousItem->nextItem;     
   }

   GET_PTR(pContext->pBaseAddress, pMemObject, pHash->memObjOffset, psonMemObject );
   
   /* The whole item is allocated in one step, header+data, to minimize */
   /* overheads of the memory allocator */
   itemLength = calculateItemLength( keyLength, dataLength );
   pItem = (psonHashTxItem*) psonMalloc( pMemObject, itemLength, pContext );
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
   pItem->nextSameKey = PSON_NULL_OFFSET;
    
   memcpy( pItem->key,     pKey, keyLength );
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
   
   *ppNewItem = pItem;

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoErrors psonHashTxResize( psonHashTx         * pHash,
                                 psonSessionContext * pContext )
{
   int newIndexLength;
   ptrdiff_t * ptr, * pArray;
   size_t len, i;
   ptrdiff_t currentOffset, nextOffset, newBucket, newOffset;
   psonHashTxItem * pItem, * pNewItem;
   psonMemObject * pMemObject;
  
   PSO_PRE_CONDITION( pHash != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_INV_CONDITION( pHash->initialized == PSON_HASH_TX_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   GET_PTR(pContext->pBaseAddress, pArray, pHash->arrayOffset, ptrdiff_t );
   PSO_INV_CONDITION( pArray != NULL );

   if ( pHash->enumResize == PSON_HASH_NO_RESIZE ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return PSO_OK;
   }
   
   GET_PTR(pContext->pBaseAddress, pMemObject, pHash->memObjOffset, psonMemObject );

   if ( pHash->enumResize == PSON_HASH_TIME_TO_GROW ) {
      newIndexLength = pHash->lengthIndex + 1;
   }
   else {
      newIndexLength = pHash->lengthIndex - 1;     
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
         GET_PTR(pContext->pBaseAddress, pItem, currentOffset, psonHashTxItem );
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
            GET_PTR(pContext->pBaseAddress, pNewItem, newOffset, psonHashTxItem );
            while ( pNewItem->nextItem != PSON_NULL_OFFSET ) {
               newOffset = pNewItem->nextItem;
               GET_PTR(pContext->pBaseAddress, pNewItem, newOffset, psonHashTxItem );
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

