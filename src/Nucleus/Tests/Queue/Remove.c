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
psonQueueItem * pQueueItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_QUEUE, 0, 0, 0 };
   psonDataDefinition fields;
   char * data = "My Data";

   pQueue = initQueueTest( &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );

   ok = psonQueueInit( pQueue, 
                       0, 1, &status,
                       SET_OFFSET(pQueue), 
                       &def, &fields, &context );
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
   
   ok = psonQueueGetFirst( pQueue, &pQueueItem, 100, &context );
   assert( ok );

   /* Must commit the insert before we attempt to remove */
   psonQueueCommitAdd( pQueue, SET_OFFSET(pQueueItem) );
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
   expect_assert_failure( psonQueueRemove( pQueue,
                                           &pQueueItem,
                                           PSON_QUEUE_FIRST,
                                           8,
                                           NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueRemove( pQueue,
                                           NULL,
                                           PSON_QUEUE_FIRST,
                                           8,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_queue( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueRemove( NULL,
                                           &pQueueItem,
                                           PSON_QUEUE_FIRST,
                                           8,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_flag( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueRemove( pQueue,
                                           &pQueueItem,
                                           PSON_QUEUE_FIRST+123,
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

   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
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
   
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         8,
                         &context );
   assert_true( ok );
   assert_true( pQueue->nodeObject.txCounter == 2 );
   
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
      unit_test_setup_teardown( test_null_item,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_queue,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_flag,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_length, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

