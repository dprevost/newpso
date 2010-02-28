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

#include "Common/Common.h"
#include <photon/Folder.h>
#include "API/Folder.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
 * 
 * This file contains the source code for some functions of the C API. 
 * For documentation for these functions, please see the distributed
 * header files.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderClose( PSO_HANDLE objectHandle )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pFolder->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pFolder->object.pSession->context );

   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pFolder->iterator.pHashItem != NULL ) {
      ok = psonFolderRelease( pMemFolder,
                              &pFolder->iterator,
                              &pFolder->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
      memset( &pFolder->iterator, 0, sizeof(psonFolderItem) );
   }

   errcode = psoaCommonObjClose( &pFolder->object );
   
   /*
    * Memory might still be around even after it is released, so we make 
    * sure future access with the handle fails by setting the type wrong!
    */
   pFolder->object.type = 0; 
   free( pFolder );

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return errcode;

error_handler:

   psocSetError( &pFolder->object.pSession->context.errorHandler, 
      g_psoErrorHandle, errcode );

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderCreateFolder( PSO_HANDLE   objectHandle,
                           const char * objectName,
                           uint32_t     nameLengthInBytes )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   pFolder->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pFolder->object.pSession->context );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }
   
   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderCreateFolder( pMemFolder,
                                   objectName,
                                   nameLengthInBytes,
                                   &pFolder->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderCreateQueue( PSO_HANDLE            objectHandle,
                          const char          * objectName,
                          uint32_t              nameLengthInBytes,
                          psoObjectDefinition * pDefinition )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   pFolder->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pFolder->object.pSession->context );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }
   
   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pDefinition == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( pDefinition->type <= PSO_FOLDER || pDefinition->type >= PSO_HASH_MAP ) {
      errcode = PSO_WRONG_OBJECT_TYPE;
      goto error_handler;
   }

   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderCreateQueue( pMemFolder,
                                  objectName,
                                  nameLengthInBytes,
                                  pDefinition,
                                  &pFolder->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderCreateMap( PSO_HANDLE            objectHandle,
                        const char          * objectName,
                        uint32_t              nameLengthInBytes,
                        psoObjectDefinition * pDefinition,
                        psoKeyDefinition    * pKeyDefinition )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   pFolder->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pFolder->object.pSession->context );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }
   
   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pDefinition == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( pDefinition->type < PSO_HASH_MAP || pDefinition->type > PSO_FAST_MAP ) {
      errcode = PSO_WRONG_OBJECT_TYPE;
      goto error_handler;
   }

   if ( pKeyDefinition == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   
   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderCreateMap( pMemFolder,
                                objectName,
                                nameLengthInBytes,
                                pDefinition,
                                pKeyDefinition,
                                &pFolder->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderDestroyObject( PSO_HANDLE   objectHandle,
                            const char * objectName,
                            uint32_t     nameLengthInBytes )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   pFolder->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pFolder->object.pSession->context );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }
   
   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderDestroyObject( pMemFolder,
                                    objectName,
                                    nameLengthInBytes,
                                    &pFolder->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetDefinition( PSO_HANDLE            objectHandle,
                            const char          * objectName,
                            uint32_t              nameLengthInBytes,
                            psoObjectDefinition * pDefinition,
                            uint32_t              length )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
   uint32_t myLength;

   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   if ( pFolder->object.pSession->terminated ) {
      return PSO_SESSION_IS_TERMINATED;
   }

   pContext = &pFolder->object.pSession->context;
   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pDefinition == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   
   if ( length < sizeof(psoObjectDefinition) ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderGetDefinition( pMemFolder,
                                    objectName,
                                    nameLengthInBytes,
                                    &pMyDefinition,
                                    &pMyKeyDefinition,
                                    pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   PSO_POST_CONDITION( pMyDefinition != NULL );

   myLength = offsetof(psoObjectDefinition, dataDef) + 
      pMyDefinition->dataDefLength;
   if ( myLength >= length ) {
      // possibly truncated. This is ok
      memcpy( pDefinition, pMyDefinition, length );
   }
   else {
      // Make sure that the "leftover" is zeroed
      memset( pDefinition, 0, length );
      memcpy( pDefinition, pMyDefinition, myLength );
   }

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetDefLength( PSO_HANDLE   objectHandle,
                           const char * objectName,
                           uint32_t     nameLengthInBytes,
                           uint32_t   * pLength )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition    * pMyKeyDefinition = NULL;

   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   if ( pFolder->object.pSession->terminated ) {
      return PSO_SESSION_IS_TERMINATED;
   }

   pContext = &pFolder->object.pSession->context;
   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *pLength = 0;
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderGetDefinition( pMemFolder,
                                    objectName,
                                    nameLengthInBytes,
                                    &pMyDefinition,
                                    &pMyKeyDefinition,
                                    pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   PSO_POST_CONDITION( pMyDefinition != NULL );

   *pLength = offsetof(psoObjectDefinition, dataDef) + 
      pMyDefinition->dataDefLength;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetFirst( PSO_HANDLE       objectHandle,
                       psoFolderEntry * pEntry )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   psonTreeNode * pNode;
   bool ok;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pFolder->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pFolder->object.pSession->context );

   if ( pEntry == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pFolder->iterator.pHashItem != NULL ) {
      ok = psonFolderRelease( pMemFolder,
                              &pFolder->iterator,
                              &pFolder->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      memset( &pFolder->iterator, 0, sizeof(psonFolderItem) );
   }

   ok = psonAPIFolderGetFirst( pMemFolder,
                               &pFolder->iterator,
                               &pFolder->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   memset( pEntry, 0, sizeof( psoFolderEntry ) );
   GET_PTR(g_pBaseAddr,  pNode, pFolder->iterator.pHashItem->dataOffset, 
                         psonTreeNode );
   pEntry->type = pNode->apiType;
   pEntry->status = pFolder->iterator.status;
   pEntry->nameLengthInBytes = pFolder->iterator.pHashItem->keyLength;
   memcpy( pEntry->name,
           pFolder->iterator.pHashItem->key,
           pFolder->iterator.pHashItem->keyLength );

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetKeyDefinition( PSO_HANDLE         objectHandle,
                               const char       * objectName,
                               uint32_t           nameLengthInBytes,
                               psoKeyDefinition * pDefinition,
                               psoUint32          length )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
   uint32_t myLength;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   if ( pFolder->object.pSession->terminated ) {
      return PSO_SESSION_IS_TERMINATED;
   }

   pContext = &pFolder->object.pSession->context;
   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pDefinition == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   
   if ( length < sizeof(psoKeyDefinition) ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }

   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderGetDefinition( pMemFolder,
                                    objectName,
                                    nameLengthInBytes,
                                    &pMyDefinition,
                                    &pMyKeyDefinition,
                                    pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_POST_CONDITION( pMyKeyDefinition != NULL );
   
   myLength = offsetof(psoKeyDefinition, definition) + 
      pMyKeyDefinition->definitionLength;
   if ( myLength >= length ) {
      // possibly truncated. This is ok
      memcpy( pDefinition, pMyKeyDefinition, length );
   }
   else {
      // Make sure that the "leftover" is zeroed
      memset( pDefinition, 0, length );
      memcpy( pDefinition, pMyKeyDefinition, myLength );
   }

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:

   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetKeyDefLength( PSO_HANDLE   objectHandle,
                              const char * objectName,
                              uint32_t     nameLengthInBytes,
                              psoUint32  * pLength )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   if ( pFolder->object.pSession->terminated ) {
      return PSO_SESSION_IS_TERMINATED;
   }

   pContext = &pFolder->object.pSession->context;
   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *pLength = 0;
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderGetDefinition( pMemFolder,
                                    objectName,
                                    nameLengthInBytes,
                                    &pMyDefinition,
                                    &pMyKeyDefinition,
                                    pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_POST_CONDITION( pMyKeyDefinition != NULL );
   
   *pLength = offsetof(psoKeyDefinition, definition) + 
      pMyKeyDefinition->definitionLength;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:

   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetNext( PSO_HANDLE       objectHandle,
                      psoFolderEntry * pEntry )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   psonTreeNode * pNode;
   bool ok;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pFolder->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pFolder->object.pSession->context );

   if ( pEntry == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( pFolder->iterator.pHashItem == NULL ) {
      errcode = PSO_INVALID_ITERATOR;
      goto error_handler;
   }

   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderGetNext( pMemFolder,
                              &pFolder->iterator,
                              &pFolder->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   memset( pEntry, 0, sizeof( psoFolderEntry ) );
   GET_PTR(g_pBaseAddr,  pNode, pFolder->iterator.pHashItem->dataOffset, psonTreeNode );
   pEntry->type = pNode->apiType;
   pEntry->status = pFolder->iterator.status;
   pEntry->nameLengthInBytes = pFolder->iterator.pHashItem->keyLength;
   memcpy( pEntry->name,
           pFolder->iterator.pHashItem->key,
           pFolder->iterator.pHashItem->keyLength );

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderOpen( PSO_HANDLE   sessionHandle,
                   const char * folderName,
                   uint32_t     nameLengthInBytes,
                   PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaFolder * pFolder = NULL;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pSession->context );

   if ( folderName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }
   
   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   pFolder = (psoaFolder *) malloc(sizeof(psoaFolder));
   if (  pFolder == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }
   
   memset( pFolder, 0, sizeof(psoaFolder) );
   pFolder->object.type = PSOA_FOLDER;
   pFolder->object.pSession = pSession;

   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   errcode = psoaCommonObjOpen( &pFolder->object,
                                PSO_FOLDER,
                                PSOA_READ_WRITE,
                                folderName,
                                nameLengthInBytes );
   if ( errcode != PSO_OK ) goto error_handler;

   *objectHandle = (PSO_HANDLE) pFolder;
   
   PSO_TRACE_EXIT_API( &pSession->context, true );
   return PSO_OK;

error_handler:

   psocSetError( &pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   
   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderStatus( PSO_HANDLE     objectHandle,
                     psoObjStatus * pStatus )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pFolder->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pFolder->object.pSession->context );

   if ( pStatus == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   
   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   if ( psonLock( &pMemFolder->memObject, 
                  &pFolder->object.pSession->context ) ) {
      psonMemObjectStatus( &pMemFolder->memObject,
                           pStatus,
                           &pFolder->object.pSession->context );

      psonAPIFolderStatus( pMemFolder,
                           pStatus,
                           &pFolder->object.pSession->context );
      pStatus->type = PSO_FOLDER;
            
      psonUnlock( &pMemFolder->memObject, &pFolder->object.pSession->context );
   }
   else {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto error_handler;
   }

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:

   psocSetError( &pFolder->object.pSession->context.errorHandler, 
      g_psoErrorHandle, errcode );

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaFolderGetDef( PSO_HANDLE             objectHandle,
                      const char           * objectName,
                      uint32_t               nameLengthInBytes,
                      psoObjectDefinition ** ppDefinition )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;

   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   if ( pFolder->object.pSession->terminated ) {
      return PSO_SESSION_IS_TERMINATED;
   }

   pContext = &pFolder->object.pSession->context;
   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( ppDefinition == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderGetDefinition( pMemFolder,
                                    objectName,
                                    nameLengthInBytes,
                                    &pMyDefinition,
                                    &pMyKeyDefinition,
                                    pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   PSO_POST_CONDITION( pMyDefinition != NULL );

   *ppDefinition = pMyDefinition;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaFolderGetKeyDef( PSO_HANDLE          objectHandle,
                         const char        * objectName,
                         uint32_t            nameLengthInBytes,
                         psoKeyDefinition ** ppDefinition )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   if ( pFolder->object.pSession->terminated ) {
      return PSO_SESSION_IS_TERMINATED;
   }

   pContext = &pFolder->object.pSession->context;
   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( objectName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( ppDefinition == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonAPIFolderGetDefinition( pMemFolder,
                                    objectName,
                                    nameLengthInBytes,
                                    &pMyDefinition,
                                    &pMyKeyDefinition,
                                    pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_POST_CONDITION( pMyKeyDefinition != NULL );
   
   *ppDefinition = pMyKeyDefinition;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:

   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

