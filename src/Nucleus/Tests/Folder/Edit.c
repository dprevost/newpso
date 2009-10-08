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
psonTxStatus status;
psonFolderItem folderItem;
psonTreeNode node;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };
   psoKeyDefinition key = { PSO_DEF_USER_DEFINED, 0, '\0' };

   pFolder = initFolderTest( &context );

   psonTxStatusInit( &status, SET_OFFSET(context.pTransaction), &context );
   psonTreeNodeInit( &node, SET_OFFSET(pFolder), PSO_FOLDER,
                     SET_OFFSET(&status), PSON_NULL_OFFSET, &context );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &node, &context );
   assert( ok );
   
   ok = psonFolderInsertMap( pFolder,
                             "test2",
                             5,
                             &mapDef,
                             &key,
                             1,
                             0,
                             &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
   pFolder = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                "test2",
                                                5,
                                                PSO_FAST_MAP,
                                                &folderItem,
                                                NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderEditObject( NULL,
                                                "test2",
                                                5,
                                                PSO_FAST_MAP,
                                                &folderItem,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                "test2",
                                                5,
                                                PSO_FAST_MAP,
                                                NULL,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                NULL,
                                                5,
                                                PSO_FAST_MAP,
                                                &folderItem,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   pFolder->memObject.objType = PSON_IDENT_HASH_MAP;
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                "test2",
                                                5,
                                                PSO_FAST_MAP,
                                                &folderItem,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                "test2",
                                                0,
                                                PSO_FAST_MAP,
                                                &folderItem,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   psonTreeNode * pMapNode;
   psonTxStatus * txItemStatus;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };
   psoKeyDefinition key = { PSO_DEF_USER_DEFINED, 0, '\0' };


   ok = psonFolderEditObject( pFolder,
                              "test2",
                              5,
                              PSO_FAST_MAP,
                              &folderItem,
                              &context );
   assert_true( ok );
   GET_PTR( pMapNode, folderItem.pHashItem->dataOffset, psonTreeNode );
   GET_PTR( txItemStatus, pMapNode->txStatusOffset, psonTxStatus );
   assert_true( txItemStatus->parentCounter == 1 );
   assert_true( status.usageCounter == 1 );
   
   ok = psonFolderEditObject( pFolder,
                              "test3",
                              5,
                              PSO_FAST_MAP,
                              &folderItem,
                              &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_NO_SUCH_OBJECT );
   
   ok = psonFolderInsertMap( pFolder,
                             "test4",
                             5,
                             &mapDef,
                             &key,
                             1,
                             0,
                             &context );
   assert_true( ok );
   
   ok = psonFolderEditObject( pFolder,
                              "test4",
                              5,
                              PSO_FAST_MAP,
                              &folderItem,
                              &context );
   assert_true( ok );
   GET_PTR( pMapNode, folderItem.pHashItem->dataOffset, psonTreeNode );
   GET_PTR( txItemStatus, pMapNode->txStatusOffset, psonTxStatus );
   assert_true( txItemStatus->parentCounter == 1 );
   assert_true( status.usageCounter == 2 );
   assert_true( node.txCounter == 4 );
   
   psonFolderFini( pFolder, &context );
   
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
      unit_test_setup_teardown( test_null_folder,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_item,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_type,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

