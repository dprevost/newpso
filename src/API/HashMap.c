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

#include "Common/Common.h"
#include <photon/HashMap.h>
#include "API/HashMap.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapClose( PSO_HANDLE objectHandle )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( ! pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      if ( psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context ) ) {
         memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }

   errcode = psoaCommonObjClose( &pHashMap->object );
   if ( errcode != 0 ) goto error_handler;
   
   /*
    * Memory might still be around even after it is released, so we make 
    * sure future access with the handle fails by setting the type wrong!
    */
   pHashMap->object.type = 0; 
   free( pHashMap );

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapDefinition( PSO_HANDLE   objectHandle,
                          PSO_HANDLE * keyDefHandle,
                          PSO_HANDLE * dataDefHandle )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   psoaDataDefinition * pDataDefinition = NULL;
   psoaKeyDefinition  * pKeyDefinition = NULL;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( keyDefHandle == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   
   if ( dataDefHandle == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   pDataDefinition = malloc( sizeof(psoaDataDefinition) );
   if ( pDataDefinition == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }
   pKeyDefinition = malloc( sizeof(psoaKeyDefinition) );
   if ( pKeyDefinition == NULL ) {
      free( pDataDefinition );
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
   
   pDataDefinition->pSession = pHashMap->object.pSession;
   pDataDefinition->definitionType = PSOA_DEF_DATA;
   GET_PTR( pDataDefinition->pMemDefinition, 
            pMemHashMap->dataDefOffset, 
            psonDataDefinition );
   *dataDefHandle = (PSO_HANDLE) pDataDefinition;

   pKeyDefinition->pSession = pHashMap->object.pSession;
   pKeyDefinition->definitionType = PSOA_DEF_KEY;
   GET_PTR( pKeyDefinition->pMemDefinition, 
            pMemHashMap->keyDefOffset, 
            psonKeyDefinition );
   *keyDefHandle = (PSO_HANDLE) pKeyDefinition;

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   if ( pDataDefinition ) free(pDataDefinition);
   if ( pKeyDefinition)   free(pKeyDefinition);
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapDelete( PSO_HANDLE   objectHandle,
                      const void * key,
                      uint32_t     keyLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) {
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

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   if ( pMemHashMap->isSystemObject ) {
      errcode = PSO_SYSTEM_OBJECT;
      goto error_handler;
   }
   ok = psonHashMapDelete( pMemHashMap,
                           key,
                           keyLength,
                           &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapGet( PSO_HANDLE   objectHandle,
                   const void * key,
                   uint32_t     keyLength,
                   void       * buffer,
                   uint32_t     bufferLength,
                   uint32_t   * returnedLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGet( pMemHashMap,
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
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapGetFirst( PSO_HANDLE   objectHandle,
                        void       * key,
                        uint32_t     keyLength,
                        void       * buffer,
                        uint32_t     bufferLength,
                        uint32_t   * retKeyLength,
                        uint32_t   * retDataLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   void * ptr;
   bool ok;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGetFirst( pMemHashMap,
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
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapGetNext( PSO_HANDLE   objectHandle,
                       void       * key,
                       uint32_t     keyLength,
                       void       * buffer,
                       uint32_t     bufferLength,
                       uint32_t   * retKeyLength,
                       uint32_t   * retDataLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   void * ptr;
   bool ok;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   ok = psonHashMapGetNext( pMemHashMap,
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
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapInsert( PSO_HANDLE   objectHandle,
                      const void * key,
                      uint32_t     keyLength,
                      const void * data,
                      uint32_t     dataLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   bool ok = true;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( key == NULL || data == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   if ( keyLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }

   if ( dataLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }

   if ( ! pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
   if ( pMemHashMap->isSystemObject ) {
      errcode = PSO_SYSTEM_OBJECT;
      goto error_handler;
   }

   ok = psonHashMapInsert( pMemHashMap,
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
   if ( errcode != 0 ) {
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

int psoHashMapOpen( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    uint32_t     nameLengthInBytes,
                    PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaHashMap * pHashMap = NULL;
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
   
   pHashMap = (psoaHashMap *) malloc(sizeof(psoaHashMap));
   if ( pHashMap == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }
   
   memset( pHashMap, 0, sizeof(psoaHashMap) );
   pHashMap->object.type = PSOA_HASH_MAP;
   pHashMap->object.pSession = pSession;

   if ( ! pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   errcode = psoaCommonObjOpen( &pHashMap->object,
                                PSO_HASH_MAP,
                                PSOA_READ_WRITE,
                                hashMapName,
                                nameLengthInBytes );
   if ( errcode != PSO_OK ) goto error_handler;

   *objectHandle = (PSO_HANDLE) pHashMap;

   PSO_TRACE_EXIT_API( &pSession->context, true );
   return PSO_OK;

error_handler:

   free(pHashMap);
   psocSetError( &pHashMap->object.pSession->context.errorHandler, 
                 g_psoErrorHandle, errcode );

   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapReplace( PSO_HANDLE   objectHandle,
                       const void * key,
                       uint32_t     keyLength,
                       const void * data,
                       uint32_t     dataLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   if ( ! pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
   if ( pMemHashMap->isSystemObject ) {
      errcode = PSO_SYSTEM_OBJECT;
      goto error_handler;
   }
   
   ok = psonHashMapReplace( pMemHashMap,
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
   if ( errcode != 0 ) {
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

int psoHashMapStatus( PSO_HANDLE     objectHandle,
                      psoObjStatus * pStatus )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   psonSessionContext * pContext;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   pContext = &pHashMap->object.pSession->context;

   if ( pStatus == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( ! pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
      
   if ( psonLock(&pMemHashMap->memObject, pContext) ) {
      psonMemObjectStatus( &pMemHashMap->memObject, pStatus, pContext );

      psonHashMapStatus( pMemHashMap, pStatus, pContext );
      pStatus->type = PSO_HASH_MAP;
            
      psonUnlock( &pMemHashMap->memObject, pContext );
   }
   else {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, true );
   return PSO_OK;

error_handler:

   psocSetError( &pHashMap->object.pSession->context.errorHandler, 
                 g_psoErrorHandle, errcode );

   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaHashMapFirst( psoaHashMap    * pHashMap,
                      unsigned char ** pKey,
                      uint32_t       * pKeyLength,
                      unsigned char ** pData,
                      uint32_t       * pDataLength )
{
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pHashMap    != NULL );
   PSO_PRE_CONDITION( pKey        != NULL );
   PSO_PRE_CONDITION( pKeyLength  != NULL );
   PSO_PRE_CONDITION( pData       != NULL );
   PSO_PRE_CONDITION( pDataLength != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_HASH_MAP );
   
   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGetFirst( pMemHashMap,
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
   return 0;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaHashMapNext( psoaHashMap    * pHashMap,
                     unsigned char ** pKey,
                     uint32_t       * pKeyLength,
                     unsigned char ** pData,
                     uint32_t       * pDataLength )
{
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pHashMap    != NULL );
   PSO_PRE_CONDITION( pKey        != NULL );
   PSO_PRE_CONDITION( pKeyLength  != NULL );
   PSO_PRE_CONDITION( pData       != NULL );
   PSO_PRE_CONDITION( pDataLength != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_HASH_MAP );
   PSO_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   ok = psonHashMapGetNext( pMemHashMap,
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
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaHashMapRetrieve( psoaHashMap    * pHashMap,
                         const void     * key,
                         uint32_t         keyLength,
                         unsigned char ** pData,
                         uint32_t       * pLength )
{
   psonHashMap * pMemHashMap;
   int errcode = 0;
   bool ok = true;
   psonHashTxItem * pHashItem;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( key      != NULL );
   PSO_PRE_CONDITION( pData    != NULL );
   PSO_PRE_CONDITION( pLength  != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_HASH_MAP );

   pHashMap->object.pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pHashMap->object.pSession->context );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGet( pMemHashMap,
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
   return 0;

error_handler:
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pHashMap->object.pSession->context, false );
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

