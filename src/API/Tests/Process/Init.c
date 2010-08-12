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

#include "API/Process.h"
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

void test_null_address( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoaProcess process;
   int errcode;
   bool ok;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   assert_true( ok );
   
   memset( &process, 0, sizeof(psoaProcess) );
   
   expect_assert_failure( psoaProcessInit( &process, NULL, "" ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_process( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psoaProcessInit( NULL, "10701", "" ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_address( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoaProcess process;
   int errcode;
   bool ok;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   assert_true( ok );

   memset( &process, 0, sizeof(psoaProcess) );

   /* try something junk... */   
   errcode = psoaProcessInit( &process, "abcdef", NULL );
   assert_true( errcode == PSO_INVALID_QUASAR_ADDRESS );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoaProcess process;
   int errcode;
   bool ok;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   assert_true( ok );
   
   memset( &process, 0, sizeof(psoaProcess) );

   errcode = psoaProcessInit( &process, "10701", NULL );
   assert_true( errcode == PSO_OK );
   
   psoaProcessFini();
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_address,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_process,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_address, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,          setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
