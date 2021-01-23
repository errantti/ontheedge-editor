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

#include "MouseConfigMenu.h"
#include "..\\OnTheEdge.h"

#include <sstream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMouseConfigMenu::CMouseConfigMenu(IMenuManager *mgr)
: CInputSettingsMenu(MENU_ID_MOUSE_CONFIGURATION, mgr, MENU_ID_CONTROLS)
{
}

CMouseConfigMenu::~CMouseConfigMenu()
{
}

void CMouseConfigMenu::CustomInit()
{
	CGameSettings &s = *CGameSettings::GetSettingsObject();

	IGUIElement *e;
	if(!s.GetSetting(GO_MOUSE_ENABLED))
	{
		e = m_window->GetChild(MC_ENABLED);
		if(e)
			e->SetCaption("Mouse: disabled");
	}

	float sen = s.GetSetting(GO_MOUSE_SENSITIVITY);

	// NOTE: mouse sensitivity scaling: 0.2 (0%) <-> 1.2 (100%)
	sen -= 0.2f;
	sen *= 10.f;

	m_sensitivityPercent = (int)sen;
	if(m_sensitivityPercent < 0)
		m_sensitivityPercent = 0;
	if(m_sensitivityPercent > 10)
		m_sensitivityPercent = 10;
	UpdateSensitivity();
}

bool CMouseConfigMenu::OnEvent(SEvent event)
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
				case MC_CLOSE:
					CloseFlowMenu();
					return true;

				case MC_SENSITIVITY:
					IncrementSensitivity();
					return true;
				case MC_ENABLED:
					ChangeMouseEnabled();
					return true;
				}
			break;
		}
	}

	return false;
}

void CMouseConfigMenu::OnClose()
{
}

void CMouseConfigMenu::UpdateSensitivity()
{
	stringstream ss;
	IGUIElement *e = m_window->GetChild(MC_SENSITIVITY);
	ss << (m_sensitivityPercent * 10);
	string str("Sensitivity: ");
	str += ss.str();
	e->SetCaption(str);

	float sens = (float)m_sensitivityPercent;

	// NOTE: mouse sensitivity scaling: 0.2 (0%) <-> 1.2 (100%)
	sens *= 0.10f;
	sens += 0.2f;

	CGameSettings::GetSettingsObject()->SetSetting(GO_MOUSE_SENSITIVITY, sens);
}

void CMouseConfigMenu::IncrementSensitivity()
{
	m_sensitivityPercent++;
	if(m_sensitivityPercent > 10)
		m_sensitivityPercent = 0;
	UpdateSensitivity();
}

void CMouseConfigMenu::ChangeMouseEnabled()
{
	CGameSettings &s = *CGameSettings::GetSettingsObject();
	SGeneralControlSet general;
	SPlayerControlSet pl1;
	IGUIElement *e = m_window->GetChild(MC_ENABLED);
	if(s.GetSetting(GO_MOUSE_ENABLED))
	{
		s.SetSetting(GO_MOUSE_ENABLED, false);
		if(e)
			e->SetCaption("Mouse: Disabled");
		GetOTE()->AddScreenMessage("Mouse controlling disabled");
		GetOTE()->AddScreenMessage("Default keyboard only controls set");
		general.FillDefaultKeyboard();
		pl1.FillDefaulKeyboard1();
		s.FillDefaultKeyboardGadgetHotkeys();
		GetOTE()->GetMouse()->ResetMouseState();
	}
	else
	{
		s.SetSetting(GO_MOUSE_ENABLED, true);
		GetEdgewalkerApp()->GetGUIEnvironment()->SetCursorVisibility(true);
		if(e)
			e->SetCaption("Mouse: Enabled");
		GetOTE()->AddScreenMessage("Mouse controlling enabled");
		GetOTE()->AddScreenMessage("Default mouse and keyboard controls set");
		general.FillDefaultKeyboardMouse();
		pl1.FillDefaultKeybMouse();
		s.FillDefaultKeybMouseGadgetHotkeys();
	}
	s.SetGeneralKeyboardControlSet(general);
	s.SetControlSet(pl1, 0);
	GetOTE()->GetPlayer(0)->SetControlSet(pl1);
}

void CMouseConfigMenu::OnUserKeySet(BYTE keycode, bool keyboard)
{
}

void CMouseConfigMenu::ResetFocusSelection()
{
}