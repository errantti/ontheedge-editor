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

#ifndef __CONTROLSMENU_H__
#define __CONTROLSMENU_H__

#include "FlowMenu.h"

enum CONTROLS_MENU_ELEMENTS{
	CTR_CLOSE=100,
	CTR_PLAYER1,
	CTR_PLAYER2,
	CTR_GENERAL_KEYBOARD,
	CTR_GENERAL_PAD,
	CTR_GADGET_HOTKEYS,
	CTR_MOUSE
};

/**
 * Handles options menu window
 */
class CControlsMenu : public CFlowMenu  
{
public:
	CControlsMenu(IMenuManager *mgr);
	virtual ~CControlsMenu();

	void CustomInit();

	void OnClose();
	bool OnEvent(SEvent event);
};

#endif // ifndef __CONTROLSMENU_H__
