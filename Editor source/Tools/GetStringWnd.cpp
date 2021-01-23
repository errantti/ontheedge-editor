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

#include "GetStringWnd.h"
#include "..\\resource.h"
#include "..\\Editor.h"

CGetStringWnd *CGetStringWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetStringWnd::CGetStringWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CGetStringWnd::~CGetStringWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CGetStringWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CGetStringWnd *p = CGetStringWnd::m_pointer;
	char szTemp[128];

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_GS_STRING), p->m_template.c_str());
		SetWindowText(hWnd, p->m_caption.c_str());
		break;

	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_GS_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_GS_OK:
			GetWindowText(GetDlgItem(hWnd, IDC_GS_STRING), szTemp, 128);
			p->m_template = szTemp;

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

bool CGetStringWnd::OpenWindow(HWND parent, std::string &rTemplate, std::string caption)
{
	m_template = rTemplate;
	m_caption = caption;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GET_STRING), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
