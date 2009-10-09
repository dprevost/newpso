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

#include "folderTest.h"

psonFolder * pFolder;
psonSessionContext context;
psoObjectDefinition def = { PSO_QUEUE, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   pFolder = initTopFolderTest( &context );
   assert( pFolder );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_name_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   char name[PSO_MAX_NAME_LENGTH+100];
   bool ok;
   int errcode;
   
   memset( name, 't', PSO_MAX_NAME_LENGTH+99 );
   name[PSO_MAX_NAME_LENGTH+99] = 0;
   
   ok = psonAPIFolderCreateQueue( pFolder,
                                  "Test1",
                                  0,
                                  &def,
                                  &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   ok = psonAPIFolderCreateQueue( pFolder,
                                  "/Test2",
                                  strlen("/Test2"),
                                  &def,
                                  &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   ok = psonAPIFolderCreateQueue( pFolder,
                                  name,
                                  PSO_MAX_NAME_LENGTH+1,
                                  &def,
                                  &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_OBJECT_NAME_TOO_LONG );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonAPIFolderCreateQueue( pFolder,
                                                    "Test1",
                                                    strlen("Test1"),
                                                    &def,
                                                    NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_definition( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonAPIFolderCreateQueue( pFolder,
                                                    "Test1",
                                                    strlen("Test1"),
                                                    NULL,
                                                    &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonAPIFolderCreateQueue( NULL,
                                                    "Test1",
                                                    strlen("Test1"),
                                                    &def,
                                                    &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonAPIFolderCreateQueue( pFolder,
                                                    NULL,
                                                    strlen("Test1"),
                                                    &def,
                                                    &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoObjectDefinition local_def = { 0, 0, 0 };

   expect_assert_failure( psonAPIFolderCreateQueue( pFolder,
                                                    "Test1",
                                                    strlen("Test1"),
                                                    &local_def,
                                                    &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonAPIFolderCreateQueue( pFolder,
                                  "Test1",
                                  strlen("Test1"),
                                  &def,
                                  &context );
   assert_true( ok );
   
   ok = psonAPIFolderCreateQueue( pFolder,
                                  "Test2",
                                  strlen("Test2"),
                                  &def,
                                  &context );
   assert_true( ok );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_name_length,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_definition, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_folder,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_type,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,            setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

