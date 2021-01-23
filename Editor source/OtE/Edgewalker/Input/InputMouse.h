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

#ifndef __CINPUTMOUSE_H__
#define __CINPUTMOUSE_H__

#include "InputEngine.h"

enum MOUSE_BUTTON_STATE {
	MOUSE_BUTTON_PRESSED=0,
	MOUSE_BUTTON_DOWN,
	MOUSE_BUTTON_RELEASED,
	MOUSE_BUTTON_UP
};

class CInputMouse
{
public:
	BOOL GetButtonDown(DWORD dwButton);
	CInputMouse();
	virtual ~CInputMouse();

	HRESULT Create(CInputEngine::INPUT_DEVICE *pDevice, DWORD dwFlags);
	void Release();
	BOOL Update();

	inline DWORD GetButtonCount() const {return m_dwButtonCount;}
	inline DIMOUSESTATE2 *GetState() {return &m_state;}

	inline bool GetMouseLeftDown() {return m_state.rgbButtons[0] ? TRUE : FALSE;}
	inline bool GetMouseRightDown() {return m_state.rgbButtons[1] ? TRUE : FALSE;}

	inline MOUSE_BUTTON_STATE GetMouseLeftState() {return m_buttonStates[0];}
	inline MOUSE_BUTTON_STATE GetMouseRightState() {return m_buttonStates[1];}
	inline MOUSE_BUTTON_STATE GetMouseMiddleState() {return m_buttonStates[2];}
	inline MOUSE_BUTTON_STATE GetMouseButtonState(int i) {return m_buttonStates[i];}

	inline int GetRelativeX() {return m_state.lX;}
	inline int GetRelativeY() {return m_state.lY;}
	inline int GetRelativeZ() {return m_state.lZ;}

	inline int GetX() {return m_position.x;}
	inline int GetY() {return m_position.y;}

	inline int GetSynchronizedX() {return m_synchronizedPosition.x;}
	inline int GetSynchronizedY() {return m_synchronizedPosition.y;}

	inline POINT GetCursorPosition() {return m_position;}
	void SetCursorPosition(POINT pos) {m_position = pos;}

	void SetSynchronizeWithWindows(bool bValue) {m_bSynchronize = bValue;}

	void ResetMouseState();

private:
	DWORD CountButtons();
	void UpdateButtonStates();
	void InitButtonStates();
	void UpdatePositionFromWindows();

	LPDIRECTINPUTDEVICE8	m_pDevice;
	DIMOUSESTATE2			m_state;
	DWORD					m_dwButtonCount;
	MOUSE_BUTTON_STATE		m_buttonStates[8];

	// Current cursor position
	POINT					m_position;
	POINT					m_synchronizedPosition;		// mouse cursor position in windows

	bool					m_bSynchronize;		// If true, the cursor position is synchronized with windows cursor position
};

#endif // #ifndef __CINPUTMOUSE_H__
