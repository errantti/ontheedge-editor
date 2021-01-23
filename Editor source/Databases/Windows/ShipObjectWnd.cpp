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

#include "ShipObjectWnd.h"
#include "..\\..\\resource.h"
#include "..\\..\\Editor.h"
#include "VesselObjectWnd.h"

CShipObjectWnd *CShipObjectWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShipObjectWnd::CShipObjectWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CShipObjectWnd::~CShipObjectWnd()
{
	m_pointer = NULL;
}

void CShipObjectWnd::OpenVesselProperties(HWND hWnd)
{
	CVesselObjectWnd vw;
	vw.OpenWindow(hWnd, m_template.vt, FALSE);
}

BOOL CALLBACK CShipObjectWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CShipObjectWnd *p = CShipObjectWnd::m_pointer;
	SShipTemplate &t = p->m_template;
	char filename[128];
	std::string str;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		sprintf(filename, "%f", t.maxLoad);
		SetWindowText(GetDlgItem(hWnd, IDC_SHIP_MAX_LOAD), filename);
		sprintf(filename, "%f", t.boostForce);
		SetWindowText(GetDlgItem(hWnd, IDC_SHIP_BOOST_FORCE), filename);
		sprintf(filename, "%f", t.boostEnergyConsumption);
		SetWindowText(GetDlgItem(hWnd, IDC_SHIP_BOOST_CON), filename);
		break;

	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_SHIP_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_SHIP_OK:
			GetWindowText(GetDlgItem(hWnd, IDC_SHIP_MAX_LOAD), filename, 12);
			t.maxLoad = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_SHIP_BOOST_FORCE), filename, 12);
			t.boostForce = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_SHIP_BOOST_CON), filename, 12);
			t.boostEnergyConsumption = (float)atof(filename);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case ID_SHIP_VESSEL:
			p->OpenVesselProperties(hWnd);
			break;
		}
		break;
	}

	return FALSE;
}

bool CShipObjectWnd::OpenWindow(HWND parent, SShipTemplate &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SHIP), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
