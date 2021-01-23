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

#include "QuestItemWnd.h"
#include "..\\..\\resource.h"
#include "..\\DatabaseUtilities.h"

CQuestItemWnd *CQuestItemWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQuestItemWnd::CQuestItemWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CQuestItemWnd::~CQuestItemWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CQuestItemWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CQuestItemWnd *p = CQuestItemWnd::m_pointer;
	SQuestItemTemplate &t = p->m_template;
	char szTemp[16];
	std::string str;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Common item information
		SetWindowText(GetDlgItem(hWnd, IDC_ITEM_NAME), t.name);
		SetWindowText(GetDlgItem(hWnd, IDC_ITEM_DESCRIPTION), t.description);
		SetWindowText(GetDlgItem(hWnd, IDC_ITEM_ICON), t.iconTexture);

		sprintf(szTemp, "%i", t.id);
		SetWindowText(GetDlgItem(hWnd, IDC_ITEM_ID), szTemp);

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
			// Common item stuff
			GetWindowText(GetDlgItem(hWnd, IDC_ITEM_NAME), t.name, 31);
			if(strlen(t.name) < 3)
			{
				::MessageBox(hWnd, "Too short name", "Error", MB_OK);
				break;
			}
			GetWindowText(GetDlgItem(hWnd, IDC_ITEM_DESCRIPTION), t.description, INVENTORY_ITEM_DESCRIPTION_LENGTH-1);
			GetWindowText(GetDlgItem(hWnd, IDC_ITEM_ICON), t.iconTexture, 127);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_ITEM_BROWSE:
			CDatabaseUtilities::GetGadgetIconFilename(hWnd, str);
			SetWindowText(GetDlgItem(hWnd, IDC_ITEM_ICON), str.c_str());
			break;
		case IDC_ITEM_REMOVE:
			SetWindowText(GetDlgItem(hWnd, IDC_ITEM_ICON), "");
			break;
		}
		break;
	}

	return FALSE;
}

bool CQuestItemWnd::OpenWindow(HWND parent, SQuestItemTemplate &rTemplate, bool bNew)
{
	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_QUESTITEM), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
