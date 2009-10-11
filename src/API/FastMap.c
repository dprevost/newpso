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

#include "Common/Common.h"
#include <photon/FastMap.h>
#include "API/FastMap.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"
#include "Nucleus/TreeNode.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapClose( PSO_HANDLE objectHandle )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      psonFastMapRelease( pMemHashMap,
                          pHashMap->iterator.pHashItem,
                          &pHashMap->object.pSession->context );
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   errcode = psoaCommonObjClose( &pHashMap->object );
   if ( errcode != PSO_OK ) goto error_handler;
   
   if ( pHashMap->editMode ) {
      pHashMap->object.pSession->numberOfEdits--;
   }
   else {
      psoaListReadersRemove( &pHashMap->object.pSession->listReaders, 
                             &pHashMap->reader );
   }

   /*
    * Memory might still be around even after it is released, so we make 
    * sure future access with the handle fails by setting the type wrong!
    */
   pHashMap->object.type = 0; 
   free( pHashMap );

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:

   psocSetError( &pHashMap->object.pSession->context.errorHandler, 
      g_psoErrorHandle, errcode );
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapDefinition( PSO_HANDLE            objectHandle,
                          psoObjectDefinition * definition,
                          psoUint32             length )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   uint32_t myLength;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( pHashMap->object.pSession->terminated ) {
      return PSO_SESSION_IS_TERMINATED;
   }

   pContext = &pHashMap->object.pSession->context;

   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( definition == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( length < sizeof(psoObjectDefinition) ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   GET_PTR( pMyDefinition, pMemHashMap->dataDefOffset, psoObjectDefinition );
   myLength = offsetof(psoObjectDefinition, dataDef) + 
      pMyDefinition->dataDefLength;
   if ( myLength >= length ) {
      // possibly truncated. This is ok
      memcpy( definition, pMyDefinition, length );
   }
   else {
      // Make sure that the "leftover" is zeroed
      memset( definition, 0, length );
      memcpy( definition, pMyDefinition, myLength );
   }

   PSO_TRACE_EXIT_API( pContext, true );
   return PSO_OK;

error_handler:

   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   
   PSO_TRACE_EXIT_API( pContext, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapDefLength( PSO_HANDLE   objectHandle,
                         psoUint32  * pLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      return errcode;
   }

   pContext = &pHashMap->object.pSession->context;

   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( pLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *pLength = 0;
   
   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   GET_PTR( pMyDefinition, pMemHashMap->dataDefOffset, psoObjectDefinition );
   *pLength = offsetof(psoObjectDefinition, dataDef) + pMyDefinition->dataDefLength;

   PSO_TRACE_EXIT_API( pContext, true );
   return PSO_OK;

error_handler:

   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   
   PSO_TRACE_EXIT_API( pContext, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapDelete( PSO_HANDLE   objectHandle,
                      const void * key,
                      uint32_t     keyLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );
   
   if ( key == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   if ( keyLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   if ( pHashMap->editMode == 0 ) {
      errcode = PSO_OBJECT_IS_READ_ONLY;
      goto error_handler;
   }

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   ok = psonFastMapDelete( pMemHashMap,
                           key,
                           keyLength,
                           &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapEdit( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    uint32_t     nameLengthInBytes,
                    PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaFastMap * pHashMap = NULL;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pSession->context );

   if ( hashMapName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }
   
   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   pHashMap = (psoaFastMap *) malloc(sizeof(psoaFastMap));
   if (  pHashMap == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }
   
   memset( pHashMap, 0, sizeof(psoaFastMap) );
   pHashMap->object.type = PSOA_MAP;
   pHashMap->object.pSession = pSession;

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   errcode = psoaCommonObjOpen( &pHashMap->object,
                                PSO_FAST_MAP,
                                PSOA_UPDATE_RO,
                                hashMapName,
                                nameLengthInBytes );
   if ( errcode != PSO_OK ) goto error_handler;
   
   *objectHandle = (PSO_HANDLE) pHashMap;
   pHashMap->editMode = 1;
   pSession->numberOfEdits++;

   PSO_TRACE_EXIT_API( &pSession->context, true );
   return PSO_OK;

error_handler:

   if ( pHashMap ) free(pHashMap);
   psocSetError( &pSession->context.errorHandler, 
      g_psoErrorHandle, errcode );
   
   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapEmpty( PSO_HANDLE objectHandle )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( pHashMap->editMode == 0 ) {
      errcode = PSO_OBJECT_IS_READ_ONLY;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      psonFastMapRelease( pMemHashMap,
                          pHashMap->iterator.pHashItem,
                          &pHashMap->object.pSession->context );
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   psonFastMapEmpty( pMemHashMap, &pHashMap->object.pSession->context );

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapGet( PSO_HANDLE   objectHandle,
                   const void * key,
                   uint32_t     keyLength,
                   void       * buffer,
                   uint32_t     bufferLength,
                   uint32_t   * returnedLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( key == NULL || buffer == NULL || returnedLength == NULL) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   if ( keyLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      psonFastMapRelease( pMemHashMap,
                          pHashMap->iterator.pHashItem,
                          &pHashMap->object.pSession->context );
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGet( pMemHashMap,
                        key,
                        keyLength,
                        &pHashMap->iterator.pHashItem,
                        bufferLength,
                        &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *returnedLength );

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapGetFirst( PSO_HANDLE   objectHandle,
                        void       * key,
                        uint32_t     keyLength,
                        void       * buffer,
                        uint32_t     bufferLength,
                        uint32_t   * retKeyLength,
                        uint32_t   * retDataLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( buffer == NULL || retDataLength == NULL || retKeyLength == NULL ||
      key == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *retDataLength = *retKeyLength = 0;
   
   if ( keyLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      psonFastMapRelease( pMemHashMap,
                          pHashMap->iterator.pHashItem,
                          &pHashMap->object.pSession->context );
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGetFirst( pMemHashMap,
                             &pHashMap->iterator,
                             keyLength,
                             bufferLength,
                             &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapGetNext( PSO_HANDLE   objectHandle,
                       void       * key,
                       uint32_t     keyLength,
                       void       * buffer,
                       uint32_t     bufferLength,
                       uint32_t   * retKeyLength,
                       uint32_t   * retDataLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( buffer == NULL || retDataLength == NULL || retKeyLength == NULL ||
      key == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *retDataLength = *retKeyLength = 0;

   if ( keyLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pHashMap->iterator.pHashItem == NULL ) {
      errcode = PSO_INVALID_ITERATOR;
      goto error_handler;
   }
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   ok = psonFastMapGetNext( pMemHashMap,
                            &pHashMap->iterator,
                            keyLength,
                            bufferLength,
                            &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapInsert( PSO_HANDLE   objectHandle,
                      const void * key,
                      uint32_t     keyLength,
                      const void * data,
                      uint32_t     dataLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( key == NULL || data == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   if ( keyLength == 0 || dataLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }

   if ( pHashMap->editMode == 0 ) {
      errcode = PSO_OBJECT_IS_READ_ONLY;
      goto error_handler;
   }

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   ok = psonFastMapInsert( pMemHashMap,
                           key,
                           keyLength,
                           data,
                           dataLength,
                           &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapKeyDefinition( PSO_HANDLE         objectHandle,
                             psoKeyDefinition * definition,
                             psoUint32          length )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoKeyDefinition * pMyDefinition = NULL;
   uint32_t myLength;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( pHashMap->object.pSession->terminated ) return PSO_SESSION_IS_TERMINATED;

   pContext = &pHashMap->object.pSession->context;
   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( definition == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( length < sizeof(psoKeyDefinition) ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   GET_PTR( pMyDefinition, pMemHashMap->keyDefOffset, psoKeyDefinition );
   myLength = offsetof(psoKeyDefinition, definition) + 
      pMyDefinition->definitionLength;
   if ( myLength >= length ) {
      // possibly truncated. This is ok
      memcpy( definition, pMyDefinition, length );
   }
   else {
      // Make sure that the "leftover" is zeroed
      memset( definition, 0, length );
      memcpy( definition, pMyDefinition, myLength );
   }

   PSO_TRACE_EXIT_API( pContext, true );
   return PSO_OK;

error_handler:

   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   
   PSO_TRACE_EXIT_API( pContext, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapKeyDefLength( PSO_HANDLE   objectHandle,
                            psoUint32  * pLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoKeyDefinition * pMyDefinition = NULL;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( pHashMap->object.pSession->terminated ) return PSO_SESSION_IS_TERMINATED;

   pContext = &pHashMap->object.pSession->context;
   pContext->indent = 0;
   PSO_TRACE_ENTER_API( pContext );

   if ( pLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *pLength = 0;
   
   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   GET_PTR( pMyDefinition, pMemHashMap->keyDefOffset, psoKeyDefinition );
   *pLength = offsetof(psoKeyDefinition, definition) + 
      pMyDefinition->definitionLength;

   PSO_TRACE_EXIT_API( pContext, true );
   return PSO_OK;

error_handler:

   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   
   PSO_TRACE_EXIT_API( pContext, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapOpen( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    uint32_t     nameLengthInBytes,
                    PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaFastMap * pHashMap = NULL;
   psonFastMap * pMemHashMap;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pSession->context );

   if ( hashMapName == NULL ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }
   
   if ( nameLengthInBytes == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   pHashMap = (psoaFastMap *) malloc(sizeof(psoaFastMap));
   if (  pHashMap == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }
   
   memset( pHashMap, 0, sizeof(psoaFastMap) );
   pHashMap->object.type = PSOA_MAP;
   pHashMap->object.pSession = pSession;

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   errcode = psoaCommonObjOpen( &pHashMap->object,
                                PSO_FAST_MAP,
                                PSOA_READ_ONLY,
                                hashMapName,
                                nameLengthInBytes );
   if ( errcode != PSO_OK ) goto error_handler;
   
   *objectHandle = (PSO_HANDLE) pHashMap;
   pHashMap->editMode = 0;
   
   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
   
   pHashMap->reader.type = PSOA_MAP;
   pHashMap->reader.address = pHashMap;
   
   psoaListReadersPut( &pHashMap->object.pSession->listReaders, 
                       &pHashMap->reader );

   PSO_TRACE_EXIT_API( &pSession->context, true );
   return PSO_OK;

error_handler:

   if ( pHashMap ) free(pHashMap);
   psocSetError( &pSession->context.errorHandler, 
      g_psoErrorHandle, errcode );
   
   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapReplace( PSO_HANDLE   objectHandle,
                       const void * key,
                       uint32_t     keyLength,
                       const void * data,
                       uint32_t     dataLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( key == NULL || data == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   if ( keyLength == 0 || dataLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }

   if ( pHashMap->editMode == 0 ) {
      errcode = PSO_OBJECT_IS_READ_ONLY;
      goto error_handler;
   }

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   ok = psonFastMapReplace( pMemHashMap,
                            key,
                            keyLength,
                            data,
                            dataLength,
                            &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapStatus( PSO_HANDLE     objectHandle,
                      psoObjStatus * pStatus )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;
   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( pStatus == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
      
   if ( psonLock(&pMemHashMap->memObject, pContext) ) {
      psonMemObjectStatus( &pMemHashMap->memObject, pStatus, pContext );

      psonFastMapStatus( pMemHashMap, pStatus, pContext );
      pStatus->type = PSO_FAST_MAP;

      psonUnlock( &pMemHashMap->memObject, pContext );
   }
   else {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:

   if ( pHashMap ) free(pHashMap);
   psocSetError( &pHashMap->object.pSession->context.errorHandler, 
      g_psoErrorHandle, errcode );
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaFastMapFirst( psoaFastMap    * pHashMap,
                      unsigned char ** pKey,
                      uint32_t       * pKeyLength,
                      unsigned char ** pData,
                      uint32_t       * pDataLength )
{
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pHashMap    != NULL );
   PSO_PRE_CONDITION( pKey        != NULL );
   PSO_PRE_CONDITION( pKeyLength  != NULL );
   PSO_PRE_CONDITION( pData       != NULL );
   PSO_PRE_CONDITION( pDataLength != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      psonFastMapRelease( pMemHashMap,
                          pHashMap->iterator.pHashItem,
                          &pHashMap->object.pSession->context );
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGetFirst( pMemHashMap,
                             &pHashMap->iterator,
                             (uint32_t) -1,
                             (uint32_t) -1,
                             &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   GET_PTR( *pData, pHashMap->iterator.pHashItem->dataOffset, void );
   *pDataLength = pHashMap->iterator.pHashItem->dataLength;
   *pKeyLength = pHashMap->iterator.pHashItem->keyLength;
   *pKey = pHashMap->iterator.pHashItem->key;
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaFastMapNext( psoaFastMap    * pHashMap,
                     unsigned char ** pKey,
                     uint32_t       * pKeyLength,
                     unsigned char ** pData,
                     uint32_t       * pDataLength )
{
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey        != NULL );
   PSO_PRE_CONDITION( pKeyLength  != NULL );
   PSO_PRE_CONDITION( pData       != NULL );
   PSO_PRE_CONDITION( pDataLength != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );
   PSO_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   ok = psonFastMapGetNext( pMemHashMap,
                            &pHashMap->iterator,
                            (uint32_t) -1,
                            (uint32_t) -1,
                            &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   GET_PTR( *pData, pHashMap->iterator.pHashItem->dataOffset, void );
   *pDataLength = pHashMap->iterator.pHashItem->dataLength;
   *pKeyLength = pHashMap->iterator.pHashItem->keyLength;
   *pKey = pHashMap->iterator.pHashItem->key;

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This function is called when a session is committed or rollbacked.
 */
void psoaFastMapResetReader( void * map )
{
   psoaFastMap * pHashMap = map;
   psonFastMap * pMemHashMap, * pMapLatest;

   psonHashTxItem * pHashItemLatest;
   psonTreeNode * pNode;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
   pHashItemLatest = GET_PTR_FAST( pMemHashMap->latestVersion, psonHashTxItem );
   pNode = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                         psonTreeNode );
   pMapLatest = GET_PTR_FAST( pNode->offset, psonFastMap );
   if ( pMapLatest != pMemHashMap ) {
      if ( pHashMap->iterator.pHashItem != NULL ) {
         psonFastMapRelease( pMemHashMap,
                             pHashMap->iterator.pHashItem,
                             &pHashMap->object.pSession->context );
         memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
      }
      pHashMap->object.pMyMemObject = pMapLatest;
   }
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaFastMapRetrieve( psoaFastMap    * pHashMap,
                         const void     * key,
                         uint32_t         keyLength,
                         unsigned char ** pData,
                         uint32_t       * pLength )
{
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   psonHashItem * pHashItem;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( key      != NULL );
   PSO_PRE_CONDITION( pData    != NULL );
   PSO_PRE_CONDITION( pLength  != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      psonFastMapRelease( pMemHashMap,
                          pHashMap->iterator.pHashItem,
                          &pHashMap->object.pSession->context );
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGet( pMemHashMap,
                        key,
                        keyLength,
                        &pHashItem,
                        (uint32_t) -1,
                        &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   GET_PTR( *pData, pHashItem->dataOffset, void );
   *pLength = pHashItem->dataLength;

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

