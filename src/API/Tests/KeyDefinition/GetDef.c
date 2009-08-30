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
#include "API/KeyDefinition.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE defHandle, sessionHandle;
   int errcode;
   psoKeyFieldDefinition keyField = { "Key_1", PSO_KEY_VARCHAR, 12 };
   enum psoDefinitionType type;
   char * name;
   psoUint32 nameLength;
   unsigned char * keyDef;
   unsigned int keyDefLength;
   
   if ( argc > 1 ) {
      errcode = psoInit( argv[1], argv[0] );
   }
   else {
      errcode = psoInit( "10701", argv[0] );
   }
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_key_definition_get_def",
                              strlen("api_key_definition_get_def"),
                              PSO_DEF_USER_DEFINED,
                              (const unsigned char *) &keyField,
                              sizeof(psoKeyFieldDefinition),
                              &defHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */
   errcode = psoaKeyDefGetDef( NULL,
                               &name,
                               &nameLength,
                               &type,
                               &keyDef,
                               &keyDefLength );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoaKeyDefGetDef( sessionHandle,
                               &name,
                               &nameLength,
                               &type,
                               &keyDef,
                               &keyDefLength );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoaKeyDefGetDef( defHandle,
                               NULL,
                               &nameLength,
                               &type,
                               &keyDef,
                               &keyDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoaKeyDefGetDef( defHandle,
                               &name,
                               NULL,
                               &type,
                               &keyDef,
                               &keyDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoaKeyDefGetDef( defHandle,
                               &name,
                               &nameLength,
                               NULL,
                               &keyDef,
                               &keyDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoaKeyDefGetDef( defHandle,
                               &name,
                               &nameLength,
                               &type,
                               NULL,
                               &keyDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoaKeyDefGetDef( defHandle,
                               &name,
                               &nameLength,
                               &type,
                               &keyDef,
                               NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoaKeyDefGetDef( defHandle,
                               &name,
                               &nameLength,
                               &type,
                               &keyDef,
                               &keyDefLength );
   assert_true( errcode == PSO_OK );
   
   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
