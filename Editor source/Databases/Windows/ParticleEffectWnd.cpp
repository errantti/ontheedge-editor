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

#include "ParticleEffectWnd.h"
#include "ParticleEmitterWnd.h"
#include "..\\..\\resource.h"
#include "..\\DatabaseUtilities.h"

CParticleEffectWnd *CParticleEffectWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticleEffectWnd::CParticleEffectWnd() : m_hWnd(NULL)
{
	m_pointer = this;
}

CParticleEffectWnd::~CParticleEffectWnd()
{
	m_pointer = NULL;
}

void CParticleEffectWnd::UpdateList()
{
	if(m_hWnd == NULL)
		return;

	SendMessage(GetDlgItem(m_hWnd, IDC_PEFFECT_LIST), LB_RESETCONTENT, 0, 0);

	vector<sEMITTER_PARAMS>::iterator it = m_template.params.begin();
	for(int i=0; it != m_template.params.end(); ++i, ++it)
		SendMessage(GetDlgItem(m_hWnd, IDC_PEFFECT_LIST), LB_INSERTSTRING, i, (LPARAM)it->name);
}

BOOL CALLBACK CParticleEffectWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CParticleEffectWnd *p = CParticleEffectWnd::m_pointer;
	SParticleSystemInfo &i = p->m_template;

	p->m_hWnd = hWnd;

	char szTemp[128];
	int selection;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		SetWindowText(GetDlgItem(hWnd, IDC_PEFFECT_NAME), i.name);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_OTHER_EFFECT), i.category);

		sprintf(szTemp, "%i", i.ID);
		SetWindowText(GetDlgItem(hWnd, IDC_PEFFECT_ID), szTemp);
		sprintf(szTemp, "%f", i.duration);
		SetWindowText(GetDlgItem(hWnd, IDC_PEFFECT_DURATION), szTemp);

		if(i.bAutoKillObject)
			SendMessage(GetDlgItem(hWnd, IDC_PEFFECT_AUTOKILL), BM_SETCHECK, 1, 0);

		p->UpdateList();

		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_PEFFECT_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case IDC_PEFFECT_SAVE:
			// Get the form information and return success
			GetWindowText(GetDlgItem(hWnd, IDC_PEFFECT_NAME), szTemp, 31);
			if(strlen(szTemp) < 3)
			{
				::MessageBox(hWnd, "Too short effect name", "Error", MB_OK);
				break;
			}
			strcpy(i.name, szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PEFFECT_DURATION), szTemp, 8);
			i.duration = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PEFFECT_ID), szTemp, 5);
			i.ID = atoi(szTemp);
			if(SendMessage(GetDlgItem(hWnd, IDC_PEFFECT_AUTOKILL), BM_GETCHECK, 0, 0))
				i.bAutoKillObject = true;
			else
				i.bAutoKillObject = false;

			//i.category[0] = '\0';
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_TYPE_EXP), BM_GETCHECK, 0, 0))
				strcpy(i.category, "Explosions");
			else if(SendMessage(GetDlgItem(hWnd, IDC_PE_TYPE_ENGINE), BM_GETCHECK, 0, 0))
				strcpy(i.category, "Engine flames");
			else if(SendMessage(GetDlgItem(hWnd, IDC_PE_TYPE_ENV), BM_GETCHECK, 0, 0))
				strcpy(i.category, "Environmental");
			else if(SendMessage(GetDlgItem(hWnd, IDC_PE_TYPE_SPECIAL), BM_GETCHECK, 0, 0))
				strcpy(i.category, "Special effects");
			else if(SendMessage(GetDlgItem(hWnd, IDC_PE_TYPE_AMMO), BM_GETCHECK, 0, 0))
				strcpy(i.category, "Ammo effects");
			else if(SendMessage(GetDlgItem(hWnd, IDC_PE_TYPE_WEAPON), BM_GETCHECK, 0, 0))
				strcpy(i.category, "Weapon effects");
			else if(SendMessage(GetDlgItem(hWnd, IDC_PE_TYPE_OTHER), BM_GETCHECK, 0, 0))
			{
				GetWindowText(GetDlgItem(hWnd, IDC_PE_OTHER_EFFECT), i.category, 31);
			}

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_PEFFECT_EDIT:
			if((selection = SendMessage(GetDlgItem(hWnd, IDC_PEFFECT_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				//if(p->m_database)
				//	p->m_database->EditTemplate(p->m_entries[selection].id, p);
				if(p->EditEmitter(selection))
					p->m_bModified = TRUE;
			}
			else
				::MessageBox(hWnd, "You haven't selected an emitter!", "Error!", MB_OK);
			break;

		case IDC_PEFFECT_ADD:
			p->AddEmitter();
			//p->m_bModified = true;
			break;

		case IDC_PEFFECT_REMOVE:
			if((selection = SendMessage(GetDlgItem(hWnd, IDC_PEFFECT_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				p->RemoveEmitter(selection);
				p->UpdateList();
					//p->m_bModified = true;
			}
			else
				::MessageBox(hWnd, "You haven't selected an emitter!", "Error!", MB_OK);
			break;

		case IDC_PEFFECT_LIST:
			if(HIWORD(wParam) == LBN_DBLCLK)
			{
				if((selection = SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0)) != LB_ERR)
				{
					if(p->EditEmitter(selection))
						p->m_bModified = true;
				}
			}
			break;
		}
		break;
	}

	return FALSE;
}

bool CParticleEffectWnd::EditEmitter(int selection)
{
	if(selection < 0 || selection >= m_template.params.size())
		return false;
	CParticleEmitterWnd wnd;
	wnd.OpenWindow(m_hWnd, m_template.params[selection], false);
	UpdateList();
	return true;
}

void CParticleEffectWnd::AddEmitter()
{
	CParticleEmitterWnd wnd;
	sEMITTER_PARAMS emitter;
	if(wnd.OpenWindow(m_hWnd, emitter, true))
	{
		m_template.params.push_back(emitter);
		UpdateList();
	}
}

bool CParticleEffectWnd::RemoveEmitter(int selection)
{
	if(selection < 0 || selection >= m_template.params.size())
		return false;

	vector<sEMITTER_PARAMS>::iterator it = m_template.params.begin();
	for(int i=0;i<selection;++i, ++it);
	m_template.params.erase(it);
	return true;
}

bool CParticleEffectWnd::OpenWindow(HWND parent, SParticleSystemInfo &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PARTICLEEFFECT), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
