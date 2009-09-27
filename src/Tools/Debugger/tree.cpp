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

#include "Tools/Debugger/tree.h"
#include "Tools/Debugger/app.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_EVENT_TABLE(MyTree, wxTreeCtrl)
  EVT_TREE_ITEM_EXPANDING( MY_TREE_ID, MyTree::OnExpanding)
END_EVENT_TABLE()

// EVT_TREE_BEGIN_LABEL_EDIT( MY_TREE_ID, Veto )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

MyTree::MyTree( wxWindow * parent, wxWindowID id )
   : wxTreeCtrl( parent, id, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void MyTree::OnExpanding( wxTreeEvent & event )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0
wxTR_EDIT_LABELS 	Use this style if you wish the user to be able to edit labels in the tree control.
wxTR_NO_BUTTONS 	For convenience to document that no buttons are to be drawn.
wxTR_HAS_BUTTONS 	Use this style to show + and - buttons to the left of parent items.
wxTR_NO_LINES 	Use this style to hide vertical level connectors.
wxTR_FULL_ROW_HIGHLIGHT 	Use this style to have the background colour and the selection highlight extend over the entire horizontal row of the tree control window. (This flag is ignored under Windows unless you specify wxTR_NO_LINES as well.)
wxTR_LINES_AT_ROOT 	Use this style to show lines between root nodes. Only applicable if wxTR_HIDE_ROOT is set and wxTR_NO_LINES is not set.
wxTR_HIDE_ROOT 	Use this style to suppress the display of the root node, effectively causing the first-level nodes to appear as a series of root nodes.
wxTR_ROW_LINES 	Use this style to draw a contrasting border between displayed rows.
wxTR_HAS_VARIABLE_ROW_HEIGHT 	Use this style to cause row heights to be just big enough to fit the content. If not set, all rows use the largest row height. The default is that this flag is unset. Generic only.
wxTR_SINGLE 	For convenience to document that only one item may be selected at a time. Selecting another item causes the current selection, if any, to be deselected. This is the default.
wxTR_MULTIPLE 	Use this style to allow a range of items to be selected. If a second range is selected, the current range, if any, is deselected.
wxTR_EXTENDED 	Use this style to allow disjoint items to be selected. (Only partially implemented; may not work in all cases.)
wxTR_DEFAULT_STYLE


EVT_TREE_BEGIN_DRAG(id, func) 	Begin dragging with the left mouse button.
EVT_TREE_BEGIN_RDRAG(id, func) 	Begin dragging with the right mouse button.
EVT_TREE_END_DRAG(id, func) 	End dragging with the left or right mouse button.
EVT_TREE_BEGIN_LABEL_EDIT(id, func) 	Begin editing a label. This can be prevented by calling Veto().
EVT_TREE_END_LABEL_EDIT(id, func) 	Finish editing a label. This can be prevented by calling Veto().
EVT_TREE_DELETE_ITEM(id, func) 	Delete an item.
EVT_TREE_GET_INFO(id, func) 	Request information from the application.
EVT_TREE_SET_INFO(id, func) 	Information is being supplied.
EVT_TREE_ITEM_ACTIVATED(id, func) 	The item has been activated, i.e. chosen by double clicking it with mouse or from keyboard
EVT_TREE_ITEM_COLLAPSED(id, func) 	The item has been collapsed.
EVT_TREE_ITEM_COLLAPSING(id, func) 	The item is being collapsed. This can be prevented by calling Veto().
EVT_TREE_ITEM_EXPANDED(id, func) 	The item has been expanded.
EVT_TREE_ITEM_EXPANDING(id, func) 	The item is being expanded. This can be prevented by calling Veto().
EVT_TREE_ITEM_RIGHT_CLICK(id, func) 	The user has clicked the item with the right mouse button.
EVT_TREE_ITEM_MIDDLE_CLICK(id, func) 	The user has clicked the item with the middle mouse button.
EVT_TREE_SEL_CHANGED(id, func) 	Selection has changed.
EVT_TREE_SEL_CHANGING(id, func) 	Selection is changing. This can be prevented by calling Veto().
EVT_TREE_KEY_DOWN(id, func) 	A key has been pressed.
EVT_TREE_ITEM_GETTOOLTIP(id, func) 	The opportunity to set the item tooltip is being given to the application (call wxTreeEvent::SetToolTip). Windows only.
EVT_TREE_ITEM_MENU(id, func) 	The context menu for the selected item has been requested, either by a right click or by using the menu key.
EVT_TREE_STATE_IMAGE_CLICK(id, func) 	The state image has been clicked. Windows only.


#endif

