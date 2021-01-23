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

#include "LoadSaveMenu.h"
#include "..\\OnTheEdge.h"
#include "MenuStackManager.h"

bool CLoadSaveMenu::m_bLoadGameMode = true;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadSaveMenu::CLoadSaveMenu(IMenuManager *mgr)
: CFlowMenu(MENU_ID_LOADSAVE, mgr, MENU_ID_MAIN), m_saveManager(NULL)
{
}

CLoadSaveMenu::~CLoadSaveMenu()
{
}

void CLoadSaveMenu::CustomInit()
{
	m_saveManager = GetOTE()->GetSaveGameManager();

	if(m_bLoadGameMode)
	{
		IGUIElement *header = m_window->GetChild(LSE_HEADER);
		if(header != NULL)
			((CGUILabel*)header)->SetCaption("Load from campaign");
	}

	IGUIElement *element;
	CGUIButton *button;

	CSaveGameManager::SSaveGameInfo **saves = m_saveManager->GetSaveSlots();

	if(m_saveManager == NULL)
		return;

	m_saveManager->UpdateSavedGameList();

	int id, i;
	for(id = LSE_SAVE1, i=0; id <= LSE_SAVE5 && i < NUM_SAVE_SLOTS; ++id, ++i)
	{
		element = m_window->GetChild(id);
		if(element == NULL)
			continue;
		button = (CGUIButton*)element;

		if(saves[i] == NULL)
		{
			button->SetCaption("- unused -");
		}
		else
		{
			button->SetCaption(saves[i]->saveName.c_str());
		}
	}

	if(m_bLoadGameMode)
	{
		element = m_window->GetChild(LSE_CAMPAIGN);
		if(element != NULL)
		{
			element->SetCaption(GetOTE()->GetCampaign()->GetName());
		}

		/*if(!m_saveManager->DoesAutosaveExist())
		{
			element = m_window->GetChild(LSE_AUTOSAVE);
			if(element != NULL)
				element->SetVisible(false);
		}*/
		element = m_window->GetChild(LSE_AUTOSAVE);
		if(element != NULL)
			element->SetVisible(true);
	}
	else
	{
		element = m_window->GetChild(LSE_AUTOSAVE);
		if(element != NULL)
			element->SetVisible(false);
	}
}

bool CLoadSaveMenu::OnEvent(SEvent event)
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
			case LSE_CLOSE:
				CloseFlowMenu();
				return true;
			case LSE_SAVE1:
				SaveOrLoad(0);
				return true;
			case LSE_SAVE2:
				SaveOrLoad(1);
				return true;
			case LSE_SAVE3:
				SaveOrLoad(2);
				return true;
			case LSE_SAVE4:
				SaveOrLoad(3);
				return true;
			case LSE_SAVE5:
				SaveOrLoad(4);
				return true;
			case LSE_AUTOSAVE:
				//SaveOrLoad(-101);
				m_manager->OpenMenu(MENU_ID_LEVEL_SELECTION, false);
				return true;
			}
			break;
		}
		break;
	}

	return false;
}

void CLoadSaveMenu::OnClose()
{
}

void CLoadSaveMenu::SaveOrLoad(int slot)
{
	if(m_saveManager == NULL)
		return;

	if(m_bLoadGameMode)
	{
		if(slot == -101)
		{
			if(m_saveManager->LoadAutosave())
				CloseAllMenus();
			return;
		}

		if(m_saveManager->GetSaveSlots()[slot] == NULL)
		{
			//GetOTE()->GetScreenMessageInterface()->AddMessage("
			return;
		}
		else
		{
			if(m_saveManager->LoadGame(slot))
				CloseAllMenus();
		}
	}
	else
	{
		if(slot == -101)
			return;

		if(m_saveManager->SaveCurrentSituation(slot))
		{
			GetOTE()->AddScreenMessage("Game saved");
			CloseAllMenus();
		}
		else
			GetOTE()->GetScreenMessageInterface()->AddMessage("Failed saving game");
	}
}