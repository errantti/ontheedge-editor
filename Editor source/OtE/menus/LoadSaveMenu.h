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

#ifndef __LOADSAVEMENU_H__
#define __LOADSAVEMENU_H__

#include "FlowMenu.h"
#include "..\\SaveGameManager.h"

enum MAIN_LOADSAVE_ELEMENT {
	LSE_CLOSE=100,
	LSE_HEADER,
	LSE_SAVE1,
	LSE_SAVE2,
	LSE_SAVE3,
	LSE_SAVE4,
	LSE_SAVE5,
	LSE_AUTOSAVE,
	LSE_CAMPAIGN
};

/**
 * Handles the load/save menu window
 */
class CLoadSaveMenu : public CFlowMenu  
{
public:
	CLoadSaveMenu(IMenuManager *mgr);
	virtual ~CLoadSaveMenu();

	void CustomInit();

	bool OnEvent(SEvent event);
	void OnClose();

	static void SetIsLoadGameMode(bool value) {m_bLoadGameMode = value;}

private:
	void SaveOrLoad(int slot);

	CSaveGameManager *m_saveManager;

	static bool m_bLoadGameMode;
};

#endif // ifndef __LOADSAVEMENU_H__
