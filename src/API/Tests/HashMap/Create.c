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
   PSO_HANDLE shmemHandle;
   int errcode;

   psoObjectDefinition definition;
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };
   psoKeyDefinition keyDef = { PSO_DEF_USER_DEFINED, 0, '\0' };

   errcode = psoInit( "10701", "Create" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_create",
                              strlen("/api_hashmap_create") );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderOpen( sessionHandle,
                            "/api_hashmap_create",
                            strlen("/api_hashmap_create"),
                            &folderHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid definition. */
   
   memset( &definition, 0, sizeof(psoObjectDefinition) );
   
   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map",
                                 strlen("my_map"),
                                 &definition,
                                 &keyDef );
   assert_true( errcode == PSO_WRONG_OBJECT_TYPE );

   errcode = psoFolderCreateMap( NULL,
                                 "my_map",
                                 strlen("my_map"),
                                 &def,
                                 &keyDef );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoFolderCreateMap( folderHandle,
                                 NULL,
                                 strlen("my_map"),
                                 &def,
                                 &keyDef );
   fprintf( stderr, "%d\n", errcode );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map",
                                 0,
                                 &def,
                                 &keyDef );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map",
                                 strlen("my_map"),
                                 NULL,
                                 &keyDef );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map",
                                 strlen("my_map"),
                                 &def,
                                 NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map",
                                 strlen("my_map"),
                                 &def,
                                 &keyDef );
   assert_true( errcode == PSO_OK );

   /* Close the folder and try to act on it */
   
   errcode = psoFolderClose( folderHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map2",
                                 strlen("my_map2"),
                                 &def,
                                 &keyDef );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   /* Reopen the folder, close the process and try to act on the session */

   errcode = psoFolderOpen( sessionHandle,
                            "/api_hashmap_create",
                            strlen("/api_hashmap_create"),
                            &folderHandle );
   assert_true( errcode == PSO_OK );
   psoExit();
   
   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map3",
                                 strlen("my_map3"),
                                 &def,
                                 &keyDef );
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

