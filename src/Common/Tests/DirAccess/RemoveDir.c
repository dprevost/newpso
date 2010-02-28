/*
 * Copyright (C) 2009-2010 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "Common/DirAccess.h"

psocErrorHandler errorHandler;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psocInitErrorHandler( &errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psocFiniErrorHandler( &errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_error( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocRemoveDir( ".", NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocRemoveDir( NULL, &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int rc, fd;
   bool ok;
   
   rc = mkdir( "junk1", 0755 );
   assert_true( rc == 0 );
   rc = mkdir( "junk1/junk2", 0755 );
   assert_true( rc == 0 );
   rc = mkdir( "junk1/junk3", 0755 );
   assert_true( rc == 0 );
   rc = mkdir( "junk1/junk2/junk4", 0755 );
   assert_true( rc == 0 );
   rc = mkdir( "junk1/junk2/junk4/junk5", 0755 );
   assert_true( rc == 0 );
   fd = creat( "junk1/test1.txt", 0644 );
   assert_false( fd == -1 );
   close( fd );
   fd = creat( "junk1/junk2/junk4/test2.txt", 0644 );
   assert_false( fd == -1 );
   close( fd );
   fd = creat( "junk1/junk2/junk4/test3.txt", 0644 );
   assert_false( fd == -1 );
   close( fd );
   fd = creat( "junk1/junk2/junk4/junk5/test4.txt", 0644 );
   assert_false( fd == -1 );
   close( fd );
   
   ok = psocRemoveDir( "junk1", &errorHandler );
   assert_true( ok );
   
   rc = access( "junk1", F_OK );
   assert_true( rc == -1 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_error, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,       setup_test, teardown_test )
   };

   psocInitErrorDefs();
   rc = run_tests(tests);
   psocFiniErrorDefs();
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
