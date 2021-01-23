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

#include "GetVectorWnd.h"
#include "..\\resource.h"

CGetVectorWnd *CGetVectorWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetVectorWnd::CGetVectorWnd() : m_bModified(FALSE)
{
	m_pointer = this; 
}

CGetVectorWnd::~CGetVectorWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CGetVectorWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CGetVectorWnd *p = CGetVectorWnd::m_pointer;
	//if(p == NULL)
	//	EndDialog(hWnd, FALSE);

	char szTemp[128];
	D3DXVECTOR3 vec = p->m_template;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		sprintf(szTemp, "%f", p->m_template.x);
		SetWindowText(GetDlgItem(hWnd, IDC_VECTOR_X), szTemp);
		sprintf(szTemp, "%f", p->m_template.y);
		SetWindowText(GetDlgItem(hWnd, IDC_VECTOR_Y), szTemp);
		sprintf(szTemp, "%f", p->m_template.z);
		SetWindowText(GetDlgItem(hWnd, IDC_VECTOR_Z), szTemp);

		SetWindowText(hWnd, p->m_caption.c_str());
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
			GetWindowText(GetDlgItem(hWnd, IDC_VECTOR_X), szTemp, 12);
			p->m_template.x = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_VECTOR_Y), szTemp, 12);
			p->m_template.y = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_VECTOR_Z), szTemp, 12);
			p->m_template.z = (float)atof(szTemp);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

bool CGetVectorWnd::OpenWindow(HWND parent, D3DXVECTOR3 &rVector, std::string caption)
{
	m_template = rVector;
	m_caption = caption;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GET_VECTOR), parent, WndProc);

	if(m_bModified)
	{
		rVector = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
