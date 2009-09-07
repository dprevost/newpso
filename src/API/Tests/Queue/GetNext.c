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
   int errcode;
   const char * data1 = "My Data1";
   const char * data2 = "My Data2";
   char buffer[200];
   uint32_t length;
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0 };
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

   errcode = psoCreateFolder( sessionHandle,
                              "/api_queue_get_next",
                              strlen("/api_queue_get_next") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_queue_get_next",
                               strlen("api_queue_get_next"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_queue_get_next/test",
                             strlen("/api_queue_get_next/test"),
                             &defQueue,
                             dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueOpen( sessionHandle,
                           "/api_queue_get_next/test",
                           strlen("/api_queue_get_next/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data1, strlen(data1), NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoQueuePush( objHandle, data2, strlen(data2), NULL );
   assert_true( errcode == PSO_OK );

   /* No GetFirst */
   errcode = psoQueueGetNext( objHandle,
                              buffer,
                              200,
                              &length );
   assert_true( errcode == PSO_INVALID_ITERATOR );

   errcode = psoQueueGetFirst( objHandle,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoQueueGetNext( NULL,
                              buffer,
                              200,
                              &length );
   assert_true( errcode == PSO_NULL_HANDLE );
   
   errcode = psoQueueGetNext( objHandle,
                              NULL,
                              200,
                              &length );
   assert_true( errcode == PSO_NULL_POINTER );
   
   errcode = psoQueueGetNext( objHandle,
                              buffer,
                              2,
                              &length );
   assert_true( errcode == INVALID_LENGTH );
   
   errcode = psoQueueGetNext( objHandle,
                              buffer,
                              200,
                              NULL );
   assert_true( errcode == PSO_NULL_POINTER );
   
   /* End of invalid args. This call should succeed. */
   errcode = psoQueueGetNext( objHandle,
                              buffer,
                              200,
                              &length );
   assert_true( errcode == PSO_OK );
   if ( memcmp( buffer, data2, strlen(data2) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Close the session and try to act on the object */

   /* Reset the iterator */
   errcode = psoQueueGetFirst( objHandle,
                               buffer,
                               200,
                               &length );
   assert_true( errcode == PSO_OK );

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoQueueGetNext( objHandle,
                              buffer,
                              200,
                              &length );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   psoExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

