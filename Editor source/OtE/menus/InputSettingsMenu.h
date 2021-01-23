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

#ifndef __INPUTSETTINGSMENU_H__
#define __INPUTSETTINGSMENU_H__

#include "FlowMenu.h"

class CInputSettingsMenu : public CFlowMenu  
{
public:
	CInputSettingsMenu(MENU_ID id, IMenuManager *mgr, MENU_ID upperLevelMenu);
	virtual ~CInputSettingsMenu();

	virtual void Update();

	virtual bool InitializeMenu();

protected:
	virtual void OnUserKeySet(BYTE keycode, bool keyboard) = 0;
	virtual void ResetFocusSelection() = 0;
	void GetUserKey(bool allowKeyboard, bool allowPad);

	bool m_bDelDown;

private:
	bool m_bAllowKeyboard, m_bAllowPad;

	bool m_bWaitingForInput;
};

#endif // ifndef __INPUTSETTINGSMENU_H__
