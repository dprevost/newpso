/*
 * Copyright (C) 2008-2010 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "API/FastMap.h"
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
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE objHandle, sessionHandle, objHandle2;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   char buffer[200];
   uint32_t length;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;

   errcode = psoInit( "10701", "Get" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_fast_map_get",
                              strlen("/api_fast_map_get") );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_fast_map_get/test",
                           strlen("/api_fast_map_get/test"),
                           &mapDef,
                           pKeyDefinition );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_fast_map_get/test",
                             strlen("/api_fast_map_get/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapOpen( sessionHandle,
                             "/api_fast_map_get/test",
                             strlen("/api_fast_map_get/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoFastMapGet( NULL,
                            key,
                            6,
                            buffer,
                            200,
                            &length );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoFastMapGet( objHandle,
                            NULL,
                            6,
                            buffer,
                            200,
                            &length );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoFastMapGet( objHandle,
                            key,
                            0,
                            buffer,
                            200,
                            &length );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoFastMapGet( objHandle,
                            key,
                            6,
                            NULL,
                            200,
                            &length );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoFastMapGet( objHandle,
                            key,
                            6,
                            buffer,
                            2,
                            &length );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoFastMapGet( objHandle,
                            key,
                            6,
                            buffer,
                            200,
                            NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoFastMapGet( objHandle2,
                            key,
                            6,
                            buffer,
                            200,
                            &length );
   assert_true( errcode == PSO_NO_SUCH_ITEM );

   /* End of invalid args. This call should succeed. */
   errcode = psoFastMapGet( objHandle,
                            key,
                            6,
                            buffer,
                            200,
                            &length );
   assert_true( errcode == PSO_OK );
   assert_memory_equal( buffer, data, 7 );
   
   errcode = psoFastMapClose( objHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapGet( objHandle2,
                            key,
                            6,
                            buffer,
                            200,
                            &length );
   assert_true( errcode == PSO_OK );

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapGet( objHandle2,
                            key,
                            6,
                            buffer,
                            200,
                            &length );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );
   
   psoExit();

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


