/* :mode=c++:  - For jedit, previous line for emacs */
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

#ifndef PSO_DBG_TREE_H
#define PSO_DBG_TREE_H

#include <wx/treectrl.h>

class MyListCtrl;
class MyNode;
class MyFolder;
struct psonSessionContext;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

class MyTreeItemData : public wxTreeItemData
{
public:
   MyTreeItemData( MyNode * node );
   
   ~MyTreeItemData();
   
   void Show( MyListCtrl * listCtrl );
   
private:
   MyNode * m_node;
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

class MyTree : public wxTreeCtrl
{
public:
   MyTree( wxWindow * parent, wxWindowID id, struct psonSessionContext * context );

   void SetListCtrl( MyListCtrl * listCtrl ) { m_listCtrl = listCtrl; }
   
   void Populate( wxTreeItemId id, MyFolder * folder );
   
private:

   MyListCtrl * m_listCtrl;
   
   struct psonSessionContext * pContext;

   void PopulateBranch( wxTreeItemId id, MyFolder * folder );
   
   void OnExpanding( wxTreeEvent & event );

   void OnActivated( wxTreeEvent & event );

   void OnSelChanged( wxTreeEvent & event );

   DECLARE_EVENT_TABLE()
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif // PSO_DBG_TREE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
