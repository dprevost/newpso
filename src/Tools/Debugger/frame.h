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

#ifndef PSO_DBG_FRAME_H
#define PSO_DBG_FRAME_H

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/splitter.h>
#include <wx/statbox.h>
#include <wx/frame.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

class MyTree;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

class MyFrame : public wxFrame 
{   
public:

   MyFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

   ~MyFrame();

   void m_splitter2OnIdle( wxIdleEvent& )
   {
      m_splitter2->SetSashPosition( 250 );
      m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MyFrame::m_splitter2OnIdle ), NULL, this );
   }

protected:

   void OnOpen( wxCommandEvent & event );
   void OnQuit( wxCommandEvent & event );
   void OnSplitterDclick( wxSplitterEvent & event ) { event.Veto(); }
   void OnUnsplit( wxSplitterEvent & event );

   wxMenuBar* m_menubar1;
   wxMenu* m_menu1;
   wxMenu* m_menu2;
   wxMenu* m_menu3;
   wxSplitterWindow* m_splitter2;
   wxPanel* m_panel1;
   MyTree * m_treeCtrl5;
   wxPanel* m_panel2;
   wxRichTextCtrl* m_richText2;

   DECLARE_EVENT_TABLE()
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif // PSO_DBG_FRAME_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

