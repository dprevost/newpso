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
#include <photon/Session.h>
#include "API/Session.h"
#include "API/Process.h"
#include "API/ShMem.h"
#include "API/CommonObject.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"
#include "Nucleus/MemoryHeader.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Process.h"
#include "Nucleus/Session.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/Folder.h"
#include "Nucleus/HashMap.h"

#include "API/tls.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaFastMapResetReader( void * map );

static inline
void psoaResetReaders( psoaSession * pSession )
{
   psoaReader * reader = NULL;
   
   if ( psoaListReadersPeakFirst( &pSession->listReaders, &reader ) ) {
      do {
         if ( reader->type == PSOA_MAP ) {
            psoaFastMapResetReader( reader->address );
         }
      } while (psoaListReadersPeakNext(&pSession->listReaders, reader, &reader) );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoCommit( PSO_HANDLE sessionHandle )
{
   int errcode = PSO_OK;
   psoaSession* pSession;
   bool ok;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( pSession->numberOfEdits > 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, 
         PSO_NOT_ALL_EDIT_ARE_CLOSED );
      return PSO_NOT_ALL_EDIT_ARE_CLOSED;
   }
   
   if ( ! pSession->terminated ) {
      ok = psonTxCommit( (psonTx*)pSession->context.pTransaction, 
                          &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) psoaResetReaders( pSession );
      else errcode = PSO_ENGINE_BUSY;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoCreateFolder( PSO_HANDLE   sessionHandle,
                     const char * objectName,
                     uint32_t     nameLengthInBytes )
{
   psoaSession* pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pSession->terminated ) {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
      ok = psonTopFolderCreateFolder( pTree,
                                      objectName,
                                      nameLengthInBytes,
                                      &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoCreateQueue( PSO_HANDLE            sessionHandle,
                    const char          * objectName,
                    uint32_t              nameLengthInBytes,
                    psoObjectDefinition * pDefinition )
{
   psoaSession* pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   //psonDataDefinition * pMemDataDefinition = NULL;
   //psoaDataDefinition * pDataDefinition;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( pDefinition->type <= PSO_FOLDER || pDefinition->type >= PSO_HASH_MAP ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_WRONG_OBJECT_TYPE );
      return PSO_WRONG_OBJECT_TYPE;
   }
   
   if ( ! pSession->terminated ) {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
      ok = psonTopFolderCreateQueue( pTree,
                                     objectName,
                                     nameLengthInBytes,
                                     pDefinition,
                                     &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoCreateMap( PSO_HANDLE            sessionHandle,
                  const char          * objectName,
                  uint32_t              nameLengthInBytes,
                  psoObjectDefinition * pDefinition,
                  psoKeyDefinition    * pKeyDefinition )
{
   psoaSession* pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   //psonDataDefinition * pMemDataDefinition = NULL;
   //psonKeyDefinition  * pMemKeyDefinition = NULL;
   //psoaDataDefinition * pDataDefinition;
   //psoaKeyDefinition  * pKeyDefinition;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( pDefinition->type < PSO_HASH_MAP || pDefinition->type > PSO_FAST_MAP ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_WRONG_OBJECT_TYPE );
      return PSO_WRONG_OBJECT_TYPE;
   }
   
   if ( pKeyDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_HANDLE );
      return PSO_NULL_POINTER;
   }
   
   if ( ! pSession->terminated ) {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
      ok = psonTopFolderCreateMap( pTree,
                                   objectName,
                                   nameLengthInBytes,
                                   pDefinition,
                                   pKeyDefinition,
                                   &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoDestroyObject( PSO_HANDLE   sessionHandle,
                      const char * objectName,
                      uint32_t     nameLengthInBytes )
{
   psoaSession* pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pSession->terminated ) {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
      ok = psonTopFolderDestroyObject( pTree,
                                       objectName,
                                       nameLengthInBytes,
                                       &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoErrorMsg( PSO_HANDLE   sessionHandle,
                 char       * message,
                 uint32_t     msgLengthInBytes )
{
   psoaSession * pSession;
   int rc = PSO_OK;
   size_t len;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( message == NULL ) return PSO_NULL_POINTER;
   
   if ( msgLengthInBytes < 32 ) return PSO_INVALID_LENGTH;
   
   if ( ! pSession->terminated ) {
      if ( psocGetLastError( &pSession->context.errorHandler ) != 0 ) {
         len = psocGetErrorMsg( &pSession->context.errorHandler,
                                message, 
                                msgLengthInBytes );
      }
      else {
         strcpy( message, "No Error!" );
      }
   }
   else {
      rc = PSO_SESSION_IS_TERMINATED;
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoExitSession( PSO_HANDLE sessionHandle )
{
   psoaSession * pSession;
   psonSession * pCleanup;
   bool ok;
   int errcode = 0;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( psoaProcessLock() ) {
      /*
       * if the exit process was called at the "same time" the session
       * might have been "removed" by the process object.
       */
      if ( ! pSession->terminated ) {
         /* ok we are still there */
         pCleanup = pSession->pCleanup;
         errcode = psoaCloseSession( pSession );
         
         /*
          * psoaCloseSession can be called by the api process object while
          * it holds the lock to the psonProcess object. And the next
          * call locks it as well. To avoid a recursive lock (leading to 
          * a deadlock) we cannot include this call in psoaCloseSession.
          */
         if ( errcode == 0 ) {
            ok = psonProcessRemoveSession( g_processInstance->pCleanup, 
                                           pCleanup, 
                                           &pSession->context );
            PSO_POST_CONDITION( ok == true || ok == false );
          }
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      
      psoaProcessUnlock();
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   /* 
    * This is a likely memory leak. But if we remove the session and there
    * are still objects open... we might have random crashes instead
    * (depending on what happen to the release memory - still attached to
    * the process or not - which is something that depends on the OS and
    * possibly other factors). 
    */
   if ( errcode == 0 && pSession->numberOfObjects == 0 ) {
      /* 
       * If someone tries to use the session handle after closing the
       * session, setting the type to zero is safer (if the free memory
       * is still attached to the process - which is often the case in
       * Unix to avoid going to the system for every malloc/free).
       */
      pSession->type = 0;
      /* Reset the list to spot logic errors (illegal accesses) with DBC */
      psoaListReadersFini( &pSession->listReaders );
      free( pSession );
   }
   
   if ( errcode != 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetDefinition( PSO_HANDLE            sessionHandle,
                      const char          * objectName,
                      uint32_t              nameLengthInBytes,
                      psoObjectDefinition * pDefinition,
                      uint32_t              length )
{
   psoaSession * pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
   uint32_t myLength;
 
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   if ( pSession->terminated ) return PSO_SESSION_IS_TERMINATED;

   pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pSession.context );
   
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

   GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
   ok = psonTopFolderGetDef( pTree, 
                             objectName,
                             nameLengthInBytes,
                             &pMyDefinition,
                             &pMyKeyDefinition,
                             &pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );

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
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetDefLength( PSO_HANDLE   sessionHandle,
                     const char * objectName,
                     uint32_t     nameLengthInBytes,
                     uint32_t   * pLength )
{
   psoaSession * pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
 
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   if ( pSession->terminated ) return PSO_SESSION_IS_TERMINATED;

   pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pSession.context );
   
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

   GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
   ok = psonTopFolderGetDef( pTree, 
                             objectName,
                             nameLengthInBytes,
                             &pMyDefinition,
                             &pMyKeyDefinition,
                             &pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );

   PSO_POST_CONDITION( pMyDefinition != NULL );

   *pLength = offsetof(psoObjectDefinition, dataDef) + 
      pMyDefinition->dataDefLength;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetInfo( PSO_HANDLE   sessionHandle,
                psoInfo    * pInfo )
{
   psoaSession* pSession;
   int errcode = PSO_OK;
   bool ok = true;
   psonMemAlloc * pAlloc;
   struct psonMemoryHeader * pHead;
   
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( pInfo == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   memset( pInfo, 0, sizeof(struct psoInfo) );
   
   if ( ! pSession->terminated ) {
      pHead = g_processInstance->pHeader;
      GET_PTR( pAlloc, pSession->pHeader->allocatorOffset, psonMemAlloc )
      ok = psonMemAllocStats( pAlloc, pInfo, &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         pInfo->memoryVersion = pHead->version;
         pInfo->bigEndian = 0;
         if (pHead->bigEndian) pInfo->bigEndian = 1;
         memcpy( pInfo->compiler,        pHead->compiler,        20 );
         memcpy( pInfo->compilerVersion, pHead->compilerVersion, 10 );
         memcpy( pInfo->platform,        pHead->cpu_type ,       20 );
         memcpy( pInfo->quasarVersion,   pHead->quasarVersion, 10 );
         memcpy( pInfo->creationTime,    pHead->creationTime,    30 );
         strncpy( pInfo->dllVersion, PACKAGE_VERSION, 10 );
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetKeyDefinition( PSO_HANDLE         sessionHandle,
                         const char       * objectName,
                         uint32_t           nameLengthInBytes,
                         psoKeyDefinition * pDefinition,
                         uint32_t           length )
{
   psoaSession * pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
   uint32_t myLength;
 
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   if ( pSession->terminated ) return PSO_SESSION_IS_TERMINATED;

   pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pSession.context );
   
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

   GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
   ok = psonTopFolderGetDef( pTree, 
                             objectName,
                             nameLengthInBytes,
                             &pMyDefinition,
                             &pMyKeyDefinition,
                             &pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );

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
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetKeyDefLength( PSO_HANDLE   sessionHandle,
                        const char * objectName,
                        uint32_t     nameLengthInBytes,
                        uint32_t   * pLength )
{
   psoaSession * pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
 
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   if ( pSession->terminated ) return PSO_SESSION_IS_TERMINATED;

   pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pSession.context );
   
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

   GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
   ok = psonTopFolderGetDef( pTree, 
                             objectName,
                             nameLengthInBytes,
                             &pMyDefinition,
                             &pMyKeyDefinition,
                             &pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );

   PSO_POST_CONDITION( pMyKeyDefinition != NULL );

   *pLength = offsetof(psoKeyDefinition, definition) + 
      pMyKeyDefinition->definitionLength;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetStatus( PSO_HANDLE     sessionHandle,
                  const char   * objectName,
                  uint32_t       nameLengthInBytes,
                  psoObjStatus * pStatus )
{
   psoaSession* pSession;
   int errcode = 0;
   psonFolder * pTree;
   bool ok = true;
   
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pStatus == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   memset( pStatus, 0, sizeof(psoObjStatus) );

   if ( ! pSession->terminated ) {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
      ok = psonTopFolderGetStatus( pTree, 
                                   objectName,
                                   nameLengthInBytes,
                                   pStatus,
                                   &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoInitSession( PSO_HANDLE psmHandle, PSO_HANDLE * sessionHandle )
{
   psoErrors errcode = PSO_OK;
   psoaSession* pSession = NULL;
   bool ok;
   psoaShMem * pShMem;
   
   pShMem = (psoaShMem *) psmHandle;
//   if ( pShMem == NULL ) return PSO_NULL_HANDLE;   
//   if ( pShMem->type != PSOA_SHMEM ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( sessionHandle == NULL ) return PSO_NULL_HANDLE;
   
   *sessionHandle = NULL;

   if ( g_processInstance == NULL ) return PSO_PROCESS_NOT_INITIALIZED;
   
   pSession = (psoaSession*) malloc(sizeof(psoaSession));
   if ( pSession == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pSession, 0, sizeof(psoaSession) );
   pSession->type = PSOA_SESSION;

   pSession->sleepPeriod.tv_sec  = 0;
   pSession->sleepPeriod.tv_usec = 100000; // every 0.1 s ?

//   pSession->context.pidLocker = getpid();
//   psocInitErrorHandler( &pSession->context.errorHandler );
   psonInitSessionContext( &pSession->context );

   /*
    * From this point we can use "goto error_handler" to recover
    * from errors;
    */
   
   pSession->pHeader = g_processInstance->pHeader;
   if ( pSession->pHeader == NULL ) {
      errcode = PSO_PROCESS_NOT_INITIALIZED;
      goto error_handler;
   }

   GET_PTR( pSession->context.pAllocator, pSession->pHeader->allocatorOffset, void );
   
   psoaListReadersInit( &pSession->listReaders );
   pSession->terminated = false;
   
   /*
    * Once a session is added, it can be accessed by other threads
    * in the process (exiting, for example). After this point we 
    * will want to use the process mutex to remove it.
    */
   ok = psonProcessAddSession( g_processInstance->pCleanup, 
                               pSession, 
                               &pSession->pCleanup, 
                               &pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   *sessionHandle = (PSO_HANDLE) pSession;

   return PSO_OK;
   
   /* Error processing... */

error_handler:

   /* 
    * The last operation that might fail is to add ourselves to the
    * psonProcess object. If this call succeeded, we would not come
    * here - this session is unknown to the rest of the software and
    * we can safely clean ourselves without locks.
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   free( pSession );
   
   return errcode;   
} 
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLastError( PSO_HANDLE sessionHandle )
{
   psoaSession* pSession;
   int rc = PSO_SESSION_CANNOT_GET_LOCK;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( ! pSession->terminated ) {
      rc = psocGetLastError( &pSession->context.errorHandler );
   }
   else {
      rc = PSO_SESSION_IS_TERMINATED;
   }

   return rc;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoRollback( PSO_HANDLE sessionHandle )
{
   int errcode = PSO_OK;
   psoaSession* pSession;
   bool ok;

   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( ! pSession->terminated ) {
      ok = psonTxRollback( (psonTx*)pSession->context.pTransaction, 
                           &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) psoaResetReaders( pSession );
      else errcode = PSO_ENGINE_BUSY;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaSessionCloseObj( psoaSession             * pSession,
                         struct psoaCommonObject * pObject )
{
   int errcode = PSO_OK;
   bool ok = true;
   
   PSO_PRE_CONDITION( pSession   != NULL );
   PSO_PRE_CONDITION( pObject    != NULL );

   if ( ! pSession->terminated ) {
      ok = psonSessionRemoveObj( pSession->pCleanup, 
                                 pObject->pObjectContext, 
                                 &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         ok = psonTopFolderCloseObject( &pObject->folderItem,
                                        &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ok ) pSession->numberOfObjects--;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaCloseSession( psoaSession * pSession )
{
   psonObjectContext* pObject = NULL;
   psoaCommonObject* pCommonObject = NULL;
   int rc, errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pSession != NULL );
   /*
    * Note: this function is always called after getting control of
    * the process mutex.
    */

   if ( ! pSession->terminated ) {
      /*
       * Setting this first may protect badly written client programs 
       * from accessing a session or its objects after the session was
       * terminated either by psoExit or psoSessionExit.
       *
       * In any case, there is a limit to how much we can protect these
       * clients and still maintain the performance of the software!
       *
       * Explanation:
       *
       * "terminated" is not protected by a mem. barrier or a lock when 
       * the other API calls test it - its value on CPU #n might not 
       * match its value on the CPU where the current code is running.
       */
      pSession->terminated = true;

      if ( pSession->context.pTransaction != NULL ) {
         psonTxRollback( (psonTx*)(pSession->context.pTransaction), 
                         &pSession->context );
         pSession->context.pTransaction = NULL;
      }
   
      if ( psonLock( &pSession->pCleanup->memObject, &pSession->context) ) {
         for (;;) {
            ok = psonSessionGetFirst( pSession->pCleanup, &pObject, &pSession->context );
            PSO_POST_CONDITION( ok == true || ok == false );
            if ( ! ok ) break;

            /* This would be an internal error... */
            if ( pObject == NULL ) continue;
           
            if ( pObject->pCommonObject == NULL ) continue;
            
            pCommonObject = (psoaCommonObject*) pObject->pCommonObject;
            rc = psonTopFolderCloseObject( &pCommonObject->folderItem, 
                                           &pSession->context );
            psoaCommonCloseObject( pCommonObject );

            ok = psonSessionRemoveFirst(pSession->pCleanup, &pSession->context );
            PSO_POST_CONDITION( ok == true );
         }
         psonUnlock( &pSession->pCleanup->memObject, &pSession->context);
      }
      
      pSession->pCleanup = NULL;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaSessionGetDef( PSO_HANDLE             sessionHandle,
                       const char           * objectName,
                       uint32_t               nameLengthInBytes,
                       psoObjectDefinition ** ppDefinition )
{
   psoaSession * pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   psonSessionContext * pContext;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
   uint32_t myLength;
 
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   if ( pSession->terminated ) return PSO_SESSION_IS_TERMINATED;

   pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pSession.context );
   
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
   
   GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
   ok = psonTopFolderGetDef( pTree, 
                             objectName,
                             nameLengthInBytes,
                             &pMyDefinition,
                             &pMyKeyDefinition,
                             &pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );

   PSO_POST_CONDITION( pMyDefinition != NULL );

   *ppDefinition = pMyDefinition;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaSessionGetKeyDef( PSO_HANDLE          sessionHandle,
                          const char        * objectName,
                          uint32_t            nameLengthInBytes,
                          psoKeyDefinition ** ppDefinition )
{
   psoaSession * pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   psoObjectDefinition * pMyDefinition = NULL;
   psoKeyDefinition  * pMyKeyDefinition = NULL;
 
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   if ( pSession->terminated ) return PSO_SESSION_IS_TERMINATED;

   pSession->context.indent = 0;
   PSO_TRACE_ENTER_API( &pSession.context );
   
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
   
   GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
   ok = psonTopFolderGetDef( pTree, 
                             objectName,
                             nameLengthInBytes,
                             &pMyDefinition,
                             &pMyKeyDefinition,
                             &pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );

   PSO_POST_CONDITION( pMyKeyDefinition != NULL );

   *ppDefinition = pMyKeyDefinition;

   PSO_TRACE_EXIT_API( &pFolder->object.pSession->context, true );
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   PSO_TRACE_EXIT_API( &pSession->context, false );
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaSessionOpenObj( psoaSession             * pSession,
                        enum psoObjectType        objectType,
                        psoaEditMode              editMode,
                        const char              * objectName,
                        uint32_t                  nameLengthInBytes,
                        struct psoaCommonObject * pObject )
{
   int errcode = PSO_OK;
   psonFolder * pTree;
   psonTreeNode * pNode;
   bool ok = true;
   
   PSO_PRE_CONDITION( pSession   != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pObject    != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );
   PSO_PRE_CONDITION( nameLengthInBytes > 0 );
   
   if ( ! pSession->terminated ) {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder );
      if ( editMode == PSOA_UPDATE_RO ) {
         ok = psonTopFolderEditObject( pTree,
                                       objectName,
                                       nameLengthInBytes,
                                       objectType,
                                       &pObject->folderItem,
                                       &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      else {
         ok = psonTopFolderOpenObject( pTree,
                                       objectName,
                                       nameLengthInBytes,
                                       objectType,
                                       &pObject->folderItem,
                                       &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      if ( ok ) {
         GET_PTR( pNode, pObject->folderItem.pHashItem->dataOffset,
                          psonTreeNode );

         ok = psonSessionAddObj( pSession->pCleanup,
                                 pNode->offset,
                                 pNode->apiType,
                                 pObject,
                                 &pObject->pObjectContext,
                                 &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         pSession->numberOfObjects++;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaSessionTrace( psoaSession * pSession,
                       int           traceFlags )
{
   PSO_PRE_CONDITION( pSession   != NULL );
   
   pSession->context.traceFlags = traceFlags;
#if !defined(PSO_USE_TRACE)
   if ( traceFlags ) fprintf( stderr, "Warning: not built with trace enable\n" );
#endif

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

