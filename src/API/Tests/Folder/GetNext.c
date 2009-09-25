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
#include "API/Folder.h"
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
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   psoFolderEntry entry;
   
   errcode = psoInit( "10701", "GetNext" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_folder_getnext",
                              strlen("/api_folder_getnext") );
   assert_true( errcode == PSO_OK );
   errcode = psoCreateFolder( sessionHandle,
                              "/api_folder_getnext/f1",
                              strlen("/api_folder_getnext/f1") );
   assert_true( errcode == PSO_OK );
   errcode = psoCreateFolder( sessionHandle,
                              "/api_folder_getnext/f2",
                              strlen("/api_folder_getnext/f2") );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_getnext",
                            strlen("/api_folder_getnext"),
                            &objHandle );
   assert_true( errcode == PSO_OK );
   
   /* No GetFirst */
   errcode = psoFolderGetNext( objHandle, &entry );
   assert_true( errcode == PSO_INVALID_ITERATOR );

   errcode = psoFolderGetFirst( objHandle, &entry );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoFolderGetNext( NULL, &entry );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoFolderGetNext( sessionHandle, &entry );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoFolderGetNext( objHandle, NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoFolderGetNext( objHandle, &entry );
   assert_true( errcode == PSO_OK );
   
   errcode = psoFolderGetNext( objHandle, &entry );
   assert_true( errcode == PSO_REACHED_THE_END );

   /* Close the session and try to act on the object */

   errcode = psoFolderGetFirst( objHandle, &entry );
   assert_true( errcode == PSO_OK );

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFolderGetNext( objHandle, &entry );
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

