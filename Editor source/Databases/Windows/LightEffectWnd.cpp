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

#include "LightEffectWnd.h"
#include "..\\..\\resource.h"
#include "..\\DatabaseUtilities.h"
#include "..\\..\\Tools\\Get4DVectorWnd.h"

CLightEffectWnd *CLightEffectWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightEffectWnd::CLightEffectWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CLightEffectWnd::~CLightEffectWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CLightEffectWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CLightEffectWnd *p = CLightEffectWnd::m_pointer;
	SDynamicPointLightParams &t = p->m_template;
	char szTemp[32];

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		// General info
		SetWindowText(GetDlgItem(hWnd, IDC_LE_NAME), t.name);

		sprintf(szTemp, "%i", t.id);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_ID), szTemp);
		sprintf(szTemp, "%f", t.transitionDurMin);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_MINTRANS), szTemp);
		sprintf(szTemp, "%f", t.transitionDurMax);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_MAXTRANS), szTemp);
		sprintf(szTemp, "%f", t.attenuation1Coef);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_ATTCOEF), szTemp);

		if(t.stateChangeEnabled)
			SendMessage(GetDlgItem(hWnd, IDC_LE_STATE2_ENABLED), BM_SETCHECK, 1, 0);

		// States
		sprintf(szTemp, "%f", t.states[0].durationMin);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_MIN_DURATION1), szTemp);
		sprintf(szTemp, "%f", t.states[0].durationMax);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_MAX_DURATION1), szTemp);
		sprintf(szTemp, "%f", t.states[0].range);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_RANGE1), szTemp);

		sprintf(szTemp, "%f", t.states[1].durationMin);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_MIN_DURATION2), szTemp);
		sprintf(szTemp, "%f", t.states[1].durationMax);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_MAX_DURATION2), szTemp);
		sprintf(szTemp, "%f", t.states[1].range);
		SetWindowText(GetDlgItem(hWnd, IDC_LE_RANGE2), szTemp);

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
			GetWindowText(GetDlgItem(hWnd, IDC_LE_NAME), t.name, 31);
			if(strlen(t.name) < 3)
			{
				::MessageBox(hWnd, "Too short name", "Error", MB_OK);
				break;
			}

			GetWindowText(GetDlgItem(hWnd, IDC_LE_MINTRANS), szTemp, 10);
			t.transitionDurMin = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_LE_MAXTRANS), szTemp, 10);
			t.transitionDurMax = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_LE_ATTCOEF), szTemp, 10);
			t.attenuation1Coef = (float)atof(szTemp);
			if(t.attenuation1Coef < 0.01f || t.attenuation1Coef > 10.f)
			{
				::MessageBox(hWnd, "Invalid attenuation coefficient", "Error", MB_OK);
				break;
			}

			if(SendMessage(GetDlgItem(hWnd, IDC_LE_STATE2_ENABLED), BM_GETCHECK, 0, 0))
				t.stateChangeEnabled = true;
			else
				t.stateChangeEnabled = false;

			// States
			GetWindowText(GetDlgItem(hWnd, IDC_LE_MIN_DURATION1), szTemp, 10);
			t.states[0].durationMin = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_LE_MAX_DURATION1), szTemp, 10);
			t.states[0].durationMax = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_LE_RANGE1), szTemp, 10);
			t.states[0].range = (float)atof(szTemp);

			GetWindowText(GetDlgItem(hWnd, IDC_LE_MIN_DURATION2), szTemp, 10);
			t.states[1].durationMin = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_LE_MAX_DURATION2), szTemp, 10);
			t.states[1].durationMax = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_LE_RANGE2), szTemp, 10);
			t.states[1].range = (float)atof(szTemp);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_LE_SET_COLOR1:
			p->GetLightColor(hWnd, 0);
			break;
		case IDC_LE_SET_COLOR2:
			p->GetLightColor(hWnd, 1);
			break;
		}
		break;
	}

	return FALSE;
}

void CLightEffectWnd::GetLightColor(HWND hWnd, int index)
{
	CGet4DVectorWnd wnd;
	D3DXVECTOR4 vec;
	vec.x = m_template.states[index].color.a;
	vec.y = m_template.states[index].color.r;
	vec.z = m_template.states[index].color.g;
	vec.w = m_template.states[index].color.b;

	std::string fields[4] = {"A", "R", "G", "B"};
	if(wnd.OpenWindow(hWnd, vec, "Set light color", fields))
	{
		m_template.states[index].color.a = vec.x;
		m_template.states[index].color.r = vec.y;
		m_template.states[index].color.g = vec.z;
		m_template.states[index].color.b = vec.w;
	}
}

bool CLightEffectWnd::OpenWindow(HWND parent, SDynamicPointLightParams &rTemplate, bool bNew)
{
	m_template = rTemplate;

	if(bNew)
		rTemplate.FillDefaults();

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LIGHTEFFECT), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
