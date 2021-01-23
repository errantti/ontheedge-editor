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

#include "PlayerControlsMenu.h"
#include "..\\OnTheEdge.h"

#include <sstream>
using namespace std;

int CPlayerControlsMenu::m_playerIndex = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayerControlsMenu::CPlayerControlsMenu(IMenuManager *mgr)
: CInputSettingsMenu(MENU_ID_PLAYER_CONTROLS , mgr, MENU_ID_CONTROLS), m_currentControlCode(NULL)
{
}

CPlayerControlsMenu::~CPlayerControlsMenu()
{
}

void CPlayerControlsMenu::CustomInit()
{
	if(m_playerIndex < 0 || m_playerIndex > 1)
		m_playerIndex = 0;

	m_controls = CGameSettings::GetSettingsObject()->GetControlSet(m_playerIndex);

	IGUIElement *e = m_window->GetChild(PCE_HEADER);
	if(e)
	{
		string s("Player ");
		stringstream ss;
		ss << m_playerIndex+1;
		s += ss.str() + " controls";
		e->SetCaption(s.c_str());
	}

	UpdateKeyCaptions();
}

void CPlayerControlsMenu::UpdateKeyCaptions()
{
	IGUIElement *e;

	CGameSettings &set = *CGameSettings::GetSettingsObject();

	e = m_window->GetChild(PCE_USE_KEYBOARD);
	if(e)
	{
		string s("Use keyboard: ");
		if(m_controls.keyboard)
			s += "true";
		else
			s += "false";
		e->SetCaption(s.c_str());
	}

	UpdateControl(PCE_ACCELERATE, "Accelerate: ", m_controls.forward);
	UpdateControl(PCE_BRAKE, "Brake: ", m_controls.back);
	UpdateControl(PCE_THRUST_LEFT, "Thrust left: ", m_controls.thrustLeft);
	UpdateControl(PCE_THRUST_RIGHT, "Thrust right: ", m_controls.thrustRight);
	UpdateControl(PCE_TURN_LEFT, "Left: ", m_controls.left);
	UpdateControl(PCE_TURN_RIGHT, "Right: ", m_controls.right);
	UpdateControl(PCE_GADGET1, "Weapon: ", m_controls.gadget1);
	UpdateControl(PCE_GADGET2, "2nd Gadget: ", m_controls.gadget2);
	UpdateControl(PCE_BOOST, "Boost: ", m_controls.boost);
}

