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

#include "TerminalCommandWnd.h"
#include "..\\resource.h"
#include "..\\Editor.h"
#include "..\\Databases\\DatabaseUtilities.h"

CTerminalCommandWnd *CTerminalCommandWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTerminalCommandWnd::CTerminalCommandWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CTerminalCommandWnd::~CTerminalCommandWnd()
{
	m_pointer = NULL;
}


BOOL CALLBACK CTerminalCommandWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CTerminalCommandWnd *p = CTerminalCommandWnd::m_pointer;
	STerminalCommand &t = p->m_template;
	char buffer[128];

	p->m_hWnd = hWnd;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_TC_NAME), t.name);
		if(t.triggerType == TERMINAL_TRIGGER_SCRIPT)
			SetWindowText(GetDlgItem(hWnd, IDC_TC_SCRIPT), t.script);
		else if(t.triggerType == TERMINAL_TRIGGER_MESSAGE || t.triggerType == TERMINAL_TRIGGER_CLOSEFUNCTION)
			SetWindowText(GetDlgItem(hWnd, IDC_TC_MESSAGE), t.message);

		sprintf(buffer, "%i", t.dialogueID);
		SetWindowText(GetDlgItem(hWnd, IDC_TC_DIALOGUE), buffer);

		if(t.bEnabled)
			SendMessage(GetDlgItem(hWnd, IDC_TC_ENABLED), BM_SETCHECK, 1, 0);
		if(t.bVisible)
			SendMessage(GetDlgItem(hWnd, IDC_TC_VISIBLE), BM_SETCHECK, 1, 0);

		switch(t.triggerType)
		{
		case TERMINAL_TRIGGER_CLOSEFUNCTION:
			SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_SHUTDOWN), BM_SETCHECK, 1, 0);
			break;
		case TERMINAL_TRIGGER_SCRIPT:
			SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_SCRIPT), BM_SETCHECK, 1, 0);
			break;
		case TERMINAL_TRIGGER_MESSAGE:
			SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_MESSAGE), BM_SETCHECK, 1, 0);
			break;
		case TERMINAL_TRIGGER_NONE:
			SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_NONE), BM_SETCHECK, 1, 0);
			break;
		case TERMINAL_TRIGGER_DIALOGUE:
			SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_DIALOGUE), BM_SETCHECK, 1, 0);
			break;
		}
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
			// Get the form information and return success
			GetWindowText(GetDlgItem(hWnd, IDC_TC_NAME), t.name, 31);
			if(strlen(t.name) < 2)
			{
				::MessageBox(hWnd, "Too short name", "Error", MB_OK);
				break;
			}

			GetWindowText(GetDlgItem(hWnd, IDC_TC_DIALOGUE), buffer, 7);
			t.dialogueID = atoi(buffer);

			if(SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_SHUTDOWN), BM_GETCHECK, 0, 0))
				t.triggerType = TERMINAL_TRIGGER_CLOSEFUNCTION;
			else if(SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_SCRIPT), BM_GETCHECK, 0, 0))
				t.triggerType = TERMINAL_TRIGGER_SCRIPT;
			else if(SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_MESSAGE), BM_GETCHECK, 0, 0))
				t.triggerType = TERMINAL_TRIGGER_MESSAGE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_NONE), BM_GETCHECK, 0, 0))
				t.triggerType = TERMINAL_TRIGGER_NONE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_TC_TRIGGER_DIALOGUE), BM_GETCHECK, 0, 0))
				t.triggerType = TERMINAL_TRIGGER_DIALOGUE;

			if(t.triggerType == TERMINAL_TRIGGER_SCRIPT)
				GetWindowText(GetDlgItem(hWnd, IDC_TC_SCRIPT), t.script, 31);
			else if(t.triggerType == TERMINAL_TRIGGER_MESSAGE || t.triggerType == TERMINAL_TRIGGER_CLOSEFUNCTION)
				GetWindowText(GetDlgItem(hWnd, IDC_TC_MESSAGE), t.message, 127);

			t.bEnabled = t.bVisible = false;
			if(SendMessage(GetDlgItem(hWnd, IDC_TC_ENABLED), BM_GETCHECK, 0, 0))
				t.bEnabled = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_TC_VISIBLE), BM_GETCHECK, 0, 0))
				t.bVisible = true;

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_TC_BROWSE_SCRIPT:
			p->SelectScript();
			break;

		case IDC_TC_BROWSE_DIALOGUE:
			p->SelectDialogue();
			break;
		}
		break;
	}

	return FALSE;
}

void CTerminalCommandWnd::SelectScript()
{
	std::string script;
	if(CDatabaseUtilities::SelectScript(m_hWnd, script))
		SetWindowText(GetDlgItem(m_hWnd, IDC_TC_SCRIPT), script.c_str());
}

void CTerminalCommandWnd::SelectDialogue()
{
	int id;
	char buf[16];
	if(CDatabaseUtilities::SelectDialogue(m_hWnd, id))
	{
		sprintf(buf, "%i", id);
		SetWindowText(GetDlgItem(m_hWnd, IDC_TC_DIALOGUE), buf);
	}
}

bool CTerminalCommandWnd::OpenWindow(HWND parent, STerminalCommand &rTemplate, bool bNew)
{
	//if(bNew)
	//	rTemplate.FillDefaults();

	m_template = rTemplate;
	m_template.bVisible = true;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TERMINALCOMMAND), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
