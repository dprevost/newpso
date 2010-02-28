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
   PSO_HANDLE objHandle,  sessionHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   PSO_HANDLE shmemHandle;
   int errcode;
   char junk[12];
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;
   const char * data1 = "My Data1";

   memset( junk, 0, 12 );

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( shmemHandle, &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_open_pass",
                              strlen("/api_queue_open_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_open_pass/test",
                             strlen("/api_queue_open_pass/test"),
                             &defQueue );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoQueueOpen( NULL,
                           "/api_queue_open_pass/test",
                           strlen("/api_queue_open_pass/test"),
                           &objHandle );
   assert_true( errcode == PSO_NULL_HANDLE );

   objHandle = (PSO_HANDLE) junk;
   errcode = psoQueueOpen( objHandle,
                           "/api_queue_open_pass/test",
                           strlen("/api_queue_open_pass/test"),
                           &objHandle );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoQueueOpen( sessionHandle,
                           NULL,
                           strlen("/api_queue_open_pass/test"),
                           &objHandle );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_open_pass/test",
                           0,
                           &objHandle );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoQueueOpen( sessionHandle,
                             "/api_queue_open_pass/test",
                             strlen("/api_queue_open_pass/test"),
                             NULL );
   assert_true( errcode == PSO_NULL_HANDLE );

   /* End of invalid args. This call should succeed. */
   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_open_pass/test",
                           strlen("/api_queue_open_pass/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueOpen( sessionHandle2,
                           "/api_queue_open_pass/test",
                           strlen("/api_queue_open_pass/test"),
                           &objHandle2 );
   assert_true( errcode == PSO_OBJECT_IS_IN_USE );

   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_open_pass/test",
                           strlen("/api_queue_open_pass/test"),
                           &objHandle2 );
   assert_true( errcode == PSO_OK );

   psoQueueClose( objHandle );
   psoRollback( sessionHandle );

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

