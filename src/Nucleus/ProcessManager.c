/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/ProcessManager.h"
#include "Nucleus/Process.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcMgrAddProcess( psonProcMgr        * pManager,
                            pid_t                pid, 
                            psonProcess       ** process,
                            psonSessionContext * pContext )
{
   psonProcess* pCurrentBuffer;
   bool ok = false;
   
   PSO_PRE_CONDITION( pManager  != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( process != NULL );
   PSO_PRE_CONDITION( pid > 0 );
   PSO_TRACE_ENTER( pContext );

   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pManager->memObject, pContext ) ) {
      pCurrentBuffer = (psonProcess*)
         psonMallocBlocks( pContext->pAllocator, PSON_ALLOC_ANY, 1, pContext );
      if ( pCurrentBuffer != NULL ) {
         ok = psonProcessInit( pCurrentBuffer, pid, pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ok ) {
            psonLinkNodeInit( &pCurrentBuffer->node, pContext );
            psonLinkedListPutLast( &pManager->listOfProcesses, 
                                   &pCurrentBuffer->node,
                                   pContext );
            *process = pCurrentBuffer;
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
      psonUnlock( &pManager->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_ENGINE_BUSY );
   }
   
   PSO_TRACE_EXIT( pContext, ok );
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonProcMgrDump( psonProcMgr * pManager, int indent )
{
   DO_INDENT( indent );
   fprintf( stderr, "psonProcMgr (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pManager, SET_OFFSET(pManager) );
   if ( pManager == NULL ) return;

   psonMemObjectDump( &pManager->memObject, indent + 2 );

   psonLinkedListDump( &pManager->listOfProcesses, indent + 2 );

   psonBlockGroupDump( &pManager->blockGroup, indent + 2 );

   DO_INDENT( indent );
   fprintf( stderr, "psonProcMgr END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcMgrFindProcess( psonProcMgr        * pManager,
                             pid_t                pid,
                             psonProcess       ** process,
                             psonSessionContext * pContext )
{
   psonProcess *pCurrent, *pNext;
   psonLinkNode * pNodeCurrent = NULL, * pNodeNext = NULL;
   psoErrors errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pManager  != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( process != NULL );
   PSO_PRE_CONDITION( pid > 0 );
   PSO_TRACE_ENTER( pContext );
   
   *process = NULL;
   
   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pManager->memObject, pContext ) ) {
      ok = psonLinkedListPeakFirst( &pManager->listOfProcesses, 
                                    &pNodeCurrent,
                                    pContext );
      if ( ok ) {
         pCurrent = (psonProcess*)
            ((char*)pNodeCurrent - offsetof( psonProcess, node ));
         if ( pCurrent->pid == pid ) *process = pCurrent;
      
         while ( (*process == NULL) &&
                 psonLinkedListPeakNext( &pManager->listOfProcesses, 
                                         pNodeCurrent, 
                                         &pNodeNext,
                                         pContext ) ) {
            pNext = (psonProcess*)
               ((char*)pNodeNext - offsetof( psonProcess, node ));
            if ( pNext->pid == pid ) *process = pNext;
            pNodeCurrent = pNodeNext;
         }
      }
      else {
         errcode = PSO_INTERNAL_ERROR;
      }
      
      psonUnlock( &pManager->memObject, pContext );
   }
   else {
      errcode = PSO_ENGINE_BUSY;
   }
   
   /* Is this possible ? */
   if ( *process == NULL ) errcode = PSO_INTERNAL_ERROR;

   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    errcode );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }

   PSO_TRACE_EXIT( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcMgrInit( psonProcMgr        * pManager,
                      psonSessionContext * pContext )
{
   psoErrors errcode;

   PSO_PRE_CONDITION( pManager != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER( pContext );

   errcode = psonMemObjectInit( &pManager->memObject, 
                                PSON_IDENT_PROCESS_MGR,
                                &pManager->blockGroup,
                                1,
                                pContext ); /* A single block */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }

   psonLinkedListInit( &pManager->listOfProcesses, pContext );
      
   PSO_TRACE_EXIT( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcMgrRemoveProcess( psonProcMgr        * pManager,
                               psonProcess        * process,
                               psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pManager != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( process != NULL );
   PSO_TRACE_ENTER( pContext );
   
   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pManager->memObject, pContext ) ) {
      psonLinkedListRemoveItem( &pManager->listOfProcesses, 
                                &process->node,
                                pContext );
   
      psonProcessFini( process, pContext );
                      
      psonUnlock( &pManager->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_ENGINE_BUSY );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   
   PSO_TRACE_EXIT( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

