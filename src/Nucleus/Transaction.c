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

#include "Transaction.h"
#include "Folder.h"
#include "HashMap.h"
#include "Queue.h"
#include "Nucleus/TransactionHash.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
bool psonAddLockTx( psonTx             * pTx,
                    psonMemObject      * pMemObj,
                    psonSessionContext * pContext )
{
   bool ok;
      
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   ok = txHashInsert( pTx, pMemObj, pContext );
   if ( !ok ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_NOT_ENOUGH_PSO_MEMORY );
   }

   PSO_TRACE_EXIT_NUCLEUS( pContext, ok );
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psonLockTx( psonTx             * pTx,
                 psonMemObject      * pMemObj,
                 psonSessionContext * pContext )
{
   bool ok;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   ok = psocTestLockPidValue( &pMemObj->lock, pContext->pidLocker );
   PSO_POST_CONDITION( ok == true || ok == false );

   if ( ! ok ) {
      ok = psocTryAcquireProcessLock ( &pMemObj->lock,
                                       pContext->pidLocker,
                                       PSON_LOCK_TIMEOUT );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         ok = psonAddLockTx( pTx, pMemObj, pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, ok );
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void psonClearLocks( psonTx             * pTx,
                     psonSessionContext * pContext )
{
   psonMemObject * pMemObj = NULL;
   bool ok;
   size_t rowNumber = 0;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   ok = txHashGetFirst( pTx, &rowNumber, &pMemObj, pContext );
   while ( ok ) {
      psocReleaseProcessLock( &pMemObj->lock );

      ok = txHashGetNext( pTx, rowNumber, &rowNumber, &pMemObj, pContext );
   }
   
   txHashEmpty( pTx, pContext );

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTxInit( psonTx             * pTx,
                 size_t               numberOfBlocks,
                 psonSessionContext * pContext )
{
   psoErrors errcode;
   bool ok;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( numberOfBlocks  > 0 );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   errcode = psonMemObjectInit( &pTx->memObject, 
                                PSON_IDENT_TRANSACTION,
                                &pTx->blockGroup,
                                numberOfBlocks,
                                pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return false;
   }

   psonLinkedListInit( &pTx->listOfOps, pContext );
   ok = txHashInit( pTx, 100, pContext );
   if ( !ok ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, false );
      return false;
   }
   
   pTx->signature = PSON_TX_SIGNATURE;
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonTxFini( psonTx             * pTx, 
                 psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->listOfOps.currentSize == 0 );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   /* Synch the shared memory */
#if 0
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   psonLinkedListFini( &pTx->listOfOps, pContext );
   pTx->signature = 0;
   
   /* This will remove the blocks of allocated memory */
   psonMemObjectFini(  &pTx->memObject, PSON_ALLOC_ANY, pContext );

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTxAddOps( psonTx             * pTx,
                   psonTxType           txType,
                   ptrdiff_t            parentOffset, 
                   psonMemObjIdent      parentType, 
                   ptrdiff_t            childOffset,
                   psonMemObjIdent      childType, 
                   psonSessionContext * pContext )
{
   psonTxOps * pOps;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( parentOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( childOffset  != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   pOps = (psonTxOps *) psonMalloc( &pTx->memObject,
                                    sizeof(psonTxOps), 
                                    pContext );
   if ( pOps != NULL ) {
      pOps->transType    = txType;
      pOps->parentOffset = parentOffset;
      pOps->parentType   = parentType;
      pOps->childOffset  = childOffset;
      pOps->childType    = childType;

      psonLinkNodeInit(  &pOps->node, pContext );
      psonLinkedListPutLast( &pTx->listOfOps, &pOps->node, pContext );

      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return true;
   }

   psocSetError( &pContext->errorHandler, 
                 g_psoErrorHandle, 
                 PSO_NOT_ENOUGH_PSO_MEMORY );

   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonTxRemoveLastOps( psonTx             * pTx, 
                          psonSessionContext * pContext )
{
   bool ok;
   psonLinkNode * pDummy = NULL;
   psonTxOps * pOps;

   PSO_PRE_CONDITION( pTx != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   ok = psonLinkedListGetLast( &pTx->listOfOps, &pDummy, pContext );

   PSO_POST_CONDITION( ok );
   
   pOps = (psonTxOps *)((char *)pDummy - offsetof( psonTxOps, node ));
   
   psonFree( &pTx->memObject,
             (unsigned char *) pOps,
             sizeof(psonTxOps), 
             pContext );
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTxCommit( psonTx             * pTx,
                   psonSessionContext * pContext )
{
   psonTxOps      * pOps = NULL;
   psonLinkNode   * pLinkNode = NULL;
   psonFolder     * parentFolder,    * pChildFolder;
   psonMemObject  * pChildMemObject, * parentMemObject;
   psonTreeNode   * pChildNode, * parentNode;
   psonTxStatus   * pChildStatus;
   psonHashMap    * pHashMap;
   psonQueue      * pQueue;
   psonHashTxItem * pHashItem;
   bool ok, okLock, okDelete;
#ifdef USE_DBC
   int pOps_invalid_type = 0;
#endif

   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
      
   /* Synch the shared memory  (commented out - too slow) */
#if 0
   int errcode = PSO_OK;

   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   if ( pTx->listOfOps.currentSize == 0 ) {
      /* All is well - nothing to commit */
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return true;
   }
   
   /*
    * First pass - we lock all objects of interest. This is the only place 
    * were we can return false.
    */
   ok = psonLinkedListPeakFirst( &pTx->listOfOps, &pLinkNode, pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   while ( ok ) {
      pChildMemObject = parentMemObject = NULL;
      
      pOps = (psonTxOps*) ((char*)pLinkNode - offsetof( psonTxOps, node ));

      /* We always lock the parent */
      GET_PTR(pContext->pBaseAddress, parentMemObject, pOps->parentOffset, psonMemObject );
      okLock = psonLockTx( pTx, parentMemObject, pContext );
      PSO_POST_CONDITION( okLock == true || okLock == false );
      if ( ! okLock ) {
         psonClearLocks( pTx, pContext );
         PSO_TRACE_EXIT_NUCLEUS( pContext, false );
         return false;
      }
      /* We only lock the child for these two ops */
      if ( pOps->transType ==  PSON_TX_ADD_OBJECT || 
         pOps->transType == PSON_TX_REMOVE_OBJECT ) {

         GET_PTR(pContext->pBaseAddress, pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR(pContext->pBaseAddress, pChildNode, pHashItem->dataOffset, psonTreeNode );
         GET_PTR(pContext->pBaseAddress, pChildMemObject, pChildNode->offset, psonMemObject );
         
         okLock = psonLockTx( pTx, pChildMemObject, pContext );
         PSO_POST_CONDITION( okLock == true || okLock == false );
         if ( ! okLock ) {
            psonClearLocks( pTx, pContext );
            PSO_TRACE_EXIT_NUCLEUS( pContext, false );
            return false;
         }
      }

      ok = psonLinkedListPeakNext( &pTx->listOfOps, 
                                   pLinkNode, &pLinkNode, pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   
   /*
    * Second pass - we commit all operations.
    */
   ok = psonLinkedListPeakFirst( &pTx->listOfOps, &pLinkNode, pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   while ( ok ) {

      parentFolder = pChildFolder = NULL;
      pChildMemObject = parentMemObject = NULL;
      pChildNode = NULL;
      pChildStatus = NULL;
      pHashMap = NULL;
      pQueue   = NULL;
      
      pOps = (psonTxOps*)
         ((char*)pLinkNode - offsetof( psonTxOps, node ));
      
      switch( pOps->transType ) {

      case PSON_TX_ADD_DATA:

         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR(pContext->pBaseAddress, pHashMap, pOps->parentOffset, psonHashMap );
            parentMemObject = &pHashMap->memObject;

            psonHashMapCommitAdd( pHashMap, pOps->childOffset, pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR(pContext->pBaseAddress, pQueue, pOps->parentOffset, psonQueue );
            parentMemObject = &pQueue->memObject;

            psonQueueCommitAdd( pQueue, pOps->childOffset, pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

      case PSON_TX_ADD_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR(pContext->pBaseAddress, parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR(pContext->pBaseAddress, pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR(pContext->pBaseAddress, pChildNode, pHashItem->dataOffset, psonTreeNode );
         GET_PTR(pContext->pBaseAddress, pChildMemObject, pChildNode->offset, psonMemObject );
         pChildStatus = &pHashItem->txStatus;
         GET_PTR(pContext->pBaseAddress, parentNode, parentFolder->nodeOffset, psonTreeNode );
         
         psonTxStatusClearTx( pChildStatus, pContext );
         parentNode->txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );

         break;

      case PSON_TX_ADD_EDIT_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR(pContext->pBaseAddress, parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR(pContext->pBaseAddress, pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR(pContext->pBaseAddress, pChildNode, pHashItem->dataOffset, psonTreeNode );
         GET_PTR(pContext->pBaseAddress, parentNode, parentFolder->nodeOffset, psonTreeNode );

         pChildMemObject = NULL;
         psonFolderCommitEdit( parentFolder, pHashItem, pOps->childType, 
                               &pChildMemObject, pContext );
         /*
          * If the oldest version of the map is deleted from memory (no current
          * access), its pointer is returned in pChildMemObject so that we
          * can avoid trying to unlocking it, if we hold the lock.
          */
         if ( pChildMemObject ) {
            /*
             * Note: the old version might be locked by this transaction or
             * it might not. We clear it just in case and do not check the
             * return code (which would tell us if the lock was taken or not).
             */
            txHashDelete( pTx, pChildMemObject, pContext );
         }
         
         parentNode->txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );

         break;

      case PSON_TX_REMOVE_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR(pContext->pBaseAddress, parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR(pContext->pBaseAddress, pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR(pContext->pBaseAddress, pChildNode, pHashItem->dataOffset, psonTreeNode );
         GET_PTR(pContext->pBaseAddress, pChildMemObject, pChildNode->offset, psonMemObject );
         pChildStatus = &pHashItem->txStatus;

         if ( pChildStatus->usageCounter > 0 || 
            pChildNode->txCounter > 0  || pChildStatus->parentCounter > 0 ) {
            /* 
             * We can't remove it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            psonTxStatusCommitRemove( pChildStatus, pContext );
         }
         else {
            /* 
             * Remove it from the folder (from the hash list)
             * (this function also decrease the txCounter of the parentFolder 
             */
            psonFolderRemoveObject( parentFolder, 
                                    pHashItem,
                                    pContext );

            okDelete = txHashDelete( pTx, pChildMemObject, pContext );
         }

         break;

      case PSON_TX_REMOVE_DATA:
         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR(pContext->pBaseAddress, pHashMap, pOps->parentOffset, psonHashMap );
            parentMemObject = &pHashMap->memObject;

            psonHashMapCommitRemove( pHashMap,
                                     pOps->childOffset,
                                     pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR(pContext->pBaseAddress, pQueue, pOps->parentOffset, psonQueue );
            parentMemObject = &pQueue->memObject;

            psonQueueCommitRemove( pQueue,
                                   pOps->childOffset,
                                   pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

      } /* end of switch on type of ops */

      ok = psonLinkedListPeakNext( &pTx->listOfOps, 
                                   pLinkNode, &pLinkNode, pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
                
   } /* end of while loop on the list of ops */

   /*
    * Third pass: we unlock everything and release the memory of the
    * transaction list.
    */
   psonClearLocks( pTx, pContext );

   while ( psonLinkedListGetFirst( &pTx->listOfOps, &pLinkNode, pContext ) ) {
      pOps = (psonTxOps*) ((char*)pLinkNode - offsetof( psonTxOps, node ));
      psonFree( &pTx->memObject, (unsigned char*) pOps, sizeof(psonTxOps), 
                pContext );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTxRollback( psonTx             * pTx,
                     psonSessionContext * pContext )
{
   psonTxOps     * pOps = NULL;
   psonLinkNode  * pLinkNode = NULL;
   psonFolder    * parentFolder,    * pChildFolder;
   psonMemObject * pChildMemObject, * parentMemObject;
   psonTreeNode  * pChildNode, * parentNode;
   psonTxStatus  * pChildStatus;
   psonHashMap   * pHashMap;
   psonQueue     * pQueue;
   psonHashTxItem  * pHashItem;
   bool isRemoved, ok, okLock, okDelete;
#ifdef USE_DBC
   int pOps_invalid_type = 0;
#endif
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

#if 0
   /* Synch the shared memory */
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   if ( pTx->listOfOps.currentSize == 0 ) {
      /* All is well - nothing to rollback */
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return true;
   }
   
   /*
    * First pass - we lock all objects of interest. This is the only place 
    * were we can return false.
    */
   ok = psonLinkedListPeakFirst( &pTx->listOfOps, &pLinkNode, pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   while ( ok ) {
      pChildMemObject = parentMemObject = NULL;
      
      pOps = (psonTxOps*) ((char*)pLinkNode - offsetof( psonTxOps, node ));

      /* We always lock the parent */
      GET_PTR(pContext->pBaseAddress, parentMemObject, pOps->parentOffset, psonMemObject );
      okLock = psonLockTx( pTx, parentMemObject, pContext );
      PSO_POST_CONDITION( okLock == true || okLock == false );
      if ( ! okLock ) {
         psonClearLocks( pTx, pContext );
         PSO_TRACE_EXIT_NUCLEUS( pContext, false );
         return false;
      }
      /* We only lock the child for these two ops */
      if ( pOps->transType ==  PSON_TX_ADD_OBJECT || 
         pOps->transType == PSON_TX_REMOVE_OBJECT ) {

         GET_PTR(pContext->pBaseAddress, pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR(pContext->pBaseAddress, pChildNode, pHashItem->dataOffset, psonTreeNode );
         GET_PTR(pContext->pBaseAddress, pChildMemObject, pChildNode->offset, psonMemObject );
         
         okLock = psonLockTx( pTx, pChildMemObject, pContext );
         PSO_POST_CONDITION( okLock == true || okLock == false );
         if ( ! okLock ) {
            psonClearLocks( pTx, pContext );
            PSO_TRACE_EXIT_NUCLEUS( pContext, false );
            return false;
         }
      }

      ok = psonLinkedListPeakNext( &pTx->listOfOps, 
                                   pLinkNode, &pLinkNode, pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
   }

   /*
    * Second pass - we rollback all operations.
    */
   ok = psonLinkedListPeakLast( &pTx->listOfOps, &pLinkNode, pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   while ( ok ) {
      parentFolder = pChildFolder = NULL;
      pChildMemObject = parentMemObject = NULL;
      pChildNode   = NULL;
      pChildStatus = NULL;
      pHashMap     = NULL;
      pQueue       = NULL;
      
      pOps = (psonTxOps*)
         ((char*)pLinkNode - offsetof( psonTxOps, node ));

      switch( pOps->transType ) {

      case PSON_TX_ADD_DATA:
         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR(pContext->pBaseAddress, pHashMap, pOps->parentOffset, psonHashMap );
            parentMemObject = &pHashMap->memObject;

            psonHashMapRollbackAdd( pHashMap, 
                                    pOps->childOffset,
                                    pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR(pContext->pBaseAddress, pQueue, pOps->parentOffset, psonQueue );
            parentMemObject = &pQueue->memObject;

            psonQueueRollbackAdd( pQueue, 
                                  pOps->childOffset,
                                  pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;
            
      case PSON_TX_ADD_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR(pContext->pBaseAddress, parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR(pContext->pBaseAddress, pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR(pContext->pBaseAddress, pChildNode, pHashItem->dataOffset, psonTreeNode );
         GET_PTR(pContext->pBaseAddress, pChildMemObject, pChildNode->offset, psonMemObject );
         pChildStatus = &pHashItem->txStatus;

         if ( pChildStatus->usageCounter > 0 || 
            pChildNode->txCounter > 0  || pChildStatus->parentCounter > 0 ) {
            /*
             * Can this really happen? Yes! No other session is supposed to
             * be able to open the object until CREATE is committed but
             * the current session might have open it to insert data and... 
             * (the rollback might be the result of an abnormal error, for
             * example or a call to psoExit() or...).
             */
            /** \todo Revisit this. Maybe rolling back a create object should
             * also automatically close the object if open. */
            /* 
             * We can't "uncreate" it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            psonTxStatusCommitRemove( pChildStatus, pContext );
         }
         else {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */

            /* Remove it from the folder (from the hash list) */
            psonFolderRemoveObject( parentFolder,
                                    pHashItem,
                                    pContext );
            okDelete = txHashDelete( pTx, pChildMemObject, pContext );
         }

         break;

      case PSON_TX_ADD_EDIT_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR(pContext->pBaseAddress, parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR(pContext->pBaseAddress, pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR(pContext->pBaseAddress, parentNode, parentFolder->nodeOffset, psonTreeNode );
         
         psonFolderRollbackEdit( parentFolder, pHashItem, pOps->childType, 
                                 &isRemoved, pContext );
         
         parentNode->txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );

         break;

      case PSON_TX_REMOVE_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR(pContext->pBaseAddress, parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR(pContext->pBaseAddress, pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR(pContext->pBaseAddress, parentNode, parentFolder->nodeOffset, psonTreeNode );
         pChildStatus = &pHashItem->txStatus;

         psonTxStatusClearTx( pChildStatus, pContext );
         parentNode->txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );
         
         break;

      case PSON_TX_REMOVE_DATA:

         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR(pContext->pBaseAddress, pHashMap, pOps->parentOffset, psonHashMap );
            parentMemObject = &pHashMap->memObject;

            psonHashMapRollbackRemove( pHashMap, 
                                       pOps->childOffset, pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR(pContext->pBaseAddress, pQueue, pOps->parentOffset, psonQueue );
            parentMemObject = &pQueue->memObject;

            psonQueueRollbackRemove( pQueue, 
                                     pOps->childOffset,
                                     pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

      } /* end of switch on type of ops */

      ok = psonLinkedListPeakPrevious( &pTx->listOfOps, 
                                       pLinkNode, &pLinkNode, pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
                
   } /* end of while loop on the list of ops */

   /*
    * Third pass: we unlock everything and release the memory of the
    * transaction list.
    */
   psonClearLocks( pTx, pContext );

   while ( psonLinkedListGetFirst( &pTx->listOfOps, &pLinkNode, pContext ) ) {
      pOps = (psonTxOps*) ((char*)pLinkNode - offsetof( psonTxOps, node ));
      psonFree( &pTx->memObject, (unsigned char*) pOps, sizeof(psonTxOps), 
                pContext );
   }

   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

