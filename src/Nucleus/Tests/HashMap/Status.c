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
psonTxStatus txStatus;
psoObjStatus status;
psonTreeNode mapNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * key3  = "my key3";
   char * data  = "my data";
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };
   psoKeyDefinition keyDef = { PSO_DEF_USER_DEFINED, 0, '\0' };

   pHashMap = initHashMapTest( &context );

   psonTxStatusInit( &txStatus, SET_OFFSET(g_pBaseAddr,  context.pTransaction ), &context );
   psonTreeNodeInit( &mapNode, SET_OFFSET(g_pBaseAddr,  pHashMap ), PSO_HASH_MAP,
                     SET_OFFSET(g_pBaseAddr,  &txStatus ), PSON_NULL_OFFSET, &context );
   
   ok = psonHashMapInit( pHashMap, 0, 1, 0, &mapNode,
                         &def, &keyDef,
                         &context );
   assert( ok );
   
   ok = psonHashMapInsert( pHashMap,
                           (const void *) key1,
                           7,
                           (const void *) data,
                           7,
                           &context );
   assert( ok );
   
   ok = psonHashMapInsert( pHashMap,
                           (const void *) key2,
                           7,
                           (const void *) data,
                           7,
                           &context );
   assert( ok );
   
   ok = psonHashMapInsert( pHashMap,
                           (const void *) key3,
                           7,
                           (const void *) data,
                           7,
                           &context );
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
   expect_assert_failure( psonHashMapStatus( pHashMap, &status, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapStatus( NULL, &status, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_status( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapStatus( pHashMap, NULL, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   
   psonHashMapStatus( pHashMap, &status, &context );

   assert_true( status.numDataItem == 3 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_hash,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_status,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


