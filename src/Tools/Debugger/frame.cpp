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

#include "Tools/Debugger/frame.h"
#include "Tools/Debugger/tree.h"
#include "Tools/Debugger/app.h"
#include "Tools/Debugger/listCtrl.h"
#include "Tools/Debugger/memoryFile.h"
#include "Tools/Debugger/memoryHeader.h"
#include "Tools/Debugger/folder.h"
#include "Common/ErrorHandler.h"
#include <wx/odcombo.h>

//#include "bitmaps/open.xpm"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
   EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
   EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
   EVT_SPLITTER_DCLICK( MY_SPLITTER_ID, MyFrame::OnSplitterDclick )
   EVT_SPLITTER_UNSPLIT( MY_SPLITTER_ID, MyFrame::OnUnsplit )
//   EVT_SIZE( MyFrame::OnSize)
   EVT_COMBOBOX(wxID_ANY,MyFrame::OnComboBoxUpdate)
END_EVENT_TABLE()


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyFrame::MyFrame( wxWindow       * parent,
                  wxWindowID       id,
                  const wxString & title,
                  const wxPoint  & pos,
                  const wxSize   & size,
                  long             style ) 
   : wxFrame( parent, id, title, pos, size, style )
{
   long x, y, sash;
   long min_panel1 = 150, min_panel2 = 300;
   wxArrayString   m_arrItems;
  
    m_arrItems.Add( wxT("Header") );
    m_arrItems.Add( wxT("Objects") );
    m_arrItems.Add( wxT("Data") );
    m_arrItems.Add( wxT("Processes") );
 //  this->SetSizeHints( wxDefaultSize, wxDefaultSize );
   
   psocInitErrorDefs();
   context = new (struct psonSessionContext );// malloc(sizeof

   m_config = new wxConfig( _T("psodbg"),
                            _T("Photon Software"),
                            wxEmptyString,
                            wxEmptyString,
                            wxCONFIG_USE_LOCAL_FILE );
   if ( ! m_config->Read( _T("x_size"), &x, 700L ) )
      m_config->Write( _T("x_size"), x );
   if ( ! m_config->Read( _T("y_size"), &y, 500L ) )
      m_config->Write( _T("y_size"), y );
   if ( ! m_config->Read( _T("sash_pos"), &sash, 200L ) )
      m_config->Write( _T("sash_pos"), sash );

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

   wxBoxSizer* sbSizer1;
   sbSizer1 = new wxBoxSizer( wxVERTICAL );
   
   m_splitter = new wxSplitterWindow( this,
                                      MY_SPLITTER_ID,
                                      wxDefaultPosition,
                                      wxDefaultSize,
                                      0 );

   m_panel1 = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
   m_panel1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
   m_panel1->SetMinSize( wxSize( min_panel1, 400 ) );
   
   wxBoxSizer* bSizer3;
   bSizer3 = new wxBoxSizer( wxVERTICAL );
   
   m_treeCtrl = new MyTree( m_panel1, MY_TREE_ID, context );
   bSizer3->Add( m_treeCtrl, 0, wxEXPAND | wxALL, 5 );
   
   m_panel1->SetSizer( bSizer3 );
   m_panel1->Layout();
   bSizer3->Fit( m_panel1 );
   m_panel2 = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
   m_panel2->SetMinSize( wxSize( min_panel2, 400 ) );
   
   wxBoxSizer* bSizer4;
   bSizer4 = new wxBoxSizer( wxVERTICAL );
   
   m_listCtrl = new MyListCtrl( m_panel2, wxID_ANY );
   bSizer4->Add( m_listCtrl, 1, wxEXPAND | wxALL, 0 );
   
   m_panel2->SetSizer( bSizer4 );
   m_panel2->Layout();
   bSizer4->Fit( m_panel2 );
   m_splitter->SplitVertically( m_panel1, m_panel2, 200 );
   
////////////////////
   wxPanel * top_panel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );

   wxOwnerDrawnComboBox * combo =
      new wxOwnerDrawnComboBox( top_panel,
                                wxID_ANY,
                                wxEmptyString,
                                wxDefaultPosition,
                                wxDefaultSize,
                                m_arrItems,
                                wxCB_READONLY|wxCC_STD_BUTTON );
   wxBoxSizer* top_sizer = new wxBoxSizer( wxHORIZONTAL );

   top_sizer->Add( combo, 0, wxALL, 5 );
   top_panel->SetSize( -1 , 200 );
   top_panel->SetSizer( top_sizer );
   top_panel->Layout();
   top_sizer->Fit( top_panel );
   combo->SetValue( wxT("Header") );
   combo->SetBackgroundColour( top_panel->GetBackgroundColour() );

   sbSizer1->Add( top_panel, 0, wxALL, 0 );
   sbSizer1->Add( m_splitter, 1, wxEXPAND | wxALL, 0 );
   
   this->SetSizer( sbSizer1 );
   this->Layout();
   sbSizer1->Fit( this );

   SetSize( x, y );
   m_splitter->SetSashPosition(sash);

   m_treeCtrl->SetListCtrl( m_listCtrl );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyFrame::~MyFrame()
{
   int width, height, sash;
   
   GetSize( &width, &height );

   sash = m_splitter->GetSashPosition();

   m_config->Write( _T("x_size"), width );
   m_config->Write( _T("y_size"), height );
   m_config->Write( _T("sash_pos"), sash );
   
   delete m_config;
   psocFiniErrorDefs();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyFrame::OnComboBoxUpdate( wxCommandEvent & WXUNUSED(event) )
{
// GetValue() 
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyFrame::OnOpen( wxCommandEvent & WXUNUSED(event) )
{
   int errcode;
   void * pAddr;
   
   wxString filename = wxFileSelector( wxT("Select memory file"),
                                       wxT("/tmp/photon_001"),
                                       wxT(""),
                                       wxT(""),
                                       wxT("Photon mem files (*.pso)|*.pso|All files|*.*") );
   if ( !filename )
      return;

   m_memFile = new MyMemoryFile( (const char *)filename.char_str() );
   
   errcode = m_memFile->Open();
   
   m_header = new MyMemoryHeader( m_memFile->BaseAddress() );

   context->pBaseAddress = (unsigned char *) m_memFile->BaseAddress();
   m_header->showHeader( *m_listCtrl );

   pAddr = m_header->GetTopFolder();
   MyFolder * folder = new MyFolder( pAddr, context );
   
   MyTreeItemData *item = new MyTreeItemData(folder);
   
   wxTreeItemId id = m_treeCtrl->AddRoot( _T("/"), -1, -1, item );
   m_treeCtrl->Populate( id, folder );

   //   wxLogError(_T("Couldn't load image from '%s'."), filename.c_str());
   fprintf( stderr, "filename = %s %d %p \n", (char *)filename.char_str(), 
      errcode, m_memFile->BaseAddress() );
     
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
      m_splitter->SplitVertically( m_panel1, m_panel2, width/2 );
      fprintf( stderr, "%d \n", width/2 );
   }
   else
      m_splitter->SplitVertically( m_panel1, m_panel2, 300 );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

