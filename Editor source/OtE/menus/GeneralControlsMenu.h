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

#ifndef __GENERALCONTROLSMENU_H__
#define __GENERALCONTROLSMENU_H__

#include "InputSettingsMenu.h"
#include "..\\GameSettings.h"

enum GENERAL_CONTROLS_MENU_ELEMENTS{
	GCE_CLOSE=100,
	GCE_HEADER,
	GCE_MENU_BACK,
	GCE_GENERAL_OK,
	GCE_OPEN_INVENTORY,
	GCE_PLAYER_TERMINAL,
	GCE_TAKE_SCREENSHOT,
	GCE_QUICKLOAD,
	GCE_QUICKSAVE,
	GCE_DISPLAY_FPS
};

/**
 * Handles options menu window
 */
class CGeneralControlsMenu : public CInputSettingsMenu  
{
public:
	CGeneralControlsMenu(IMenuManager *mgr);
	virtual ~CGeneralControlsMenu();

	void CustomInit();

	void OnClose();
	bool OnEvent(SEvent event);

	static void SetUseKeyboard(bool value) {m_bUseKeyboard = value;}

protected:
	virtual void OnUserKeySet(BYTE keycode, bool keyboard);
	virtual void ResetFocusSelection();

	void UpdateKeyCaptions();
	void UpdateControl(GENERAL_CONTROLS_MENU_ELEMENTS id, std::string text, BYTE code);
	void SetKeyCaption(GENERAL_CONTROLS_MENU_ELEMENTS id, std::string text);

	void SetKeyCodeToDefaultNull(BYTE &code);

	SGeneralControlSet m_controls;
	BYTE *m_currentControlCode;

	static bool m_bUseKeyboard;
};

#endif // ifndef __GENERALCONTROLSMENU_H__
