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
#include "API/CommonObject.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   psoObjStatus status;

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
                              "/api_session_status_pass",
                              strlen("/api_session_status_pass") );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoGetStatus( NULL,
                           "/api_session_status_pass",
                           strlen("/api_session_status_pass"),
                           &status );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoGetStatus( sessionHandle,
                           NULL,
                           strlen("/api_session_status_pass"),
                           &status );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoGetStatus( sessionHandle,
                           "/api_session_status_pass",
                           0,
                           &status );
   assert_true( errcode == INVALID_LENGTH );

   errcode = psoGetStatus( sessionHandle,
                           "/api_session_status_pass",
                           strlen("/api_session_status_pass"),
                           NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoGetStatus( sessionHandle,
                           "/api_session_status_pass",
                           strlen("/api_session_status_pass"),
                           &status );
   assert_true( errcode == PSO_OK );

   /* Close the process and try to act on the session */

   psoExit();
   
   errcode = psoGetStatus( sessionHandle,
                           "/api_session_status_pass",
                           strlen("/api_session_status_pass"),
                           &status );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

