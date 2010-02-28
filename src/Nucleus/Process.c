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

#include "Nucleus/Process.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessAddSession( psonProcess        * process,
                            void               * pApiSession,
                            psonSession       ** ppSession,
                            psonSessionContext * pContext )
{
   psonSession * pCurrentBuffer;
   bool ok = false;
   
   PSO_PRE_CONDITION( process    != NULL );
   PSO_PRE_CONDITION( pApiSession != NULL );
   PSO_PRE_CONDITION( ppSession   != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   *ppSession = NULL;
   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &process->memObject, pContext ) ) {
      pCurrentBuffer = (psonSession*) 
          psonMallocBlocks( pContext->pAllocator, PSON_ALLOC_ANY, 1, pContext );
      if ( pCurrentBuffer != NULL ) {
         ok = psonSessionInit( pCurrentBuffer, pApiSession, pContext );
         PSO_PRE_CONDITION( ok == true || ok == false );
         if ( ok ) {
            psonLinkNodeInit( &pCurrentBuffer->node, pContext );
            psonLinkedListPutLast( &process->listOfSessions, 
                                   &pCurrentBuffer->node,
                                   pContext );
            *ppSession = pCurrentBuffer;
         }
         else {
            psonFreeBlocks( pContext->pAllocator, 
                            PSON_ALLOC_ANY,
                            (unsigned char *)pCurrentBuffer, 
                            1, pContext );
         }
      }
      else {
         psocSetError( &pContext->errorHandler, 
                       g_psoErrorHandle, 
                       PSO_NOT_ENOUGH_PSO_MEMORY );
      }

      /* 
       * If the init was a success,  this is now initialized. We must
       * add the previouslock otherwise... the unlock will fail (segv).
       */
      if ( pContext->lockOffsets != NULL ) {
         pContext->lockOffsets[*pContext->numLocks] = SET_OFFSET(g_pBaseAddr, &process->memObject);
         (*pContext->numLocks)++;
      }
      psonUnlock( &process->memObject, pContext );
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

#if defined(PSO_USE_TRACE)
void psonProcessDump( psonProcess        * process,
                      int                  indent,
                      psonSessionContext * pContext )
{
   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonProcess (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      process, SET_OFFSET(g_pBaseAddr, process) );
   if ( process == NULL ) return;

   psonMemObjectDump( &process->memObject, indent + 2, pContext );

   psonLinkNodeDump( &process->node, indent + 2, pContext );

   DO_INDENT( pContext, indent + 2 );
   fprintf( pContext->tracefp, "Process pid: %ud\n", process->pid );

   psonLinkedListDump( &process->listOfSessions, indent + 2, pContext );

   if ( process->processIsTerminating ) {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "This process is ending\n" );
   }
   else {
      DO_INDENT( pContext, indent + 2 );
      fprintf( pContext->tracefp, "This process is not ending\n" );
   }

   psonBlockGroupDump( &process->blockGroup, indent + 2, pContext );

   DO_INDENT( pContext, indent );
   fprintf( pContext->tracefp, "psonProcess END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonProcessFini( psonProcess        * process,
                      psonSessionContext * pContext )
{
   psonSession  * pSession = NULL;
   psonLinkNode * pNode    = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( process != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( process->memObject.objType == PSON_IDENT_PROCESS );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   /*
    * Eliminate all sessions in the list. This is probably not needed
    * as we will release the blocks of memory to the allocator as the
    * last step. This might be reviewed eventually.
    */

   while ( psonLinkedListPeakFirst( &process->listOfSessions, 
                                    &pNode,
                                    pContext ) ) {
      pSession = (psonSession*)
         ((char*)pNode - offsetof( psonSession, node ));

      ok = psonProcessRemoveSession( process, pSession, pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
   }

   /* 
    * This will remove the blocks of allocated memory.
    * It must be the last operation on that piece of memory.
    */
   psonMemObjectFini(  &process->memObject, PSON_ALLOC_ANY, pContext );

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessGetFirstSession( psonProcess        * process,
                                 psonSession       ** ppSession,
                                 psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( process  != NULL );
   PSO_PRE_CONDITION( ppSession != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   ok = psonLinkedListPeakFirst( &process->listOfSessions, &pNode, pContext );
   if ( ok ) {
      *ppSession = (psonSession *)
         ((char*)pNode - offsetof( psonSession, node ));
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, ok );
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessGetNextSession( psonProcess        * process,
                                psonSession        * pCurrent,
                                psonSession       ** ppNext,
                                psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( process != NULL );
   PSO_PRE_CONDITION( pCurrent != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( ppNext   != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   ok = psonLinkedListPeakNext( &process->listOfSessions,
                                &pCurrent->node,
                                &pNode,
                                pContext );
   if ( ok ) {
      *ppNext = (psonSession*)
         ((char*)pNode - offsetof( psonSession, node ));
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, ok );
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessInit( psonProcess        * process,
                      pid_t                pid,
                      psonSessionContext * pContext )
{
   psoErrors errcode;

   PSO_PRE_CONDITION( process != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pid > 0 );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   errcode = psonMemObjectInit( &process->memObject, 
                                PSON_IDENT_PROCESS,
                                &process->blockGroup,
                                1,
                                pContext ); /* A single block */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return false;
   }

   process->pid = pid;
   process->processIsTerminating = false;

   psonLinkedListInit( &process->listOfSessions, pContext );

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessRemoveSession( psonProcess        * process,
                               psonSession        * pSession,
                               psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( process != NULL );
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &process->memObject, pContext ) ) {
      psonLinkedListRemoveItem( &process->listOfSessions, 
                                &pSession->node,
                                pContext );
      
      psonSessionFini( pSession, pContext );

      psonUnlock( &process->memObject, pContext );
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

