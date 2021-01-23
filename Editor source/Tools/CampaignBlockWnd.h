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

#ifndef __CDYNAMICOBJECTWND_H__
#define __CDYNAMICOBJECTWND_H__

#include <windows.h>
#include <vector>
#include <string>

#include "..\\OtE\\Campaign.h"

class CCampaignBlockWnd
{
public:
	CCampaignBlockWnd();
	~CCampaignBlockWnd();

	bool OpenWindow(HWND parent, CCampaign::SCampaignBlock &rTemplate, bool bNew);

private:
	void BrowseScript(HWND hWnd);
	void BrowseLevel(HWND hWnd);

	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static CCampaignBlockWnd *m_pointer;

	CCampaign::SCampaignBlock m_template;
	bool m_bModified;
};

#endif // #ifndef __CDYNAMICOBJECTWND_H__
