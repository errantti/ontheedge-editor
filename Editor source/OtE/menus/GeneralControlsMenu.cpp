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

#include "GeneralControlsMenu.h"
#include "..\\OnTheEdge.h"

#include <sstream>
using namespace std;

bool CGeneralControlsMenu::m_bUseKeyboard = true;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeneralControlsMenu::CGeneralControlsMenu(IMenuManager *mgr)
: CInputSettingsMenu(MENU_ID_GENERAL_CONTROLS , mgr, MENU_ID_CONTROLS),
m_currentControlCode(NULL)
{
}

CGeneralControlsMenu::~CGeneralControlsMenu()
{
}

void CGeneralControlsMenu::CustomInit()
{
	if(m_bUseKeyboard)
		m_controls = CGameSettings::GetSettingsObject()->GetGeneralKeyboardControlSet();
	else
		m_controls = CGameSettings::GetSettingsObject()->GetGeneralPadControlSet();

	IGUIElement *e = m_window->GetChild(GCE_HEADER);
	if(e)
	{
		if(m_bUseKeyboard)
			e->SetCaption("General keyboard controls");
		else
			e->SetCaption("General pad controls");
	}

	if(!m_bUseKeyboard)
	{
		// Remove useless items if in pad menu
		e = m_window->GetChild(GCE_QUICKLOAD);
		if(e)
			e->Remove();

		e = m_window->GetChild(GCE_QUICKSAVE);
		if(e)
			e->Remove();

		e = m_window->GetChild(GCE_TAKE_SCREENSHOT);
		if(e)
			e->Remove();

		e = m_window->GetChild(GCE_DISPLAY_FPS);
		if(e)
			e->Remove();
	}

	UpdateKeyCaptions();
}

void CGeneralControlsMenu::UpdateKeyCaptions()
{
	UpdateControl(GCE_MENU_BACK, "Menu back: ", m_controls.menuBack);
	UpdateControl(GCE_GENERAL_OK, "General Ok: ", m_controls.generalOk);
	UpdateControl(GCE_OPEN_INVENTORY, "Open inventory: ", m_controls.openInventory);
	UpdateControl(GCE_PLAYER_TERMINAL, "Player terminal: ", m_controls.openPlayerTerminal);
	UpdateControl(GCE_TAKE_SCREENSHOT, "Screenshot: ", m_controls.takeScreenshot);
	UpdateControl(GCE_QUICKLOAD, "Quickload: ", m_controls.quickLoad);
	UpdateControl(GCE_QUICKSAVE, "Quicksave: ", m_controls.quickSave);
	UpdateControl(GCE_DISPLAY_FPS, "Display FPS: ", m_controls.displayFPS);
}

void CGeneralControlsMenu::UpdateControl(GENERAL_CONTROLS_MENU_ELEMENTS id, std::string text, BYTE code)
{
	IGUIElement *e;
	e = m_window->GetChild(id);
	if(e)
	{
		if(m_bUseKeyboard)
			text += CGameSettings::GetSettingsObject()->GetControlSymbolOrCode(code);
		else
		{
			stringstream ss;
			ss << (int)code;
			text += ss.str();
		}
		e->SetCaption(text.c_str());
	}
}

void CGeneralControlsMenu::SetKeyCaption(GENERAL_CONTROLS_MENU_ELEMENTS id, std::string text)
{
	IGUIElement *e;
	e = m_window->GetChild(id);
	if(e)
		e->SetCaption(text.c_str());
}

