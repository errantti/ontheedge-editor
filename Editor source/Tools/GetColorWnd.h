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

#ifndef __CGETCOLORWND_H__
#define __CGETCOLORWND_H__

#include <windows.h>
#include <string>
#include <d3dx9.h>

/**
 * Handles a window that let's edit a single color both
 * manually and picking the color value from a palette
 */
class CGetColorWnd
{
public:
	CGetColorWnd();
	~CGetColorWnd();

	bool OpenWindow(HWND parent, D3DCOLOR &rColor);
	bool OpenWindow(HWND parent, D3DXCOLOR &rColor);
	bool OpenWindow(HWND parent, D3DCOLORVALUE &rColor);

private:
	void PickColor(HWND hWnd);

	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static CGetColorWnd *m_pointer;

	D3DXCOLOR m_template;
	bool m_bModified;
};

#endif // #ifndef __CGET4DVECTORWND_H__
