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
   const char * data = "My Data";
   uint32_t length, keyLength;
   char buffer[20], keyBuff[20];
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

   errcode = psoInit( "10701", "Delete" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_delete_pass",
                              strlen("/api_hashmap_delete_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_DeletePass",
                              strlen("API_Hashmap_DeletePass"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_DeletePass",
                               strlen("API_Hashmap_DeletePass"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_delete_pass/test",
                           strlen("/api_hashmap_delete_pass/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapOpen( sessionHandle,
                             "/api_hashmap_delete_pass/test",
                             strlen("/api_hashmap_delete_pass/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoHashMapOpen( sessionHandle2,
                             "/api_hashmap_delete_pass/test",
                             strlen("/api_hashmap_delete_pass/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );
   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );
   
   /* Invalid arguments to tested function. */

   errcode = psoHashMapDelete( NULL,
                               key,
                               6 );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoHashMapDelete( sessionHandle,
                               key,
                               6 );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoHashMapDelete( objHandle,
                               NULL,
                               6 );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoHashMapDelete( objHandle,
                               key,
                               0 );
   assert_true( errcode == PSO_INVALID_LENGTH );

   /* End of invalid args. This call should succeed. */
   errcode = psoHashMapDelete( objHandle,
                               key,
                               6 );
   assert_true( errcode == PSO_OK );

   /*
    * Additional stuff to check while the Delete() is uncommitted:
    *  - cannot get access to the item from first session.
    *  - can get access to the item from second session.
    *  - cannot modify it from second session.
    */
   errcode = psoHashMapGet( objHandle, 
                            key, 
                            6,
                            buffer, 20, &length );
   assert_true( errcode == PSO_ITEM_IS_DELETED );
   errcode = psoHashMapGet( objHandle2, 
                            key, 
                            6,
                            buffer, 20, &length );
   assert_true( errcode == PSO_OK );
   errcode = psoHashMapDelete( objHandle2, key, 6 );
   assert_true( errcode == PSO_ITEM_IS_IN_USE );
   errcode = psoHashMapGetFirst( objHandle2, keyBuff, 20, buffer, 20, 
                                 &keyLength, &length );
   assert_true( errcode == PSO_OK );
   errcode = psoHashMapGetFirst( objHandle, keyBuff, 20, buffer, 20, 
                                 &keyLength, &length );
   assert_true( errcode == PSO_IS_EMPTY );

   /*
    * Additional stuff to check after the commit:
    *  - cannot get access to the item from first session.( no such item)
    *  - cannot get access to the item from second session.
    *  - can insert new item with same key.
    *
    * Note to make sure that the deleted item is still in shared memory,
    * we will act on the first session (the second session holds
    * an internal pointer to the data record).
    */
   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoHashMapGet( objHandle, 
                            key, 
                            6,
                            buffer, 20, &length );
   assert_true( errcode == PSO_NO_SUCH_ITEM );
   errcode = psoHashMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );
   
   /* rollback to be able to test the Get() with session 2 */
   errcode = psoRollback( sessionHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoHashMapGet( objHandle2, 
                            key, 
                            6,
                            buffer, 20, &length );
   assert_true( errcode == PSO_NO_SUCH_ITEM );
   
   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoHashMapDelete( objHandle,
                               key,
                               6 );
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

