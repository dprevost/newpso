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
#include "Tests/PrintError.h"
#include "API/HashMap.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE objHandle,  sessionHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   char buffer[20], keyBuff[20];
   uint32_t length, keyLength;
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   
   if ( argc > 1 ) {
      errcode = psoInit( argv[1], argv[0] );
   }
   else {
      errcode = psoInit( "10701", argv[0] );
   }
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_insert_pass",
                              strlen("/api_hashmap_insert_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_InsertPass",
                              strlen("API_Hashmap_InsertPass"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_InsertPass",
                               strlen("API_Hashmap_InsertPass"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_insert_pass/test",
                           strlen("/api_hashmap_insert_pass/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_insert_pass/test",
                             strlen("/api_hashmap_insert_pass/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoHashMapOpen( sessionHandle2,
                             "/api_hashmap_insert_pass/test",
                             strlen("/api_hashmap_insert_pass/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoHashMapInsert( NULL,
                               key,
                               6,
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoHashMapInsert( sessionHandle,
                               key,
                               6,
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoHashMapInsert( objHandle,
                               NULL,
                               6,
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               0,
                               data,
                               7,
                               NULL );
   assert_true( errcode == INVALID_LENGTH );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               NULL,
                               7,
                               NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               0,
                               NULL );
   assert_true( errcode == INVALID_LENGTH );

   /* End of invalid args. This call should succeed. */
   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_OK );

   /*
    * Additional stuff to check:
    *  - cannot get access to the item from second session.
    *  - can get access to the item from first session.
    *  - cannot modify it from first session.
    */
   errcode = psoHashMapGet( objHandle2, 
                            key, 
                            6,
                            buffer, 20, &length );
   if ( errcode != PSO_ITEM_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoHashMapGet( objHandle, 
                            key, 
                            6,
                            buffer, 20, &length );
   assert_true( errcode == PSO_OK );
   errcode = psoHashMapDelete( objHandle, key, 6 );
   if ( errcode != PSO_ITEM_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoHashMapGetFirst( objHandle, keyBuff, 20, buffer, 20, 
                                 &keyLength, &length );
   assert_true( errcode == PSO_OK );
   errcode = psoHashMapGetFirst( objHandle2, keyBuff, 20, buffer, 20, 
                                 &keyLength, &length );
   assert_true( errcode == PSO_IS_EMPTY );

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

