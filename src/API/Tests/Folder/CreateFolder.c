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
   PSO_HANDLE sessionHandle, folderHandle;
   int errcode;
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;
   
   errcode = psoInit( "10701", "CreateFolder" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_folder_create_folder",
                              strlen("/api_folder_create_folder") );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_create_folder",
                            strlen("/api_folder_create_folder"),
                            &folderHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "Definition",
                               strlen("Definition"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoFolderCreateFolder( NULL,
                                    "api_folder_create",
                                    strlen("api_folder_create") );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoFolderCreateFolder( sessionHandle,
                                    "api_folder_create",
                                    strlen("api_folder_create") );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoFolderCreateFolder( folderHandle,
                                    NULL,
                                    strlen("api_folder_create") );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoFolderCreateFolder( folderHandle,
                                    "api_folder_create",
                                    0 );
   assert_true( errcode == PSO_INVALID_LENGTH );

   /* End of invalid args. This call should succeed. */
   errcode = psoFolderCreateFolder( folderHandle,
                                    "api_folder_create",
                                    strlen("api_folder_create") );
   assert_true( errcode == PSO_OK );

   /* Close the folder and try to act on it */
   
   errcode = psoFolderClose( folderHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoFolderCreateFolder( folderHandle,
                                    "api_folder_create2",
                                    strlen("api_folder_create2") );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   /* Reopen the folder, close the process and try to act on the session */

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_create_folder",
                            strlen("/api_folder_create_folder"),
                            &folderHandle );
   assert_true( errcode == PSO_OK );
   psoExit();
   
   errcode = psoFolderCreateFolder( folderHandle,
                                    "api_folder_create3",
                                    strlen("api_folder_create3") );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );
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

