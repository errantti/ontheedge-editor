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

#ifndef __IMENUMANAGER_H__
#define __IMENUMANAGER_H__

#include "..\\Edgewalker\\GUI\\GUIEnvironment.h"


enum MENU_ID {
	MENU_ID_MAIN = 1,
	MENU_ID_CREDITS,
	MENU_ID_OPTIONS,
	MENU_ID_CONTROLS,
	MENU_ID_PLAYER_CONTROLS,
	MENU_ID_GENERAL_CONTROLS,
	MENU_ID_LOADSAVE,
	MENU_ID_INVENTORY,
	MENU_ID_INVENTORY_BOX1,
	MENU_ID_INVENTORY_BOX2,
	MENU_ID_INVENTORY_BOX3,
	MENU_ID_INVENTORY_BOX4,
	MENU_ID_INVENTORY_BOX5,
	MENU_ID_TERMINAL,
	MENU_ID_DIALOGUE,
	MENU_ID_LOGO,
	MEDU_ID_GADGET_HOTKEYS,
	MENU_ID_ASSISTANT_VIEW,
	MENU_ID_MOUSE_CONFIGURATION,
	MENU_ID_LEVEL_SELECTION,
	MENU_ID_CAMPAIGN_SELECTION,
	MENU_COUNT
};

class IMenuHandler;

class IMenuManager
{
public:
	//virtual void SetEnvironment(CGUIEnvironment *env) {m_environment = env;}
	virtual void InitializeMenus() = 0;

	// OpenMenu
	// Loads menu controls and sets the target menu active (if found)
	// /param bRemoveOld - if true, the current menu (before loading the new)
	// is removed completely. If false the old menu will be left visible
	// but unresponsive
	virtual void OpenMenu(MENU_ID id, bool bRemoveOld = TRUE) = 0;

	// Closes the current menu - removes all elements belonging to the current
	// menu from the gui environment
	virtual void CloseMenu() = 0;

	virtual void ReportMenuClosed(IMenuHandler *menu) = 0;

	// returns the menu manager object
	static IMenuManager *GetManager() {return m_menuManager;}

	virtual MENU_ID GetCurrentMenuID() = 0;

	virtual bool IsMenuOpen() {return m_bMenuOpen;}

protected:
	static IMenuManager *m_menuManager;
	CGUIEnvironment *m_environment;

	bool m_bMenuOpen;
};

#endif // #ifndef __IMENUMANAGER_H__