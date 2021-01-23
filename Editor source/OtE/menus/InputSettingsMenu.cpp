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

#include "InputSettingsMenu.h"
#include "..\\Edgewalker\\Edgewalker.h"
#include "..\\GameSettings.h"

CInputSettingsMenu::CInputSettingsMenu(MENU_ID id, IMenuManager *mgr, MENU_ID upperLevelMenu)
: CFlowMenu(id, mgr, upperLevelMenu), m_bWaitingForInput(false), m_bDelDown(false)
{
}

CInputSettingsMenu::~CInputSettingsMenu()
{
}

bool CInputSettingsMenu::InitializeMenu()
{
	m_bWaitingForInput = false;
	return CFlowMenu::InitializeMenu();
}

void CInputSettingsMenu::Update()
{
	CInputKeyboard *k = GetEdgewalkerApp()->GetKeyboard();
	CInputJoystick *j = GetEdgewalkerApp()->GetJoystick();

	CGameSettings *s = CGameSettings::GetSettingsObject();

	BYTE i;

	if(m_bWaitingForInput)
	{
		if(s->IsGeneralOkDown() && m_bEnterDown)
			return;

		if(!s->IsGeneralOkDown())
			m_bEnterDown = FALSE;

		if(m_bAllowPad)
		{
			BYTE *pad = j->GetState()->rgbButtons;
			for(i=0;i<128;++i)
				if(pad[i] != 0)
				{
					OnUserKeySet(i, false);
					m_bEnterDown = true;
					m_bEscDown = true;
					m_bUpDown = true;
					m_bDownDown = true;
					m_bWaitingForInput = false;
					return;
				}
		}

		if(m_bAllowKeyboard)
		{
			for(i=0;i<255;++i)
				if(k->GetButtonDown(i))
				{
					OnUserKeySet(i, true);
					m_bEnterDown = true;
					m_bEscDown = true;
					m_bWaitingForInput = false;
					m_bUpDown = true;
					m_bDownDown = true;
					return;
				}
		}
	}
	else
	{
		if(k->GetButtonDown(DIK_DELETE) && !m_bDelDown)
		{
			m_bDelDown = true;
			ResetFocusSelection();
		}
		if(!k->GetButtonDown(DIK_DELETE))
			m_bDelDown = false;

		CFlowMenu::Update();
	}
}

void CInputSettingsMenu::GetUserKey(bool allowKeyboard, bool allowPad)
{
	if(!allowKeyboard && !allowPad)
		return;
	m_bWaitingForInput = true;
	m_bAllowKeyboard = allowKeyboard;
	m_bAllowPad = allowPad;
}