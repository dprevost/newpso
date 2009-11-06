/* :mode=c++:  - For jedit, previous line for emacs */
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

#ifndef PSO_DBG_NODE_H
#define PSO_DBG_NODE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include <wx/string.h>

class MyListCtrl;
struct psonSessionContext;
struct psonMemObject;
struct psonLinkedList;
struct psocProcessLock;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

class MyNode
{
public:
   MyNode(psonSessionContext * context) : pContext(context) {}
   
   virtual ~MyNode() {}

   virtual void Show( MyListCtrl * listCtrl ) = 0;

   void ShowLinkedList( MyListCtrl            * listCtrl,
                        struct psonLinkedList * list,
                        wxString              & prefix );

   void ShowMemObject( MyListCtrl           * listCtrl,
                       struct psonMemObject * memObject );

   void ShowLock( MyListCtrl             * listCtrl,
                  struct psocProcessLock * lock,
                  wxString               & prefix  );

protected:
   struct psonSessionContext * pContext;
   
   void GetObjectType( wxString & buf, int ident );
   
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
