/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "API/Folder.h"
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
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0 };
   psoFieldDefinition dataDef = { "Field_1", PSO_VARCHAR, {120} };
   psoKeyFieldDefinition keyDef = { "Key1", PSO_KEY_VARCHAR, 80 };
   PSO_HANDLE dataDefHandle, keyDefHandle;
   PSO_HANDLE returnedDef;
   
   errcode = psoInit( "10701", "GetDataDef" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_folder_getdef",
                              strlen("/api_folder_getdef") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_folder_getdef",
                               strlen("api_folder_getdef"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)&dataDef,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_folder_getdef",
                              strlen("api_folder_getdef"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_folder_getdef/map1",
                           strlen("/api_folder_getdef/map1"),
                           &def,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_getdef",
                            strlen("/api_folder_getdef"),
                            &objHandle );
   assert_true( errcode == PSO_OK );
   
   /* Invalid arguments to tested function. */
   errcode = psoFolderGetDataDefinition( NULL,
                                         "map1",
                                         strlen("map1"),
                                         &returnedDef );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoFolderGetDataDefinition( sessionHandle,
                                         "map1",
                                         strlen("map1"),
                                         &returnedDef );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoFolderGetDataDefinition( objHandle,
                                         NULL,
                                         strlen("map1"),
                                         &returnedDef );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );
   errcode = psoFolderGetDataDefinition( objHandle,
                                         "map1",
                                         0,
                                         &returnedDef );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoFolderGetDataDefinition( objHandle,
                                         "map1",
                                         strlen("map1"),
                                         NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoFolderGetDataDefinition( objHandle,
                                         "map1",
                                         strlen("map1"),
                                         &returnedDef );
   assert_true( errcode == PSO_OK );

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderGetDataDefinition( objHandle,
                                         "map1",
                                         strlen("map1"),
                                         &returnedDef );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   psoExit();
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

