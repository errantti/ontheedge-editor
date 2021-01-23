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

#ifndef __LEVELSELECTIONMENU_H__
#define __LEVELSELECTIONMENU_H__

#include "FlowMenu.h"
#include "..\\SaveGameManager.h"

enum LEVELSELECTION_ELEMENT {
	LS_CLOSE=100,
	LS_LEVEL1,
	LS_LEVEL2,
	LS_LEVEL3,
	LS_LEVEL4,
	LS_LEVEL5,
	LS_NEXT,
	LS_PREV
};

/**
 * Handles the level selection menu window (accessed through load-view)
 */
class CLevelSelectionMenu : public CFlowMenu
{
public:
	CLevelSelectionMenu(IMenuManager *mgr);
	virtual ~CLevelSelectionMenu();

	void CustomInit();

	bool OnEvent(SEvent event);
	void OnClose();

private:
	void LoadLevel(int slot);
	void Next();
	void Prev();

	void UpdateAutosaveList();

	CSaveGameManager *m_saveManager;

	std::vector<CSaveGameManager::SSaveGameInfo> *m_autosaves;

	int m_firstLevelIndex;
};

#endif // ifndef __LEVELSELECTIONMENU_H__
