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

#include "queueTest.h"

psonQueue * pQueue;
psonSessionContext context;
psonTxStatus status;
psonQueueItem * pQueueItem;
psonTreeNode queueNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_QUEUE, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };
   char * data = "My Data";
   
   pQueue = initQueueTest( &context );

   psonTxStatusInit( &status, SET_OFFSET(context.pBaseAddress, context.pTransaction), &context );
   psonTreeNodeInit( &queueNode, SET_OFFSET(context.pBaseAddress, pQueue ), PSO_QUEUE,
                     SET_OFFSET(context.pBaseAddress, &status), PSON_NULL_OFFSET, &context );

   ok = psonQueueInit( pQueue, 
                       0, 1, &queueNode,
                       &def, &context );
   assert( ok );

   ok = psonQueueInsert( pQueue,
                         data,
                         8,
                         PSON_QUEUE_FIRST,
                         &context );
   assert( ok );
   
   ok = psonQueueInsert( pQueue,
                         data,
                         6,
                         PSON_QUEUE_LAST,
                         &context );
   assert( ok );
   
   /* Must commit the insert before we attempt to remove */
   ok = psonQueueGetFirst( pQueue, &pQueueItem, 100, &context );
   assert( ok );
   psonQueueCommitAdd( pQueue, SET_OFFSET(context.pBaseAddress, pQueueItem), &context );

   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         20,
                         &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (context.pBaseAddress) free(context.pBaseAddress);
   context.pBaseAddress = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueRollbackRemove( pQueue, 
                                                   SET_OFFSET(context.pBaseAddress, pQueueItem ),
                                                   NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueRollbackRemove( pQueue, 
                                                   PSON_NULL_OFFSET,
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_queue( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueRollbackRemove( NULL, 
                                                   SET_OFFSET(context.pBaseAddress, pQueueItem ),
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonQueueRollbackRemove( pQueue, SET_OFFSET(context.pBaseAddress, pQueueItem ), &context );
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
      unit_test_setup_teardown( test_null_offset,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_queue,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

