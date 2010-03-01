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

#ifndef PSON_MEMORY_OBJECT_H
#define PSON_MEMORY_OBJECT_H

#include "Nucleus/Engine.h"
#include "Nucleus/LinkNode.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/SessionContext.h"
#include "Nucleus/BlockGroup.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Memory objects include all the data containers (queues, etc.), the folders
 * and all "hidden" top level objects (the allocator, the sessions recovery
 * objects, etc.).
 *
 * This struct should always be the first member of the struct defining such
 * an object. This way, the identifier is always at the top of a block and it
 * should help debug, recover from crashes, etc. 
 *
 * Furthermore, some of the code (transactions, for example) depend on this
 * struct being first.
 *
 * The psonBlockGroup struct is NOT included in this struct since it
 * contains a variable array size. The psonBlockGroup struct should be
 * put at the end of the container that owns a psonMemObject.
 */
struct psonMemObject
{
   /** Type of memory object */
   psonMemObjIdent objType;
   
   /** The lock... obviously */
   psocProcessLock lock;

   /**
    * Counter for committed changes.
    *
    * This counter is useful to detect changes to the object.
    */
   uint32_t committedChanges;

   /** Total number of blocks for the current object */
   size_t totalBlocks;
   
   psonLinkedList listBlockGroup;
   
};

typedef struct psonMemObject psonMemObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonMemObjectDump( psonMemObject      * pMemObj,
                        int                  indent,
                        psonSessionContext * pContext );
#endif

enum psoErrors 
psonMemObjectInit( psonMemObject      * pMemObj,
                   psonMemObjIdent      objType,
                   psonBlockGroup     * pGroup,
                   size_t               numBlocks,
                   psonSessionContext * pContext );

enum psoErrors 
psonMemObjectFini( psonMemObject      * pMemObj,
                   psonAllocTypeEnum    allocType,
                   psonSessionContext * pContext );

unsigned char* psonMalloc( psonMemObject      * pMemObj,
                           size_t               numBytes,
                           psonSessionContext * pContext );

void psonFree( psonMemObject      * pMemObj,
               unsigned char      * ptr, 
               size_t               numBytes,
               psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psonLock( psonMemObject      * pMemObj,
               psonSessionContext * pContext )
{
   bool ok;

   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   if ( pContext->lockOffsets != NULL ) {
      psonSessionAddLock( pContext, SET_OFFSET(pContext->pBaseAddress, pMemObj ) );
   }
   
   ok = psocTryAcquireProcessLock ( &pMemObj->lock,
                                   pContext->pidLocker,
                                   PSON_LOCK_TIMEOUT );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      if ( pContext->lockOffsets != NULL ) {
         psonSessionRemoveLock( pContext, SET_OFFSET(pContext->pBaseAddress, pMemObj ) );
      }
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, ok );
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonLockNoFailure( psonMemObject      * pMemObj,
                        psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   if ( pContext->lockOffsets != NULL ) {
      psonSessionAddLock( pContext, SET_OFFSET(pContext->pBaseAddress, pMemObj ) );
   }
   
   psocAcquireProcessLock ( &pMemObj->lock, pContext->pidLocker );

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonMemObjectStatus( psonMemObject      * pMemObject, 
                          psoObjStatus       * pStatus,
                          psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonUnlock( psonMemObject      * pMemObj,
                 psonSessionContext * pContext  )
{
   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   if ( pContext->lockOffsets != NULL ) {
      psonSessionRemoveLock( pContext, SET_OFFSET(pContext->pBaseAddress, pMemObj ) );
   }
   
   psocReleaseProcessLock ( &pMemObj->lock );

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_MEMORY_OBJECT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

