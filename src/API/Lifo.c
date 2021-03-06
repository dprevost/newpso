/*
 * Copyright (C) 2008-2010 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/Lifo.h>
#include "API/Lifo.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoClose( PSO_HANDLE objectHandle )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

      /* Reinitialize the iterator, if needed */
      if ( pLifo->iterator != NULL ) {
         if ( psonQueueRelease( pMemLifo,
                                pLifo->iterator,
                                &pLifo->object.pSession->context ) ) {
            pLifo->iterator = NULL;
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      }

      if ( errcode == PSO_OK ) {
         errcode = psoaCommonObjClose( &pLifo->object );
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode == PSO_OK ) {
      /*
       * Memory might still be around even after it is released, so we make 
       * sure future access with the handle fails by setting the type wrong!
       */
      pLifo->object.type = 0; 
      free( pLifo );
   }
   else {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoDefinition( PSO_HANDLE            objectHandle, 
                       psoObjectDefinition * definition,
                       psoUint32             length )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   uint32_t myLength;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( definition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( length < sizeof(psoObjectDefinition) ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;
      
      GET_PTR( pLifo->object.pSession->context.pBaseAddress, pMyDefinition, pMemLifo->dataDefOffset, psoObjectDefinition );
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
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoDefLength( PSO_HANDLE   objectHandle, 
                      psoUint32  * pLength )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( pLength == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   *pLength = 0;

   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;
      
      GET_PTR( pLifo->object.pSession->context.pBaseAddress, pMyDefinition, pMemLifo->dataDefOffset, psoObjectDefinition );
      *pLength = offsetof(psoObjectDefinition, dataDef) + 
         pMyDefinition->dataDefLength;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoGetFirst( PSO_HANDLE   objectHandle,
                     void       * buffer,
                     uint32_t     bufferLength,
                     uint32_t   * returnedLength )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      pLifo->iterator = NULL;
   }

   ok = psonQueueGetFirst( pMemLifo,
                           &pLifo->iterator,
                           bufferLength,
                           &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoGetNext( PSO_HANDLE   objectHandle,
                    void       * buffer,
                    uint32_t     bufferLength,
                    uint32_t   * returnedLength )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pLifo->iterator == NULL ) {
      errcode = PSO_INVALID_ITERATOR;
      goto error_handler;
   }
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   ok = psonQueueGetNext( pMemLifo,
                          &pLifo->iterator,
                          bufferLength,
                          &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoOpen( PSO_HANDLE   sessionHandle,
                 const char * queueName,
                 uint32_t     nameLengthInBytes,
                 PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaLifo * pLifo = NULL;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( queueName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   pLifo = (psoaLifo *) malloc(sizeof(psoaLifo));
   if (  pLifo == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pLifo, 0, sizeof(psoaLifo) );
   pLifo->object.type = PSOA_LIFO;
   pLifo->object.pSession = pSession;

   if ( ! pLifo->object.pSession->terminated ) {
      errcode = psoaCommonObjOpen( &pLifo->object,
                                   PSO_LIFO,
                                   PSOA_READ_WRITE,
                                   queueName,
                                   nameLengthInBytes );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
      free(pLifo);
   }
   else {
      *objectHandle = (PSO_HANDLE) pLifo;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoPop( PSO_HANDLE   objectHandle,
                void       * buffer,
                uint32_t     bufferLength,
                uint32_t   * returnedLength )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      pLifo->iterator = NULL;
   }

   ok = psonQueueRemove( pMemLifo,
                         &pLifo->iterator,
                         PSON_QUEUE_LAST,
                         bufferLength,
                         &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoPush( PSO_HANDLE   objectHandle,
                 const void * data,
                 uint32_t     dataLength )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength == 0 ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

      ok = psonQueueInsert( pMemLifo,
                            data,
                            dataLength,
                            PSON_QUEUE_LAST,
                            &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoStatus( PSO_HANDLE     objectHandle,
                   psoObjStatus * pStatus )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( pStatus == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;
      
      if ( psonLock(&pMemLifo->memObject, pContext) ) {
         psonMemObjectStatus( &pMemLifo->memObject, pStatus, pContext );

         psonQueueStatus( pMemLifo, pStatus, pContext );
         pStatus->type = PSO_LIFO;

         psonUnlock( &pMemLifo->memObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaLifoFirst( psoaLifo       * pLifo,
                   unsigned char ** pData,
                   uint32_t       * pLength )
{
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo   != NULL );
   PSO_PRE_CONDITION( pData   != NULL );
   PSO_PRE_CONDITION( pLength != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSOA_LIFO );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      pLifo->iterator = NULL;
   }

   ok = psonQueueGetFirst( pMemLifo,
                           &pLifo->iterator,
                           (uint32_t) -1,
                           &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *pData = pLifo->iterator->data;
   *pLength = pLifo->iterator->dataLength;
      
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaLifoGetDef( PSO_HANDLE             objectHandle, 
                    psoObjectDefinition ** ppDefinition )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( ppDefinition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;
      
      GET_PTR( pLifo->object.pSession->context.pBaseAddress, pMyDefinition, pMemLifo->dataDefOffset, psoObjectDefinition );
      
      *ppDefinition = pMyDefinition;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaLifoNext( psoaLifo       * pLifo,
                  unsigned char ** pData,
                  uint32_t       * pLength )
{
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo   != NULL );
   PSO_PRE_CONDITION( pData   != NULL );
   PSO_PRE_CONDITION( pLength != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSOA_LIFO );
   PSO_PRE_CONDITION( pLifo->iterator != NULL );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   ok = psonQueueGetNext( pMemLifo,
                          &pLifo->iterator,
                          (uint32_t) -1,
                          &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *pData = pLifo->iterator->data;
   *pLength = pLifo->iterator->dataLength;

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaLifoRemove( psoaLifo       * pLifo,
                    unsigned char ** pData,
                    uint32_t       * pLength )
{
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo   != NULL );
   PSO_PRE_CONDITION( pData   != NULL );
   PSO_PRE_CONDITION( pLength != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSOA_LIFO )

   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      pLifo->iterator = NULL;
   }

   ok = psonQueueRemove( pMemLifo,
                         &pLifo->iterator,
                         PSON_QUEUE_LAST,
                         (uint32_t) -1,
                         &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *pData = pLifo->iterator->data;
   *pLength = pLifo->iterator->dataLength;
   
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

