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
#include "API/HashMap.h"
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
   PSO_HANDLE objHandle,  sessionHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   PSO_HANDLE shmemHandle;
   int errcode;
   char junk[12];
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   const char * data1 = "My Data1";
   psoKeyDefinition * pKeyDefinition;
   
   pKeyDefinition = malloc( offsetof( psoKeyDefinition, definition) +
                           sizeof(psoKeyFieldDefinition) );
   assert_false( pKeyDefinition == NULL );

   pKeyDefinition->type = PSO_DEF_PHOTON_ODBC_SIMPLE;
   pKeyDefinition->definitionLength = sizeof(psoKeyFieldDefinition);
   memcpy( pKeyDefinition->definition, &keyDef, sizeof(psoKeyFieldDefinition) );
   
   memset( junk, 0, 12 );
   
   errcode = psoInit( "10701", "Open" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( shmemHandle, &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_open",
                              strlen("/api_hashmap_open") );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_open/test",
                           strlen("/api_hashmap_open/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoHashMapOpen( NULL,
                             "/api_hashmap_open/test",
                             strlen("/api_hashmap_open/test"),
                             &objHandle );
   assert_true( errcode == PSO_NULL_HANDLE );

   objHandle = (PSO_HANDLE) junk;
   errcode = psoHashMapOpen( objHandle,
                             "/api_hashmap_open/test",
                             strlen("/api_hashmap_open/test"),
                             &objHandle );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoHashMapOpen( sessionHandle,
                             NULL,
                             strlen("/api_hashmap_open/test"),
                             &objHandle );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_open/test",
                             0,
                             &objHandle );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_open/test",
                             strlen("/api_hashmap_open/test"),
                             NULL );
   assert_true( errcode == PSO_NULL_HANDLE );

   /* End of invalid args. This call should succeed. */
   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_open/test",
                             strlen("/api_hashmap_open/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle2,
                             "/api_hashmap_open/test",
                             strlen("/api_hashmap_open/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OBJECT_IS_IN_USE );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_open/test",
                             strlen("/api_hashmap_open/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );

   psoHashMapClose( objHandle );
   psoRollback( sessionHandle );

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

