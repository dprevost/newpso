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

#ifndef PSON_HASH_H
#define PSON_HASH_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/HashCommon.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PSON_HASH_SIGNATURE  ((unsigned int)0xa126fec4)

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Descriptor/container for elements of the hash array. 
 *
 * Note: the data is to be found after the key. It MUST be properly 
 * aligned if we eventually allow direct access to the data (from the 
 * API) or even to be able to use it easily internally. 
 */
struct psonHashItem
{
   /** Next item in this bucket */
   ptrdiff_t     nextItem;
   
   uint32_t      keyLength;
   ptrdiff_t     dataOffset;
   uint32_t      dataLength;
   size_t        bucket;
   unsigned char key[1];
   
};

typedef struct psonHashItem psonHashItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The current version of the Hash uses buckets (the first version was
 * built with linear probing).
 *
 * One of the main advantage of buckets is that it gives us a better 
 * method for controlling when the Hash is reorganized (it have to be 
 * done in sync with a transaction to avoid confusion - with linear probing 
 * there is a hard limit on how long we can wait for a reorganization 
 * (when the array is 100% populated). There is no such limit with buckets 
 * since it is an array of linked lists).
 */
struct psonHash
{
   /** offset of the memory object we need to use for allocating memory. */
   ptrdiff_t memObjOffset;
   
   /** Offset to an array of offsets to psonHashItem objects */
   ptrdiff_t    arrayOffset; 
   
   /** Number of items stored in this hash map. */
   size_t       numberOfItems;
   
   /** Total amount of bytes of data stored in this hash */
   size_t       totalDataSizeInBytes;

   /** The index into the array of lengths (aka the number of buckets). */
   int lengthIndex;

   /** The mimimum shrinking factor that we can tolerate to accommodate
    *  the reservedSize argument of psonHashInit() ) */
   int lengthIndexMinimum;

   /** Indicator of the current status of the array. */
   psonHashResizeEnum enumResize;
   
   /** Set to PSON_HASH_SIGNATURE at initialization. */
   unsigned int initialized;

};

typedef struct psonHash psonHash;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Used to create a copy of a read-only hash map for editing (updates)
 */
enum psoErrors psonHashCopy( psonHash           * pOldHash,
                             psonHash           * pNewHash,
                             psonSessionContext * pContext );

/*
 * Used to delete an hash item when you know its exact position
 * (through the psonHashItem) 
 */
void psonHashDelWithItem( psonHash            * pHash,
                          psonHashItem        * pItem,
                          psonSessionContext  * pContext );

/* Direct delete using the key and nothing else. */
bool psonHashDelWithKey( psonHash            * pHash,
                         const unsigned char * pKey, 
                         uint32_t              keyLength,
                         psonSessionContext  * pContext );

#if defined(PSO_USE_TRACE)
void psonHashDump( psonHash           * pHash,
                   int                  indent,
                   psonSessionContext * pContext );
#endif

void psonHashEmpty( psonHash           * pHash,
                    psonSessionContext * pContext );

void psonHashFini( psonHash           * pHash,
                   psonSessionContext * pContext );

bool psonHashGet( psonHash            * pHash,
                  const unsigned char * pkey,
                  uint32_t              keyLength,
                  psonHashItem       ** ppItem,
                  size_t              * pBucket,
                  psonSessionContext  * pContext );

bool psonHashGetFirst( psonHash           * pHash,
                       psonHashItem      ** ppItem,
                       psonSessionContext * pContext );

bool psonHashGetNext( psonHash           * pHash,
                      psonHashItem       * previousItem,
                      psonHashItem      ** nextItem,
                      psonSessionContext * pContext );

enum psoErrors psonHashInit( psonHash           * pHash,
                             ptrdiff_t            memObjOffset,
                             size_t               reservedSize, 
                             psonSessionContext * pContext );

/*
 * ppNewItem is used to access the original name of 
 * objects and the psonTxStatus by the objects themselves 
 */
enum psoErrors psonHashInsert( psonHash            * pHash,
                               const unsigned char * pKey,
                               uint32_t              keyLength,
                               const void          * pData,
                               uint32_t              dataLength,
                               psonHashItem       ** ppHashItem,
                               psonSessionContext  * pContext );

enum psoErrors psonHashResize( psonHash           * pHash,
                               psonSessionContext * pContext );

enum psoErrors psonHashUpdate( psonHash            * pHash,
                               const unsigned char * pKey,
                               uint32_t              keyLength,
                               const void          * pData,
                               uint32_t              dataLength,
                               psonHashItem       ** ppHashItem,
                               psonSessionContext  * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
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
#  define PSON_PRIME_NUMBER_ARRAY_LENGTH 28
extern size_t g_psonArrayLengths[PSON_PRIME_NUMBER_ARRAY_LENGTH];
#else
#  define PSON_PRIME_NUMBER_ARRAY_LENGTH 60
extern size_t g_psonArrayLengths[PSON_PRIME_NUMBER_ARRAY_LENGTH];
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_HASH_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

