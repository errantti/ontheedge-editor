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

#include "ComponentWnd.h"
#include "..\\..\\resource.h"
#include "..\\DatabaseUtilities.h"

CComponentWnd *CComponentWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComponentWnd::CComponentWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CComponentWnd::~CComponentWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CComponentWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CComponentWnd *p = CComponentWnd::m_pointer;
	SGadgetComponentTemplate &t = p->m_template;
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

		// Component specific inits
		sprintf(szTemp, "%i", t.targetGadgetID);
		SetWindowText(GetDlgItem(hWnd, IDC_COMPONENT_GADGET1), szTemp);
		sprintf(szTemp, "%i", t.upgradedGadgetID);
		SetWindowText(GetDlgItem(hWnd, IDC_COMPONENT_GADGET2), szTemp);

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

			// Component specific
			GetWindowText(GetDlgItem(hWnd, IDC_COMPONENT_GADGET1), szTemp, 8);
			t.targetGadgetID = atoi(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_COMPONENT_GADGET2), szTemp, 8);
			t.upgradedGadgetID = atoi(szTemp);

			if(t.targetGadgetID < 0 || t.upgradedGadgetID < 0)
			{
				::MessageBox(hWnd, "You must assign source and upgraded gadgets", "Error", MB_OK);
				break;
			}

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

		case IDC_COMPONENT_BROWSEGADGET1:
			p->SelectGadget(hWnd, 0, IDC_COMPONENT_GADGET1);
			break;
		case IDC_COMPONENT_BROWSEGADGET2:
			p->SelectGadget(hWnd, 1, IDC_COMPONENT_GADGET2);
			break;
		}
		break;
	}

	return FALSE;
}

void CComponentWnd::SelectGadget(HWND hWnd, int index, int dlgItem)
{
	long temp = 0;
	char buf[16];
	if(CDatabaseUtilities::SelectGadget(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, dlgItem), buf);
	}
}

bool CComponentWnd::OpenWindow(HWND parent, SGadgetComponentTemplate &rTemplate, bool bNew)
{
	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_COMPONENT), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
