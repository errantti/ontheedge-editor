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

#ifndef __CINPUTENGINE_H__
#define __CINPUTENGINE_H__

#include <dinput.h>
#include <tchar.h>

#include <vector>

#include "IInputReceiver.h"

class CInputEngine  
{
public:
	struct INPUT_DEVICE
	{
		GUID gdDevice;
		TCHAR strName[256];
	};

	void Release();
	HRESULT Create(HWND hWnd);
	CInputEngine();
	virtual ~CInputEngine();

	// return the direct input object
	inline LPDIRECTINPUT8 GetDirectInput() {return m_pDI;}

	// return the window handle
	inline HWND GetWindow() {return m_hWnd;}

	// return pointer to currently initialized
	// input engine
	static CInputEngine *GetInputEngine() {return m_pInputEngine;}

	int GetKeyboards(std::vector<INPUT_DEVICE> *parrDevices);
	int GetMice(std::vector<INPUT_DEVICE> *parrDevices);
	int GetJoysticks(std::vector<INPUT_DEVICE> *parrDevices);

	void AddInputReceiver(IInputReceiver *receiver);
	void RemoveInputReceiver(IInputReceiver *receiver);
	void PriorizeReceiver(IInputReceiver *receiver);

	void UpdateReceivers();

private:
	// EnumDeviceCallback
	// static callback function for enumerating
	// input devices
	static int WINAPI EnumDeviceCallback(LPCDIDEVICEINSTANCE pInst, void *pParam);

	// the Direct Input object
	LPDIRECTINPUT8 m_pDI;

	// window handle
	HWND m_hWnd;

	std::vector<IInputReceiver*> m_inputStack;

	// the static member pointer used to access
	// input engine from anywhere
	static CInputEngine *m_pInputEngine;
};

#endif // #ifndef __CINPUTENGINE_H__
