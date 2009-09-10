/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
psonLinkNode oldNode, newNode;
psonSessionContext context;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   initTest( &context );
   InitMem();
   
   psonLinkNodeInit( &oldNode, &context );
   psonLinkNodeInit( &newNode, &context );

   psonLinkedListInit( &list, &context );
   psonLinkedListPutFirst( &list, &oldNode, &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psonLinkedListFini( &list, &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int save = list.initialized;

   list.initialized = 0;
   expect_assert_failure( psonLinkedListReplaceItem( &list, 
                                                     &oldNode,
                                                     &newNode,
                                                     &context ) );
   list.initialized = save;
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListReplaceItem( &list,
                                                     &oldNode,
                                                     &newNode,
                                                     NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_list( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListReplaceItem( NULL,
                                                     &oldNode,
                                                     &newNode,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_new( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListReplaceItem( &list,
                                                     &oldNode,
                                                     NULL,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_next( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   newNode.nextOffset = 0x12345;
   expect_assert_failure( psonLinkedListReplaceItem( &list,
                                                     &oldNode,
                                                     &newNode,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_old( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListReplaceItem( &list,
                                                     NULL,
                                                     &newNode,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_previous( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   newNode.nextOffset = 0x12345;
   expect_assert_failure( psonLinkedListReplaceItem( &list,
                                                     &oldNode,
                                                     &newNode,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonLinkedList list;
   psonLinkNode oldNode, newNode, * pDummy = NULL;
   psonLinkNode node1, node2;
   psonSessionContext context;
   
   initTest( &context );
   InitMem();
   
   psonLinkNodeInit( &oldNode, &context );
   psonLinkNodeInit( &newNode, &context );
   psonLinkNodeInit( &node1, &context );
   psonLinkNodeInit( &node2, &context );

   psonLinkedListInit( &list, &context );

   /* Test 1 - replace alone */
   psonLinkedListPutFirst( &list, &oldNode, &context );

   psonLinkedListReplaceItem( &list, &oldNode, &newNode, &context );

   assert_true( list.currentSize == 1 );
   
   psonLinkedListPeakFirst( &list, &pDummy, &context );
   assert_true( pDummy == &newNode );
   
   assert_true( TestList( &list ) == 0 );
   
   /* Test 2 - replace at tail */
   psonLinkedListPutFirst( &list, &node1, &context );
   psonLinkNodeInit( &oldNode, &context );
   psonLinkedListReplaceItem( &list, &newNode, &oldNode, &context );

   assert_true( list.currentSize == 2 );
   
   psonLinkedListPeakLast( &list, &pDummy, &context );
   assert_true( pDummy == &oldNode );
   assert_true( TestList( &list ) == 0 );
   
   /* Test 3 - replace in the middle */
   psonLinkedListPutLast( &list, &node2, &context );
   psonLinkNodeInit( &newNode, &context );
   psonLinkedListReplaceItem( &list, &oldNode, &newNode, &context );

   assert_true( list.currentSize == 3 );
   
   psonLinkedListPeakNext( &list, &node1, &pDummy, &context );
   assert_true( pDummy == &newNode );
   assert_true( TestList( &list ) == 0 );
   
   /* Test 4 - replace at head */
   psonLinkNodeInit( &oldNode, &context );
   psonLinkedListReplaceItem( &list, &node1, &oldNode, &context );

   assert_true( list.currentSize == 3 );
   
   psonLinkedListPeakFirst( &list, &pDummy, &context );
   assert_true( pDummy == &oldNode );
   assert_true( TestList( &list ) == 0 );
   
   psonLinkedListFini( &list, &context );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_sig,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_list,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_new,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_next,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_old,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_previous, setup_test, teardown_test ),
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

