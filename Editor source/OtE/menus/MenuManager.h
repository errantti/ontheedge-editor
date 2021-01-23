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

#ifndef __CMENUMANAGER_H__
#define __CMENUMANAGER_H__

#include "IMenuHandler.h"

class CMenuManager : public IMenuManager
{
public:
	CMenuManager();
	virtual ~CMenuManager();

	void InitializeMenus();

	void AddMenu(IMenuHandler *menu);
	IMenuHandler *GetMenuHandler(MENU_ID id);

	void OpenMenu(MENU_ID id, bool bRemoveOld = TRUE);
	void OpenIndependentMenu(MENU_ID id);
	void CloseMenu();
	void ReportAllMenusClosed();
	void ReportMenuClosed(IMenuHandler *menu);

	virtual MENU_ID GetCurrentMenuID() {return (m_bMenuOpen)?m_currentMenuID:MENU_COUNT;}

	//static CMenuManager *GetManager() {return m_menuManager;}

protected:
	vector<IMenuHandler*> m_menuHandlers;

	IMenuHandler *m_activeMenu;
	MENU_ID m_currentMenuID;

	CGUIEnvironment *m_environment;

	//static CMenuManager *m_menuManager;
};

#endif // #ifndef __CMENUMANAGER_H__
