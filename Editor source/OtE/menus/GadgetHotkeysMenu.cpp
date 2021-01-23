// On the Edge Editor
// 
// Copyright © 2004-2021 Jukka Tykkyläinen
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
// and associated documentation files (the "Software"), to deal in the Software without 
// restriction, including without limitation the rights to use, copy, modify, merge, publish, 
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or 
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "GadgetHotkeysMenu.h"
#include "..\\OnTheEdge.h"

#include <sstream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGadgetHotkeysMenu::CGadgetHotkeysMenu(IMenuManager *mgr)
: CInputSettingsMenu(MEDU_ID_GADGET_HOTKEYS, mgr, MENU_ID_CONTROLS),
m_currentControlCode(NULL)
{
}

CGadgetHotkeysMenu::~CGadgetHotkeysMenu()
{
}

void CGadgetHotkeysMenu::CustomInit()
{
	m_gadgetHotkeys = CGameSettings::GetSettingsObject()->GetGadgetShortcuts();

	UpdateKeyCaptions();
}

void CGadgetHotkeysMenu::UpdateKeyCaptions()
{
	UpdateControl(GH_HOTKEY1, "Hotkey 1: ", m_gadgetHotkeys[0]);
	UpdateControl(GH_HOTKEY2, "Hotkey 2: ", m_gadgetHotkeys[1]);
	UpdateControl(GH_HOTKEY3, "Hotkey 3: ", m_gadgetHotkeys[2]);
	UpdateControl(GH_HOTKEY4, "Hotkey 4: ", m_gadgetHotkeys[3]);
	UpdateControl(GH_HOTKEY5, "Hotkey 5: ", m_gadgetHotkeys[4]);
}

void CGadgetHotkeysMenu::UpdateControl(GADGET_HOTKEYS_MENU_ELEMENTS id, std::string text, BYTE code)
{
	IGUIElement *e;
	e = m_window->GetChild(id);
	if(e)
	{
		text += CGameSettings::GetSettingsObject()->GetControlSymbolOrCode(code);
		e->SetCaption(text.c_str());
	}
}

void CGadgetHotkeysMenu::SetKeyCaption(GADGET_HOTKEYS_MENU_ELEMENTS id, std::string text)
{
	IGUIElement *e;
	e = m_window->GetChild(id);
	if(e)
		e->SetCaption(text.c_str());
}

bool CGadgetHotkeysMenu::OnEvent(SEvent event)
{
	if(CFlowMenu::OnEvent(event))
		return true;

	IGUIElement *e = (IGUIElement*)event.guiEvent.element;
	switch(event.type)
	{
	case EW_GUI_EVENT:
		switch(event.guiEvent.type)
		{
			case EWGUI_ELEMENT_PRESSED:
				switch(e->GetID())
				{
				case GH_CLOSE:
					CloseFlowMenu();
					return true;

				case GH_HOTKEY1:
					SetKeyCaption((GADGET_HOTKEYS_MENU_ELEMENTS)e->GetID(), "Hotkey 1: ??");
					m_currentControlCode = &m_gadgetHotkeys[0];
					GetUserKey(true, false);
					return true;
				case GH_HOTKEY2:
					SetKeyCaption((GADGET_HOTKEYS_MENU_ELEMENTS)e->GetID(), "Hotkey 2: ??");
					m_currentControlCode = &m_gadgetHotkeys[1];
					GetUserKey(true, false);
					return true;
				case GH_HOTKEY3:
					SetKeyCaption((GADGET_HOTKEYS_MENU_ELEMENTS)e->GetID(), "Hotkey 3: ??");
					m_currentControlCode = &m_gadgetHotkeys[2];
					GetUserKey(true, false);
					return true;
				case GH_HOTKEY4:
					SetKeyCaption((GADGET_HOTKEYS_MENU_ELEMENTS)e->GetID(), "Hotkey 4: ??");
					m_currentControlCode = &m_gadgetHotkeys[3];
					GetUserKey(true, false);
					return true;
				case GH_HOTKEY5:
					SetKeyCaption((GADGET_HOTKEYS_MENU_ELEMENTS)e->GetID(), "Hotkey 5: ??");
					m_currentControlCode = &m_gadgetHotkeys[4];
					GetUserKey(true, false);
					return true;
				}
			break;
		}
	}

	return false;
}

void CGadgetHotkeysMenu::OnClose()
{
}

void CGadgetHotkeysMenu::OnUserKeySet(BYTE keycode, bool keyboard)
{
	if(m_currentControlCode != NULL)
		*m_currentControlCode = keycode;
	UpdateKeyCaptions();
}

void CGadgetHotkeysMenu::ResetFocusSelection()
{
	IGUIElement *e = m_environment->GetFocusElement();
	if(e == NULL)
		return;
	if(e->GetParent() == m_window)
	{
		switch(e->GetID())
		{
		case GH_HOTKEY1:
			SetKeyCodeToDefaultNull(m_gadgetHotkeys[0]);
			break;
		case GH_HOTKEY2:
			SetKeyCodeToDefaultNull(m_gadgetHotkeys[1]);
			break;
		case GH_HOTKEY3:
			SetKeyCodeToDefaultNull(m_gadgetHotkeys[2]);
			break;
		case GH_HOTKEY4:
			SetKeyCodeToDefaultNull(m_gadgetHotkeys[3]);
			break;
		case GH_HOTKEY5:
			SetKeyCodeToDefaultNull(m_gadgetHotkeys[4]);
			break;
		default:
			return;
		}
	}

	UpdateKeyCaptions();
}

void CGadgetHotkeysMenu::SetKeyCodeToDefaultNull(BYTE &code)
{
	code = 255;
}
