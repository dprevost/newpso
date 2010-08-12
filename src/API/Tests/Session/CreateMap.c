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
#include "API/Tests/quasar-run.h"

psoKeyDefinition * pKeyDefinition;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };

   pKeyDefinition = malloc( offsetof( psoKeyDefinition, definition) +
                           sizeof(psoKeyFieldDefinition) );
   assert( pKeyDefinition != NULL );

   pKeyDefinition->type = PSO_DEF_PHOTON_ODBC_SIMPLE;
   pKeyDefinition->definitionLength = sizeof(psoKeyFieldDefinition);
   memcpy( pKeyDefinition->definition, &keyDef, sizeof(psoKeyFieldDefinition) );

   assert( startQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if ( pKeyDefinition ) free( pKeyDefinition );
   pKeyDefinition = NULL;
   assert( stopQuasar() );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE sessionHandle;
   PSO_HANDLE shmemHandle;
   int errcode;
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   psoKeyFieldDefinition keyDef = { "Key1", PSO_KEY_VARCHAR, 100 };
   PSO_HANDLE dataDefHandle, keyDefHandle;
   psoKeyDefinition * pKeyDefinition;
   
   pKeyDefinition = malloc( offsetof( psoKeyDefinition, definition) +
                           sizeof(psoKeyFieldDefinition) );
   assert_false( pKeyDefinition == NULL );

   pKeyDefinition->type = PSO_DEF_PHOTON_ODBC_SIMPLE;
   pKeyDefinition->definitionLength = sizeof(psoKeyFieldDefinition);
   memcpy( pKeyDefinition->definition, &keyDef, sizeof(psoKeyFieldDefinition) );

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoCreateMap( NULL,
                           "/api_session_create_keyed_object",
                           strlen("/api_session_create_keyed_object"),
                           &def,
                           pKeyDefinition );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoCreateMap( sessionHandle,
                           NULL,
                           strlen("/api_session_create_keyed_object"),
                           &def,
                           pKeyDefinition );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoCreateMap( sessionHandle,
                           "/api_session_create_keyed_object",
                           0,
                           &def,
                           pKeyDefinition );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoCreateMap( sessionHandle,
                           "/api_session_create_keyed_object",
                           strlen("/api_session_create_keyed_object"),
                           NULL,
                           pKeyDefinition );
   assert_true( errcode == PSO_NULL_POINTER );

   def.type = PSO_QUEUE;
   errcode = psoCreateMap( sessionHandle,
                           "/api_session_create_keyed_object",
                           strlen("/api_session_create_keyed_object"),
                           &def,
                           pKeyDefinition );
   assert_true( errcode == PSO_WRONG_OBJECT_TYPE );
   def.type = PSO_HASH_MAP;

   errcode = psoCreateMap( sessionHandle,
                           "/api_session_create_keyed_object",
                           strlen("/api_session_create_keyed_object"),
                           &def,
                           NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoCreateMap( sessionHandle,
                           "/api_session_create_keyed_object",
                           strlen("/api_session_create_keyed_object"),
                           &def,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   /* Close the process and try to act on the session */

   psoExit();
   
   errcode = psoCreateMap( sessionHandle,
                           "/api_session_create_keyed_object",
                           strlen("/api_session_create_keyed_object"),
                           &def,
                           pKeyDefinition );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );
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

