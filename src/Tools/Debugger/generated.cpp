///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "generated.h"

///////////////////////////////////////////////////////////////////////////

MyFrame3::MyFrame3( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	m_menubar1 = new wxMenuBar( 0 );
	m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu1, wxID_ANY, wxString( wxT("Open...") ) + wxT('\t') + wxT("Ctrl+O"), wxEmptyString, wxITEM_NORMAL );
	m_menu1->Append( m_menuItem1 );
	
	m_menubar1->Append( m_menu1, wxT("File") );
	
	m_menu2 = new wxMenu();
	m_menubar1->Append( m_menu2, wxT("Edit") );
	
	m_menu3 = new wxMenu();
	m_menubar1->Append( m_menu3, wxT("View") );
	
	this->SetMenuBar( m_menubar1 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_splitter2 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( MyFrame3::m_splitter2OnIdle ), NULL, this );
	m_panel1 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel1->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
	m_panel1->SetMinSize( wxSize( 200,-1 ) );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_treeCtrl5 = new wxTreeCtrl( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer3->Add( m_treeCtrl5, 0, wxALL, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel2->SetMinSize( wxSize( 100,-1 ) );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_richText2 = new wxRichTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0|wxHSCROLL|wxNO_BORDER|wxVSCROLL );
	bSizer4->Add( m_richText2, 1, wxEXPAND | wxALL, 5 );
	
	m_panel2->SetSizer( bSizer4 );
	m_panel2->Layout();
	bSizer4->Fit( m_panel2 );
	m_splitter2->SplitVertically( m_panel1, m_panel2, 658 );
	sbSizer1->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	this->SetSizer( sbSizer1 );
	this->Layout();
	sbSizer1->Fit( this );
	
	// Connect Events
	m_treeCtrl5->Connect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( MyFrame3::Veto ), NULL, this );
	m_treeCtrl5->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( MyFrame3::NodeActivated ), NULL, this );
	m_treeCtrl5->Connect( wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( MyFrame3::NodeExpanded ), NULL, this );
	m_treeCtrl5->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( MyFrame3::NodeChanged ), NULL, this );
}

MyFrame3::~MyFrame3()
{
	// Disconnect Events
	m_treeCtrl5->Disconnect( wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( MyFrame3::Veto ), NULL, this );
	m_treeCtrl5->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( MyFrame3::NodeActivated ), NULL, this );
	m_treeCtrl5->Disconnect( wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( MyFrame3::NodeExpanded ), NULL, this );
	m_treeCtrl5->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( MyFrame3::NodeChanged ), NULL, this );
}
