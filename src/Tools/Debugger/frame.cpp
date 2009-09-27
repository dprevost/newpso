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

#include "Tools/Debugger/frame.h"
#include "Tools/Debugger/tree.h"
#include "Tools/Debugger/app.h"
#include "Tools/Debugger/richText.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
   EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
   EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
   EVT_SPLITTER_DCLICK( MY_SPLITTER_ID, MyFrame::OnSplitterDclick )
   EVT_SPLITTER_UNSPLIT( MY_SPLITTER_ID, MyFrame::OnUnsplit )
END_EVENT_TABLE()
//

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyFrame::MyFrame( wxWindow       * parent,
                    wxWindowID       id,
                    const wxString & title,
                    const wxPoint  & pos,
                    const wxSize   & size,
                    long             style ) 
   : wxFrame( parent, id, title, pos, size, style )
{
 //  this->SetSizeHints( wxDefaultSize, wxDefaultSize );
   
   m_menubar1 = new wxMenuBar( 0 );

   m_menu1 = new wxMenu();
   m_menu1->Append( wxID_OPEN );
   m_menu1->Append( wxID_EXIT );
   
   m_menubar1->Append( m_menu1, wxT("File") );
   
   m_menu2 = new wxMenu();
   m_menubar1->Append( m_menu2, wxT("Edit") );
   
   m_menu3 = new wxMenu();
   m_menubar1->Append( m_menu3, wxT("View") );
   
   this->SetMenuBar( m_menubar1 );
   
   wxStaticBoxSizer* sbSizer1;
   sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
   
   m_splitter2 = new wxSplitterWindow( this,
                                       MY_SPLITTER_ID,
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0 );

   m_panel1 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
   m_panel1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
   m_panel1->SetMinSize( wxSize( 300, 500 ) );
   
   wxBoxSizer* bSizer3;
   bSizer3 = new wxBoxSizer( wxVERTICAL );
   
   m_treeCtrl5 = new MyTree( m_panel1, MY_TREE_ID );
   bSizer3->Add( m_treeCtrl5, 0, wxEXPAND | wxALL, 5 );
   
   m_panel1->SetSizer( bSizer3 );
   m_panel1->Layout();
   bSizer3->Fit( m_panel1 );
   m_panel2 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
   m_panel2->SetMinSize( wxSize( 300, 500 ) );
   
   wxBoxSizer* bSizer4;
   bSizer4 = new wxBoxSizer( wxVERTICAL );
   
   m_richText2 = new MyRichText( m_panel2, wxID_ANY );
   bSizer4->Add( m_richText2, 1, wxEXPAND | wxALL, 5 );
   
   m_panel2->SetSizer( bSizer4 );
   m_panel2->Layout();
   bSizer4->Fit( m_panel2 );
   m_splitter2->SplitVertically( m_panel1, m_panel2, 658 );
   sbSizer1->Add( m_splitter2, 1, wxEXPAND, 3 );
   
   this->SetSizer( sbSizer1 );
   this->Layout();
   sbSizer1->Fit( this );
   
}

MyFrame::~MyFrame()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyFrame::OnOpen( wxCommandEvent & event )
{
   wxBell();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyFrame::OnQuit( wxCommandEvent & event )
{
   Close(true);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyFrame::OnUnsplit( wxSplitterEvent & event )
{
   int width, height;
   
   GetClientSize( &width, &height);
   fprintf( stderr, "%d %d\n", width, height );
   if ( width > 0 ) {
      m_splitter2->SplitVertically( m_panel1, m_panel2, width/2 );
      fprintf( stderr, "%d \n", width/2 );
   }
   else
      m_splitter2->SplitVertically( m_panel1, m_panel2, 300 );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

