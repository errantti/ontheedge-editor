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

#include "DialogueLineWnd.h"
#include "..\\resource.h"
#include "..\\Editor.h"

CDialogueLineWnd *CDialogueLineWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialogueLineWnd::CDialogueLineWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CDialogueLineWnd::~CDialogueLineWnd()
{
	m_pointer = NULL;
}


BOOL CALLBACK CDialogueLineWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CDialogueLineWnd *p = CDialogueLineWnd::m_pointer;
	CDialogue::SComment &t = p->m_template;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_DL_SPEAKER), t.speaker);
		SetWindowText(GetDlgItem(hWnd, IDC_DL_COMMENT), t.comment);
		break;

	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_DL_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_DL_OK:
			::memset(&t, 0, sizeof(CDialogue::SComment));

			// Get the form information and return success
			GetWindowText(GetDlgItem(hWnd, IDC_DL_SPEAKER), t.speaker, 32);
			GetWindowText(GetDlgItem(hWnd, IDC_DL_COMMENT), t.comment, 1024);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

bool CDialogueLineWnd::OpenWindow(HWND parent, CDialogue::SComment &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOGUE_LINE), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
