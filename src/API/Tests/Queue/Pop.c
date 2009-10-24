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

#include "Common/Common.h"
#include <photon/photon.h>
#include "API/Queue.h"
#include "API/Tests/quasar-run.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   assert( startQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   assert( stopQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
   PSO_HANDLE sessionHandle, objHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * data1 = "My Data1";
   char buffer[200];
   uint32_t length;
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_pop",
                              strlen("/api_queue_pop") );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_pop/test",
                             strlen("/api_queue_pop/test"),
                             &defQueue );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_pop/test",
                           strlen("/api_queue_pop/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoQueueOpen( sessionHandle2,
                           "/api_queue_pop/test",
                           strlen("/api_queue_pop/test"),
                           &objHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );
   
   /* Invalid arguments to tested function. */

   errcode = psoQueuePop( NULL,
                          buffer,
                          200,
                          &length );
   assert_true( errcode == PSO_NULL_HANDLE );
   
   errcode = psoQueuePop( objHandle,
                          NULL,
                          200,
                          &length );
   assert_true( errcode == PSO_NULL_POINTER );
   
   errcode = psoQueuePop( objHandle,
                          buffer,
                          2,
                          &length );
   assert_true( errcode == PSO_INVALID_LENGTH );
   
   errcode = psoQueuePop( objHandle,
                          buffer,
                          200,
                          NULL );
   assert_true( errcode == PSO_NULL_POINTER );
   
   /* End of invalid args. This call should succeed. */
   errcode = psoQueuePop( objHandle,
                          buffer,
                          200,
                          &length );
   assert_true( errcode == PSO_OK );
   assert_true( memcmp( buffer, data1, strlen(data1) ) == 0 );
   
   /*
    * Additional stuff to check while the Pop is uncommitted:
    *  - cannot get access to the item from first session.
    *  - can get access to the item from second session.
    *  - cannot modify it from second session.
    */
   errcode = psoQueueGetFirst( objHandle,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_ITEM_IS_IN_USE );
   errcode = psoQueueGetFirst( objHandle2,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_OK );
   errcode = psoQueuePop( objHandle2,
                          buffer,
                          200,
                          &length );
   assert_true( errcode == PSO_ITEM_IS_IN_USE );
   
   /*
    * Additional stuff to check after the commit:
    *  - cannot get access to the item from first session.
    *  - cannot get access to the item from second session.
    *  And that the error is PSO_EMPTY.
    *
    * Note to make sure that the deleted item is still in shared memory,
    * we first call GetFirst to get a pointer to the item from
    * session 2 (the failed call to Pop just above release that
    * internal pointer).
    */
   errcode = psoQueueGetFirst( objHandle2,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_OK );
   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueGetFirst( objHandle,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_IS_EMPTY );
   
   errcode = psoQueueGetFirst( objHandle2,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_IS_EMPTY );

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePop( objHandle,
                          buffer,
                          200,
                          &length );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_pass, setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

