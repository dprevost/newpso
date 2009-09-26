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

#include "Tools/Debugger/richText.h"
#include "Tools/Debugger/app.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_EVENT_TABLE(MyRichText, wxRichTextCtrl)
//   EVT_SPLITTER_DCLICK( MY_SPLITTER_ID, MyFrame::OnSplitterDclick )
//   EVT_SPLITTER_UNSPLIT( MY_SPLITTER_ID, MyFrame::OnUnsplit )
END_EVENT_TABLE()
//

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyRichText::MyRichText( wxWindow   * parent,
                        wxWindowID   id )
   : wxRichTextCtrl( parent, 
                     id,
                     wxEmptyString,
                     wxDefaultPosition,
                     wxDefaultSize,
                     wxHSCROLL|wxNO_BORDER|wxVSCROLL )
{
}

MyRichText::~MyRichText()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

//void MyFrame::OnOpen( wxCommandEvent & event )
//{
//   wxBell();
//}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

