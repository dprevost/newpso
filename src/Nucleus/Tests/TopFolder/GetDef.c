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

#include "folderTest.h"
#include <photon/psoCommon.h>

psonFolder * pTopFolder;
psonSessionContext context;
psoKeyDefinition * retKeyDef = NULL;
psoObjectDefinition * retDef = NULL;

psoObjectDefinition * def;
psoKeyDefinition * keyDef;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   def = (psoObjectDefinition *) malloc(
      offsetof(psoObjectDefinition, dataDef) + 50 );
   assert( def != NULL );
   def->type = PSO_HASH_MAP;
   def->minNumOfDataRecords = 0;
   def->minNumBlocks = 1;
   def->dataDefType = PSO_DEF_USER_DEFINED;
   def->dataDefLength = 50;

   keyDef = malloc( offsetof( psoKeyDefinition, definition) + 20 );
   assert( keyDef != NULL );

   keyDef->type = PSO_DEF_USER_DEFINED;
   keyDef->definitionLength = 20;

   pTopFolder = initTopFolderTest( &context );
   
   ok = psonTopFolderCreateMap( pTopFolder,
                                "Test1",
                                strlen("Test1"),
                                def,
                                keyDef,
                                &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if ( def != NULL ) free( def );
   def = NULL;
   if ( keyDef != NULL ) free( keyDef );
   keyDef = NULL;
   
   if (g_pBaseAddr) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( pTopFolder,
                                               "Test1",
                                               strlen("Test1"),
                                               &retDef,
                                               &retKeyDef,
                                               NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_def( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( pTopFolder,
                                               "Test1",
                                               strlen("Test1"),
                                               NULL,
                                               &retKeyDef,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( NULL,
                                               "Test1",
                                               strlen("Test1"),
                                               &retDef,
                                               &retKeyDef,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_key_def( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( pTopFolder,
                                               "Test1",
                                               strlen("Test1"),
                                               &retDef,
                                               NULL,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( pTopFolder,
                                               NULL,
                                               strlen("Test1"),
                                               &retDef,
                                               &retKeyDef,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonTopFolderGetDef( pTopFolder,
                             "Test1",
                             strlen("Test1"),
                             &retDef,
                             &retKeyDef,
                             &context );
   assert_true( ok );
   
//   fprintf(stderr, "%d\n", def->type
   assert_memory_equal( retKeyDef, keyDef, 
      offsetof(psoKeyDefinition,definition) + 20 );
   assert_memory_equal( retDef, def, 
      offsetof(psoObjectDefinition,dataDef) + 50 );
   
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
      unit_test_setup_teardown( test_null_def,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_folder,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_key_def, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

