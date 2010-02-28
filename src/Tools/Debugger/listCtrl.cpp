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

#include "Tools/Debugger/listCtrl.h"
#include "Tools/Debugger/app.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)
//   EVT_SPLITTER_DCLICK( MY_SPLITTER_ID, MyFrame::OnSplitterDclick )
//   EVT_SPLITTER_UNSPLIT( MY_SPLITTER_ID, MyFrame::OnUnsplit )
END_EVENT_TABLE()
//

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyListCtrl::MyListCtrl( wxWindow   * parent,
                        wxWindowID   id )
   : wxListCtrl( parent, 
                 id,
                 wxDefaultPosition,
                 wxDefaultSize,
                 wxLC_REPORT|wxLC_NO_HEADER )
{
#if 0
    * wxLC_LIST:
      Multicolumn list view, with optional small icons. Columns are computed automatically, i.e. you dont set columns as in wxLC_REPORT. In other words, the list wraps, unlike a wxListBox.
    * wxLC_REPORT:
      Single or multicolumn report view, with optional header.
    * wxLC_VIRTUAL:
      The application provides items text on demand. May only be used with wxLC_REPORT.
    * wxLC_ICON:
      Large icon view, with optional labels.
    * wxLC_SMALL_ICON:
      Small icon view, with optional labels.
    * wxLC_ALIGN_TOP:
      Icons align to the top. Win32 default, Win32 only.
    * wxLC_ALIGN_LEFT:
      Icons align to the left.
    * wxLC_AUTOARRANGE:
      Icons arrange themselves. Win32 only.
    * wxLC_EDIT_LABELS:
      Labels are editable: the application will be notified when editing starts.
    * wxLC_NO_HEADER:
      No header in report mode.
    * wxLC_SINGLE_SEL:
      Single selection (default is multiple).
    * wxLC_SORT_ASCENDING:
      Sort in ascending order. (You must still supply a comparison callback in wxListCtrl::SortItems.)
    * wxLC_SORT_DESCENDING:
      Sort in descending order. (You must still supply a comparison callback in wxListCtrl::SortItems.)
    * wxLC_HRULES:
      Draws light horizontal rules between rows in report mode.
    * wxLC_VRULES:
      Draws light vertical rules between columns in report mode.

   wxListCtrl (wxWindow *parent,
      wxWindowID id,
      const wxPoint &pos=wxDefaultPosition,
      const wxSize &size=wxDefaultSize,
      long style=wxLC_ICON, 
      const wxValidator &validator=wxDefaultValidator,
      const wxString &name=wxListCtrlNameStr)
#endif
}

MyListCtrl::~MyListCtrl()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

//void MyFrame::OnOpen( wxCommandEvent & event )
//{
//   wxBell();
//}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