void CPlayerControlsMenu::UpdateControl(PLAYER_CONTROLS_MENU_ELEMENTS id, std::string text, BYTE code)
{
	IGUIElement *e;
	e = m_window->GetChild(id);
	if(e)
	{
		if(m_controls.keyboard)
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

void CPlayerControlsMenu::SetKeyCaption(PLAYER_CONTROLS_MENU_ELEMENTS id, std::string text)
{
	IGUIElement *e;
	e = m_window->GetChild(id);
	if(e)
		e->SetCaption(text.c_str());
}

bool CPlayerControlsMenu::OnEvent(SEvent event)
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
				case PCE_CLOSE:
					CloseFlowMenu();
					return true;
				case PCE_USE_KEYBOARD:
					m_controls.keyboard = !m_controls.keyboard;
					if(m_controls.keyboard)
					{
						if(m_playerIndex == 0)
						{
							if(CGameSettings::GetSettingsObject()->GetSetting(GO_MOUSE_ENABLED))
							{
								m_controls.FillDefaultKeybMouse();
								GetOTE()->AddScreenMessage("Default keyboard-mouse controls set");
							}
							else
							{
								m_controls.FillDefaulKeyboard1();
								GetOTE()->AddScreenMessage("Default keyboard-only controls set");
							}
						}
						else
							m_controls.FillDefaultKeyboard2();
					}
					else
					{
						m_controls.FillDefaultGamepad();
						GetOTE()->AddScreenMessage("Default gamepad controls set");
					}
					UpdateKeyCaptions();
					return true;

				case PCE_ACCELERATE:
					SetKeyCaption(PCE_ACCELERATE, "Accelerate: ??");
					m_currentControlCode = &m_controls.forward;
					GetUserKey(m_controls.keyboard, !m_controls.keyboard);
					return true;
				case PCE_BRAKE:
					SetKeyCaption(PCE_BRAKE, "Brake: ??");
					m_currentControlCode = &m_controls.back;
					GetUserKey(m_controls.keyboard, !m_controls.keyboard);
					return true;
				case PCE_THRUST_LEFT:
					SetKeyCaption(PCE_THRUST_LEFT, "Thrust left: ??");
					m_currentControlCode = &m_controls.thrustLeft;
					GetUserKey(m_controls.keyboard, !m_controls.keyboard);
					return true;
				case PCE_THRUST_RIGHT:
					SetKeyCaption(PCE_THRUST_RIGHT, "Thrust right: ??");
					m_currentControlCode = &m_controls.thrustRight;
					GetUserKey(m_controls.keyboard, !m_controls.keyboard);
					return true;
				case PCE_TURN_LEFT:
					SetKeyCaption(PCE_TURN_LEFT, "Left: ??");
					m_currentControlCode = &m_controls.left;
					GetUserKey(m_controls.keyboard, !m_controls.keyboard);
					return true;
				case PCE_TURN_RIGHT:
					SetKeyCaption(PCE_TURN_RIGHT, "Right: ??");
					m_currentControlCode = &m_controls.right;
					GetUserKey(m_controls.keyboard, !m_controls.keyboard);
					return true;
				case PCE_GADGET1:
					SetKeyCaption(PCE_GADGET1, "Weapon: ??");
					m_currentControlCode = &m_controls.gadget1;
					GetUserKey(m_controls.keyboard, !m_controls.keyboard);
					return true;
				case PCE_GADGET2:
					SetKeyCaption(PCE_GADGET2, "2nd Gadget: ??");
					m_currentControlCode = &m_controls.gadget2;
					GetUserKey(m_controls.keyboard, !m_controls.keyboard);
					return true;
				case PCE_BOOST:
					SetKeyCaption(PCE_BOOST, "Boost: ??");
					m_currentControlCode = &m_controls.boost;
					GetUserKey(m_controls.keyboard, !m_controls.keyboard);
					return true;
				}
			break;
		}
	}

	return false;
}

void CPlayerControlsMenu::OnClose()
{
	CGameSettings::GetSettingsObject()->SetControlSet(m_controls, m_playerIndex);
	GetOTE()->GetPlayer(m_playerIndex)->SetControlSet(m_controls);
}

void CPlayerControlsMenu::OnUserKeySet(BYTE keycode, bool keyboard)
{
	if(m_currentControlCode != NULL)
		*m_currentControlCode = keycode;
	UpdateKeyCaptions();
}

void CPlayerControlsMenu::ResetFocusSelection()
{
	IGUIElement *e = m_environment->GetFocusElement();
	if(e == NULL)
		return;
	if(e->GetParent() == m_window)
	{
		switch(e->GetID())
		{
		case PCE_ACCELERATE:
			SetKeyCodeToDefaultNull(m_controls.forward);
			break;
		case PCE_BRAKE:
			SetKeyCodeToDefaultNull(m_controls.back);
			break;
		case PCE_THRUST_LEFT:
			SetKeyCodeToDefaultNull(m_controls.thrustLeft);
			break;
		case PCE_THRUST_RIGHT:
			SetKeyCodeToDefaultNull(m_controls.thrustRight);
			break;
		case PCE_TURN_LEFT:
			SetKeyCodeToDefaultNull(m_controls.left);
			break;
		case PCE_TURN_RIGHT:
			SetKeyCodeToDefaultNull(m_controls.right);
			break;
		case PCE_GADGET1:
			SetKeyCodeToDefaultNull(m_controls.gadget1);
			break;
		case PCE_GADGET2:
			SetKeyCodeToDefaultNull(m_controls.gadget2);
			break;
		case PCE_BOOST:
			SetKeyCodeToDefaultNull(m_controls.boost);
			break;
		default:
			return;
		}
	}

	UpdateKeyCaptions();
}

void CPlayerControlsMenu::SetKeyCodeToDefaultNull(BYTE &code)
{
	if(m_controls.keyboard)
		code = 255;
	else
		code = 100;
}