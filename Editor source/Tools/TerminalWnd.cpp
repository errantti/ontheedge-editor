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

#include "TerminalWnd.h"
#include "TerminalCommandWnd.h"
#include "..\\resource.h"
#include "..\\Editor.h"

CTerminalWnd *CTerminalWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTerminalWnd::CTerminalWnd() : m_bModified(FALSE), m_hWnd(NULL)
{
	m_pointer = this;
}

CTerminalWnd::~CTerminalWnd()
{
	m_pointer = NULL;
}

void CTerminalWnd::OpenTerminalCommandProperties(int index)
{
	CTerminalCommandWnd wnd;
	bool bNew = false;
	if(!strcmp(m_template.GetTerminalCommandData()[index].name, ""))
		bNew = true;
	wnd.OpenWindow(m_hWnd, m_template.GetTerminalCommandData()[index], bNew);
	UpdateTerminalCommandNames();
}

void CTerminalWnd::OpenTerminalOnCloseProperties()
{
	CTerminalCommandWnd wnd;
	wnd.OpenWindow(m_hWnd, *m_template.GetOnCloseCommand(), false);
}

void CTerminalWnd::UpdateTerminalCommandNames()
{
	STerminalCommand *temps = m_template.GetTerminalCommandData();

	SetWindowText(GetDlgItem(m_hWnd, IDC_TERMINAL_BUTTON1), temps[0].name);
	SetWindowText(GetDlgItem(m_hWnd, IDC_TERMINAL_BUTTON2), temps[1].name);
	SetWindowText(GetDlgItem(m_hWnd, IDC_TERMINAL_BUTTON3), temps[2].name);
	SetWindowText(GetDlgItem(m_hWnd, IDC_TERMINAL_BUTTON4), temps[3].name);
	SetWindowText(GetDlgItem(m_hWnd, IDC_TERMINAL_BUTTON5), temps[4].name);
	SetWindowText(GetDlgItem(m_hWnd, IDC_TERMINAL_BUTTON6), temps[5].name);
}

/*****
 * Database selection window callback
 *****/
BOOL CALLBACK CTerminalWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CTerminalWnd *p = CTerminalWnd::m_pointer;
	int selection = -1;
	p->m_hWnd = hWnd;
	char szTemp[128];

	STerminalCommand *temps = p->m_template.GetTerminalCommandData();

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->UpdateTerminalCommandNames();

		SetWindowText(GetDlgItem(hWnd, IDC_TERMINAL_NAME), p->m_template.GetName().c_str());
		SetWindowText(GetDlgItem(hWnd, IDC_TERMINAL_WELCOME), p->m_template.GetWelcomeMessage().c_str());
		SetWindowText(GetDlgItem(hWnd, IDC_TERMINAL_DISFUNCTIONAL), p->m_template.GetDisfunctionalMessage().c_str());

		sprintf(szTemp, "%i", p->m_template.GetID());
		SetWindowText(GetDlgItem(hWnd, IDC_TERMINAL_ID), szTemp);

		if(p->m_template.IsFunctional())
			SendMessage(GetDlgItem(hWnd, IDC_TERMINAL_FUNCTIONAL), BM_SETCHECK, 1, 0);

		p->m_bModified = FALSE;
		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			GetWindowText(GetDlgItem(hWnd, IDC_TERMINAL_NAME), szTemp, 63);
			if(strlen(szTemp) < 2)
			{
				::MessageBox(hWnd, "Too short name", "Error", MB_OK);
				break;
			}
			p->m_template.SetName(szTemp);

			GetWindowText(GetDlgItem(hWnd, IDC_TERMINAL_WELCOME), szTemp, 127);
			p->m_template.SetWelcomeMessage(szTemp);

			GetWindowText(GetDlgItem(hWnd, IDC_TERMINAL_DISFUNCTIONAL), szTemp, 127);
			p->m_template.SetDisfunctionalMessage(szTemp);

			if(SendMessage(GetDlgItem(hWnd, IDC_TERMINAL_FUNCTIONAL), BM_GETCHECK, 0, 0))
				p->m_template.SetIsFunctional(true);
			else
				p->m_template.SetIsFunctional(false);

			EndDialog(hWnd, FALSE);
			p->m_bModified = TRUE;
			break;

		case IDCANCEL:
			EndDialog(hWnd, FALSE);
			break;

		case IDC_TERMINAL_BUTTON1:
			p->OpenTerminalCommandProperties(0);
			break;
		case IDC_TERMINAL_BUTTON2:
			p->OpenTerminalCommandProperties(1);
			break;
		case IDC_TERMINAL_BUTTON3:
			p->OpenTerminalCommandProperties(2);
			break;
		case IDC_TERMINAL_BUTTON4:
			p->OpenTerminalCommandProperties(3);
			break;
		case IDC_TERMINAL_BUTTON5:
			p->OpenTerminalCommandProperties(4);
			break;
		case IDC_TERMINAL_BUTTON6:
			p->OpenTerminalCommandProperties(5);
			break;
		case IDC_TERMINAL_ONCLOSE:
			p->OpenTerminalOnCloseProperties();
			break;
		}
		break;
	}

	return FALSE;
}

bool CTerminalWnd::OpenWindow(HWND parent, CTerminal &rTemplate, bool bNew)
{
	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TERMINAL), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
