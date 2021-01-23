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

#ifndef __FLOWMENU_H__
#define __FLOWMENU_H__

#include "IMenuHandler.h"

class CFlowMenu : public IMenuHandler  
{
public:
	CFlowMenu(MENU_ID id, IMenuManager *mgr, MENU_ID upperLevelMenu);
	virtual ~CFlowMenu();

	virtual bool OnEvent(SEvent event);
	virtual bool InitializeMenu();

	virtual void Update();

	virtual void CloseFlowMenu();
	virtual void CloseAllMenus();

	virtual void CustomInit() = 0;

protected:
	bool OnMenuBackPressed();

	bool m_bClosing;

	MENU_ID m_upperLevelMenu;
};

#endif // ifndef __CREDITS_H__
