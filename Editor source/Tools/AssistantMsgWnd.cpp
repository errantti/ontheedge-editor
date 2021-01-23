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

#include "AssistantMsgWnd.h"
#include "..\\resource.h"

CAssistantMsgWnd *CAssistantMsgWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAssistantMsgWnd::CAssistantMsgWnd()
{
	m_pointer = this;
}

CAssistantMsgWnd::~CAssistantMsgWnd()
{
	m_pointer = NULL;
}


BOOL CALLBACK CAssistantMsgWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CAssistantMsgWnd *p = CAssistantMsgWnd::m_pointer;
	std::pair<int,std::string> &t = p->m_template;
	char buffer[512];

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_AM_MSG), t.second.c_str());
		sprintf(buffer, "%i", t.first);
		SetWindowText(GetDlgItem(hWnd, IDC_AM_ID), buffer);
		break;

	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case IDOK:
			GetWindowText(GetDlgItem(hWnd, IDC_AM_MSG), buffer, 511);
			t.second = buffer;

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

bool CAssistantMsgWnd::OpenWindow(HWND parent, std::pair<int,std::string> &rTemplate, bool bNew)
{
	if(bNew)
	{
		rTemplate.first = -1;
		rTemplate.second = "";
	}

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ASSISTANTMSG), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
