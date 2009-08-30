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
#include "Tests/PrintError.h"
#include "API/Queue.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle, objHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * data1 = "My Data1";
   char buffer[200];
   uint32_t length;
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;
   
   if ( argc > 1 ) {
      errcode = psoInit( argv[1], argv[0] );
   }
   else {
      errcode = psoInit( "10701", argv[0] );
   }
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_push_pass",
                              strlen("/api_queue_push_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_queue_push_pass",
                               strlen("api_queue_push_pass"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_push_pass/test",
                             strlen("/api_queue_push_pass/test"),
                             &defQueue,
                             dataDefHandle );
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

   errcode = psoQueuePush( NULL, data1, strlen(data1), NULL );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoQueuePush( sessionHandle, data1, strlen(data1), NULL );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoQueuePush( objHandle, NULL, strlen(data1), NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoQueuePush( objHandle, data1, 0, NULL );
   assert_true( errcode == INVALID_LENGTH );

   /* End of invalid args. This call should succeed. */
   errcode = psoQueuePush( objHandle, data1, strlen(data1), NULL );
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
   if ( errcode != PSO_ITEM_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoQueueGetFirst( objHandle,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_OK );
   errcode = psoQueuePop( objHandle,
                          buffer,
                          200,
                          &length );
   if ( errcode != PSO_ITEM_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data1, strlen(data1), NULL );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   psoExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

