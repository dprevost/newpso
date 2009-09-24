///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 21 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __generated__
#define __generated__

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

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MyFrame3
///////////////////////////////////////////////////////////////////////////////
class MyFrame3 : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* m_menubar1;
		wxMenu* m_menu1;
		wxMenu* m_menu2;
		wxMenu* m_menu3;
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel1;
		wxTreeCtrl* m_treeCtrl5;
		wxPanel* m_panel2;
		wxRichTextCtrl* m_richText2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void Veto( wxTreeEvent& event ){ event.Skip(); }
		virtual void NodeActivated( wxTreeEvent& event ){ event.Skip(); }
		virtual void NodeExpanded( wxTreeEvent& event ){ event.Skip(); }
		virtual void NodeChanged( wxTreeEvent& event ){ event.Skip(); }
		
	
	public:
		MyFrame3( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~MyFrame3();
		void m_splitter2OnIdle( wxIdleEvent& )
		{
		m_splitter2->SetSashPosition( 658 );
		m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MyFrame3::m_splitter2OnIdle ), NULL, this );
		}
		
	
};

#endif //__generated__
