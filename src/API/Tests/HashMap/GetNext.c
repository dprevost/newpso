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
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   char buffer[200];
   char buffKey[50];
   uint32_t dataLength, keyLength;
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

   errcode = psoInit( "10701", "GetNext" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_get_next",
                              strlen("/api_hashmap_get_next") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_GetNext",
                              strlen("API_Hashmap_GetNext"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_GetNext",
                               strlen("API_Hashmap_GetNext"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_get_next/test",
                           strlen("/api_hashmap_get_next/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_get_next/test",
                             strlen("/api_hashmap_get_next/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key1,
                               7,
                               data1,
                               8 );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key2,
                               7,
                               data2,
                               8 );
   assert_true( errcode == PSO_OK );

   /* No GetFirst... */
   errcode = psoHashMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   assert_true( errcode == PSO_INVALID_ITERATOR );

   errcode = psoHashMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoHashMapGetNext( NULL,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoHashMapGetNext( objHandle,
                                NULL,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoHashMapGetNext( objHandle,
                                buffKey,
                                2,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoHashMapGetNext( objHandle,
                                buffKey,
                                50,
                                NULL,
                                200,
                                &keyLength,
                                &dataLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoHashMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                2,
                                &keyLength,
                                &dataLength );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoHashMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                NULL,
                                &dataLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoHashMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoHashMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   assert_true( errcode == PSO_OK );

   /* Close the session and try to act on the object */

   /* Reset the internal iterator first... */
   errcode = psoHashMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   assert_true( errcode == PSO_OK );

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
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

