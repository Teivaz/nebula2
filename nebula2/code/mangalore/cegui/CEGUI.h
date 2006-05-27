/***********************************************************************
	filename: 	CEGUI.h
	created:	21/2/2004
	author:		Paul D Turner

	purpose:	Main system include for client code
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#ifndef _CEGUI_h_
#define _CEGUI_h_

// base stuff
#include "cegui/CEGUIBase.h"
#include "cegui/CEGUIString.h"
#include "cegui/CEGUIRect.h"
#include "cegui/CEGUIExceptions.h"
#include "cegui/CEGUITexture.h"
#include "cegui/CEGUIRenderer.h"
#include "cegui/CEGUIImageset.h"
#include "cegui/CEGUIImagesetManager.h"
#include "cegui/CEGUILogger.h"
#include "cegui/CEGUIMouseCursor.h"
#include "cegui/CEGUIFont.h"
#include "cegui/CEGUIFontManager.h"
#include "cegui/CEGUIEventArgs.h"
#include "cegui/CEGUIEvent.h"
#include "cegui/CEGUIEventSet.h"
#include "cegui/CEGUIGlobalEventSet.h"
#include "cegui/CEGUIProperty.h"
#include "cegui/CEGUIPropertySet.h"
#include "cegui/CEGUIWindow.h"
#include "cegui/CEGUIWindowFactory.h"
#include "cegui/CEGUIWindowFactoryManager.h"
#include "cegui/CEGUIWindowManager.h"
#include "cegui/CEGUIWindowRenderer.h"
#include "cegui/CEGUIWindowRendererManager.h"
#include "cegui/CEGUIScheme.h"
#include "cegui/CEGUISchemeManager.h"
#include "cegui/CEGUISystem.h"
#include "cegui/CEGUIScriptModule.h"
#include "cegui/CEGUICoordConverter.h"

// Falagard core system include
#include "cegui/falagard/CEGUIFalWidgetLookManager.h"

// gui elements
#include "cegui/elements/CEGUIButtonBase.h"
#include "cegui/elements/CEGUIPushButton.h"
#include "cegui/elements/CEGUICheckbox.h"
#include "cegui/elements/CEGUIRadioButton.h"
#include "cegui/elements/CEGUITitlebar.h"
#include "cegui/elements/CEGUIFrameWindow.h"
#include "cegui/elements/CEGUIProgressBar.h"
#include "cegui/elements/CEGUIEditbox.h"
#include "cegui/elements/CEGUIThumb.h"
#include "cegui/elements/CEGUISlider.h"
#include "cegui/elements/CEGUIScrollbar.h"
#include "cegui/elements/CEGUIListbox.h"
#include "cegui/elements/CEGUICombobox.h"
#include "cegui/elements/CEGUIListHeader.h"
#include "cegui/elements/CEGUIMultiColumnList.h"
#include "cegui/elements/CEGUIGUISheet.h"
#include "cegui/elements/CEGUIListboxTextItem.h"
#include "cegui/elements/CEGUIComboDropList.h"
#include "cegui/elements/CEGUIMultiLineEditbox.h"
#include "cegui/elements/CEGUITabButton.h"
#include "cegui/elements/CEGUITabPane.h"
#include "cegui/elements/CEGUITabControl.h"
#include "cegui/elements/CEGUISpinner.h"
#include "cegui/elements/CEGUIDragContainer.h"
#include "cegui/elements/CEGUIScrolledContainer.h"
#include "cegui/elements/CEGUIScrollablePane.h"
#include "cegui/elements/CEGUITooltip.h"
#include "cegui/elements/CEGUIItemEntry.h"
#include "cegui/elements/CEGUIMenuItem.h"
#include "cegui/elements/CEGUIItemListBase.h"
#include "cegui/elements/CEGUIMenuBase.h"
#include "cegui/elements/CEGUIMenubar.h"
#include "cegui/elements/CEGUIPopupMenu.h"
#include "cegui/elements/CEGUIScrolledItemListBase.h"
#include "cegui/elements/CEGUIItemListbox.h"

#endif	// end of guard _CEGUI_h_
