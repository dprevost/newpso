/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/FastMap.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declaration of static functions */

static
void psonFastMapReleaseNoLock( psonFastMap        * pHashMap,
                               psonHashItem       * pHashItem,
                               psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapCopy( psonFastMap        * pOldMap, 
                      psonFastMap        * pNewMap,
                      psonHashTxItem     * pNewHashItem,
                      psonSessionContext * pContext )
{
   int errcode;
   
   PSO_PRE_CONDITION( pOldMap      != NULL );
   PSO_PRE_CONDITION( pNewMap      != NULL );
   PSO_PRE_CONDITION( pNewHashItem != NULL );
   PSO_PRE_CONDITION( pContext     != NULL );
   PSO_TRACE_ENTER( pContext );
   
   errcode = psonMemObjectInit( &pNewMap->memObject, 
                                PSON_IDENT_MAP,
                                &pNewMap->blockGroup,
                                pOldMap->memObject.totalBlocks,
                                pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }

   pNewMap->nodeOffset = pNewHashItem->dataOffset;
   
   errcode = psonHashInit( &pNewMap->hashObj,
                           SET_OFFSET(&pNewMap->memObject),
                           pOldMap->hashObj.numberOfItems,
                           pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    errcode );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }

   pNewMap->dataDefOffset = pOldMap->dataDefOffset;
   pNewMap->keyDefOffset = pOldMap->keyDefOffset;
   
   errcode = psonHashCopy( &pOldMap->hashObj, &pNewMap->hashObj, pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   pNewMap->latestVersion = pOldMap->latestVersion;
   pOldMap->editVersion = SET_OFFSET( pNewHashItem );
   pNewMap->editVersion = SET_OFFSET( pNewHashItem );
   
   PSO_TRACE_EXIT( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapDelete( psonFastMap        * pHashMap,
                        const void         * pKey,
                        uint32_t             keyLength, 
                        psonSessionContext * pContext )
{
   bool found;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey     != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );
   
   found = psonHashDelWithKey( &pHashMap->hashObj, 
                               (unsigned char *)pKey,
                               keyLength,
                               pContext );
   if ( ! found ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NO_SUCH_ITEM );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }

   /*
    * Note: we do not check the return value of psonHashResize since the
    *       current function removes memory. It would make little sense
    *       to return "not enough memory", specially since the call 
    *       itself did work!
    */
   if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
      psonHashResize( &pHashMap->hashObj, pContext );
   }

   PSO_TRACE_EXIT( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(PSO_USE_TRACE)
void psonFastMapDump( psonFastMap * pMap, int indent )
{
   DO_INDENT( indent );
   fprintf( stderr, "psonFastMap (%p) offset = "PSO_PTRDIFF_T_FORMAT"\n",
      pMap, SET_OFFSET(pMap) );
   if ( pMap == NULL ) return;

   psonMemObjectDump( &pMap->memObject, indent + 2 );

   DO_INDENT( indent + 2 );
   fprintf( stderr, "Node offset: "PSO_PTRDIFF_T_FORMAT"\n", pMap->nodeOffset );

   psonHashDump( &pMap->hashObj, indent + 2 );

   DO_INDENT( indent + 2 );
   fprintf( stderr, "Data definition offset: "PSO_PTRDIFF_T_FORMAT"\n",
      pMap->dataDefOffset );

   DO_INDENT( indent + 2 );
   fprintf( stderr, "Key definition offset: "PSO_PTRDIFF_T_FORMAT"\n",
      pMap->keyDefOffset );

   DO_INDENT( indent + 2 );
   fprintf( stderr, "Latest version offset: "PSO_PTRDIFF_T_FORMAT"\n",
      pMap->latestVersion );

   DO_INDENT( indent + 2 );
   fprintf( stderr, "Edit version offset: "PSO_PTRDIFF_T_FORMAT"\n",
      pMap->editVersion );

   psonBlockGroupDump( &pMap->blockGroup, indent + 2 );

   DO_INDENT( indent );
   fprintf( stderr, "psonFastMap END\n" );
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFastMapEmpty( psonFastMap        * pHashMap,
                       psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );
   
   psonHashEmpty( &pHashMap->hashObj, pContext );

   /*
    * Note: we do not check the return value of psonHashResize since the
    *       current function removes memory. It would make little sense
    *       to return "not enough memory", specially since the call 
    *       itself did work!
    */
   if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
      psonHashResize( &pHashMap->hashObj, pContext );
   }
   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFastMapFini( psonFastMap        * pHashMap,
                      psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );

   psonHashFini( &pHashMap->hashObj, pContext );
   
   /* 
    * Must be the last call since it will release the blocks of
    * memory to the allocator.
    */
   psonMemObjectFini(  &pHashMap->memObject, PSON_ALLOC_API_OBJ, pContext );

   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapGet( psonFastMap        * pHashMap,
                     const void         * pKey,
                     uint32_t             keyLength, 
                     psonHashItem      ** ppHashItem,
                     uint32_t             bufferLength,
                     psonSessionContext * pContext )
{
   psonHashItem* pHashItem = NULL;
   psoErrors errcode;
   psonTxStatus * txHashMapStatus;
   size_t bucket;
   bool found;
   psonTreeNode * pMapNode = NULL;
   
   PSO_PRE_CONDITION( pHashMap   != NULL );
   PSO_PRE_CONDITION( pKey       != NULL );
   PSO_PRE_CONDITION( ppHashItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );

   GET_PTR( pMapNode, pHashMap->nodeOffset, psonTreeNode );
   GET_PTR( txHashMapStatus, pMapNode->txStatusOffset, psonTxStatus );
   
   if ( txHashMapStatus->status & PSON_TXS_DESTROYED || 
      txHashMapStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
      errcode = PSO_OBJECT_IS_DELETED;
      goto the_exit;
   }
   
   found = psonHashGet( &pHashMap->hashObj, 
                        (unsigned char *)pKey, 
                        keyLength,
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      errcode = PSO_NO_SUCH_ITEM;
      goto the_exit;
   }

   /*
    * This test cannot be done in the API (before calling the current
    * function) since we do not know the item size. It could be done
    * after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      errcode = PSO_INVALID_LENGTH;
      goto the_exit;
   }
      
   /*
    * We must increment the usage counter since we are passing back
    * a pointer to the data, not a copy. 
    */
   txHashMapStatus->usageCounter++;
   *ppHashItem = pHashItem;
   
   PSO_TRACE_EXIT( pContext, true );
   return true;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   PSO_TRACE_EXIT( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapGetFirst( psonFastMap        * pHashMap,
                          psonFastMapItem    * pItem,
                          uint32_t             keyLength,
                          uint32_t             bufferLength,
                          psonSessionContext * pContext )
{
   psonHashItem * pHashItem = NULL;
   psonTxStatus * txHashMapStatus;
   bool found;
   psonTreeNode * pMapNode = NULL;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );

   GET_PTR( pMapNode, pHashMap->nodeOffset, psonTreeNode );
   GET_PTR( txHashMapStatus, pMapNode->txStatusOffset, psonTxStatus );

   if ( txHashMapStatus->status & PSON_TXS_DESTROYED || 
      txHashMapStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_IS_DELETED );

      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   
   found = psonHashGetFirst( &pHashMap->hashObj, 
                             &pHashItem,
                             pContext );
   if ( ! found ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_IS_EMPTY );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   
   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, PSO_INVALID_LENGTH );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   if ( keyLength < pHashItem->keyLength ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, PSO_INVALID_LENGTH );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }

   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;

   PSO_TRACE_EXIT( pContext, true );
   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapGetNext( psonFastMap        * pHashMap,
                         psonFastMapItem    * pItem,
                         uint32_t             keyLength,
                         uint32_t             bufferLength,
                         psonSessionContext * pContext )
{
   psonHashItem * pHashItem = NULL;
   psonHashItem * previousHashItem = NULL;
   psonTxStatus * txHashMapStatus;
   bool found;
   psonTreeNode * pMapNode = NULL;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pItem    != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_PRE_CONDITION( pItem->pHashItem  != NULL );
   PSO_TRACE_ENTER( pContext );
   
   GET_PTR( pMapNode, pHashMap->nodeOffset, psonTreeNode );
   GET_PTR( txHashMapStatus, pMapNode->txStatusOffset, psonTxStatus );

   if ( txHashMapStatus->status & PSON_TXS_DESTROYED || 
      txHashMapStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_IS_DELETED );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   
   previousHashItem = pItem->pHashItem;
   
   found = psonHashGetNext( &pHashMap->hashObj, 
                            previousHashItem,
                            &pHashItem,
                            pContext );
   if ( ! found ) {
      /* 
       * If we come here, there are no additional data items to retrieve. As 
       * long as we clearly say that the internal iterator is reset (in case a 
       * "Get Previous" is implemented later), we can just release the iterator
       * at this point.
       */
      pItem->pHashItem = NULL;
      psonFastMapReleaseNoLock( pHashMap, previousHashItem, pContext );
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_REACHED_THE_END );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   
   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, PSO_INVALID_LENGTH );
      return false;
   }
   if ( keyLength < pHashItem->keyLength ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, PSO_INVALID_LENGTH );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }

   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;
   psonFastMapReleaseNoLock( pHashMap, previousHashItem, pContext );

   PSO_TRACE_EXIT( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapInit( psonFastMap         * pHashMap,
                      ptrdiff_t             parentOffset,
                      size_t                numberOfBlocks,
                      size_t                expectedNumOfItems,
                      psonTreeNode        * pNode,
                      ptrdiff_t             hashItemOffset,
                      psoObjectDefinition * pDefinition,
                      psonKeyDefinition   * pKeyDefinition,
                      psonDataDefinition  * pDataDefinition,
                      psonSessionContext  * pContext )
{
   psoErrors errcode;
   
   PSO_PRE_CONDITION( pHashMap        != NULL );
   PSO_PRE_CONDITION( pContext        != NULL );
   PSO_PRE_CONDITION( pNode           != NULL );
   PSO_PRE_CONDITION( pDefinition     != NULL );
   PSO_PRE_CONDITION( pKeyDefinition  != NULL );
   PSO_PRE_CONDITION( pDataDefinition != NULL );
   PSO_PRE_CONDITION( hashItemOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( parentOffset   != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks > 0 );
   PSO_TRACE_ENTER( pContext );
   
   errcode = psonMemObjectInit( &pHashMap->memObject, 
                                PSON_IDENT_MAP,
                                &pHashMap->blockGroup,
                                numberOfBlocks,
                                pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }

   pHashMap->nodeOffset = SET_OFFSET( pNode );
   
   errcode = psonHashInit( &pHashMap->hashObj, 
                           SET_OFFSET(&pHashMap->memObject),
                           expectedNumOfItems, 
                           pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   
   pHashMap->dataDefOffset = SET_OFFSET(pDataDefinition);
   pHashMap->keyDefOffset  = SET_OFFSET(pKeyDefinition);
   pHashMap->latestVersion = hashItemOffset;
   pHashMap->editVersion   = PSON_NULL_OFFSET;
   
   PSO_TRACE_EXIT( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapInsert( psonFastMap        * pHashMap,
                        const void         * pKey,
                        uint32_t             keyLength, 
                        const void         * pItem,
                        uint32_t             itemLength,
                        psonSessionContext * pContext )
{
   psoErrors errcode;
   psonHashItem * pHashItem = NULL;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey     != NULL )
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( itemLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );

   errcode = psonHashInsert( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pItem, 
                             itemLength,
                             &pHashItem,
                             pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   
   PSO_TRACE_EXIT( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFastMapRelease( psonFastMap        * pHashMap,
                         psonHashItem       * pHashItem,
                         psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHashMap  != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );

   psonFastMapReleaseNoLock( pHashMap,
                             pHashItem,
                             pContext );
   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * This version of Release is only called internally, by the current
 * object. Always use the standard one when calling from the API.
 */
static
void psonFastMapReleaseNoLock( psonFastMap        * pHashMap,
                               psonHashItem       * pHashItem,
                               psonSessionContext * pContext )
{
   psonTxStatus * txHashMapStatus;
   psonTreeNode * pMapNode = NULL;
   
   PSO_PRE_CONDITION( pHashMap  != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );

   GET_PTR( pMapNode, pHashMap->nodeOffset, psonTreeNode );
   GET_PTR( txHashMapStatus, pMapNode->txStatusOffset, psonTxStatus );

   txHashMapStatus->usageCounter--;

   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txHashMapStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of psonHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   if ( txHashMapStatus->usageCounter == 0 ) {
      if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
         psonHashResize( &pHashMap->hashObj, pContext );
      }
   }
   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapReplace( psonFastMap        * pHashMap,
                         const void         * pKey,
                         uint32_t             keyLength, 
                         const void         * pData,
                         uint32_t             dataLength,
                         psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   psonHashItem * pHashItem = NULL;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey     != NULL )
   PSO_PRE_CONDITION( pData    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( dataLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );

   errcode = psonHashUpdate( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pData,
                             dataLength,
                             &pHashItem,
                             pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      PSO_TRACE_EXIT( pContext, false );
      return false;
   }
   
   PSO_TRACE_EXIT( pContext, true );
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFastMapStatus( psonFastMap        * pHashMap,
                        psoObjStatus       * pStatus,
                        psonSessionContext * pContext )
{
   psonHashItem * pHashItem = NULL;
   psonTxStatus  * txHashMapStatus;
   bool found;
   psonTreeNode * pMapNode = NULL;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pStatus  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_TRACE_ENTER( pContext );
   
   GET_PTR( pMapNode, pHashMap->nodeOffset, psonTreeNode );
   GET_PTR( txHashMapStatus, pMapNode->txStatusOffset, psonTxStatus );

   pStatus->status = txHashMapStatus->status;
   pStatus->numDataItem = pHashMap->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) {
      PSO_TRACE_EXIT( pContext, true );
      return;
   }
   
   found = psonHashGetFirst( &pHashMap->hashObj, &pHashItem, pContext );
   while ( found ) {
      if ( pHashItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pHashItem->dataLength;
      }
      if ( pHashItem->keyLength > pStatus->maxKeyLength ) {
         pStatus->maxKeyLength = pHashItem->keyLength;
      }

      found = psonHashGetNext( &pHashMap->hashObj, 
                               pHashItem,
                               &pHashItem,
                               pContext );
   }
   PSO_TRACE_EXIT( pContext, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

