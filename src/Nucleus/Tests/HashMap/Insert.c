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

#include "hashMapTest.h"

psonHashMap * pHashMap;
psonSessionContext context;
psonTxStatus status;
psonTreeNode mapNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0 };
   psonKeyDefinition keyDef;
   psonDataDefinition fields;

   pHashMap = initHashMapTest( &context );

   psonTxStatusInit( &status, SET_OFFSET(context.pTransaction), &context );
   psonTreeNodeInit( &mapNode, SET_OFFSET( pHashMap ), PSO_HASH_MAP,
                     SET_OFFSET(&status), PSON_NULL_OFFSET, &context );
   
   ok = psonHashMapInit( pHashMap, 0, 1, 0, &mapNode,
                         &def, &keyDef,
                         &fields, &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapInsert( pHashMap,
                                             "my key 1",
                                             strlen("my key 1"),
                                             "my data 1",
                                             strlen("my data 1"),
                                             NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_data( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapInsert( pHashMap,
                                             "my key 1",
                                             strlen("my key 1"),
                                             NULL,
                                             strlen("my data 1"),
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapInsert( NULL,
                                             "my key 1",
                                             strlen("my key 1"),
                                             "my data 1",
                                             strlen("my data 1"),
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_key( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapInsert( pHashMap,
                                             NULL,
                                             strlen("my key 1"),
                                             "my data 1",
                                             strlen("my data 1"),
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length_data( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapInsert( pHashMap,
                                             "my key 1",
                                             strlen("my key 1"),
                                             "my data 1",
                                             0,
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length_key( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapInsert( pHashMap,
                                             "my key 1",
                                             0,
                                             "my data 1",
                                             strlen("my data 1"),
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonHashMapInsert( pHashMap,
                           "my key 1",
                           strlen("my key 1"),
                           "my data 1",
                           strlen("my data 1"),
                           &context );
   assert_true( ok );
   assert_true( mapNode.txCounter == 1 );
   
   ok = psonHashMapInsert( pHashMap,
                           "my key 2",
                           strlen("my key 2"),
                           "my data 2",
                           strlen("my data 2"),
                           &context );
   assert_true( ok );
   assert_true( mapNode.txCounter == 2 );
   assert_true( pHashMap->hashObj.numberOfItems == 2 );
   
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

