/*
 * Copyright (C) 2006-2010 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "ListTestCommon.h"
#include "Nucleus/Tests/EngineTestCommon.h"

psonLinkedList list;
psonLinkNode node;
psonSessionContext context;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   initTest( &context );
   InitMem( &context );
   
   psonLinkNodeInit( &node, &context );
   psonLinkedListInit( &list, &context );
   
   psonLinkedListPutLast( &list, &node, &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psonLinkedListFini( &list, &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_absent( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonLinkNode node, *pNode;

   psonLinkedListGetFirst( &list, &pNode, &context );

   expect_assert_failure( psonLinkedListRemoveItem( &list, &node, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int save = list.initialized;

   list.initialized = 0;
   expect_assert_failure( psonLinkedListRemoveItem( &list, &node, &context ) );
   list.initialized = save;
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListRemoveItem( &list, &node, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListRemoveItem( &list, NULL, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_list( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListRemoveItem( NULL, &node, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_next( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   node.nextOffset = PSON_NULL_OFFSET;
   expect_assert_failure( psonLinkedListRemoveItem( &list, &node, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_previous( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   node.previousOffset = PSON_NULL_OFFSET;
   expect_assert_failure( psonLinkedListRemoveItem( &list, &node, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonLinkedList list1;
   psonLinkedList list2;
   psonLinkNode node1, node2;
   psonSessionContext context;
   
   initTest( &context );
   InitMem( &context );
   
   psonLinkNodeInit( &node1, &context );
   psonLinkNodeInit( &node2, &context );
   psonLinkedListInit( &list1, &context );
   psonLinkedListInit( &list2, &context );
   
   psonLinkedListPutLast( &list1, &node1, &context );
   psonLinkedListPutLast( &list2, &node2, &context );

   /* Remove it from the wrong list - should work but... */
   psonLinkedListRemoveItem( &list2, &node1, &context );

   /* Should crash since the number of items in list2 is zero */
   expect_assert_failure( psonLinkedListRemoveItem( &list2, &node2, &context ) );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   
   psonLinkedListRemoveItem( &list, &node, &context );

   assert_true( list.currentSize == 0 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_absent,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_invalid_sig,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_item,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_list,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_next,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_previous, setup_test, teardown_test ),
      unit_test( test_wrong_item ),
      unit_test_setup_teardown( test_pass,          setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

