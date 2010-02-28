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

#if defined WIN32
#  pragma warning(disable:4273)
#endif
#include "API/Process.c"
#include "API/Connector.c"
#include "API/Standalone.c"
#if defined WIN32
#  pragma warning(default:4273)
#endif
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

void test_null_context( void ** state )
{
   psoaProcess process;
   int errcode;
   psonSessionContext context;
   bool ok;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   assert_true( ok );
      
   memset( &process, 0, sizeof(psoaProcess) );
   errcode = psoaProcessInit( &process, "10701", "" );
   assert_true( errcode == PSO_OK );
   
   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   psocInitErrorHandler( &context.errorHandler );
   
   expect_assert_failure( psoaOpenMemory( &process, "dummy", 100, NULL ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_filename( void ** state )
{
   psoaProcess process;
   int errcode;
   psonSessionContext context;
   bool ok;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   assert_true( ok );
      
   memset( &process, 0, sizeof(psoaProcess) );
   errcode = psoaProcessInit( &process, "10701", "" );
   assert_true( errcode == PSO_OK );

   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   psocInitErrorHandler( &context.errorHandler );
   
   expect_assert_failure( psoaOpenMemory( &process, NULL, 100, &context ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_process( void ** state )
{
   psoaProcess process;
   int errcode;
   psonSessionContext context;
   bool ok;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   assert_true( ok );
      
   memset( &process, 0, sizeof(psoaProcess) );
   errcode = psoaProcessInit( &process, "10701", "" );
   assert_true( errcode == PSO_OK );

   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   psocInitErrorHandler( &context.errorHandler );
   
   expect_assert_failure( psoaOpenMemory( NULL, "dummy", 100, &context ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
   psoaProcess process;
   int errcode;
   psonSessionContext context;
   bool ok;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   assert_true( ok );

   memset( &process, 0, sizeof(psoaProcess) );
   errcode = psoaProcessInit( &process, "10701", "" );
   assert_true( errcode == PSO_OK );

   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   psocInitErrorHandler( &context.errorHandler );
   
   errcode = psoaOpenMemory( &process, "dummy", 100, &context );

   assert_true( errcode == PSO_BACKSTORE_FILE_MISSING );

   psoaProcessFini();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_filename, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_process,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,          setup_test, teardown_test ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

