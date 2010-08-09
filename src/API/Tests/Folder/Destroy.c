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
#include <photon/photon.h>
#include "API/Tests/quasar-run.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   assert( startQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   assert( stopQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE sessionHandle, folderHandle;
   PSO_HANDLE objHandle, sessionHandle2;
   PSO_HANDLE shmemHandle;
   int errcode;
   
   errcode = psoInit( "10701", "Destroy" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( shmemHandle, &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_folder_destroy",
                              strlen("/api_folder_destroy") );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_destroy",
                            strlen("/api_folder_destroy"),
                            &folderHandle );
   assert_true( errcode == PSO_OK );

   /* Destroy non-existing object. */
   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   assert_true( errcode == PSO_NO_SUCH_OBJECT );

   errcode = psoFolderCreateFolder( folderHandle,
                                    "test1",
                                    strlen("test1") );
   assert_true( errcode == PSO_OK );

   /* Destroy without a commit - should fail */
   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   assert_true( errcode == PSO_OBJECT_IS_IN_USE );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );
   
   /* Invalid arguments to tested function. */

   errcode = psoFolderDestroyObject( NULL,
                                     "test1",
                                     strlen("test1") );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoFolderDestroyObject( folderHandle,
                                     NULL,
                                     strlen("test1") );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     0 );
   assert_true( errcode == PSO_INVALID_LENGTH );

   /* End of invalid args. This call should succeed. */

   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   assert_true( errcode == PSO_OK );

   /* Open on the same session - should fail */
   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_destroy/test1",
                            strlen("/api_folder_destroy/test1"),
                            &objHandle );
   assert_true( errcode == PSO_OBJECT_IS_DELETED );

   /* Open with a different session - should work */
   errcode = psoFolderOpen( sessionHandle2,
                            "/api_folder_destroy/test1",
                            strlen("/api_folder_destroy/test1"),
                            &objHandle );
   assert_true( errcode == PSO_OK );

   /* 
    * Commit with session #2 having the object open. The object should 
    * still be in shared memory but we should be able to create a new one.
    */
   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   assert_true( errcode == PSO_NO_SUCH_OBJECT );

   errcode = psoFolderCreateFolder( folderHandle,
                                    "test1",
                                    strlen("test1") );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_destroy/test1",
                            strlen("/api_folder_destroy/test1"),
                            &objHandle );
   assert_true( errcode == PSO_OK );
   
   /* Close the process and try to act on the session */

   psoExit();
   
   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_pass, setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

