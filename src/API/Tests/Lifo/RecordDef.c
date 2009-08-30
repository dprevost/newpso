/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   psoObjectDefinition lifoDef = { PSO_LIFO, 0, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle, returnedDef;
   
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
                              "/api_lifo_record_def",
                              strlen("/api_lifo_record_def") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_lifo_record_def",
                               strlen("api_lifo_record_def"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_lifo_record_def/test",
                             strlen("/api_lifo_record_def/test"),
                             &lifoDef,
                             dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoOpen( sessionHandle,
                          "/api_lifo_record_def/test",
                          strlen("/api_lifo_record_def/test"),
                          &objHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoLifoRecordDefinition( NULL, &returnedDef );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoLifoRecordDefinition( sessionHandle, &returnedDef );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoLifoRecordDefinition( objHandle, NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoLifoRecordDefinition( objHandle, &returnedDef );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoClose( objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefClose( returnedDef );
   assert_true( errcode == PSO_OK );
   
   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

