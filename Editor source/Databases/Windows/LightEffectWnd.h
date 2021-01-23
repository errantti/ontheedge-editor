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

#ifndef __CLIGHTEFFECTWND_H__
#define __CLIGHTEFFECTWND_H__

#include <windows.h>
#include <vector>
#include <string>

#include "..\\..\\OtE\\LightEffectManager.h"

/**
 * Handles a window that lets the user edit
 * one light effect's properties
 */
class CLightEffectWnd
{
public:
	CLightEffectWnd();
	~CLightEffectWnd();

	bool OpenWindow(HWND parent, SDynamicPointLightParams &rTemplate, bool bNew);

private:
	void GetLightColor(HWND hWnd, int index);

	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static CLightEffectWnd *m_pointer;

	SDynamicPointLightParams m_template;
	bool m_bModified;
};

#endif // #ifndef __CVESSELOBJECTWND_H__
