/*
 * Copyright (C) 2006-2010 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "Quasar/ShMemHandler.h"
#include "Quasar/MemoryManager.h"
#include "Quasar/Quasar.h"
#include "Quasar/VerifyShMem.h"
#include "Quasar/QuasarCommon.h"
#include "Nucleus/SessionContext.h"
#include "Nucleus/InitEngine.h"
#include "Nucleus/Folder.h"
#include "Nucleus/ProcessManager.h"
#include "Nucleus/Session.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/HashMap.h"
#include <photon/psoPhotonODBC.h>

extern psocErrMsgHandle g_qsrErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool qsrHandlerAddSystemObjects( qsrHandler * pHandler )
{
   bool ok;
   psoObjectDefinition defMap = { PSO_HASH_MAP, 0, 0 };
   psonFolder * pTree, * pFolder;
   psonHashMap * pHashMap;
//   psonDataDefinition dummyDataDefinition;
//   psonKeyDefinition  dummyKeyDefinition;
   
   psonFolderItem folderItem;
   psonTreeNode * pNode;
   psonDataDefinition * pMemDataDefinition = NULL;
   psonKeyDefinition  * pMemKeyDefinition = NULL;
   uint32_t recLength;
   psoFieldDefinition fields = { "Default", PSO_LONGVARBINARY, {0} };
   psoKeyFieldDefinition keys = { "Default", PSO_LONGVARBINARY, 0 };

   GET_PTR(pHandler->context.pBaseAddress, pTree, pHandler->pMemHeader->treeMgrOffset, psonFolder )

   ok = psonTopFolderCreateFolder( pTree,
                                   "system",
                                   strlen("system"),
                                   &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;
   
   ok = psonTopFolderCreateFolder( pTree,
                                   "proc",
                                   strlen("proc"),
                                   &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;

#if 0
   ok = psonTopFolderCreateMap( pTree,
                                "system/Data_Definition",
                                strlen("system/Data_Definition"),
                                &defMap,
                                &dummyKeyDefinition,
                                &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;

   ok = psonTopFolderCreateMap( pTree,
                                "system/Key_Definition",
                                strlen("system/Key_Definition"),
                                &defMap,
                                &dummyKeyDefinition,
                                &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;
#endif

   /*
    * Setting the field "isSystemObject" should only be done after all
    * objects are created, specially folders.
    */
   ok = psonTopFolderOpenObject( pTree,
                                 "system",
                                 strlen("system"),
                                 PSO_FOLDER,
                                 &folderItem,
                                 &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;

   GET_PTR(pHandler->context.pBaseAddress, pNode, folderItem.pHashItem->dataOffset, psonTreeNode );
   GET_PTR(pHandler->context.pBaseAddress, pFolder, pNode->offset, psonFolder );
   pFolder->isSystemObject = true;
   
   ok = psonTopFolderCloseObject( &folderItem, &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;
   
   ok = psonTopFolderOpenObject( pTree,
                                 "proc",
                                 strlen("proc"),
                                 PSO_FOLDER,
                                 &folderItem,
                                 &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;

   GET_PTR(pHandler->context.pBaseAddress, pNode, folderItem.pHashItem->dataOffset, psonTreeNode );
   GET_PTR(pHandler->context.pBaseAddress, pFolder, pNode->offset, psonFolder );
   pFolder->isSystemObject = true;
   
   ok = psonTopFolderCloseObject( &folderItem, &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;

#if 0
   ok = psonTopFolderOpenObject( pTree,
                                 "system/Data_Definition",
                                 strlen("system/Data_Definition"),
                                 PSO_HASH_MAP,
                                 &folderItem,
                                 &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;

   GET_PTR(pHandler->context.pBaseAddress, pNode, folderItem.pHashItem->dataOffset, psonTreeNode );
   GET_PTR(pHandler->context.pBaseAddress, pHashMap, pNode->offset, psonHashMap );
   pHashMap->isSystemObject = true;
   pHandler->pMemHeader->dataDefMapOffset = pNode->offset;

   /*
    * Insert a default data definition.
    */
   /* We need to serialize the inputs to insert the record in the hash map */
   recLength = offsetof( psonDataDefinition, definition ) + 
      sizeof(psoFieldDefinition);
   pMemDataDefinition = malloc( recLength );
   if ( pMemDataDefinition == NULL ) {
      return false;
   }
   pMemDataDefinition->type = PSO_DEF_PHOTON_ODBC_SIMPLE;
   pMemDataDefinition->definitionLength = sizeof(psoFieldDefinition);
   memcpy( pMemDataDefinition->definition, &fields, sizeof(psoFieldDefinition) );

   ok = psonHashMapInsert( pHashMap,
                           "Default",
                           strlen("Default"),
                           pMemDataDefinition,
                           recLength,
                           &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   free( pMemDataDefinition );
   if ( ! ok ) return false;

   ok = psonTopFolderCloseObject( &folderItem, &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;

   ok = psonTopFolderOpenObject( pTree,
                                 "system/Key_Definition",
                                 strlen("system/Key_Definition"),
                                 PSO_HASH_MAP,
                                 &folderItem,
                                 &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;

   GET_PTR(pHandler->context.pBaseAddress, pNode, folderItem.pHashItem->dataOffset, psonTreeNode );
   GET_PTR(pHandler->context.pBaseAddress, pHashMap, pNode->offset, psonHashMap );
   pHashMap->isSystemObject = true;
   pHandler->pMemHeader->keyDefMapOffset = pNode->offset;
   
   /*
    * Insert a default data definition.
    */
   /* We need to serialize the inputs to insert the record in the hash map */
   recLength = offsetof( psonKeyDefinition, definition ) + 
      sizeof(psoKeyFieldDefinition);
   pMemKeyDefinition = malloc( recLength );
   if ( pMemKeyDefinition == NULL ) {
      return false;
   }
   pMemKeyDefinition->type = PSO_DEF_PHOTON_ODBC_SIMPLE;
   pMemKeyDefinition->definitionLength = sizeof(psoKeyFieldDefinition);
   memcpy( pMemKeyDefinition->definition, &keys, sizeof(psoKeyFieldDefinition) );

   ok = psonHashMapInsert( pHashMap,
                           "Default",
                           strlen("Default"),
                           pMemKeyDefinition,
                           recLength,
                           &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   free( pMemKeyDefinition );
   if ( ! ok ) return false;

   ok = psonTopFolderCloseObject( &folderItem, &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;
#endif

   ok = psonTxCommit( (psonTx*)pHandler->context.pTransaction, 
                       &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;

   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool qsrHandlerCreateSession( qsrHandler * pHandler )
{
   psonProcMgr* pCleanupManager;
   bool ok;
   struct psonProcess * pCleanup;
   void * pSession = (void *)pHandler;
   struct psonSession * pSessionCleanup;

   GET_PTR(pHandler->context.pBaseAddress, pHandler->context.pAllocator, 
            pHandler->pMemHeader->allocatorOffset, void );
   GET_PTR(pHandler->context.pBaseAddress, pCleanupManager, 
            pHandler->pMemHeader->processMgrOffset, psonProcMgr );

   ok = psonProcMgrAddProcess( pCleanupManager,
                               getpid(), 
                               &pCleanup,
                               &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );

//   psoErrors errcode = PSO_OK;
//   psoaSession* pSession = NULL;
//   void* ptr = NULL;
//   bool ok;
   
//   if ( sessionHandle == NULL ) return PSO_NULL_HANDLE;
   
//   *sessionHandle = NULL;

//   if ( g_processInstance == NULL ) return PSO_PROCESS_NOT_INITIALIZED;
   
//   pSession = (psoaSession*) malloc(sizeof(psoaSession));
//   if ( pSession == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
   
//   memset( pSession, 0, sizeof(psoaSession) );
//   pSession->type = PSOA_SESSION;

   /*
    * From this point we can use "goto error_handler" to recover
    * from errors;
    */
   
   ok = psonProcessAddSession( pCleanup, 
                               pSession, 
                               &pSessionCleanup, 
                               &pHandler->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrHandlerFini( qsrHandler * pHandler )
{
   PSO_PRE_CONDITION( pHandler != NULL );

   pHandler->pConfig = NULL;
   pHandler->pMemManager = NULL;
   pHandler->pMemHeader = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool qsrHandlerInit( qsrHandler          * pHandler,
                     struct ConfigParams * pConfig,
                     psonMemoryHeader   ** ppMemoryAddress,
                     bool                  verifyMemOnly )
{
   bool ok;
   int errcode = 0;
   char path[PATH_MAX];
   char logFile[PATH_MAX];
   int path_len;
   psocMemoryFileStatus fileStatus;
   psocMemoryFile memFile;
   FILE * fp = NULL;
   char timeBuf[30];
   time_t t;
   struct tm formattedTime;
   size_t numObjectsOK = 0;
   size_t numObjectsRepaired = 0;
   size_t numObjectsDeleted = 0;
   size_t numObjectsError = 0;

   PSO_PRE_CONDITION( pHandler        != NULL );
   PSO_PRE_CONDITION( pConfig         != NULL );
   PSO_PRE_CONDITION( ppMemoryAddress != NULL );

   pHandler->pMemHeader = NULL;

   memset( &pHandler->context, 0, sizeof(psonSessionContext) );
   pHandler->context.pidLocker = getpid();
   
   ok = psonInitEngine();
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in ShMemHandler.c\n", __LINE__ );
      exit(1);
   }
   psocInitErrorHandler( &pHandler->context.errorHandler );

   pHandler->pConfig = pConfig;

   pHandler->pMemManager = (qsrMemoryManager *)calloc( 
      sizeof(qsrMemoryManager), 1 );
   if ( pHandler->pMemManager == NULL ) {
      psocSetError( &pHandler->context.errorHandler,
                    g_qsrErrorHandle,
                    QSR_NOT_ENOUGH_HEAP_MEMORY );
      return false;
   }
   qsrMemoryManagerInit( pHandler->pMemManager );
   
   path_len = strlen( pConfig->memLocation ) + strlen( PSO_DIR_SEPARATOR ) +
      strlen( PSO_MEMFILE_NAME )  + strlen( ".bak" );
   if ( path_len >= PATH_MAX ) {
      psocSetError( &pHandler->context.errorHandler,
                    g_qsrErrorHandle,
                    QSR_CFG_BCK_LOCATION_TOO_LONG );
      return false;
   }
      
   sprintf( path, "%s%s%s", pConfig->memLocation, PSO_DIR_SEPARATOR,
            PSO_MEMFILE_NAME );
   
   psocInitMemoryFile ( &memFile, pConfig->memorySizekb, path );
   psocBackStoreStatus( &memFile, &fileStatus );

   if ( ! fileStatus.fileExist ) {
      if ( verifyMemOnly ) {
         psocSetError( &pHandler->context.errorHandler,
                       g_qsrErrorHandle,
                       QSR_NO_VERIFICATION_POSSIBLE );
         return false;
      }
      ok = qsrCreateMem( pHandler->pMemManager,
                         path,
                         pConfig->memorySizekb,
                         pConfig->filePerms,
                         ppMemoryAddress,
                         &pHandler->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return false;
      
   }
   else {
      if ( ! fileStatus.fileReadable || ! fileStatus.fileWritable || 
         ! fileStatus.lenghtOK ) {
         psocSetError( &pHandler->context.errorHandler,
                       g_qsrErrorHandle,
                       QSR_FILE_NOT_ACCESSIBLE );
         return false;
      }

      memset( logFile, '\0', PATH_MAX );
      memset( timeBuf, '\0', 30 );

      t = time(NULL);
      localtime_r( &t, &formattedTime );
      strftime( timeBuf, 30, "%Y_%m_%d_%H_%M_%S", &formattedTime );

      path_len = strlen( pConfig->memLocation ) + strlen( PSO_DIR_SEPARATOR ) +
      strlen("Logs") + strlen( PSO_DIR_SEPARATOR ) +
      strlen("Verify_") + strlen(timeBuf) + strlen(".log");
      if ( path_len < PATH_MAX ) {
        sprintf( logFile, "%s%s%s", 
            pConfig->memLocation, 
            PSO_DIR_SEPARATOR,
            "Logs" );
      
         errcode = mkdir( logFile, pConfig->dirPerms );
         if ( errcode != 0 ) {
            if ( errno != EEXIST ) {
               psocSetError( &pHandler->context.errorHandler,
                             PSOC_ERRNO_HANDLE,
                             errno );
               psocChainError( &pHandler->context.errorHandler,
                               g_qsrErrorHandle,
                               QSR_MKDIR_FAILURE );
               return false;
            }
         }
         sprintf( logFile, "%s%s%s%s%s%s%s", 
            pConfig->memLocation, 
            PSO_DIR_SEPARATOR,
            "Logs",
            PSO_DIR_SEPARATOR,
            "Verify_",
            timeBuf,
            ".log" );
      
         fp = fopen( logFile, "w" );
      }
      if ( fp == NULL ) fp = stderr;

      if ( ! verifyMemOnly ) {
         ok = psocCopyBackstore( &memFile,
                                 pConfig->filePerms,
                                 &pHandler->context.errorHandler );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) {
            psocChainError( &pHandler->context.errorHandler,
                            g_qsrErrorHandle,
                            QSR_COPY_BCK_FAILURE );
            return false;
         }
      }
      
      ok = qsrOpenMem( pHandler->pMemManager,
                        path,
                        pConfig->memorySizekb,
                        ppMemoryAddress,
                        &pHandler->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return false;
      
      fprintf( stderr, "Starting the recovery of the shared memory, please be patient\n" );
      if ( verifyMemOnly ) {
         qsrVerify( *ppMemoryAddress, 
                     &numObjectsOK,
                     &numObjectsRepaired,
                     &numObjectsDeleted,
                     &numObjectsError,
                     fp,
                     &pHandler->context );
         if ( fp != stderr ) fclose( fp );
         return (numObjectsError == 0);
      }
      qsrRepair( *ppMemoryAddress, 
                  &numObjectsOK,
                  &numObjectsRepaired,
                  &numObjectsDeleted,
                  &numObjectsError,
                  fp,
                  &pHandler->context );
      if ( fp != stderr ) fclose( fp );

      fprintf( stderr, "Number of objects with no defects: "PSO_SIZE_T_FORMAT"\n", numObjectsOK );
      fprintf( stderr, "Number of objects with small defects: "PSO_SIZE_T_FORMAT"\n", numObjectsRepaired );
      fprintf( stderr, "Number of deleted objects (added but not committed, etc.): "PSO_SIZE_T_FORMAT"\n", numObjectsDeleted );
      fprintf( stderr, "Number of objects with errors: "PSO_SIZE_T_FORMAT"\n", numObjectsError );
      if ( numObjectsError != 0 ) {
         fprintf( stderr, "Failure repairing the shared memory (see log for details)\n" );
         fprintf( stderr, "Please save the backup (%s%s) \n%s\n",
            path, ".bak",
            "before running this program a second time." );
         return false;
      }

   }

   pHandler->pMemHeader = *ppMemoryAddress;

   ok = qsrHandlerCreateSession( pHandler );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) return false;
   
   if ( ! fileStatus.fileExist ) {
      ok = qsrHandlerAddSystemObjects( pHandler );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) return false;
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

//void CleanSession( psonSession * pSession )
//{
//   fprintf( stderr, "Session %p\n", pSession );

/*
- need to loop on all objects to see if we hold a lock somewhere
 */
//}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrHandleCrash( qsrHandler * pHandler, pid_t pid )
{
//   int errcode;
   PSO_PRE_CONDITION( pHandler != NULL );

#if 0   
   psonProcess* process = NULL;
   psonSessionContext context;
   psonSession* pFirstSession = NULL;
   psonSession* pCurrSession  = NULL;
   psonSession* pNextSession  = NULL;

   memset( &context, 0, sizeof context );
   context.lockValue = getpid();
   context.pAllocator = &pHandler->pMemHeader->allocator;
   
//      GET_PTR(pHandler->context.pBaseAddress, m_pMemHeader->allocatorOffset, MemoryAllocator );

   psonProcessManager* pCleanupManager = 
      GET_PTR(pHandler->context.pBaseAddress, m_pMemHeader->cleanupMgrOffset, psonProcessManager );

   // Start by checking if we are not holding the lock to the 
   // cleanup manager, the memory allocator, etc...

   //////////////////////////////// ADD CODE HERE

   // Find the linked list of all "recovery information" for our crash
   // process - more specifically there are:
   //  - one or more linked list, one per session, for all objects 
   //    associated with that session
   //  - and a process wide linked list of all these session-related 
   //    linked lists
   // (to be complete: the cleanup manager is itself a linked list made 
   // of all the process wide linked lists.)
   // 
   errcode = pCleanupManager->FindProcess( pid, &process, &context );
   
   fprintf( stderr, "Abnormal %d %d %p %p\n", pid, errcode, pCleanupManager, process );

   if ( errcode != 0 ) {
      // log it
      return;
   }

   /*
    * Loop on all sessions to clean them up. It is indeed possible to 
    * have no current session - a non-null errcode is not an error.
    */
   errcode = process->GetFirstSession( &pCurrSession, &context );
   fprintf( stderr, "Errcode %d %p \n", errcode, pCurrSession );
   if ( errcode == 0 ) {
      CleanSession( pCurrSession );

      errcode = process->GetNextSession( pCurrSession, 
                                          &pNextSession,
                                          &context );
      while ( errcode == 0 ) {
         CleanSession( pNextSession );
         pCurrSession = pNextSession;
         errcode = process->GetNextSession( pCurrSession, 
                                             &pNextSession,
                                             &context );
      }
   }
   
   /*
    * Remove the CleanupProcess object (it will iteratively remove all 
    * its "children").
    */
   errcode = pCleanupManager->RemoveProcess( process, &context );
   if ( errcode != 0 ) {
      // log it
   }
#endif // #if 0
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

