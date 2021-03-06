/*
 * Copyright (C) 2008-2010 Daniel Prevost <dprevost@photonsoftware.org>
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
psonFastMap * pOldMap, * pNewMap;
psonSessionContext context;
psonHashTxItem   hashItem;
char * key1  = "my key1";
char * key2  = "my key2";
char * data1 = "my data1";
char * data2 = "my data2";
psonTreeNode mapNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_FAST_MAP, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };
   psoKeyDefinition keyDef = { PSO_DEF_USER_DEFINED, 0, '\0' };
   
   initHashMapCopyTest( &pOldMap, &pNewMap, &context );

   psonTxStatusInit( &hashItem.txStatus, 
                     SET_OFFSET(context.pBaseAddress, context.pTransaction ), &context );
   psonTreeNodeInit( &mapNode, SET_OFFSET(context.pBaseAddress, pOldMap ), PSO_FAST_MAP,
                     SET_OFFSET(context.pBaseAddress, &hashItem.txStatus ),
                     PSON_NULL_OFFSET, &context );
   hashItem.dataOffset = SET_OFFSET(context.pBaseAddress, &mapNode );
   
   ok = psonFastMapInit( pOldMap, 0, 1, 0, &mapNode,
                         SET_OFFSET(context.pBaseAddress, pOldMap), &def, &keyDef, 
                         &context );
   assert( ok );
   
   ok = psonFastMapInsert( pOldMap,
                           (const void *) key1,
                           7,
                           (const void *) data1,
                           8,
                           &context );
   assert( ok );
   ok = psonFastMapInsert( pOldMap,
                           (const void *) key2,
                           7,
                           (const void *) data2,
                           8,
                           &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( context.pBaseAddress );
   context.pBaseAddress = NULL;
   pOldMap = pNewMap = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapCopy( pOldMap, 
                                           pNewMap,
                                           &hashItem,
                                           NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapCopy( pOldMap, 
                                           pNewMap,
                                           NULL,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_new_map( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapCopy( pOldMap, 
                                           NULL,
                                           &hashItem,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_old_map( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapCopy( NULL, 
                                           pNewMap,
                                           &hashItem,
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
   psonHashItem * pItem;
   
   ok = psonFastMapCopy( pOldMap, 
                         pNewMap,
                         &hashItem,
                         &context );
   
   ok = psonFastMapGet( pNewMap,
                        (const void *) key1,
                        7,
                        &pItem,
                        20,
                        &context );
   assert_true( ok );
   GET_PTR(context.pBaseAddress, ptr, pItem->dataOffset, char );
   assert_memory_equal( data1, ptr, 8 );

   ok = psonFastMapGet( pNewMap,
                        (const void *) key2,
                        7,
                        &pItem,
                        20,
                        &context );
   assert_true( ok );
   GET_PTR(context.pBaseAddress, ptr, pItem->dataOffset, char );
   assert_memory_equal( data2, ptr, 8 );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_hash_item, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_new_map,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_old_map,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,            setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

