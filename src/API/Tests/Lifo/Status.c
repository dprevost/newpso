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
#include "API/Lifo.h"
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
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   const char * data1 = "My Data1";
   const char * data2 = "My Data2";
   const char * data3 = "My Data3";
   psoObjStatus status;
   psoObjectDefinition defLifo = { PSO_LIFO, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle;

   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_lifo_status",
                              strlen("/api_lifo_status") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_lifo_status",
                               strlen("api_lifo_status"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateQueue( sessionHandle,
                             "/api_lifo_status/test",
                             strlen("/api_lifo_status/test"),
                             &defLifo );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoOpen( sessionHandle,
                           "/api_lifo_status/test",
                           strlen("/api_lifo_status/test"),
                           &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoPush( objHandle, data1, strlen(data1) );
   assert_true( errcode == PSO_OK );
   errcode = psoLifoPush( objHandle, data2, strlen(data2) );
   assert_true( errcode == PSO_OK );
   errcode = psoLifoPush( objHandle, data3, strlen(data3) );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoLifoStatus( NULL, &status );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoLifoStatus( objHandle, NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoLifoStatus( objHandle, &status );
   assert_true( errcode == PSO_OK );

   assert_true( status.numDataItem == 3 );
   assert_true( status.numBlocks == 1 );
   assert_true( status.numBlockGroup == 1 );
   assert_false( status.freeBytes == 0 || status.freeBytes >=PSON_BLOCK_SIZE );
   
   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoLifoStatus( objHandle, &status );
   assert_true( errcode == PSO_SESSION_IS_TERMINATED );

   psoExit();
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

