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
#include "API/FastMap.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
#if defined(USE_DBC)
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   unsigned char * buffer;

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
                              "/api_fast_map_retrieve_null_length",
                              strlen("/api_fast_map_retrieve_null_length") );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_fast_map_retrieve_null_length/test",
                           strlen("/api_fast_map_retrieve_null_length/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_fast_map_retrieve_null_length/test",
                             strlen("/api_fast_map_retrieve_null_length/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoaFastMapRetrieve( objHandle,
                                  key,
                                  6,
                                  &buffer,
                                  NULL );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
#  if defined(WIN32)
   exit(3);
#  else
   abort();
#  endif
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

