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
   PSO_HANDLE sessionHandle;
   int errcode;
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   psoKeyFieldDefinition keyDef = { "Key1", PSO_KEY_VARCHAR, 100 };
   PSO_HANDLE dataDefHandle, keyDefHandle, returnedDef;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_session_key_definition",
                               strlen("api_session_key_definition"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoKeyDefCreate( sessionHandle,
                              "api_session_key_definition",
                              strlen("api_session_key_definition"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_session_key_definition",
                           strlen("/api_session_key_definition"),
                           &def,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoGetKeyDefinition( NULL,
                                  "/api_session_key_definition",
                                  strlen("/api_session_key_definition"),
                                  &returnedDef );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoGetKeyDefinition( sessionHandle,
                                  NULL,
                                  strlen("/api_session_key_definition"),
                                  &returnedDef );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   errcode = psoGetKeyDefinition( sessionHandle,
                                  "/api_session_key_definition",
                                  0,
                                  &returnedDef );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoGetKeyDefinition( sessionHandle,
                                  "/api_session_key_definition",
                                  strlen("/api_session_key_definition"),
                                  NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoGetKeyDefinition( sessionHandle,
                                  "/api_session_key_definition",
                                  strlen("/api_session_key_definition"),
                                  &returnedDef );
   assert_true( errcode == PSO_OK );

   /* Close the process and try to act on the session */

   psoExit();
   
   errcode = psoGetKeyDefinition( sessionHandle,
                                  "/api_session_key_definition",
                                  strlen("/api_session_key_definition"),
                                  &returnedDef );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );
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

