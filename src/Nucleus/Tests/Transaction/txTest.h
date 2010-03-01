/*
 * Copyright (C) 2007-2010 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef TX_TEST_COMMON_H
#define TX_TEST_COMMON_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/InitEngine.h"
#include "Nucleus/Folder.h"
#include <photon/psoPhotonODBC.h>

psocErrMsgHandle g_psoErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes the memory needed by the transaction
 * object.
 *
 * A reminder: transactions are memory objects - when they need memory, 
 * they ask it from their internalmemObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
psonTx* initTxTest( psonSessionContext* pContext )
{
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psonTx* pDummy;
   size_t allocatedLength = PSON_BLOCK_SIZE * 10;

   memset( pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psonInitEngine();
   assert( ok );
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   assert( ptr != NULL );
   pContext->pBaseAddress = ptr;
   pAlloc = (psonMemAlloc*)(pContext->pBaseAddress + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object */
   pDummy = (psonTx*) psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, pContext );
   assert( pDummy != NULL );
   
   return pDummy;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/**
 * This function initializes the memory needed by the folder
 * object.
 *
 * A reminder: folders are memory objects - when they need memory, 
 * they ask it from their internal memObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
psonFolder* initFolderTest( psonSessionContext* pContext )
{
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psonTx* pTx;
   psonFolder* pFolder;
   size_t allocatedLength = PSON_BLOCK_SIZE * 25;
   bool ok;

   memset( pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psonInitEngine();
   assert( ok );
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   assert( ptr != NULL );
   pContext->pBaseAddress = ptr;
   pAlloc = (psonMemAlloc*)(pContext->pBaseAddress + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (psonTx*)psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, pContext );
   assert( pTx != NULL );
   ok = psonTxInit( pTx, 1, pContext );
   assert( ok );
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the folder object */
   pFolder = (psonFolder*)psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, pContext );
   assert( pFolder != NULL );
   
   return pFolder;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* TX_TEST_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

