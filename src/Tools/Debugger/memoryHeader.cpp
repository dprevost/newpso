/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Tools/Debugger/memoryHeader.h"
#include "Tools/Debugger/listCtrl.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyMemoryHeader::MyMemoryHeader( void * address )
{
   pHeader = (struct psonMemoryHeader *)address;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyMemoryHeader::showHeader( MyListCtrl & listCtrl )
{
   wxString buf;
   wxListItem itemCol;
   int i = 0;
   const char * str;
   
   itemCol.SetText(_T("Column 1"));
   itemCol.SetImage(-1);
   listCtrl.InsertColumn(0, itemCol);

   itemCol.SetText(_T("Column 2"));
//   itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
   listCtrl.InsertColumn(1, itemCol);

   long tmp = listCtrl.InsertItem(i, _T("Memory Version:"), 0);
   listCtrl.SetItemData(tmp, i);
   buf.Printf(_T("%d"), pHeader->version );
   listCtrl.SetItem(tmp, 1, buf);
   i++;

   tmp = listCtrl.InsertItem(i, _T("Running Status:"), 0);
   listCtrl.SetItemData(tmp, i);
   if ( pHeader->running )
      listCtrl.SetItem(tmp, 1, _T("On") );
   else
      listCtrl.SetItem(tmp, 1, _T("Off") );
   i++;

   //ptrdiff_t       treeMgrOffset;
   //ptrdiff_t       processMgrOffset;
   //ptrdiff_t       allocatorOffset;
   //ptrdiff_t       dataDefMapOffset;
   //ptrdiff_t       keyDefMapOffset;
   tmp = listCtrl.InsertItem(i, _T("Total Length of Memory (bytes):"), 0);
   listCtrl.SetItemData(tmp, i);
   buf.Printf(_T("%d"), pHeader->totalLength );
   listCtrl.SetItem(tmp, 1, buf);
   i++;
   
//   size_t   sizeofPtr;
//   bool     bigEndian;
//   size_t   blockSize;
//   size_t   alignmentStruct;
//   size_t   alignmentChar;
//   size_t   alignmentInt16;
//   size_t   alignmentInt32;
//   size_t   alignmentInt64;
//   size_t   allocationUnit;
   
   tmp = listCtrl.InsertItem(i, _T("CPU Type:"), 0);
   listCtrl.SetItemData(tmp, i);
//   buf.Empty();
   wxString buf1(pHeader->cpu_type, wxConvUTF8);
//str = (const char *) (pHeader->cpu_type);
  // buf = (wxChar*)str; // << (const char *) (pHeader->cpu_type);
//   buf.Printf(_T("%s"), pHeader->cpu_type );
   listCtrl.SetItem(tmp, 1, buf1 );
   i++;
fprintf( stderr, "%d\n", pHeader->cpu_type[0] );

   tmp = listCtrl.InsertItem(i, _T("C Compiler:"), 0);
   listCtrl.SetItemData(tmp, i);
   buf.Printf(_T("%s"), pHeader->compiler );
   listCtrl.SetItem(tmp, 1, buf );
   i++;

   tmp = listCtrl.InsertItem(i, _T("C++ Compiler:"), 0);
   listCtrl.SetItemData(tmp, i);
   buf.Printf(_T("%s"), pHeader->cxxcompiler );
   listCtrl.SetItem(tmp, 1, buf );
   i++;

//   char     compiler[20];
//   char     cxxcompiler[20];
//   char     compilerVersion[10];
//   char     quasarVersion[10];
//   char     creationTime[30];
   
   // The txstatus of the top folder is the txstatus of this "pseudo" 
   // txitem object (not used in an hash object since the top is alone)
//   psonHashTxItem topHashItem;
   
//   psonTreeNode topTreeNode;

   listCtrl.SetColumnWidth( 0, wxLIST_AUTOSIZE );
   listCtrl.SetColumnWidth( 1, wxLIST_AUTOSIZE );

   fprintf(stderr, "ok --------------\n" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

