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

void test_null_data( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned int length;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_retrieve_null_data",
                              strlen("/api_hashmap_retrieve_null_data") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_RetrieveNullData",
                              strlen("API_Hashmap_RetrieveNullData"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_RetrieveNullData",
                               strlen("API_Hashmap_RetrieveNullData"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_retrieve_null_data/test",
                           strlen("/api_hashmap_retrieve_null_data/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_retrieve_null_data/test",
                             strlen("/api_hashmap_retrieve_null_data/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapRetrieve( objHandle,
                                               key,
                                               6,
                                               NULL,
                                               &length ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_handle( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * buffer;
   unsigned int length;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_retrieve_null_handle",
                              strlen("/api_hashmap_retrieve_null_handle") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_RetrieveNullHandle",
                              strlen("API_Hashmap_RetrieveNullHandle"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_RetrieveNullHandle",
                               strlen("API_Hashmap_RetrieveNullHandle"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_retrieve_null_handle/test",
                           strlen("/api_hashmap_retrieve_null_handle/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_retrieve_null_handle/test",
                             strlen("/api_hashmap_retrieve_null_handle/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapRetrieve( NULL,
                                               key,
                                               6,
                                               &buffer,
                                               &length ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_key( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * buffer;
   unsigned int length;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_retrieve_null_key",
                              strlen("/api_hashmap_retrieve_null_key") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_RetrieveNullKey",
                              strlen("API_Hashmap_RetrieveNullKey"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_RetrieveNullKey",
                               strlen("API_Hashmap_RetrieveNullKey"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_retrieve_null_key/test",
                           strlen("/api_hashmap_retrieve_null_key/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_retrieve_null_key/test",
                             strlen("/api_hashmap_retrieve_null_key/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapRetrieve( objHandle,
                                               NULL,
                                               6,
                                               &buffer,
                                               &length ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_length( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * buffer;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_retrieve_null_length",
                              strlen("/api_hashmap_retrieve_null_length") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_RetrieveNullLength",
                              strlen("API_Hashmap_RetrieveNullLength"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_RetrieveNullLength",
                               strlen("API_Hashmap_RetrieveNullLength"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_retrieve_null_length/test",
                           strlen("/api_hashmap_retrieve_null_length/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_retrieve_null_length/test",
                             strlen("/api_hashmap_retrieve_null_length/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapRetrieve( objHandle,
                                               key,
                                               6,
                                               &buffer,
                                               NULL ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_handle( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * buffer;
   unsigned int length;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_retrieve_wrong_handle",
                              strlen("/api_hashmap_retrieve_wrong_handle") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_RetrieveWrongHandle",
                              strlen("API_Hashmap_RetrieveWrongHandle"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_RetrieveWrongHandle",
                               strlen("API_Hashmap_RetrieveWrongHandle"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_retrieve_wrong_handle/test",
                           strlen("/api_hashmap_retrieve_wrong_handle/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_retrieve_wrong_handle/test",
                             strlen("/api_hashmap_retrieve_wrong_handle/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapRetrieve( sessionHandle,
                                               key,
                                               6,
                                               &buffer,
                                               &length ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * buffer;
   unsigned int length;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_retrieve_zero_length",
                              strlen("/api_hashmap_retrieve_zero_length") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_RetrieveZeroLength",
                              strlen("API_Hashmap_RetrieveZeroLength"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_RetrieveZeroLength",
                               strlen("API_Hashmap_RetrieveZeroLength"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_retrieve_zero_length/test",
                           strlen("/api_hashmap_retrieve_zero_length/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_retrieve_zero_length/test",
                             strlen("/api_hashmap_retrieve_zero_length/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapRetrieve( objHandle,
                                               key,
                                               0,
                                               &buffer,
                                               &length ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * buffer;
   unsigned int length;

   errcode = psoInit( "10701", "Retrieve" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_retrieve_pass",
                              strlen("/api_hashmap_retrieve_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_RetrievePass",
                              strlen("API_Hashmap_RetrievePass"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_RetrievePass",
                               strlen("API_Hashmap_RetrievePass"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_retrieve_pass/test",
                           strlen("/api_hashmap_retrieve_pass/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_retrieve_pass/test",
                             strlen("/api_hashmap_retrieve_pass/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );

   errcode = psoaHashMapRetrieve( objHandle,
                            key,
                            6,
                            &buffer,
                            &length );
   assert_true( errcode == PSO_OK );
   assert_true( memcmp( buffer, data, 7 ) == 0 );
   
   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_data,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_handle,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_key,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_handle, setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

