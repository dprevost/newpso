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

#include "Tools/Debugger/folder.h"
#include "Tools/Debugger/listCtrl.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyFolder::MyFolder( void * addr, psonSessionContext * context )
   : MyNode         ( context ),
     previousOffset ( PSON_NULL_OFFSET )
{
   m_folder = (psonFolder *)addr;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyFolder::~MyFolder()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool MyFolder::GetFirst( struct MyObject & obj )
{
   ptrdiff_t offset;
   psonHashTxItem * pItem;
   psonTreeNode * pNode = NULL;
   void * pObject;
   bool found;
   
   /* The easy case */
   if ( m_folder->hashObj.numberOfItems == 0 ) return false;

   found = psonHashTxGetFirst( &m_folder->hashObj, &offset, pContext );
   if ( found ) {
      GET_PTR(pContext->pBaseAddress, pItem, offset, psonHashTxItem );
      GET_PTR(pContext->pBaseAddress, pNode, pItem->dataOffset, psonTreeNode );
      GET_PTR(pContext->pBaseAddress, pObject, pNode->offset, void );
      
      obj.addr = pObject;
      obj.type = pNode->apiType;
      obj.name = (char *)pItem->key;
      obj.length = pItem->keyLength;

      previousOffset = offset;
      return true;
   }

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool MyFolder::GetNext( struct MyObject & obj )
{
   ptrdiff_t offset;
   psonHashTxItem * pItem;
   psonTreeNode * pNode = NULL;
   void * pObject;
   bool found;
   
   /* The easy case */
   if ( m_folder->hashObj.numberOfItems == 0 ) return false;

   found = psonHashTxGetNext( &m_folder->hashObj,
                              previousOffset,
                              &offset,
                              pContext );

   if ( found ) {
      GET_PTR(pContext->pBaseAddress, pItem, offset, psonHashTxItem );
      GET_PTR(pContext->pBaseAddress, pNode, pItem->dataOffset, psonTreeNode );
      GET_PTR(pContext->pBaseAddress, pObject, pNode->offset, void );
      
      obj.addr = pObject;
      obj.type = pNode->apiType;
      obj.name = (char *)pItem->key;
      obj.length = pItem->keyLength;

      previousOffset = offset;
      return true;
   }

   previousOffset = PSON_NULL_OFFSET;
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyFolder::Show( MyListCtrl * listCtrl )
{
   listCtrl->DeleteAllItems();
   /** The memory object struct. */
   ShowMemObject( listCtrl, &m_folder->memObject );
#if 0

   /** Basic info for all leaves and branches of our tree. */
   ptrdiff_t nodeOffset;
   
   struct psonHashTx hashObj;
   
   psoObjectDefinition definition;
   /*
    * This field cannot be set or modified by the API. It is set to false
    * by the "constructor". Quasar will set it to true when creating a new
    * shared-memory and constructing its system objects.
    *
    * When this field is set to true, no data can be added or removed 
    * directly by accessing the object through the API. Updates are done 
    * indirectly as a consequence of some API functions.
    */
   bool isSystemObject;

   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;
#endif
   listCtrl->SetColumnWidth( 0, wxLIST_AUTOSIZE );
   listCtrl->SetColumnWidth( 1, wxLIST_AUTOSIZE );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
