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

#include "ShipPatchWnd.h"
#include "..\\..\\resource.h"
#include "..\\DatabaseUtilities.h"

CShipPatchWnd *CShipPatchWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShipPatchWnd::CShipPatchWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CShipPatchWnd::~CShipPatchWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CShipPatchWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CShipPatchWnd *p = CShipPatchWnd::m_pointer;
	SShipPatchTemplate &t = p->m_template;
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
		sprintf(szTemp, "%f", t.params[0]);
		SetWindowText(GetDlgItem(hWnd, IDC_PATCH_PARAM1), szTemp);
		sprintf(szTemp, "%f", t.params[1]);
		SetWindowText(GetDlgItem(hWnd, IDC_PATCH_PARAM2), szTemp);

		if(t.type == PATCH_TYPE_HEALTH)
			SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE1), BM_SETCHECK, 1, 0);
		else if(t.type == PATCH_TYPE_ENERGY)
			SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE2), BM_SETCHECK, 1, 0);
		else if(t.type == PATCH_TYPE_ENERGY_REGENERATION)
			SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE3), BM_SETCHECK, 1, 0);
		else if(t.type == PATCH_TYPE_TURN_SPEED)
			SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE4), BM_SETCHECK, 1, 0);
		else if(t.type == PATCH_TYPE_SPEED)
			SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE5), BM_SETCHECK, 1, 0);
		else if(t.type == PATCH_TYPE_BOOST_SPEED)
			SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE6), BM_SETCHECK, 1, 0);

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
			GetWindowText(GetDlgItem(hWnd, IDC_PATCH_PARAM1), szTemp, 10);
			t.params[0] = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PATCH_PARAM2), szTemp, 10);
			t.params[1] = (float)atof(szTemp);

			if(SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE1), BM_GETCHECK, 0, 0))
				t.type = PATCH_TYPE_HEALTH;
			else if(SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE2), BM_GETCHECK, 0, 0))
				t.type = PATCH_TYPE_ENERGY;
			else if(SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE3), BM_GETCHECK, 0, 0))
				t.type = PATCH_TYPE_ENERGY_REGENERATION;
			else if(SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE4), BM_GETCHECK, 0, 0))
				t.type = PATCH_TYPE_TURN_SPEED;
			else if(SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE5), BM_GETCHECK, 0, 0))
				t.type = PATCH_TYPE_SPEED;
			else if(SendMessage(GetDlgItem(hWnd, IDC_PATCH_TYPE6), BM_GETCHECK, 0, 0))
				t.type = PATCH_TYPE_BOOST_SPEED;

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

bool CShipPatchWnd::OpenWindow(HWND parent, SShipPatchTemplate &rTemplate, bool bNew)
{
	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SHIPPATCH), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
