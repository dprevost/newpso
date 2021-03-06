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
psonQueueItem * pItem = NULL;
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
   expect_assert_failure( psonQueueGetFirst( pQueue,
                                             &pItem,
                                             8,
                                             NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_iterator( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueGetFirst( pQueue,
                                             NULL,
                                             8,
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_queue( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueGetFirst( NULL,
                                             &pItem,
                                             8,
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   int errcode;
   
   ok = psonQueueGetFirst( pQueue,
                           &pItem,
                           7,
                           &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_INVALID_LENGTH );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   psonTxStatus * txItemStatus;
   
   ok = psonQueueGetFirst( pQueue,
                           &pItem,
                           8,
                           &context );
   assert_true( ok );
   assert_true( pItem->dataLength == 8 );
   txItemStatus = &pItem->txStatus;
   assert_true( txItemStatus->usageCounter == 1 );
   assert_true( status.usageCounter == 1 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_iterator, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_queue,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,          setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

