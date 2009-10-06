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
   PSO_HANDLE objHandle, sessionHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * key  = "My Key";
   const char * data1 = "My Data1";
   const char * data2 = "My Data 2";
   char buffer[20];
   uint32_t length;
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   psoKeyDefinition * pKeyDefinition;
   
   pKeyDefinition = malloc( offsetof( psoKeyDefinition, definition) +
                           sizeof(psoKeyFieldDefinition) );
   assert_false( pKeyDefinition == NULL );

   pKeyDefinition->type = PSO_DEF_PHOTON_ODBC_SIMPLE;
   pKeyDefinition->definitionLength = sizeof(psoKeyFieldDefinition);
   memcpy( pKeyDefinition->definition, &keyDef, sizeof(psoKeyFieldDefinition) );

   errcode = psoInit( "10701", "Replace" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_replace",
                              strlen("/api_hashmap_replace") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_Replace",
                              strlen("API_Hashmap_Replace"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_Replace",
                               strlen("API_Hashmap_Replace"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_replace/test",
                           strlen("/api_hashmap_replace/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_replace/test",
                             strlen("/api_hashmap_replace/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoHashMapOpen( sessionHandle2,
                             "/api_hashmap_replace/test",
                             strlen("/api_hashmap_replace/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data1,
                               strlen(data1) );
   assert_true( errcode == PSO_OK );

   /* Must commit the insert before replacing */
   errcode = psoHashMapReplace( objHandle,
                                key,
                                6,
                                data2,
                                strlen(data2) );
   assert_true( errcode == PSO_ITEM_IS_IN_USE );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoHashMapReplace( NULL,
                                key,
                                6,
                                data2,
                                strlen(data2) );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoHashMapReplace( objHandle,
                                NULL,
                                6,
                                data2,
                                strlen(data2) );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoHashMapReplace( objHandle,
                                key,
                                0,
                                data2,
                                strlen(data2) );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoHashMapReplace( objHandle,
                                key,
                                6,
                                NULL,
                                strlen(data2) );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoHashMapReplace( objHandle,
                                key,
                                6,
                                data2,
                                0 );
   assert_true( errcode == PSO_INVALID_LENGTH );

   /* End of invalid args. This call should succeed. */
   errcode = psoHashMapReplace( objHandle,
                                key,
                                6,
                                data2,
                                strlen(data2) );
   assert_true( errcode == PSO_OK );

   /*
    * Additional stuff to check while the Replace() is uncommitted:
    *  - first session get new value.
    *  - second session get old value
    *  - cannot modify it from any session.
    */
   errcode = psoHashMapGet( objHandle,
                            key,
                            6,
                            buffer,
                            20,
                            &length );
   assert_true( errcode == PSO_OK );
   assert_true( length == strlen(data2) );
   assert_true( memcmp( buffer, data2, length ) == 0 );
   
   errcode = psoHashMapGet( objHandle2,
                            key,
                            6,
                            buffer,
                            20,
                            &length );
   assert_true( errcode == PSO_OK );
   assert_true( length == strlen(data1) );
   assert_true( memcmp( buffer, data1, length ) == 0 );

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapReplace( objHandle,
                                key,
                                6,
                                data2,
                                strlen(data2) );
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

