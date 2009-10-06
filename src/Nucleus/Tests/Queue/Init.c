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

#include "queueTest.h"

psonQueue * pQueue;
psonSessionContext context;
psonTxStatus status;
psoObjectDefinition def = { PSO_QUEUE, 0, 0 };
psonDataDefinition fields;
psonTreeNode queueNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   pQueue = initQueueTest( &context );

   psonTxStatusInit( &status, SET_OFFSET(context.pTransaction), &context );
   psonTreeNodeInit( &queueNode, SET_OFFSET( pQueue ), PSO_QUEUE,
                     SET_OFFSET(&status), PSON_NULL_OFFSET, &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (g_pBaseAddr) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueInit( pQueue, 
                                         0,
                                         1,
                                         &queueNode,
                                         &def,
                                         NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_definition( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueInit( pQueue, 
                                         0,
                                         1,
                                         &queueNode,
                                         NULL,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_node( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueInit( pQueue, 
                                         0,
                                         1,
                                         NULL,
                                         &def,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_parent( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueInit( pQueue, 
                                         PSON_NULL_OFFSET,
                                         1,
                                         &queueNode,
                                         &def,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_queue( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueInit( NULL, 
                                         0,
                                         1,
                                         &queueNode,
                                         &def,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_blocks( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueInit( pQueue, 
                                         0,
                                         0,
                                         &queueNode,
                                         &def,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonQueueInit( pQueue, 
                       0,
                       1,
                       &queueNode,
                       &def,
                       &context );
   assert_true( ok );
   
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
      unit_test_setup_teardown( test_null_definition, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_node,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_parent,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_queue,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_blocks,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,            setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

