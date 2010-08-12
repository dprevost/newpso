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
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE sessionHandle, objHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   PSO_HANDLE shmemHandle;
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
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( shmemHandle, &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_push_pass",
                              strlen("/api_queue_push_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_push_pass/test",
                             strlen("/api_queue_push_pass/test"),
                             &defQueue );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_push_pass/test",
                           strlen("/api_queue_push_pass/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoQueueOpen( sessionHandle2,
                           "/api_queue_push_pass/test",
                           strlen("/api_queue_push_pass/test"),
                           &objHandle2 );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoQueuePush( NULL, data1, strlen(data1) );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoQueuePush( sessionHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoQueuePush( objHandle, NULL, strlen(data1) );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoQueuePush( objHandle, data1, 0 );
   assert_true( errcode == PSO_INVALID_LENGTH );

   /* End of invalid args. This call should succeed. */
   errcode = psoQueuePush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_OK );

   /*
    * Additional stuff to check:
    *  - cannot get access to the item from second session.
    *  - can get access to the item from first session.
    *  - cannot modify it from first session.
    */
   errcode = psoQueueGetFirst( objHandle2,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_ITEM_IS_IN_USE );
   errcode = psoQueueGetFirst( objHandle,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_OK );
   errcode = psoQueuePop( objHandle,
                          buffer,
                          200,
                          &length );
   assert_true( errcode == PSO_ITEM_IS_IN_USE );
   
   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   psoExit();
#endif
   return;
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

