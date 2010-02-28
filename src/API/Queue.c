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
#include <photon/Queue.h>
#include "API/Queue.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueClose( PSO_HANDLE queueHandle )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   
   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

      /* Reinitialize the iterator, if needed */
      if ( pQueue->iterator != NULL ) {
         if ( psonQueueRelease( pMemQueue,
                                pQueue->iterator,
                                &pQueue->object.pSession->context ) ) {
            pQueue->iterator = NULL;
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      }

      if ( errcode == PSO_OK ) {
         errcode = psoaCommonObjClose( &pQueue->object );
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
      pQueue->object.type = 0; 
      free( pQueue );
   }
   else {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueDefinition( PSO_HANDLE            queueHandle, 
                        psoObjectDefinition * definition,
                        psoUint32             length )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   uint32_t myLength;
   
   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( definition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( length < sizeof(psoObjectDefinition) ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

      GET_PTR( pQueue->object.pSession->context.pBaseAddress, pMyDefinition, pMemQueue->dataDefOffset, psoObjectDefinition );
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

int psoQueueDefLength( PSO_HANDLE   queueHandle, 
                       psoUint32  * pLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   
   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( pLength == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   *pLength = 0;
   
   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

      GET_PTR( pQueue->object.pSession->context.pBaseAddress, pMyDefinition, pMemQueue->dataDefOffset, psoObjectDefinition );
      *pLength = offsetof(psoObjectDefinition, dataDef) + 
         pMyDefinition->dataDefLength;
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_SESSION_IS_TERMINATED );
      return PSO_SESSION_IS_TERMINATED;
   }
   
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueGetFirst( PSO_HANDLE   queueHandle,
                      void       * buffer,
                      uint32_t     bufferLength,
                      uint32_t   * returnedLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      pQueue->iterator = NULL;
   }

   ok = psonQueueGetFirst( pMemQueue,
                           &pQueue->iterator,
                           bufferLength,
                           &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueGetNext( PSO_HANDLE   queueHandle,
                     void       * buffer,
                     uint32_t     bufferLength,
                     uint32_t   * returnedLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pQueue->iterator == NULL ) {
      errcode = PSO_INVALID_ITERATOR;
      goto error_handler;
   }
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   ok = psonQueueGetNext( pMemQueue,
                          &pQueue->iterator,
                          bufferLength,
                          &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );
   
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueOpen( PSO_HANDLE   sessionHandle,
                  const char * queueName,
                  uint32_t     nameLengthInBytes,
                  PSO_HANDLE * queueHandle )
{
   psoaSession * pSession;
   psoaQueue * pQueue = NULL;
   int errcode;
   
   if ( queueHandle == NULL ) return PSO_NULL_HANDLE;
   *queueHandle = NULL;

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
   
   pQueue = (psoaQueue *) malloc(sizeof(psoaQueue));
   if (  pQueue == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pQueue, 0, sizeof(psoaQueue) );
   pQueue->object.type = PSOA_QUEUE;
   pQueue->object.pSession = pSession;

   if ( ! pQueue->object.pSession->terminated ) {
      errcode = psoaCommonObjOpen( &pQueue->object,
                                   PSO_QUEUE,
                                   PSOA_READ_WRITE,
                                   queueName,
                                   nameLengthInBytes );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
      free(pQueue);
   }
   else {
      *queueHandle = (PSO_HANDLE) pQueue;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePop( PSO_HANDLE   queueHandle,
                 void       * buffer,
                 uint32_t     bufferLength,
                 uint32_t   * returnedLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      pQueue->iterator = NULL;
   }

   ok = psonQueueRemove( pMemQueue,
                         &pQueue->iterator,
                         PSON_QUEUE_FIRST,
                         bufferLength,
                         &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePopWait( PSO_HANDLE   queueHandle,
                     uint32_t     waitPeriodInSecs,
                     void       * buffer,
                     uint32_t     bufferLength,
                     uint32_t   * returnedLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;
   uint32_t version;
   
   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      pQueue->iterator = NULL;
   }

   ok = psonQueueRemove( pMemQueue,
                         &pQueue->iterator,
                         PSON_QUEUE_FIRST,
                         bufferLength,
                         &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );

   if ( ! ok ) {
      int ms = pQueue->object.pSession->sleepPeriod.tv_sec  * 1000 +   
               pQueue->object.pSession->sleepPeriod.tv_usec / 1000;
      if ( ms == 0 ) ms = 1; // division by zero
      int iterations = waitPeriodInSecs/ms;
      int i;
      volatile uint32_t version = pMemQueue->memObject.committedChanges;

      for ( i = 0; i < iterations; ++i ) {
         psocLockSleep( &pQueue->object.pSession->sleepPeriod );
         if ( version != pMemQueue->memObject.committedChanges ) {
            ok = psonQueueRemove( pMemQueue,
                                  &pQueue->iterator,
                                  PSON_QUEUE_FIRST,
                                  bufferLength,
                                  &pQueue->object.pSession->context );
            if ( ok ) break;
            version = pMemQueue->memObject.committedChanges;
         }
      }
      if ( ! ok ) goto error_handler;
   }

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePush( PSO_HANDLE   queueHandle,
                  const void * data,
                  uint32_t     dataLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;
   
   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength == 0 ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
      ok = psonQueueInsert( pMemQueue,
                            data,
                            dataLength,
                            PSON_QUEUE_LAST,
                            &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePushNow( PSO_HANDLE   queueHandle,
                     const void * data,
                     uint32_t     dataLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength == 0 ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
      ok = psonQueueInsertNow( pMemQueue,
                               data,
                               dataLength,
                               PSON_QUEUE_LAST,
                               &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueStatus( PSO_HANDLE     queueHandle,
                    psoObjStatus * pStatus )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( pStatus == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
      
      if ( psonLock(&pMemQueue->memObject, pContext) ) {
         psonMemObjectStatus( &pMemQueue->memObject, pStatus, pContext );

         psonQueueStatus( pMemQueue, pStatus, pContext );
         pStatus->type = PSO_QUEUE;

         psonUnlock( &pMemQueue->memObject, pContext );
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

/* The not-published API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaQueueFirst( psoaQueue      * pQueue,
                    unsigned char ** pData,
                    uint32_t       * pLength )
{
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pQueue  != NULL );
   PSO_PRE_CONDITION( pData   != NULL );
   PSO_PRE_CONDITION( pLength != NULL );
   PSO_PRE_CONDITION( pQueue->object.type == PSOA_QUEUE );
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      pQueue->iterator = NULL;
   }

   ok = psonQueueGetFirst( pMemQueue,
                           &pQueue->iterator,
                           (uint32_t) -1,
                           &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *pData = pQueue->iterator->data;
   *pLength = pQueue->iterator->dataLength;
   
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaQueueGetDef( PSO_HANDLE             queueHandle, 
                     psoObjectDefinition ** ppDefinition )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   
   pQueue = (psoaQueue *) queueHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( ppDefinition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

      GET_PTR( pQueue->object.pSession->context.pBaseAddress, pMyDefinition, pMemQueue->dataDefOffset, psoObjectDefinition );
      
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

int psoaQueueNext( psoaQueue      * pQueue,
                   unsigned char ** pData,
                   uint32_t       * pLength )
{
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pQueue  != NULL );
   PSO_PRE_CONDITION( pData   != NULL );
   PSO_PRE_CONDITION( pLength != NULL );
   PSO_PRE_CONDITION( pQueue->object.type == PSOA_QUEUE );
   PSO_PRE_CONDITION( pQueue->iterator != NULL );
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   ok = psonQueueGetNext( pMemQueue,
                          &pQueue->iterator,
                          (uint32_t) -1,
                          &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *pData = pQueue->iterator->data;
   *pLength = pQueue->iterator->dataLength;

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaQueueRemove( psoaQueue      * pQueue,
                     unsigned char ** pData,
                     uint32_t       * pLength )
{
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pQueue  != NULL );
   PSO_PRE_CONDITION( pData   != NULL );
   PSO_PRE_CONDITION( pLength != NULL );
   PSO_PRE_CONDITION( pQueue->object.type == PSOA_QUEUE )

   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      pQueue->iterator = NULL;
   }

   ok = psonQueueRemove( pMemQueue,
                         &pQueue->iterator,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *pData = pQueue->iterator->data;
   *pLength = pQueue->iterator->dataLength;

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

