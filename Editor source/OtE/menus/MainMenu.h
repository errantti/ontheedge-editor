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

#ifndef __CMAINMENU_H__
#define __CMAINMENU_H__

#include "IMenuHandler.h"

enum MAIN_MENU_ELEMENT {
	MME_BG=10,
	MME_SINGLE,
	MME_MULTI,
	MME_LOAD,
	MME_SAVE,
	MME_OPTIONS,	
	MME_CREDITS,
	MME_EXIT,
	MME_NOTIFICATION,
	MME_COUNT
};

class CMainMenu : public IMenuHandler
{
public:
	CMainMenu(IMenuManager *mgr);
	virtual ~CMainMenu();

	virtual bool InitializeMenu();
	virtual void OnClose();

	virtual bool OnEvent(SEvent event);

	virtual void Update();

private:
	void InitiateMultiplayer();
	void LoadGame();
	void SaveGame();
	void NewGame();
	void CloseOrExit();
	void ReturnToGame();

	void OnFocus();

	virtual bool OnMenuBackPressed();

	CGUIWindow *m_logoWindow;
};

#endif // #ifndef __CMAINMENU_H__
