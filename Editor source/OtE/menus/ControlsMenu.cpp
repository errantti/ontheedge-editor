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

#include "ControlsMenu.h"
#include "..\\OnTheEdge.h"
#include "PlayerControlsMenu.h"
#include "GeneralControlsMenu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CControlsMenu::CControlsMenu(IMenuManager *mgr)
: CFlowMenu(MENU_ID_CONTROLS , mgr, MENU_ID_OPTIONS)
{
}

CControlsMenu::~CControlsMenu()
{
}

void CControlsMenu::CustomInit()
{
}

bool CControlsMenu::OnEvent(SEvent event)
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
				case CTR_CLOSE:
					CloseFlowMenu();
					return true;
				case CTR_PLAYER1:
					CPlayerControlsMenu::SetPlayer(0);
					m_manager->OpenMenu(MENU_ID_PLAYER_CONTROLS, false);
					return true;
				case CTR_PLAYER2:
					CPlayerControlsMenu::SetPlayer(1);
					m_manager->OpenMenu(MENU_ID_PLAYER_CONTROLS, false);
					return true;
				case CTR_MOUSE:
					m_manager->OpenMenu(MENU_ID_MOUSE_CONFIGURATION, false);
					return true;
				case CTR_GENERAL_KEYBOARD:
					CGeneralControlsMenu::SetUseKeyboard(true);
					m_manager->OpenMenu(MENU_ID_GENERAL_CONTROLS, false);
					return true;
				case CTR_GENERAL_PAD:
					CGeneralControlsMenu::SetUseKeyboard(false);
					m_manager->OpenMenu(MENU_ID_GENERAL_CONTROLS, false);
					return true;
				case CTR_GADGET_HOTKEYS:
					m_manager->OpenMenu(MEDU_ID_GADGET_HOTKEYS, false);
					return true;
				}
			break;
		}
	}

	return false;
}

void CControlsMenu::OnClose()
{
}