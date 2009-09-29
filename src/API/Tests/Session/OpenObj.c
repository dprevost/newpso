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
#include "API/CommonObject.h"
#include "API/Session.h"
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

void test_not_created( void ** state )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   struct psoaCommonObject object;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoaSessionOpenObj( (psoaSession *) sessionHandle,
                                 PSO_FOLDER,
                                 false,
                                 "/api_session_openobj_not_created",
                                 strlen("/api_session_openobj_not_created"),
                                 &object );
   assert_true( errcode == PSO_NO_SUCH_OBJECT );

   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   struct psoaCommonObject object;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_openobj_null_name",
                              strlen("/api_session_openobj_null_name") );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaSessionOpenObj( (psoaSession *) sessionHandle,
                                              PSO_FOLDER,
                                              false,
                                              NULL,
                                              strlen("/asoono"),
                                              &object ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_object( void ** state )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_openobj_null_object",
                              strlen("/api_session_openobj_null_object") );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaSessionOpenObj( (psoaSession *) sessionHandle,
                                              PSO_FOLDER,
                                              false,
                                              "/api_session_openobj_null_object",
                                              strlen("/api_session_openobj_null_object"),
                                              NULL ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_session( void ** state )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   struct psoaCommonObject object;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_openobj_null_session",
                              strlen("/api_session_openobj_null_session") );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaSessionOpenObj( NULL,
                                              PSO_FOLDER,
                                              false,
                                              "/api_session_openobj_null_session",
                                              strlen("/api_session_openobj_null_session"),
                                              &object ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_type( void ** state )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   struct psoaCommonObject object;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_openobj_wrong_type",
                              strlen("/api_session_openobj_wrong_type") );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaSessionOpenObj( (psoaSession *) sessionHandle,
                                              0,
                                              false,
                                              "/api_session_openobj_wrong_type",
                                              strlen("/api_session_openobj_wrong_type"),
                                              &object ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   struct psoaCommonObject object;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_openobj_zero_length",
                              strlen("/api_session_openobj_zero_length") );
   assert_true( errcode == PSO_OK );

   expect_assert_failure( psoaSessionOpenObj( (psoaSession *) sessionHandle,
                                              PSO_FOLDER,
                                              false,
                                              "/api_session_openobj_zero_length",
                                              0,
                                              &object ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   struct psoaCommonObject object;
   
   errcode = psoInit( "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_openobj_pass",
                              strlen("/api_session_openobj_pass") );
   assert_true( errcode == PSO_OK );

   errcode = psoaSessionOpenObj( (psoaSession *) sessionHandle,
                                 PSO_FOLDER,
                                 false,
                                 "/api_session_openobj_pass",
                                 strlen("/api_session_openobj_pass"),
                                 &object );
   assert_true( errcode == PSO_OK );

   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_not_created,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_object,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_session, setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_type,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

