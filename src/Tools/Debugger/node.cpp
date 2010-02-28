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

#include "Tools/Debugger/node.h"
#include "Tools/Debugger/listCtrl.h"
#include "Nucleus/MemoryObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyNode::ShowLinkedList( MyListCtrl            * listCtrl,
                             struct psonLinkedList * pList,
                             wxString              & prefix )
{
   long tmp;
   int i;
   wxString buf;
   
   i = listCtrl->GetItemCount();

   tmp = listCtrl->InsertItem(i, prefix + _T("psonLinkedList::currentSize:"), 0);
   listCtrl->SetItemData(tmp, i);
   buf.Printf(_T("%d"), pList->currentSize );
   listCtrl->SetItem(tmp, 1, buf);
   i++;

   tmp = listCtrl->InsertItem(i, prefix + _T("psonLinkedList::initialized:"), 0);
   listCtrl->SetItemData(tmp, i);
   if ( pList->initialized == PSON_LIST_SIGNATURE ) {
      listCtrl->SetItem(tmp, 1, _T( "Signature is ok" ) );
   }
   else {
      buf.Printf( _T("Signature is wrong - value is %d, exepted: %d"),
                  pList->initialized,
                  PSON_LIST_SIGNATURE );
      listCtrl->SetItem(tmp, 1, buf );
   }
   i++;

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyNode::ShowLock( MyListCtrl             * listCtrl,
                       struct psocProcessLock * lock,
                       wxString               & prefix  )
{
   long tmp;
   int i;
   wxString buf;
   
   i = listCtrl->GetItemCount();

   tmp = listCtrl->InsertItem(i, prefix + _T("psocProcessLock::initialized:"), 0);
   listCtrl->SetItemData(tmp, i);
   if ( lock->initialized == PSOC_LOCK_SIGNATURE ) {
      listCtrl->SetItem(tmp, 1, _T( "Signature is ok" ) );
   }
   else {
      buf.Printf( _T("Signature is wrong - value is %d, exepted: %d"),
                  lock->initialized,
                  PSOC_LOCK_SIGNATURE );
      listCtrl->SetItem(tmp, 1, buf );
   }
   i++;

   tmp = listCtrl->InsertItem(i, prefix + _T("psocProcessLock::lock:"), 0);
   listCtrl->SetItemData(tmp, i);
   if ( lock->lock == 0 ) {
      listCtrl->SetItem(tmp, 1, _T( "Object is unlocked" ) );
   }
   else {
      listCtrl->SetItem(tmp, 1, _T( "Object is locked" ) );
   }
   i++;

   tmp = listCtrl->InsertItem(i, prefix + _T("psocProcessLock::pid:"), 0);
   listCtrl->SetItemData(tmp, i);
   buf.Printf(_T("%d"), lock->pid );
   listCtrl->SetItem(tmp, 1, buf);
   i++;

   tmp = listCtrl->InsertItem(i, prefix + _T("psocProcessLock::counter:"), 0);
   listCtrl->SetItemData(tmp, i);
   buf.Printf(_T("%d"), lock->counter );
   listCtrl->SetItem(tmp, 1, buf);
   i++;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyNode::ShowMemObject( MyListCtrl *           listCtrl,
                            struct psonMemObject * memObject )
{
   long tmp;
   int i;
   wxString buf;
   
   i = listCtrl->GetItemCount();
   
   tmp = listCtrl->InsertItem(i, _T("psonMemObject::objType:"), 0);
   listCtrl->SetItemData(tmp, i);
   GetObjectType( buf, memObject->objType );
   listCtrl->SetItem(tmp, 1, buf);
   i++;

   buf = _T("psonMemObject::");
   ShowLock( listCtrl, &memObject->lock, buf );

   /** Total number of blocks for the current object */
   tmp = listCtrl->InsertItem(i, _T("psonMemObject::totalBlocks:"), 0);
   listCtrl->SetItemData(tmp, i);
   buf.Printf(_T("%d"), memObject->totalBlocks );
   listCtrl->SetItem(tmp, 1, buf);
   i++;

   buf = _T("psonMemObject::");
   ShowLinkedList( listCtrl,
                   &memObject->listBlockGroup,
                   buf );

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyNode::GetObjectType( wxString & buf, int ident )
{
   psonMemObjIdent identifier = (psonMemObjIdent)ident;
 
   switch( identifier ) {
   case PSON_IDENT_CLEAR:
      buf = _T( "\"Clear!!!\" likely an error" );
      break;
      
   case PSON_IDENT_LIMBO:
      buf = _T( "\"Limbo!!!\" possibly an error" );
      break;
      
   case PSON_IDENT_ALLOCATED:
      buf = _T( "\"Allocated!!!\" likely an error" );
      break;
      
   case PSON_IDENT_FIRST:
      buf = _T( "\"First!!!\" likely an error" );
      break;
      
   case PSON_IDENT_ALLOCATOR:
      buf = _T( "Main memory allocator" );
      break;
      
   case PSON_IDENT_FOLDER:
      buf = _T( "Folder" );
      break;
      
   case PSON_IDENT_HASH_MAP:
      buf = _T( "Hash Map" );
      break;
      
   case PSON_IDENT_PROCESS:
      buf = _T( "Process" );
      break;
      
   case PSON_IDENT_PROCESS_MGR:
      buf = _T( "Process Manager" );
      break;
      
   case PSON_IDENT_QUEUE:
      buf = _T( "Queue" );
      break;
      
   case PSON_IDENT_SESSION:
      buf = _T( "Session" );
      break;
      
   case PSON_IDENT_TRANSACTION:
      buf = _T( "Transaction" );
      break;
      
   case PSON_IDENT_MAP:
      buf = _T( "Fast Map" );
      break;
      
   case PSON_IDENT_CURSOR:
      buf = _T( "Cursor" );
      break;
      
   case PSON_IDENT_LAST:
      buf = _T( "\"Last!!!\" likely an error" );
      break;
      
   default:
      buf = _T(" Unknown, likely an error" );
      break;
   }
}
