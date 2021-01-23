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

#ifndef __CINPUTEJOYSTICK_H__
#define __CINPUTEJOYSTICK_H__

#include "InputEngine.h"

class CInputJoystick  
{
public:
	HRESULT FF_Reset();
	void SetDeadZone(DWORD dwAxis, int iDeadzone);
	void SetAxisRange(DWORD dwAxis, int iMin, int iMax);
	BOOL GetButtonDown(DWORD dwButton);
	BOOL Update();
	void Release();
	CInputJoystick();
	virtual ~CInputJoystick();

	HRESULT Create(CInputEngine::INPUT_DEVICE *pDevice, DWORD dwFlags);

	// returns pointer to the current joystick state
	inline DIJOYSTATE2 *GetState() {return &m_state;}

	// returns number of buttons supported by the device
	inline DWORD GetButtonCount() const {return m_dwButtonCount;}

	// returns true if joystick supports force feedback
	inline BOOL IsForce() const {return m_bIsForce;}

	// returns direct input device interface
	inline LPDIRECTINPUTDEVICE8 GetDevice() {return m_pDevice;}

private:
	DWORD CountButtons();
	LPDIRECTINPUTDEVICE8	m_pDevice;
	DIJOYSTATE2				m_state;
	DWORD					m_dwButtonCount;
	BOOL					m_bIsForce;
};

#endif // #ifndef __CINPUTEJOYSTICK_H__
