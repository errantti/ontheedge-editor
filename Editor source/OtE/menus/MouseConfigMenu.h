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

#ifndef __MOUSECONFIGMENU_H__
#define __MOUSECONFIGMENU_H__

#include "InputSettingsMenu.h"
#include "..\\GameSettings.h"

enum MOUSE_CONFIGURATION_MENU_ELEMENTS{
	MC_CLOSE=2500,
	MC_SENSITIVITY,
	MC_ENABLED
};

/**
 * Handles gadget hotkeys menu window
 */
class CMouseConfigMenu : public CInputSettingsMenu  
{
public:
	CMouseConfigMenu(IMenuManager *mgr);
	virtual ~CMouseConfigMenu();

	void CustomInit();

	void OnClose();
	bool OnEvent(SEvent event);

protected:
	void UpdateSensitivity();
	void IncrementSensitivity();
	void ChangeMouseEnabled();

	int m_sensitivityPercent;

	virtual void OnUserKeySet(BYTE keycode, bool keyboard);
	virtual void ResetFocusSelection();

	/*void UpdateKeyCaptions();
	void UpdateControl(GADGET_HOTKEYS_MENU_ELEMENTS id, std::string text, BYTE code);
	void SetKeyCaption(GADGET_HOTKEYS_MENU_ELEMENTS id, std::string text);

	void SetKeyCodeToDefaultNull(BYTE &code);

	BYTE *m_currentControlCode;*/
};

#endif // ifndef __MOUSECONFIGMENU_H__
