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
#include "API/CommonObject.h"
#include "API/Tests/quasar-run.h"

PSO_HANDLE shmemHandle;

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

void test_null_object( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE sessionHandle;
   int errcode;
   struct psoaCommonObject object;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_closeobj_null_object",
                              strlen("/api_session_closeobj_null_object") );
   assert_true( errcode == PSO_OK );

   errcode = psoaSessionOpenObj( (psoaSession *) sessionHandle,
                                 PSO_FOLDER,
                                 false,
                                 "/api_session_closeobj_null_object",
                                 strlen("/api_session_closeobj_null_object"),
                                 &object );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaSessionCloseObj( (psoaSession *) sessionHandle,
                                               NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_session( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE sessionHandle;
   int errcode;
   struct psoaCommonObject object;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_closeobj_null_session",
                              strlen("/api_session_closeobj_null_session") );
   assert_true( errcode == PSO_OK );

   errcode = psoaSessionOpenObj( (psoaSession *) sessionHandle,
                                 PSO_FOLDER,
                                 false,
                                 "/api_session_closeobj_null_session",
                                 strlen("/api_session_closeobj_null_session"),
                                 &object );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaSessionCloseObj( NULL, &object ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE sessionHandle;
   int errcode;
   struct psoaCommonObject object;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( shmemHandle, &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_closeobj_pass",
                              strlen("/api_session_closeobj_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoaSessionOpenObj( (psoaSession *) sessionHandle,
                                 PSO_FOLDER,
                                 false,
                                 "/api_session_closeobj_pass",
                                 strlen("/api_session_closeobj_pass"),
                                 &object );
   assert_true( errcode == PSO_OK );

   errcode = psoaSessionCloseObj( (psoaSession *) sessionHandle,
                                  &object );
   assert_true( errcode == PSO_OK );

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
      unit_test_setup_teardown( test_null_object,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_session, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

