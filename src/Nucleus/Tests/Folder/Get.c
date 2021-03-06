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

#include "folderTest.h"

psonFolder * pFolder;
psonSessionContext context;
psonTxStatus status;
psonFolderItem folderItem;
psoObjectDefinition def = { PSO_FOLDER, 0, 0 };
psonTreeNode node;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psonTreeNode * pDescriptor;
   psonTxStatus * txItemStatus;

   pFolder = initFolderTest( &context );

   psonTxStatusInit( &status, SET_OFFSET(context.pBaseAddress, context.pTransaction), &context );
   psonTreeNodeInit( &node, SET_OFFSET(context.pBaseAddress, pFolder), PSO_FOLDER,
                     SET_OFFSET(context.pBaseAddress, &status), PSON_NULL_OFFSET, &context );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &node, &context );
   assert( ok );
   
   ok = psonFolderInsertFolder( pFolder,
                                "test2",
                                5,
                                &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( context.pBaseAddress );
   context.pBaseAddress = NULL;
   pFolder = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   pFolder->memObject.objType = 0;
   expect_assert_failure( psonFolderGetObject( pFolder,
                                               "test2",
                                               5,
                                               PSO_FOLDER,
                                               &folderItem,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderGetObject( pFolder,
                                               "test2",
                                               5,
                                               PSO_FOLDER,
                                               &folderItem,
                                               NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderGetObject( NULL,
                                               "test2",
                                               5,
                                               PSO_FOLDER,
                                               &folderItem,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderGetObject( pFolder,
                                               "test2",
                                               5,
                                               PSO_FOLDER,
                                               NULL,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderGetObject( pFolder,
                                               NULL,
                                               5,
                                               PSO_FOLDER,
                                               &folderItem,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonFolderGetObject( pFolder,
                             "test2",
                             5,
                             PSO_QUEUE,
                             &folderItem,
                             &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_WRONG_OBJECT_TYPE );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderGetObject( pFolder,
                                               "test2",
                                               0,
                                               PSO_FOLDER,
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
   psonTreeNode * pDescriptor;
   psonTxStatus * txItemStatus;

   ok = psonFolderGetObject( pFolder,
                             "test2",
                             5,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   assert_true( ok );
   GET_PTR(context.pBaseAddress, pDescriptor, folderItem.pHashItem->dataOffset, psonTreeNode );
   GET_PTR(context.pBaseAddress, txItemStatus, pDescriptor->txStatusOffset, psonTxStatus );
   assert_true( txItemStatus->parentCounter == 1 );
   assert_true( status.usageCounter == 1 );
   
   ok = psonFolderGetObject( pFolder,
                             "test3",
                             5,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_NO_SUCH_OBJECT );
      
   ok = psonFolderInsertFolder( pFolder,
                                "test4",
                                5,
                                &context );
   assert_true( ok );
   
   ok = psonFolderGetObject( pFolder,
                             "test4",
                             5,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   assert_true( ok );
   GET_PTR(context.pBaseAddress, pDescriptor, folderItem.pHashItem->dataOffset, psonTreeNode );
   GET_PTR(context.pBaseAddress, txItemStatus, pDescriptor->txStatusOffset, psonTxStatus );
   assert_true( txItemStatus->parentCounter == 1 );
   assert_true( status.usageCounter == 2 );
   assert_true( node.txCounter == 2 );
   
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
      unit_test_setup_teardown( test_invalid_sig,  setup_test, teardown_test ),
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

