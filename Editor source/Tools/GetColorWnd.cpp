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

#include "GetColorWnd.h"
#include "..\\resource.h"

CGetColorWnd *CGetColorWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetColorWnd::CGetColorWnd() : m_bModified(FALSE)
{
	m_pointer = this; 
}

CGetColorWnd::~CGetColorWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CGetColorWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CGetColorWnd *p = CGetColorWnd::m_pointer;
	//if(p == NULL)
	//	EndDialog(hWnd, FALSE);

	char szTemp[128];

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		sprintf(szTemp, "%f", p->m_template.a);
		SetWindowText(GetDlgItem(hWnd, IDC_SC_A), szTemp);
		sprintf(szTemp, "%f", p->m_template.r);
		SetWindowText(GetDlgItem(hWnd, IDC_SC_R), szTemp);
		sprintf(szTemp, "%f", p->m_template.g);
		SetWindowText(GetDlgItem(hWnd, IDC_SC_G), szTemp);
		sprintf(szTemp, "%f", p->m_template.b);
		SetWindowText(GetDlgItem(hWnd, IDC_SC_B), szTemp);
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
			GetWindowText(GetDlgItem(hWnd, IDC_SC_A), szTemp, 12);
			p->m_template.a = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_SC_R), szTemp, 12);
			p->m_template.r = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_SC_G), szTemp, 12);
			p->m_template.g = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_SC_B), szTemp, 12);
			p->m_template.b = (float)atof(szTemp);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_SC_PICK:
			p->PickColor(hWnd);
			break;
		}
		break;
	}

	return FALSE;
}

void CGetColorWnd::PickColor(HWND hWnd)
{
	CHOOSECOLOR cc;

	static COLORREF CColorList[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	ZeroMemory(&cc, sizeof(CHOOSECOLOR));
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hWnd;
	cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;/**/
	cc.lpCustColors = CColorList;

	D3DCOLOR color = D3DCOLOR_XRGB((int)(255.f * m_template.r), (int)(255.f * m_template.g), (int)(255.f * m_template.b));
	cc.rgbResult = RGB((color>>16) % 0xffff00ff, (color>>8) % 0xffff00ff, color % 0xffffff00);

	if(ChooseColor(&cc) == FALSE)
		return;

	color = D3DCOLOR_XRGB(GetRValue(cc.rgbResult), GetGValue(cc.rgbResult), GetBValue(cc.rgbResult));

	float a = m_template.a;
	m_template = D3DXCOLOR(color);
	m_template.a = a;

	//
	char szTemp[16];
	sprintf(szTemp, "%f", m_template.r);
	SetWindowText(GetDlgItem(hWnd, IDC_SC_R), szTemp);
	sprintf(szTemp, "%f", m_template.g);
	SetWindowText(GetDlgItem(hWnd, IDC_SC_G), szTemp);
	sprintf(szTemp, "%f", m_template.b);
	SetWindowText(GetDlgItem(hWnd, IDC_SC_B), szTemp);
}

bool CGetColorWnd::OpenWindow(HWND parent, D3DCOLORVALUE &rColor)
{
	D3DXCOLOR c;
	c.a = rColor.a;
	c.r = rColor.r;
	c.g = rColor.g;
	c.b = rColor.b;

	if(OpenWindow(parent, c))
	{
		rColor.a = c.a;
		rColor.r = c.r;
		rColor.g = c.g;
		rColor.b = c.b;
		return true;
	}

	return false;
}

bool CGetColorWnd::OpenWindow(HWND parent, D3DCOLOR &rColor)
{
	//D3DCOLORVALUE v(rColor);
	D3DXCOLOR c(rColor);
	/*c.a = v.a;
	c.r = v.r;
	c.g = v.g;
	c.b = v.b;*/

	if(OpenWindow(parent, c))
	{
		rColor = D3DCOLOR_ARGB((int)(255.f * c.a), (int)(255.f * c.r), (int)(255.f * c.g), (int)(255.f * c.b));
		return true;
	}

	return false;
}

bool CGetColorWnd::OpenWindow(HWND parent, D3DXCOLOR &rVector)
{
	m_template = rVector;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECTCOLOR), parent, WndProc);

	if(m_bModified)
	{
		rVector = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
