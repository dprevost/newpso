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

#include "Nucleus/Session.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/SessionContext.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/Cursor.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSessionAddObj( psonSession        * pSession,
                        ptrdiff_t            objOffset, 
                        enum psoObjectType   objType, 
                        void               * pCommonObject,
                        psonObjectContext ** ppObject,
                        psonSessionContext * pContext )
{
   bool ok = false;
   psonObjectContext * pCurrentBuffer;

   PSO_PRE_CONDITION( pSession      != NULL );
   PSO_PRE_CONDITION( pCommonObject != NULL );
   PSO_PRE_CONDITION( ppObject      != NULL );
   PSO_PRE_CONDITION( pContext      != NULL );
   PSO_PRE_CONDITION( objOffset     != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( objType > 0 && objType < PSO_LAST_OBJECT_TYPE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pSession->memObject, pContext ) ) {
      pCurrentBuffer = (psonObjectContext *)
         psonMalloc( &pSession->memObject, sizeof(psonObjectContext), pContext );
      if ( pCurrentBuffer != NULL ) {
         pCurrentBuffer->offset    = objOffset;
         pCurrentBuffer->type      = objType;
         pCurrentBuffer->pCommonObject = pCommonObject;

         psonLinkNodeInit( &pCurrentBuffer->node, pContext );
         psonLinkedListPutLast( &pSession->listOfObjects, 
                                &pCurrentBuffer->node,
                                pContext );
         *ppObject = pCurrentBuffer;
         ok = true;
      }
      else {
         psocSetError( &pContext->errorHandler,
                       g_psoErrorHandle,
                       PSO_NOT_ENOUGH_PSO_MEMORY );
      }
      
      psonUnlock( &pSession->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    PSO_ENGINE_BUSY );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, ok );
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSessionCloseCursor( psonSession        * pSession,
                             psonCursorContext  * pCursorContext,
                             psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pSession       != NULL );
   PSO_PRE_CONDITION( pCursorContext != NULL );
   PSO_PRE_CONDITION( pContext       != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pSession->memObject, pContext ) ) {
      
      psonCursorEmpty( pCursorContext->cursor, pContext );
      psonCursorFini( pCursorContext->cursor, pContext );

      psonLinkedListRemoveItem( &pSession->listOfCursors, 
                                &pCursorContext->node,
                                pContext );
      psonFree( &pSession->memObject, 
                (unsigned char *)pCursorContext, 
                sizeof(psonCursorContext),
                pContext );

      psonUnlock( &pSession->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    PSO_ENGINE_BUSY );
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return false;
   }

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonSessionDump( psonSession        * pSession,
                      int                  indent,
                      psonSessionContext * pContext )
{
   int i;
   
   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonSession (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pSession, SET_OFFSET(pContext->pBaseAddress, pSession) );
   if ( pSession == NULL ) return;

   psonMemObjectDump( &pSession->memObject, indent + 2, pContext );

   psonLinkNodeDump( &pSession->node, indent + 2, pContext );

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Pointer to the psoaSession: %p\n",
      pSession->pApiSession );
   
   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Pointer to our transaction object: %p\n",
      pSession->pTransaction );
   
   psonLinkedListDump( &pSession->listOfObjects, indent + 2, pContext );

   psonLinkedListDump( &pSession->listOfCursors, indent + 2, pContext );

   for ( i = 0; i < PSON_MAX_LOCK_DEPTH; ++i ) {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "Offset to lock %d: "PSO_PTRDIFF_T_FORMAT"\n",
         i, pSession->lockOffsets[i] );
   }
   
   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Number of locks we are holding: %d\n", 
      pSession->numLocks );
   
   psonBlockGroupDump( &pSession->blockGroup, indent + 2, pContext );

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonSession END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonSessionFini( psonSession        * pSession,
                      psonSessionContext * pContext )
{
   psonObjectContext * pObject = NULL;
   psonLinkNode * pNode = NULL;

   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pSession->memObject.objType == PSON_IDENT_SESSION );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   /*
    * Eliminate all objects in the list. This is probably not needed
    * as we will release the blocks of memory to the allocator as the
    * last step. This might be reviewed eventually.
    */

   while ( psonLinkedListPeakFirst( &pSession->listOfObjects, 
                                    &pNode,
                                    pContext ) ) {
      pObject = (psonObjectContext*)
         ((char*)pNode - offsetof( psonObjectContext, node ));
      psonSessionRemoveObj( pSession, pObject, pContext );
   }
   
   /* Terminates our associated transaction objects. We presume that
    * either commit or rollback was called to clear it.
    */
   psonTxFini( pSession->pTransaction, pContext );

   /* This will remove the blocks of allocated memory */
   psonMemObjectFini(  &pSession->memObject, PSON_ALLOC_ANY, pContext );

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Lock and Unlock must be used before calling this function */
bool psonSessionGetFirst( psonSession        * pSession,
                          psonObjectContext ** ppObject,
                          psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   bool ok;

   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( ppObject != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   ok = psonLinkedListPeakFirst( &pSession->listOfObjects, 
                                 &pNode,
                                 pContext );
   if ( ok ) {
      *ppObject = (psonObjectContext*)
         ((char*)pNode - offsetof( psonObjectContext, node ));
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, ok );
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSessionInit( psonSession        * pSession,
                      void               * pApiSession,
                      psonSessionContext * pContext )
{
   psoErrors errcode;
   psonTx * pTx;
   bool ok, rc = false;
   
   PSO_PRE_CONDITION( pSession    != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( pApiSession != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   errcode = psonMemObjectInit( &pSession->memObject, 
                                PSON_IDENT_SESSION,
                                &pSession->blockGroup,
                                1,
                                pContext ); /* A single block */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
   }
   else {
      psonLinkedListInit( &pSession->listOfObjects, pContext );

      pTx = (psonTx*) psonMallocBlocks( pContext->pAllocator, PSON_ALLOC_ANY, 1, pContext );
      if ( pTx != NULL ) {
         ok = psonTxInit( pTx, 1, pContext );
         PSO_PRE_CONDITION( ok == true || ok == false );
         if ( ok ) {
            pSession->numLocks = 0;
            pSession->pTransaction = pTx;
            pContext->pTransaction = (void *) pTx;            
            pContext->lockOffsets = pSession->lockOffsets;
            pContext->numLocks = &pSession->numLocks;
            pSession->pApiSession = pApiSession;
            
            rc = true;
         }
         else {
            psonFreeBlocks( pContext->pAllocator, 
                            PSON_ALLOC_ANY,
                            (unsigned char *)pTx, 
                            1, pContext );
         }
      }
      else {
         psocSetError( &pContext->errorHandler, 
                       g_psoErrorHandle, 
                       PSO_NOT_ENOUGH_PSO_MEMORY );
      }
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, rc );
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSessionOpenCursor( psonSession        * pSession,
                            psonCursorContext ** ppCursor,
                            psonSessionContext * pContext )
{
   bool ok = false;
   psonCursorContext * pCurrentBuffer;
   psonCursor * pCursor;
   
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( ppCursor != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pSession->memObject, pContext ) ) {
      
      pCursor = (psonCursor *) psonMallocBlocks( pContext->pAllocator, 
                                                 PSON_ALLOC_API_OBJ, 1, pContext );
      if ( pCursor != NULL ) {
         ok = psonCursorInit( pCursor, SET_OFFSET(pContext->pBaseAddress, pSession), 1, pContext );
         PSO_PRE_CONDITION( ok == true || ok == false );

         if ( ok ) {
            pCurrentBuffer = (psonCursorContext *) 
               psonMalloc( &pSession->memObject, sizeof(psonCursorContext), pContext );
            if ( pCurrentBuffer != NULL ) {
               pCurrentBuffer->offset = SET_OFFSET(pContext->pBaseAddress, pCursor);
               pCurrentBuffer->cursor = pCursor;
               
               psonLinkNodeInit( &pCurrentBuffer->node, pContext );
               psonLinkedListPutLast( &pSession->listOfCursors, 
                                      &pCurrentBuffer->node,
                                      pContext );
               *ppCursor = pCurrentBuffer;
               ok = true;
            }
            else {
               psocSetError( &pContext->errorHandler,
                             g_psoErrorHandle,
                             PSO_NOT_ENOUGH_PSO_MEMORY );
               psonFreeBlocks( pContext->pAllocator, 
                               PSON_ALLOC_API_OBJ,
                               (unsigned char *)pCursor, 
                               1, pContext );
            }
         }
         else {
            psonFreeBlocks( pContext->pAllocator, 
                            PSON_ALLOC_API_OBJ,
                            (unsigned char *)pCursor, 
                            1, pContext );
         }
      }
      else {
         psocSetError( &pContext->errorHandler, 
                       g_psoErrorHandle, 
                       PSO_NOT_ENOUGH_PSO_MEMORY );
      }      
      
      psonUnlock( &pSession->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    PSO_ENGINE_BUSY );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, ok );
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Lock and Unlock must be used before calling this function */
bool psonSessionRemoveFirst( psonSession        * pSession,
                             psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   psonObjectContext * pObject;
   
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   if ( psonLinkedListGetFirst(&pSession->listOfObjects, &pNode, pContext) ) {
      pObject = (psonObjectContext*)
         ((char*)pNode - offsetof( psonObjectContext, node ));

      psonFree( &pSession->memObject, 
                (unsigned char *)pObject, 
                sizeof(psonObjectContext),
                pContext );
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return true;
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSessionRemoveObj( psonSession        * pSession,
                          psonObjectContext  * pObject,
                          psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( pObject  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pSession->memObject, pContext ) ) {
      psonLinkedListRemoveItem( &pSession->listOfObjects, 
                                &pObject->node,
                                pContext );
      psonFree( &pSession->memObject, 
                (unsigned char *)pObject, 
                sizeof(psonObjectContext),
                pContext );

      psonUnlock( &pSession->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    PSO_ENGINE_BUSY );
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return false;
   }

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

