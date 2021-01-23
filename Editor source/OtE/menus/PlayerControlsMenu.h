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

#ifndef __PLAYERCONTROLSMENU_H__
#define __PLAYERCONTROLSMENU_H__

#include "InputSettingsMenu.h"
#include "..\\Controllers\\Player.h"

enum PLAYER_CONTROLS_MENU_ELEMENTS{
	PCE_CLOSE=100,
	PCE_HEADER,
	PCE_USE_KEYBOARD,
	PCE_ACCELERATE,
	PCE_BRAKE,
	PCE_TURN_LEFT,
	PCE_TURN_RIGHT,
	PCE_GADGET1,
	PCE_GADGET2,
	PCE_BOOST,
	PCE_THRUST_LEFT,
	PCE_THRUST_RIGHT
};

/**
 * Handles options menu window
 */
class CPlayerControlsMenu : public CInputSettingsMenu  
{
public:
	CPlayerControlsMenu(IMenuManager *mgr);
	virtual ~CPlayerControlsMenu();

	void CustomInit();

	void OnClose();
	bool OnEvent(SEvent event);

	static void SetPlayer(int index) {m_playerIndex = index;}

protected:
	virtual void OnUserKeySet(BYTE keycode, bool keyboard);
	virtual void ResetFocusSelection();
	void UpdateKeyCaptions();
	void UpdateControl(PLAYER_CONTROLS_MENU_ELEMENTS id, std::string text, BYTE code);
	void SetKeyCaption(PLAYER_CONTROLS_MENU_ELEMENTS id, std::string text);

	void SetKeyCodeToDefaultNull(BYTE &code);

	SPlayerControlSet m_controls;
	BYTE *m_currentControlCode;

	static int m_playerIndex;
};

#endif // ifndef __PLAYERCONTROLSMENU_H__
