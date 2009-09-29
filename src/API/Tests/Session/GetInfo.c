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
   PSO_HANDLE handle, sessionHandle;
   int errcode;
   psoInfo info, info2;
   size_t allocSpace;
   int xyz = 12345;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoGetInfo( NULL, &info );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoGetInfo( sessionHandle, NULL );
   assert_true( errcode == PSO_NULL_POINTER );
   
   handle = (PSO_HANDLE) &xyz;
   errcode = psoGetInfo( handle, &info );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );
   
   /* End of invalid args. This call should succeed. */
   errcode = psoGetInfo( sessionHandle, &info );
   assert_true( errcode == PSO_OK );
   allocSpace = info.allocatedSizeInBytes;
   
   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_get_info",
                              strlen("/api_session_get_info") );
   assert_true( errcode == PSO_OK );
   
   errcode = psoGetInfo( sessionHandle, &info2 );
   assert_true( errcode == PSO_OK );
   assert_false( info.allocatedSizeInBytes >= info2.allocatedSizeInBytes );
   assert_true( info.numObjects+1 == info2.numObjects );
   assert_true( info.numGroups+1 == info2.numGroups );
   assert_true( info.numMallocs+1 == info2.numMallocs );
   assert_true( info.numFrees == info2.numFrees );
   
   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoDestroyObject( sessionHandle,
                               "/api_session_get_info",
                               strlen("/api_session_get_info") );
   assert_true( errcode == PSO_OK );
   errcode = psoGetInfo( sessionHandle, &info );
   assert_true( errcode == PSO_OK );
   assert_false( info.allocatedSizeInBytes < info2.allocatedSizeInBytes );
   assert_true( info.numObjects == info2.numObjects );
   assert_true( info.numGroups == info2.numGroups );
   assert_true( info.numMallocs == info2.numMallocs );
   assert_true( info.numFrees == info2.numFrees );
   
   psoCommit( sessionHandle );
   errcode = psoGetInfo( sessionHandle, &info2 );
   assert_true( errcode == PSO_OK );
   assert_true( info2.allocatedSizeInBytes == allocSpace );
   assert_true( info.numObjects == info2.numObjects+1 );
   assert_true( info.numGroups == info2.numGroups+1 );
   assert_true( info.numMallocs == info2.numMallocs );
   assert_true( info.numFrees+1 == info2.numFrees );
   
   /* Close the process and try to act on the session */

   psoExit();
   
   errcode = psoGetInfo( sessionHandle, &info );
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

