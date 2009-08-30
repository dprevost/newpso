/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "API/Lifo.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   psoObjectDefinition defLifo = { PSO_LIFO, 0, 0, 0 };
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
                              "/api_lifo_close",
                              strlen("/api_lifo_close") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_lifo_close",
                               strlen("api_lifo_close"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_lifo_close/test",
                             strlen("/api_lifo_close/test"),
                             &defLifo,
                             dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoOpen( sessionHandle,
                          "/api_lifo_close/test",
                          strlen("/api_lifo_close/test"),
                          &objHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoLifoClose( NULL );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoLifoClose( sessionHandle );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   /* End of invalid args. This call should succeed. */
   errcode = psoLifoClose( objHandle );
   assert_true( errcode == PSO_OK );

   /* Close the session and try to act on the object */

   errcode = psoLifoOpen( sessionHandle,
                          "/api_lifo_close/test",
                          strlen("/api_lifo_close/test"),
                          &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoClose( objHandle );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   psoExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

