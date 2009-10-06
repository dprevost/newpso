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

psoKeyDefinition * pKeyDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };

   pKeyDefinition = malloc( offsetof( psoKeyDefinition, definition) +
                           sizeof(psoKeyFieldDefinition) );
   assert( pKeyDefinition != NULL );

   pKeyDefinition->type = PSO_DEF_PHOTON_ODBC_SIMPLE;
   pKeyDefinition->definitionLength = sizeof(psoKeyFieldDefinition);
   memcpy( pKeyDefinition->definition, &keyDef, sizeof(psoKeyFieldDefinition) );

   assert( startQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if ( pKeyDefinition ) free( pKeyDefinition );
   pKeyDefinition = NULL;
   assert( stopQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_no_first( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * keyBuffer, * buffer;
   unsigned int keyLength, bufferLength;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_next_no_first",
                              strlen("/api_hashmap_next_no_first") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_NextNoFirst",
                              strlen("API_Hashmap_NextNoFirst"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_NextNoFirst",
                               strlen("API_Hashmap_NextNoFirst"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_next_no_first/test",
                           strlen("/api_hashmap_next_no_first/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_next_no_first/test",
                             strlen("/api_hashmap_next_no_first/test"),
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

   expect_assert_failure( psoaHashMapNext( objHandle,
                                           &keyBuffer,
                                           &keyLength, 
                                           &buffer,
                                           &bufferLength ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_data( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * keyBuffer, * buffer;
   unsigned int keyLength, bufferLength;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_next_null_data",
                              strlen("/api_hashmap_next_null_data") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_NextNullData",
                              strlen("API_Hashmap_NextNullData"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_NextNullData",
                               strlen("API_Hashmap_NextNullData"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_next_null_data/test",
                           strlen("/api_hashmap_next_null_data/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_next_null_data/test",
                             strlen("/api_hashmap_next_null_data/test"),
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

   errcode = psoaHashMapFirst( objHandle, &keyBuffer, &keyLength, 
                               &buffer, &bufferLength );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapNext( objHandle,
                                           &keyBuffer,
                                           &keyLength, 
                                           NULL,
                                           &bufferLength ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_datalength( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * keyBuffer, * buffer;
   unsigned int keyLength, bufferLength;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_next_null_data_length",
                              strlen("/api_hashmap_next_null_data_length") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_NextNullDataLength",
                              strlen("API_Hashmap_NextNullDataLength"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_NextNullDataLength",
                               strlen("API_Hashmap_NextNullDataLength"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_next_null_data_length/test",
                           strlen("/api_hashmap_next_null_data_length/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_next_null_data_length/test",
                             strlen("/api_hashmap_next_null_data_length/test"),
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

   errcode = psoaHashMapFirst( objHandle, &keyBuffer, &keyLength, 
                               &buffer, &bufferLength );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapNext( objHandle,
                                           &keyBuffer,
                                           &keyLength, 
                                           &buffer,
                                           NULL ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_handle( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * keyBuffer, * buffer;
   unsigned int keyLength, bufferLength;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_next_null_handle",
                              strlen("/api_hashmap_next_null_handle") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_NextNullHandle",
                              strlen("API_Hashmap_NextNullHandle"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_NextNullHandle",
                               strlen("API_Hashmap_NextNullHandle"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_next_null_handle/test",
                           strlen("/api_hashmap_next_null_handle/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_next_null_handle/test",
                             strlen("/api_hashmap_next_null_handle/test"),
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

   errcode = psoaHashMapFirst( objHandle, &keyBuffer, &keyLength, 
                               &buffer, &bufferLength );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapNext( NULL,
                                           &keyBuffer,
                                           &keyLength, 
                                           &buffer,
                                           &bufferLength ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_key( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * keyBuffer, * buffer;
   unsigned int keyLength, bufferLength;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_next_null_key",
                              strlen("/api_hashmap_next_null_key") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_NextNullKey",
                              strlen("API_Hashmap_NextNullKey"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_NextNullKey",
                               strlen("API_Hashmap_NextNullKey"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_next_null_key/test",
                           strlen("/api_hashmap_next_null_key/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_next_null_key/test",
                             strlen("/api_hashmap_next_null_key/test"),
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

   errcode = psoaHashMapFirst( objHandle, &keyBuffer, &keyLength, 
                               &buffer, &bufferLength );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapNext( objHandle,
                                           NULL,
                                           &keyLength, 
                                           &buffer,
                                           &bufferLength ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_keylength( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * keyBuffer, * buffer;
   unsigned int keyLength, bufferLength;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_next_null_key_length",
                              strlen("/api_hashmap_next_null_key_length") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_NextNullKeyLength",
                              strlen("API_Hashmap_NextNullKeyLength"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_NextNullKeyLength",
                               strlen("API_Hashmap_NextNullKeyLength"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_next_null_key_length/test",
                           strlen("/api_hashmap_next_null_key_length/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_next_null_key_length/test",
                             strlen("/api_hashmap_next_null_key_length/test"),
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

   errcode = psoaHashMapFirst( objHandle, &keyBuffer, &keyLength, 
                               &buffer, &bufferLength );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapNext( objHandle,
                                           &keyBuffer,
                                           NULL, 
                                           &buffer,
                                           &bufferLength ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_handle( void ** state )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * keyBuffer, * buffer;
   unsigned int keyLength, bufferLength;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_next_wrong_handle",
                              strlen("/api_hashmap_next_wrong_handle") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_NextWrongHandle",
                              strlen("API_Hashmap_NextWrongHandle"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_NextWrongHandle",
                               strlen("API_Hashmap_NextWrongHandle"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_next_wrong_handle/test",
                           strlen("/api_hashmap_next_wrong_handle/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                            "/api_hashmap_next_wrong_handle/test",
                             strlen("/api_hashmap_next_wrong_handle/test"),
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

   errcode = psoaHashMapFirst( objHandle, &keyBuffer, &keyLength, 
                               &buffer, &bufferLength );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaHashMapNext( sessionHandle,
                                           &keyBuffer,
                                           &keyLength, 
                                           &buffer,
                                           &bufferLength ) );
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
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * keyBuffer, * buffer;
   unsigned int keyLength, bufferLength;

   errcode = psoInit( "10701", "Next" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_next_pass",
                              strlen("/api_hashmap_next_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_NextPass",
                              strlen("API_Hashmap_NextPass"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_NextPass",
                               strlen("API_Hashmap_NextPass"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_next_pass/test",
                           strlen("/api_hashmap_next_pass/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                            "/api_hashmap_next_pass/test",
                             strlen("/api_hashmap_next_pass/test"),
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

   errcode = psoaHashMapFirst( objHandle, &keyBuffer, &keyLength, 
                               &buffer, &bufferLength );
   assert_true( errcode == PSO_OK );

   errcode = psoaHashMapNext( objHandle, &keyBuffer, &keyLength, 
                              &buffer, &bufferLength );
   assert_true( errcode == PSO_OK );

//   if ( memcmp( buffer, data, 7 ) != 0 )
//      ERROR_EXIT( expectedToPass, NULL, ; );

   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_no_first,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_data,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_datalength, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_handle,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_key,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_keylength,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_handle,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,            setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

