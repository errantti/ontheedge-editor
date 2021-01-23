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

#include "MenuManager.h"

#include "MainMenu.h"
#include "CreditsMenu.h"
#include "LoadSaveMenu.h"
#include "OptionsMenu.h"
#include "ControlsMenu.h"
#include "PlayerControlsMenu.h"
#include "GeneralControlsMenu.h"
#include "GadgetHotkeysMenu.h"
#include "InventoryMenu.h"
#include "TerminalView.h"
#include "DialogueWindow.h"
#include "MenuStackManager.h"
#include "AssistantView.h"
#include "MouseConfigMenu.h"
#include "LevelSelectionMenu.h"
#include "CampaignSelectionMenu.h"

IMenuManager *IMenuManager::m_menuManager = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuManager::CMenuManager() : m_activeMenu(NULL), m_environment(NULL)
{
	m_bMenuOpen = FALSE;
	if(m_menuManager == NULL)
		m_menuManager = this;
}

CMenuManager::~CMenuManager()
{
	vector<IMenuHandler*>::iterator it = m_menuHandlers.begin();
	for(; it != m_menuHandlers.end(); ++it)
		if(*it)
			delete (*it);
	if(m_menuManager == this)
		m_menuManager = NULL;
}

void CMenuManager::InitializeMenus()
{
	CMainMenu *main = new CMainMenu(this);
	AddMenu(main);
	CCreditsMenu *cre = new CCreditsMenu(this);
	AddMenu(cre);
	CLoadSaveMenu *lsm = new CLoadSaveMenu(this);
	AddMenu(lsm);
	COptionsMenu *options = new COptionsMenu(this);
	AddMenu(options);
	CControlsMenu *controls = new CControlsMenu(this);
	AddMenu(controls);
	CPlayerControlsMenu *plcontrols = new CPlayerControlsMenu(this);
	AddMenu(plcontrols);
	CGeneralControlsMenu *generalControls = new CGeneralControlsMenu(this);
	AddMenu(generalControls);
	CGadgetHotkeysMenu *gadgetHotkeys = new CGadgetHotkeysMenu(this);
	AddMenu(gadgetHotkeys);
	CInventoryMenu *inv = new CInventoryMenu(this);
	AddMenu(inv);
	CTerminalView *tv = new CTerminalView(this);
	AddMenu(tv);
	CDialogueWindow *dw = new CDialogueWindow(this);
	AddMenu(dw);
	CAssistantView *aw = new CAssistantView(this);
	AddMenu(aw);
	CMouseConfigMenu *mc = new CMouseConfigMenu(this);
	AddMenu(mc);
	CLevelSelectionMenu *ls = new CLevelSelectionMenu(this);
	AddMenu(ls);
	CCampaignSelectionMenu *cs = new CCampaignSelectionMenu(this);
	AddMenu(cs);
	m_environment = CGUIEnvironment::GetGUIEnvironment();
}

void CMenuManager::AddMenu(IMenuHandler *menu)
{
	if(menu == NULL)
		return;

	vector<IMenuHandler*>::iterator it = m_menuHandlers.begin();
	for(; it != m_menuHandlers.end(); ++it)
		if(*it == menu)
			return;

	m_menuHandlers.push_back(menu);
}

IMenuHandler *CMenuManager::GetMenuHandler(MENU_ID id)
{
	vector<IMenuHandler*>::iterator it = m_menuHandlers.begin();
	for(; it != m_menuHandlers.end(); ++it)
		if((*it)->GetID() == id)
			return (*it);

	return NULL;
}

void CMenuManager::OpenMenu(MENU_ID id, bool bRemoveOld)
{
	//if(bRemoveOld)
	//	CloseMenu();

	m_environment->ForceHoverElement(NULL);
	m_environment->ForceFocusElement(NULL);

	vector<IMenuHandler*>::iterator it = m_menuHandlers.begin();
	for(; it != m_menuHandlers.end(); ++it)
		if((*it)->GetID() == id)
		{
			if(m_environment)
				m_environment->AddWindow(NULL, rect<int>(0, 0, 512, 512), (long)id, *it);
			m_activeMenu = *it;
			m_bMenuOpen = TRUE;
			m_currentMenuID = id;
			return;
		}
	CPrinter::logError("CMenuManager::OpenMenu: unknown menu id");
}

void CMenuManager::OpenIndependentMenu(MENU_ID id)
{
	m_environment->ForceHoverElement(NULL);
	m_environment->ForceFocusElement(NULL);

	vector<IMenuHandler*>::iterator it = m_menuHandlers.begin();
	for(; it != m_menuHandlers.end(); ++it)
		if((*it)->GetID() == id)
		{
			if(m_environment)
				m_environment->AddWindow(NULL, rect<int>(0, 0, 512, 512), (long)id, *it);
			return;
		}
	CPrinter::logError("CMenuManager::OpenIndependentMenu: unknown menu id");
}

void CMenuManager::CloseMenu()
{
	if(m_activeMenu != NULL)
		m_activeMenu->SendCloseMessage();
	m_activeMenu = NULL;
	m_bMenuOpen = FALSE;
}

void CMenuManager::ReportMenuClosed(IMenuHandler *menu)
{
	if(menu == m_activeMenu)
	{
		m_activeMenu = NULL;
		m_bMenuOpen = false;
	}

	if(CMenuStackManager::GetManager().GetTopmostPointer() != NULL)
		m_bMenuOpen = true;
	else if(m_activeMenu == NULL)
		m_bMenuOpen = false;
}

void CMenuManager::ReportAllMenusClosed()
{
	m_activeMenu = NULL;
	m_bMenuOpen = false;
}