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

#ifndef __CCOMPONENTWND_H__
#define __CCOMPONENTWND_H__

#include <windows.h>
#include <vector>
#include <string>

#include "..\\..\\OtE\\Items\\ItemManager.h"

/**
 * handles a window that let's edit a single
 * gadget component item's information
 */
class CComponentWnd
{
public:
	CComponentWnd();
	~CComponentWnd();

	bool OpenWindow(HWND parent, SGadgetComponentTemplate &rTemplate, bool bNew);

private:
	void SelectGadget(HWND hWnd, int index, int dlgItem);

	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static CComponentWnd *m_pointer;

	SGadgetComponentTemplate m_template;
	bool m_bModified;
};

#endif // #ifndef __CVESSELOBJECTWND_H__
