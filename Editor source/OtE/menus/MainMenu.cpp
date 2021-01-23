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

// MainMenu.cpp: implementation of the CMainMenu class.
//
//////////////////////////////////////////////////////////////////////

// 24.5.05 - Jukka: updated graphical improvements made by Ismo and altered
//		functionalities

#include "MainMenu.h"
#include "..\\OnTheEdge.h"
#include "MenuStackManager.h"
#include "LoadSaveMenu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainMenu::CMainMenu(IMenuManager *mgr)
: IMenuHandler(MENU_ID_MAIN, mgr), m_logoWindow(NULL)
{
	m_bRequiresUpdate = TRUE;
}

CMainMenu::~CMainMenu()
{
}

bool CMainMenu::OnEvent(SEvent event)
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
			OnClose();
			return true;

		case EWGUI_ELEMENT_FOCUS:
			//if(CMenuStackManager::GetManager()
			OnFocus();
			return true;

		case EWGUI_WINDOW_ANIMATION_SEQUENCE_FINISHED:
			m_window->TabSelect(true);
			return true;

		/*case EWGUI_ELEMENT_HOVERED:
			if(e->IsTabStop())
				PlayHoverSound();
			break;*/

		case EWGUI_ELEMENT_PRESSED:
			if(!e->GetVisible())
				break;
			//PlayActivationSound();
			switch(e->GetID())
			{
			case MME_SINGLE:
				//m_manager->OpenMenu(MENU_SINGLEPLAYER, true);
				if(GetOTE()->IsGameRunning())
					ReturnToGame();
				else
				{
					//NewGame();
					m_manager->OpenMenu(MENU_ID_CAMPAIGN_SELECTION, true);
				}
				return true;
			case MME_MULTI:
				//m_manager->OpenMenu(MENU_MULTIPLAYER_SELECTION,true);
				InitiateMultiplayer();
				return true;
			case MME_LOAD:
				CLoadSaveMenu::SetIsLoadGameMode(true);
				m_manager->OpenMenu(MENU_ID_LOADSAVE,true);
				return true;
			case MME_SAVE:
				CLoadSaveMenu::SetIsLoadGameMode(false);
				m_manager->OpenMenu(MENU_ID_LOADSAVE,true);
				return true;
			case MME_OPTIONS:
				m_manager->OpenMenu(MENU_ID_OPTIONS, true);
				return true;
			case MME_CREDITS:
				//SendCloseMessage();
				m_manager->OpenMenu(MENU_ID_CREDITS, true);
				return true;

			case MME_EXIT:
				CloseOrExit();
				return true;
			}
			break;
		}
		break;
	}

	return false;
}

bool CMainMenu::InitializeMenu()
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();
	CGUIWindow *win = m_window;
	if(win == NULL)
		return false;

	CResourceManager::GetManager().RestoreDefaultColorkey();

	GetOTE()->GetMenuDatabase()->ApplyMenuData(win, MENU_ID_MAIN, this);
	GetOTE()->PauseGame();

	// Create separate logo window
	m_logoWindow = (CGUIWindow*)m_environment->AddWindow(NULL, rect<int>(0, 0, 1, 1), MENU_ID_LOGO, NULL);
	GetOTE()->GetMenuDatabase()->ApplyMenuData(m_logoWindow, MENU_ID_LOGO, NULL);

	CMenuStackManager &stack = CMenuStackManager::GetManager();
	stack.InstantClearStack();
	stack.AddFlowMenu(m_logoWindow, false);
	stack.AddFlowMenu(win, false);

	IGUIElement *save = win->GetChild(MME_SAVE);
	if(save != NULL)
	{
		if(!GetOTE()->IsGameRunning() || GetOTE()->IsMultiplayer())
		{
			save->SetVisible(false);
			save->SetEnabled(false);
		}
	}

	if(GetOTE()->IsGameRunning())
	{
		IGUIElement *exitButton = win->GetChild(MME_EXIT);
		if(exitButton != NULL)
			exitButton->SetCaption("Close current game");
		IGUIElement *newGame = win->GetChild(MME_SINGLE);
		if(newGame != NULL)
			newGame->SetCaption("Return to game");

		//env->AddLabel(win, rect<int>(150, 350, 80, 100), MME_NOTIFICATION, "Press esc to return to game");
	}
	
	m_environment->SetCursorVisibility(TRUE);
	m_bEscDown = TRUE;

	PlayMusic();

	m_window->TabSelect(true);

	return true;
}

void CMainMenu::OnClose()
{
	if(m_logoWindow != NULL)
		m_logoWindow->Remove();

	m_environment->ForceFocusElement(NULL);

	return;
}

void CMainMenu::InitiateMultiplayer()
{
	std::string msg;

	if(GetOTE()->GetMultiplayerHandler()->GetNumMultiplayerMaps() <= 0)
	{
		msg = "There are no multiplayer maps currently available.";
		GetOTE()->AddScreenMessage(msg);
		return;
	}

	m_manager->CloseMenu();
	GetOTE()->StartMultiplayer();
	m_environment->SetCursorVisibility(FALSE);
}

void CMainMenu::LoadGame()
{
	if(GetOTE()->GetCampaign()->LoadGame("save001.svg"))
		m_manager->CloseMenu();
}

void CMainMenu::SaveGame()
{
	if(GetOTE()->IsGameRunning() == FALSE)
		return;

	GetOTE()->GetCampaign()->SaveGame("save001.svg");
}

void CMainMenu::NewGame()
{
	CMenuStackManager::GetManager().InstantClearStack();
	m_manager->ReportMenuClosed(this);
	GetOTE()->StartCampaign(1);
	m_window->Remove();
	m_logoWindow->Remove();
}

void CMainMenu::OnFocus()
{
	// TODO: this is bugged, CloseFlowMenu is not called
	//if(CMenuStackManager::GetManager().GetTopmostPointer() != m_window)
	//	CMenuStackManager::GetManager().RemoveTopmost(true);
}

void CMainMenu::CloseOrExit()
{
	// If game is running then pressing exit (named Close current game) closes the current level
	if(GetOTE()->IsGameRunning())
	{
		IGUIElement *element = m_window->GetChild(MME_EXIT);
		if(element != NULL)
			element->SetCaption("Exit");

		element = m_window->GetChild(MME_SINGLE);
		if(element != NULL)
			element->SetCaption("Start campaign");

		element = m_window->GetChild(MME_SAVE);
		if(element != NULL)
			element->SetVisible(FALSE);

		GetOTE()->CloseLevel();
	}
	else
		GetDXApp()->Close();
}

void CMainMenu::ReturnToGame()
{
	CMenuStackManager::GetManager().InstantClearStack();
	m_manager->ReportMenuClosed(this);
	m_environment->SetCursorVisibility(FALSE);
	SendCloseMessage();
	GetOTE()->ContinueGame();
}

bool CMainMenu::OnMenuBackPressed()
{
	if(GetOTE()->IsGameRunning())
	{
		//m_manager->CloseMenu();
		ReturnToGame();
		return true;
	}

	return false;
}

void CMainMenu::Update()
{
	UpdateSelection(true);
}