bool CGeneralControlsMenu::OnEvent(SEvent event)
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
				case GCE_CLOSE:
					CloseFlowMenu();
					return true;

				case GCE_MENU_BACK:
					SetKeyCaption((GENERAL_CONTROLS_MENU_ELEMENTS)e->GetID(), "Menu back: ??");
					m_currentControlCode = &m_controls.menuBack;
					GetUserKey(m_bUseKeyboard, !m_bUseKeyboard);
					return true;
				case GCE_GENERAL_OK:
					SetKeyCaption((GENERAL_CONTROLS_MENU_ELEMENTS)e->GetID(), "General Ok: ??");
					m_currentControlCode = &m_controls.generalOk;
					GetUserKey(m_bUseKeyboard, !m_bUseKeyboard);
					return true;
				case GCE_OPEN_INVENTORY:
					SetKeyCaption((GENERAL_CONTROLS_MENU_ELEMENTS)e->GetID(), "Open inventory: ??");
					m_currentControlCode = &m_controls.openInventory;
					GetUserKey(m_bUseKeyboard, !m_bUseKeyboard);
					return true;
				case GCE_PLAYER_TERMINAL:
					SetKeyCaption((GENERAL_CONTROLS_MENU_ELEMENTS)e->GetID(), "Player terminal: ??");
					m_currentControlCode = &m_controls.openPlayerTerminal;
					GetUserKey(m_bUseKeyboard, !m_bUseKeyboard);
					return true;
				case GCE_TAKE_SCREENSHOT:
					SetKeyCaption((GENERAL_CONTROLS_MENU_ELEMENTS)e->GetID(), "Screenshot: ??");
					m_currentControlCode = &m_controls.takeScreenshot;
					GetUserKey(m_bUseKeyboard, !m_bUseKeyboard);
					return true;
				case GCE_QUICKLOAD:
					SetKeyCaption((GENERAL_CONTROLS_MENU_ELEMENTS)e->GetID(), "Quickload: ??");
					m_currentControlCode = &m_controls.quickLoad;
					GetUserKey(m_bUseKeyboard, !m_bUseKeyboard);
					return true;
				case GCE_QUICKSAVE:
					SetKeyCaption((GENERAL_CONTROLS_MENU_ELEMENTS)e->GetID(), "Quicksave: ??");
					m_currentControlCode = &m_controls.quickSave;
					GetUserKey(m_bUseKeyboard, !m_bUseKeyboard);
					return true;
				case GCE_DISPLAY_FPS:
					SetKeyCaption((GENERAL_CONTROLS_MENU_ELEMENTS)e->GetID(), "Display FPS: ??");
					m_currentControlCode = &m_controls.displayFPS;
					GetUserKey(m_bUseKeyboard, !m_bUseKeyboard);
					return true;
				}
			break;
		}
	}

	return false;
}

void CGeneralControlsMenu::OnClose()
{
	if(m_bUseKeyboard)
		CGameSettings::GetSettingsObject()->SetGeneralKeyboardControlSet(m_controls);
	else
		CGameSettings::GetSettingsObject()->SetGeneralPadControlSet(m_controls);
}

void CGeneralControlsMenu::OnUserKeySet(BYTE keycode, bool keyboard)
{
	if(m_currentControlCode != NULL)
		*m_currentControlCode = keycode;
	UpdateKeyCaptions();
}

void CGeneralControlsMenu::ResetFocusSelection()
{
	IGUIElement *e = m_environment->GetFocusElement();
	if(e == NULL)
		return;
	if(e->GetParent() == m_window)
	{
		switch(e->GetID())
		{
		case GCE_MENU_BACK:
			SetKeyCodeToDefaultNull(m_controls.menuBack);
			break;
		case GCE_GENERAL_OK:
			SetKeyCodeToDefaultNull(m_controls.generalOk);
			break;
		case GCE_OPEN_INVENTORY:
			SetKeyCodeToDefaultNull(m_controls.openInventory);
			break;
		case GCE_PLAYER_TERMINAL:
			SetKeyCodeToDefaultNull(m_controls.openPlayerTerminal);
			break;
		case GCE_TAKE_SCREENSHOT:
			SetKeyCodeToDefaultNull(m_controls.takeScreenshot);
			break;
		case GCE_QUICKLOAD:
			SetKeyCodeToDefaultNull(m_controls.quickLoad);
			break;
		case GCE_QUICKSAVE:
			SetKeyCodeToDefaultNull(m_controls.quickSave);
			break;
		case GCE_DISPLAY_FPS:
			SetKeyCodeToDefaultNull(m_controls.displayFPS);
			break;
		default:
			return;
		}
	}

	UpdateKeyCaptions();
}

void CGeneralControlsMenu::SetKeyCodeToDefaultNull(BYTE &code)
{
	if(m_bUseKeyboard)
		code = 255;
	else
		code = 100;
}
