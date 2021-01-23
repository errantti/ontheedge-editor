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

#include "LevelSelectionMenu.h"
#include "..\\OnTheEdge.h"
#include "MenuStackManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevelSelectionMenu::CLevelSelectionMenu(IMenuManager *mgr)
: CFlowMenu(MENU_ID_LEVEL_SELECTION, mgr, MENU_ID_LOADSAVE), m_saveManager(NULL),
	m_firstLevelIndex(0)
{
}

CLevelSelectionMenu::~CLevelSelectionMenu()
{
}

void CLevelSelectionMenu::CustomInit()
{
	m_saveManager = GetOTE()->GetSaveGameManager();

	IGUIElement *element;

	element = m_window->GetChild(LS_PREV);
	if(element != NULL)
		element->SetVisible(false);

	if(m_saveManager == NULL)
		return;

	m_saveManager->UpdateLevelSavePointList();
	m_autosaves = &m_saveManager->GetAutosaves();

	int numAutosaves = (int)m_autosaves->size();
	if(numAutosaves <= 5)
	{
		element = m_window->GetChild(LS_NEXT);
		if(element != NULL)
			element->SetVisible(false);
	}

	m_firstLevelIndex = 0;

	UpdateAutosaveList();
}

bool CLevelSelectionMenu::OnEvent(SEvent event)
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
			case LS_CLOSE:
				CloseFlowMenu();
				return true;
			case LS_LEVEL1:
				LoadLevel(0);
				return true;
			case LS_LEVEL2:
				LoadLevel(1);
				return true;
			case LS_LEVEL3:
				LoadLevel(2);
				return true;
			case LS_LEVEL4:
				LoadLevel(3);
				return true;
			case LS_LEVEL5:
				LoadLevel(4);
				return true;
			case LS_PREV:
				Prev();
				return true;
			case LS_NEXT:
				Next();
				return true;
			}
			break;
		}
		break;
	}

	return false;
}

void CLevelSelectionMenu::OnClose()
{
}

void CLevelSelectionMenu::UpdateAutosaveList()
{
	if(m_autosaves == NULL)
		return;

	int numAutosaves = (int)m_autosaves->size();

	IGUIElement *element;

	int i=0;
	long id = LS_LEVEL1;
	for(; i < 5 && i + m_firstLevelIndex < numAutosaves; ++i, ++id)
	{
		element = m_window->GetChild(id);
		if(element != NULL)
		{
			element->SetVisible(true);
			element->SetCaption((*m_autosaves)[i + m_firstLevelIndex].saveName);
		}
	}
	// Set the remaining autosave elements invisible
	for(; i < 5; ++i, ++id)
	{
		element = m_window->GetChild(id);
		if(element != NULL)
			element->SetVisible(false);
	}
}

void CLevelSelectionMenu::LoadLevel(int slot)
{
	if(m_saveManager == NULL)
		return;

	if(m_saveManager->LoadGame((*m_autosaves)[m_firstLevelIndex + slot].filename))
		CloseAllMenus();

	/*if(m_saveManager->GetSaveSlots()[slot] == NULL)
	{
		//GetOTE()->GetScreenMessageInterface()->AddMessage("
		return;
	}
	else
	{
		if(m_saveManager->LoadGame(slot))
			CloseAllMenus();
	}*/
}

void CLevelSelectionMenu::Prev()
{
	IGUIElement *element;

	if(m_firstLevelIndex > 0)
	{
		m_firstLevelIndex -= 5;
		if(m_firstLevelIndex < 1)
		{
			m_firstLevelIndex = 0;

			element = m_window->GetChild(LS_PREV);
			if(element != NULL)
				element->SetVisible(false);
		}

		element = m_window->GetChild(LS_NEXT);
		if(element != NULL)
			element->SetVisible(true);
	}

	UpdateAutosaveList();
}

void CLevelSelectionMenu::Next()
{
	IGUIElement *element;

	int numAutosaves = (int)m_autosaves->size();
	if(numAutosaves > m_firstLevelIndex + 5)
	{
		m_firstLevelIndex += 5;

		element = m_window->GetChild(LS_PREV);
		if(element != NULL)
			element->SetVisible(true);

		UpdateAutosaveList();
	}

	if(numAutosaves <= m_firstLevelIndex + 5)
	{
		IGUIElement *element = m_window->GetChild(LS_NEXT);
		if(element != NULL)
			element->SetVisible(false);
	}
}