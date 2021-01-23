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

#include "FlowMenu.h"
#include "..\\OnTheEdge.h"
#include "MenuStackManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlowMenu::CFlowMenu(MENU_ID id, IMenuManager *mgr, MENU_ID upperLevelMenu)
: IMenuHandler(id, mgr), m_bClosing(false), m_upperLevelMenu(upperLevelMenu)
{
	m_bRequiresUpdate = TRUE;
}

CFlowMenu::~CFlowMenu()
{
}

bool CFlowMenu::InitializeMenu()
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();
	CGUIWindow *win = m_window;
	if(win == NULL)
		return false;

	GetOTE()->GetMenuDatabase()->ApplyMenuData(win, m_ID, this);

	CMenuStackManager &stack = CMenuStackManager::GetManager();
	stack.AddFlowMenu(win, true);
	m_bClosing = false;

	CustomInit();

	return true;
}

bool CFlowMenu::OnEvent(SEvent event)
{
	HandleRoutineInput(event);

	IGUIElement *e = (IGUIElement*)event.guiEvent.element;
	switch(event.type)
	{
	case EW_GUI_EVENT:
		switch(event.guiEvent.type)
		{
		case EWGUI_ELEMENT_CREATED:
			m_window = (CGUIWindow*)event.guiEvent.element;
			InitializeMenu();
			return true;
		case EWGUI_ELEMENT_DESTROYED:
			//OnClose();
			return true;

		case EWGUI_WINDOW_ANIMATION_SEQUENCE_FINISHED:
			if(m_bClosing)
			{
				OnClose();
				m_manager->ReportMenuClosed(this);
				((CGUIWindow*)event.guiEvent.element)->Remove();
				return true;
			}
			else
				m_window->TabSelect(true);
			return true;
		}
		break;
	}

	return false;
}

void CFlowMenu::CloseFlowMenu()
{
	CMenuStackManager::GetManager().RemoveTopmost(true);
	m_manager->ReportMenuClosed(this);
	m_bClosing = true;
}

void CFlowMenu::CloseAllMenus()
{
	CMenuStackManager::GetManager().InstantCloseAllStackMenus();
	((CMenuManager*)m_manager)->ReportAllMenusClosed();
	if(GetOTE()->IsGameRunning())
	{
		m_environment->SetCursorVisibility(FALSE);
		GetOTE()->ContinueGame();
	}
}

bool CFlowMenu::OnMenuBackPressed()
{
	CloseFlowMenu();
	return true;
}

void CFlowMenu::Update()
{
	UpdateSelection(true);
}