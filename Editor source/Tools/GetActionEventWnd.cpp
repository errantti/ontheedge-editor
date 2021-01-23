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

#include "GetActionEventWnd.h"
#include "..\\resource.h"
#include "..\\Databases\\DatabaseUtilities.h"

CGetActionEventWnd *CGetActionEventWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGetActionEventWnd::CGetActionEventWnd() : m_bModified(FALSE)
{
	m_pointer = this; 
}

CGetActionEventWnd::~CGetActionEventWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CGetActionEventWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CGetActionEventWnd *p = CGetActionEventWnd::m_pointer;
	//if(p == NULL)
	//	EndDialog(hWnd, FALSE);

	char szTemp[128];
	std::string caption;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		sprintf(szTemp, "%i", p->m_template.particleEffectID);
		SetWindowText(GetDlgItem(hWnd, IDC_AE_TXT1), szTemp);
		sprintf(szTemp, "%i", p->m_template.soundID);
		SetWindowText(GetDlgItem(hWnd, IDC_AE_TXT2), szTemp);

		SetWindowText(GetDlgItem(hWnd, IDC_AE_TXT3), p->m_template.scriptName);

		caption = "Set ";
		caption += p->m_caption;
		caption += " event action";
		SetWindowText(hWnd, p->m_caption.c_str());

		if(p->m_enabledFields != NULL)
		{
			if(!p->m_enabledFields[0]) 
			{
				EnableWindow(GetDlgItem(hWnd, IDC_AE_BROWSE1), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_AE_REMOVE1), FALSE);
			}
			if(!p->m_enabledFields[1]) 
			{
				EnableWindow(GetDlgItem(hWnd, IDC_AE_BROWSE2), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_AE_REMOVE2), FALSE);
			}
			if(!p->m_enabledFields[2]) 
			{
				EnableWindow(GetDlgItem(hWnd, IDC_AE_BROWSE3), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_AE_REMOVE3), FALSE);
			}
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
			GetWindowText(GetDlgItem(hWnd, IDC_AE_TXT1), szTemp, 8);
			p->m_template.particleEffectID = atoi(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_AE_TXT2), szTemp, 8);
			p->m_template.soundID = atoi(szTemp);

			GetWindowText(GetDlgItem(hWnd, IDC_AE_TXT3), p->m_template.scriptName, 31);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_AE_BROWSE1:
			CDatabaseUtilities::SelectParticleEffect(hWnd, p->m_template.particleEffectID);
			sprintf(szTemp, "%i", p->m_template.particleEffectID);
			SetWindowText(GetDlgItem(hWnd, IDC_AE_TXT1), szTemp);
			break;
		case IDC_AE_REMOVE1:
			SetWindowText(GetDlgItem(hWnd, IDC_AE_TXT1), "-1");
			break;

		case IDC_AE_BROWSE2:
			CDatabaseUtilities::SelectSound(hWnd, p->m_template.soundID);
			sprintf(szTemp, "%i", p->m_template.soundID);
			SetWindowText(GetDlgItem(hWnd, IDC_AE_TXT2), szTemp);
			break;
		case IDC_AE_REMOVE2:
			SetWindowText(GetDlgItem(hWnd, IDC_AE_TXT2), "-1");
			break;

		case IDC_AE_BROWSE3:
			if(CDatabaseUtilities::SelectScript(hWnd, caption, p->m_bUseGlobalScripts, p->m_bUseLocalScripts))
				SetWindowText(GetDlgItem(hWnd, IDC_AE_TXT3), caption.c_str());
			break;
		case IDC_AE_REMOVE3:
			SetWindowText(GetDlgItem(hWnd, IDC_AE_TXT3), "");
			break;
		}
		break;
	}

	return FALSE;
}

bool CGetActionEventWnd::OpenWindow(HWND parent, SActionEvent &ae, std::string caption, bool useGlobalScripts, bool useLocalScripts, bool *enabledFields)
{
	m_template = ae;
	m_caption = caption;
	m_bUseGlobalScripts = useGlobalScripts;
	m_bUseLocalScripts = useLocalScripts;
	m_enabledFields = enabledFields;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EVENTACTION), parent, WndProc);

	if(m_bModified)
	{
		ae = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
