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

#include "hashMapTest.h"

psonHashMap * pHashMap;
psonSessionContext context;
psonTxStatus status;
char * key  = "my key";
char * data1 = "my data1";
char * data2 = "my data2";
psonHashTxItem * pItem;
psonTreeNode mapNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };
   psoKeyDefinition keyDef = { PSO_DEF_USER_DEFINED, 0, '\0' };

   pHashMap = initHashMapTest( &context );

   psonTxStatusInit( &status, SET_OFFSET(context.pBaseAddress, context.pTransaction), &context );
   psonTreeNodeInit( &mapNode, SET_OFFSET(context.pBaseAddress, pHashMap ), PSO_HASH_MAP,
                     SET_OFFSET(context.pBaseAddress, &status), PSON_NULL_OFFSET, &context );
   
   ok = psonHashMapInit( pHashMap, 0, 1, 0, &mapNode,
                         &def, &keyDef,
                         &context );
   assert( ok );
   
   ok = psonHashMapInsert( pHashMap,
                           (const void *) key,
                           6,
                           (const void *) data1,
                           strlen(data1),
                           &context );
   assert( ok );
   
   /*
    * We use get to get to the hash item in order to commit it 
    * (we need to commit the insertion before replacing it)
    */
   ok = psonHashMapGet( pHashMap,
                        (const void *) key,
                        6,
                        &pItem,
                        20,
                        &context );
   assert( ok );
   
   psonHashMapCommitAdd( pHashMap, SET_OFFSET(context.pBaseAddress, pItem), &context );

   ok = psonHashMapRelease( pHashMap,
                            pItem,
                            &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( context.pBaseAddress );
   context.pBaseAddress = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapReplace( pHashMap,
                                              (const void *) key,
                                              6,
                                              (const void *) data2,
                                              strlen(data2),
                                              NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_data( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapReplace( pHashMap,
                                              (const void *) key,
                                              6,
                                              NULL,
                                              strlen(data2),
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapReplace( NULL,
                                              (const void *) key,
                                              6,
                                              (const void *) data2,
                                              strlen(data2),
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_key( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapReplace( pHashMap,
                                              NULL,
                                              6,
                                              (const void *) data2,
                                              strlen(data2),
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length_data( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapReplace( pHashMap,
                                              (const void *) key,
                                              6,
                                              (const void *) data2,
                                              0,
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length_key( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapReplace( pHashMap,
                                              (const void *) key,
                                              0,
                                              (const void *) data2,
                                              strlen(data2),
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   char * ptr;

   ok = psonHashMapReplace( pHashMap,
                            (const void *) key,
                            6,
                            (const void *) data2,
                            strlen(data2),
                            &context );
   assert_true( ok );
   
   ok = psonHashMapGet( pHashMap,
                        (const void *) key,
                        6,
                        &pItem,
                        20,
                        &context );
   assert_true( ok );
   GET_PTR(context.pBaseAddress, ptr, pItem->dataOffset, char );
   assert_true( memcmp( data2, ptr, strlen(data2)) == 0 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_data,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_hash,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_key,         setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length_data, setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length_key,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,             setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

