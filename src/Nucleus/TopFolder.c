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

#include "Nucleus/Folder.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/HashMap.h"
#include "Nucleus/FastMap.h"
#include "Nucleus/Queue.h"
#include "Nucleus/MemoryHeader.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next 7 functions should only be used by the API, to create, destroy,
 * open or close a memory object.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderCloseObject( psonFolderItem     * pFolderItem,
                               psonSessionContext * pContext )
{
   psonFolder   * parentFolder;
   psonTxStatus * txItemStatus, * txFolderStatus;
   psonTreeNode * pChildNode, * parentNode;
   
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   GET_PTR(pContext->pBaseAddress, pChildNode, pFolderItem->pHashItem->dataOffset, psonTreeNode );
   
   /* Special case, the top folder */
   if ( pChildNode->myParentOffset == PSON_NULL_OFFSET ) {
      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return true;
   }
   
   GET_PTR(pContext->pBaseAddress, parentFolder, pChildNode->myParentOffset, psonFolder );
   GET_PTR(pContext->pBaseAddress, parentNode, parentFolder->nodeOffset, psonTreeNode );
   GET_PTR(pContext->pBaseAddress, txFolderStatus, parentNode->txStatusOffset, psonTxStatus );
   
   if ( psonLock( &parentFolder->memObject, pContext ) ) {
      GET_PTR(pContext->pBaseAddress, txItemStatus, pChildNode->txStatusOffset, psonTxStatus );
      txItemStatus->parentCounter--;
      txFolderStatus->usageCounter--;
      
      /* 
       * if parentCounter is equal to zero, the object is not open. Since 
       * we hold the lock on the folder, no session can therefore open it
       * or use it in an iteration. We can remove it without problems if
       * a remove was indeed committed.
       */
      if ( (txItemStatus->parentCounter == 0) && 
         (txItemStatus->usageCounter == 0) ) {
         if ( txItemStatus->status & PSON_TXS_DESTROYED_COMMITTED ||
            txItemStatus->status & PSON_TXS_EDIT_COMMITTED ) {
            /* Time to really delete the object and the record! */
            psonFolderRemoveObject( parentFolder,
                                    pFolderItem->pHashItem,
                                    pContext );
         }
      }
      psonUnlock( &parentFolder->memObject, pContext );

      PSO_TRACE_EXIT_NUCLEUS( pContext, true );
      return true;
   }
   
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_ENGINE_BUSY );
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderCreateFolder( psonFolder          * pFolder,
                                const char          * objectName,
                                uint32_t              nameLengthInBytes,
                                psonSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength;
   bool ok;
   uint32_t first = 0;
   const char * name = objectName;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   strLength = nameLengthInBytes;

   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
      if ( strLength == 0 ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderInsertFolder() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock( &pFolder->memObject, pContext ) ) {
      ok = psonFolderInsertFolder( pFolder,
                                   &(name[first]),
                                   strLength, 
                                   pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;

error_handler:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderCreateMap( psonFolder          * pFolder,
                             const char          * objectName,
                             uint32_t              nameLengthInBytes,
                             psoObjectDefinition * pDefinition,
                             psoKeyDefinition    * pKeyDefinition,
                             psonSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength;
   bool ok;
   uint32_t first = 0;
   const char * name = objectName;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pDefinition->type > PSO_FOLDER && 
                      pDefinition->type < PSO_LAST_OBJECT_TYPE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   strLength = nameLengthInBytes;

   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
      if ( strLength == 0 ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderInsertMap() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock( &pFolder->memObject, pContext ) ) {
      ok = psonFolderInsertMap( pFolder,
                                &(name[first]),
                                strLength, 
                                pDefinition,
                                pKeyDefinition,
                                1, /* numBlocks, */
                                0, /* expectedNumOfChilds, */
                                pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;

error_handler:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderCreateQueue( psonFolder          * pFolder,
                               const char          * objectName,
                               uint32_t              nameLengthInBytes,
                               psoObjectDefinition * pDefinition,
                               psonSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength;
   bool ok;
   uint32_t first = 0;
   const char * name = objectName;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pDefinition->type > PSO_FOLDER && 
                      pDefinition->type < PSO_LAST_OBJECT_TYPE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );
   
   strLength = nameLengthInBytes;

   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
      if ( strLength == 0 ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderInsertQueue() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock( &pFolder->memObject, pContext ) ) {
      ok = psonFolderInsertQueue( pFolder,
                                  &(name[first]),
                                  strLength, 
                                  pDefinition,
                                  1, /* numBlocks, */
                                  0, /* expectedNumOfChilds, */
                                  pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;

error_handler:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderDestroyObject( psonFolder         * pFolder,
                                 const char         * objectName,
                                 uint32_t             nameLengthInBytes,
                                 psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength;
   uint32_t first = 0;
   const char * name = objectName;
   bool ok;
   
   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
      if ( strLength == 0 ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderDeleteObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock( &pFolder->memObject, pContext ) ) {
      ok = psonFolderDeleteObject( pFolder,
                                   &(name[first]), 
                                   strLength,
                                   pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;

error_handler:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderEditObject( psonFolder         * pFolder,
                              const char         * objectName,
                              uint32_t             nameLengthInBytes,
                              enum psoObjectType   objectType, 
                              psonFolderItem     * pFolderItem,
                              psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength;
   bool ok;
   uint32_t first = 0;
   const char * name = objectName;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   strLength = nameLengthInBytes;
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* 
       * Opening the top folder (special case). No lock is needed here
       * since all we do is to retrieve the pointer (and we do not 
       * count access since the object is undeletable).
       */
      if ( objectType != PSO_FOLDER ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto error_handler;
      }
      pFolderItem->pHashItem = &((psonMemoryHeader *) pContext->pBaseAddress)->topHashItem;
   }
   else {
      /*
       * There is no psonUnlock here - the recursive nature of the 
       * function psonFolderEditObject() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         ok = psonFolderEditObject( pFolder,
                                    &(name[first]), 
                                    strLength, 
                                    objectType,
                                    pFolderItem,
                                    pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;

error_handler:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderGetDef( psonFolder           * pFolder,
                          const char           * objectName,
                          uint32_t               nameLengthInBytes,
                          psoObjectDefinition ** ppDefinition,
                          psoKeyDefinition    ** ppKeyDefinition,
                          psonSessionContext   * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength;
   bool ok;
   uint32_t first = 0;

   const char * name = objectName;

   PSO_PRE_CONDITION( pFolder          != NULL );
   PSO_PRE_CONDITION( objectName       != NULL );
   PSO_PRE_CONDITION( ppDefinition     != NULL );
   PSO_PRE_CONDITION( ppKeyDefinition  != NULL );
   PSO_PRE_CONDITION( pContext         != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   *ppDefinition = NULL;
   *ppKeyDefinition = NULL;
   
   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* Getting the status of the top folder (special case). */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         *ppDefinition = &pFolder->definition;
         
         psonUnlock( &pFolder->memObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      /*
       * There is no psonUnlock here - the recursive nature of the 
       * function psonFolderGetDefinition() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         ok = psonFolderGetDefinition( pFolder,
                                       &(name[first]), 
                                       strLength, 
                                       ppDefinition,
                                       ppKeyDefinition,
                                       pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;

error_handler:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderGetStatus( psonFolder         * pFolder,
                             const char         * objectName,
                             uint32_t             nameLengthInBytes,
                             psoObjStatus       * pStatus,
                             psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength;
   bool ok;
   uint32_t first = 0;

   const char * name = objectName;

   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( pStatus    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* Getting the status of the top folder (special case). */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         psonMemObjectStatus( &pFolder->memObject, pStatus, pContext );
         pStatus->type = PSO_FOLDER;
         
         psonAPIFolderStatus( pFolder, pStatus, pContext );

         psonUnlock( &pFolder->memObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      /*
       * There is no psonUnlock here - the recursive nature of the 
       * function psonFolderGetStatus() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         ok = psonFolderGetStatus( pFolder,
                                   &(name[first]), 
                                   strLength, 
                                   pStatus,
                                   pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;

error_handler:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderOpenObject( psonFolder         * pFolder,
                              const char         * objectName,
                              uint32_t             nameLengthInBytes,
                              enum psoObjectType   objectType, 
                              psonFolderItem     * pFolderItem,
                              psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength;
   uint32_t first = 0;
   const char * name = objectName;
   bool ok;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );
   PSO_TRACE_ENTER_NUCLEUS( pContext );

   strLength = nameLengthInBytes;
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* 
       * Opening the top folder (special case). No lock is needed here
       * since all we do is to retrieve the pointer (and we do not 
       * count access since the object is undeletable).
       */
      if ( objectType != PSO_FOLDER ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto error_handler;
      }
      pFolderItem->pHashItem = &((psonMemoryHeader *) pContext->pBaseAddress)->topHashItem;
   }
   else {
      /*
       * There is no psonUnlock here - the recursive nature of the 
       * function psonFolderGetObject() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         ok = psonFolderGetObject( pFolder,
                                   &(name[first]), 
                                   strLength, 
                                   objectType,
                                   pFolderItem,
                                   pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, true );
   return true;

error_handler:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   PSO_TRACE_EXIT_NUCLEUS( pContext, false );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

