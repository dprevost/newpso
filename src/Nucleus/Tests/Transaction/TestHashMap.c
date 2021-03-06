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

#include "txTest.h"
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonTx* pTx;
   psonFolder * pFolder;
   psonSessionContext context;
   bool ok;
   psonFolderItem item;
   psonTxStatus status;
   psonTreeNode * pMapNode;
   psonHashMap * pHashMap;
   char * key1  = "My key1";
   char * key2  = "My key2";
   char * key3  = "My key3";
   char * data1 = "My data1";
   char * data2 = "My data2";
   char * data3 = "My data3";
   psonHashTxItem * pHashItem;
//   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };
//   psonKeyDefinition key;
   psonTreeNode node;
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, PSO_DEF_USER_DEFINED, 0, '\0' };
   psoKeyDefinition key = { PSO_DEF_USER_DEFINED, 0, '\0' };

   pFolder = initFolderTest( &context );
   pTx = context.pTransaction;
   
   psonTxStatusInit( &status, SET_OFFSET(context.pBaseAddress, pTx ), &context );
   psonTreeNodeInit( &node, SET_OFFSET(context.pBaseAddress, pFolder), PSO_FOLDER,
                     SET_OFFSET(context.pBaseAddress, &status), PSON_NULL_OFFSET, &context );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &node, &context );
   assert_true( ok );
   
   ok = psonFolderInsertMap( pFolder,
                             "test2",
                             5,
                             &def,
                             &key,
                             1,
                             0,
                             &context );
   assert_true( ok );
   psonTxCommit( pTx, &context );

   ok = psonFolderGetObject( pFolder,
                             "test2",
                             5,
                             PSO_HASH_MAP,
                             &item,
                             &context );
   assert_true( ok );
   GET_PTR(context.pBaseAddress, pMapNode, item.pHashItem->dataOffset, psonTreeNode );
   GET_PTR(context.pBaseAddress, pHashMap, pMapNode->offset, psonHashMap );

   /* Test 1 */
   ok = psonHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   assert_true( ok );
   ok = psonHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   assert_true( ok );
   ok = psonHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   assert_true( ok );
   
   assert_true( pMapNode->txCounter == 3 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   psonTxRollback( pTx, &context );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 0 );
   
   /* Test 2 */
   ok = psonHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   assert_true( ok );
   ok = psonHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   assert_true( ok );
   ok = psonHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   assert_true( ok );
   
   assert_true( pMapNode->txCounter == 3 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   psonTxCommit( pTx, &context );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   ok = psonHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   assert_true( ok );
   ok = psonHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   assert_true( ok );
   ok = psonHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   assert_true( ok );
   
   assert_true( pMapNode->txCounter == 3 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   psonTxRollback( pTx, &context );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   ok = psonHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   assert_true( ok );
   ok = psonHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   assert_true( ok );
   ok = psonHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   assert_true( ok );
   
   assert_true( pMapNode->txCounter == 3 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   psonTxCommit( pTx, &context );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 0 );
   
   /* Test 3 */
   ok = psonHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   assert_true( ok );
   ok = psonHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   assert_true( ok );
   ok = psonHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   assert_true( ok );
   
   ok = psonHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (uint32_t) -1,
                        &context );
   assert_true( ok );
   
   assert_true( pMapNode->txCounter == 3 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   psonTxRollback( pTx, &context );
   assert_true( pMapNode->txCounter == 1 );
   assert_true( pHashMap->hashObj.numberOfItems == 1 );
   
   ok = psonHashMapRelease( pHashMap,
                            pHashItem,
                            &context );
   assert_true( ok );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 0 );
   
   /* Test 4 */
   ok = psonHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   assert_true( ok );
   ok = psonHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   assert_true( ok );
   ok = psonHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   assert_true( ok );
   
   ok = psonHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (uint32_t) -1,
                        &context );
   assert_true( ok );
   
   psonTxCommit( pTx, &context );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   ok = psonHashMapRelease( pHashMap,
                            pHashItem,
                            &context );
   assert_true( ok );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   ok = psonHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (uint32_t) -1,
                        &context );
   assert_true( ok );
   ok = psonHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   assert_true( ok );
   ok = psonHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   assert_true( ok );
   ok = psonHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   assert_true( ok );
   
   psonTxRollback( pTx, &context );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   ok = psonHashMapRelease( pHashMap,
                            pHashItem,
                            &context );
   assert_true( ok );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 3 );
   
   ok = psonHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (uint32_t) -1,
                        &context );
   assert_true( ok );
   
   ok = psonHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   assert_true( ok );
   ok = psonHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   assert_true( ok );
   ok = psonHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   assert_true( ok );
   
   psonTxCommit( pTx, &context );
   assert_true( pMapNode->txCounter == 1 );
   assert_true( pHashMap->hashObj.numberOfItems == 1 );
   
   ok = psonHashMapRelease( pHashMap,
                            pHashItem,
                            &context );
   assert_true( ok );
   assert_true( pMapNode->txCounter == 0 );
   assert_true( pHashMap->hashObj.numberOfItems == 0 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


