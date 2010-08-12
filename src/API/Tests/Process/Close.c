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

/*
 * On Windows, we either must include Connector.c or... we must export 
 * the function. Exporting the function for a single test? Not worth it.
 */
#if defined WIN32
#  pragma warning(disable:4273)
#  include "API/Connector.c"
#  pragma warning(default:4273)
#endif

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

void test_null_context( void ** state )
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

   expect_assert_failure( psoaCloseMemory( &process, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_process( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoaProcess process;
   int errcode;
   bool ok;
   psonSessionContext context;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   assert_true( ok );
   
   memset( &process, 0, sizeof(psoaProcess) );

   errcode = psoaProcessInit( &process, "10701", NULL );
   assert_true( errcode == PSO_OK );

   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   psocInitErrorHandler( &context.errorHandler );

   expect_assert_failure( psoaCloseMemory( NULL, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoaProcess process;
   int errcode;
   psonSessionContext context;
   bool ok;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   assert_true( ok );
   
   memset( &process, 0, sizeof(psoaProcess) );
   errcode = psoaProcessInit( &process, "10701", NULL );
   assert_true( errcode == PSO_OK );

   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   psocInitErrorHandler( &context.errorHandler );
   
   psoaCloseMemory( &process, &context );

   /* Cannot call psoaProcessFini since it calls CloseMemory()  */
   psoaDisconnect( &process.connector, &context.errorHandler );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_process, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

