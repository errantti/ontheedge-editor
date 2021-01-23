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

#include "TriggerWnd.h"
#include "..\\resource.h"
#include "..\\Editor.h"
#include "..\\Databases\\DatabaseUtilities.h"
#include "GetActionEventWnd.h"

CTriggerWnd *CTriggerWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTriggerWnd::CTriggerWnd() : m_bModified(FALSE), m_bDialogueSelected(TRUE)
{
	m_pointer = this;
}

CTriggerWnd::~CTriggerWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CTriggerWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CTriggerWnd *p = CTriggerWnd::m_pointer;
	STriggerInfo &t = p->m_template;
	char filename[128];
	std::string str;
	int temp;
	long lTemp;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		p->SelectTriggerType(t.type, hWnd);

		// Set form information
		sprintf(filename, "%i", t.actionID);
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_TARGET), filename);
		sprintf(filename, "%i", t.id);
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_ID), filename);
		sprintf(filename, "%i", t.triggeringObjectID);
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_TRIGGERING_ID), filename);
		sprintf(filename, "%f", t.multitriggeringDelay);
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_DELAY), filename);
		sprintf(filename, "%f", t.damage);
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_DAMAGE), filename);

		switch(t.type)
		{
		case TRIGGER_TYPE_DIALOGUE:
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_DIALOGUE), BM_SETCHECK, 1, 0);
			//EnableWindow(GetDlgItem(hWnd, IDC_TRIGGER_BROWSE), TRUE);
			break;
		case TRIGGER_TYPE_TERMINAL:
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_TERMINAL), BM_SETCHECK, 1, 0);
			break;
		case TRIGGER_TYPE_ASSISTANTMSG:
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_MESSAGE), BM_SETCHECK, 1, 0);
			break;
		case TRIGGER_TYPE_DAMAGES:
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_DAMAGE), BM_SETCHECK, 1, 0);
			break;
		case TRIGGER_TYPE_EFFECT:
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_EFFECT), BM_SETCHECK, 1, 0);
			break;
		}

		if(t.triggeringObjectTypes & TRIGGER_OBJECT_PLAYER)
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_OPLAYER), BM_SETCHECK, 1, 0);
		if(t.triggeringObjectTypes & TRIGGER_OBJECT_ENEMY)
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_OENEMY), BM_SETCHECK, 1, 0);
		if(t.triggeringObjectTypes & TRIGGER_OBJECT_OBJECT)
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_ODO), BM_SETCHECK, 1, 0);
		if(t.triggeringObjectTypes & TRIGGER_OBJECT_POSSESSED_ENEMY)
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_OPOSSESSED), BM_SETCHECK, 1, 0);
		if(t.triggeringObjectTypes & TRIGGER_OBJECT_SPECIFIC_ID)
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_OSPECIFIC), BM_SETCHECK, 1, 0);

		if(t.damageType == DAMAGE_TYPE_ENERGY)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_ENERGY), BM_SETCHECK, 1, 0);
		else if(t.damageType == DAMAGE_TYPE_PLASMA)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PLASMA), BM_SETCHECK, 1, 0);
		else if(t.damageType == DAMAGE_TYPE_PROJECTILE)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PROJECTILE), BM_SETCHECK, 1, 0);
		else if(t.damageType == DAMAGE_TYPE_STUN)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_STUN), BM_SETCHECK, 1, 0);

		if(t.bMultitriggering)
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_MULTITRIGGERING), BM_SETCHECK, 1, 0);
		if(t.bAEAtObject)
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_EFFECT_AT_OBJECT), BM_SETCHECK, 1, 0);
		if(t.bActiveByDefault)
			SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_ACTIVE), BM_SETCHECK, 1, 0);
		break;

	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_TRIGGER_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_TRIGGER_OK:
			GetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_TARGET), filename, 8);
			t.actionID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_TRIGGERING_ID), filename, 8);
			t.triggeringObjectID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_DELAY), filename, 8);
			t.multitriggeringDelay = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_DAMAGE), filename, 8);
			t.damage = (float)atof(filename);

			if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_DIALOGUE), BM_GETCHECK, 0, 0))
				t.type = TRIGGER_TYPE_DIALOGUE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_TERMINAL), BM_GETCHECK, 0, 0))
				t.type = TRIGGER_TYPE_TERMINAL;
			else if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_MESSAGE), BM_GETCHECK, 0, 0))
				t.type = TRIGGER_TYPE_ASSISTANTMSG;
			else if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_EFFECT), BM_GETCHECK, 0, 0))
				t.type = TRIGGER_TYPE_EFFECT;
			else if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_DAMAGE), BM_GETCHECK, 0, 0))
				t.type = TRIGGER_TYPE_DAMAGES;

			t.triggeringObjectTypes = 0;
			t.bMultitriggering = FALSE;
			if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_OPLAYER), BM_GETCHECK, 0, 0))
				t.triggeringObjectTypes |= TRIGGER_OBJECT_PLAYER;
			if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_OENEMY), BM_GETCHECK, 0, 0))
				t.triggeringObjectTypes |= TRIGGER_OBJECT_ENEMY;
			if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_ODO), BM_GETCHECK, 0, 0))
				t.triggeringObjectTypes |= TRIGGER_OBJECT_OBJECT;
			if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_OPOSSESSED), BM_GETCHECK, 0, 0))
				t.triggeringObjectTypes |= TRIGGER_OBJECT_POSSESSED_ENEMY;
			if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_OSPECIFIC), BM_GETCHECK, 0, 0))
				t.triggeringObjectTypes |= TRIGGER_OBJECT_SPECIFIC_ID;

			t.damageType = DAMAGE_TYPE_NONE;
			if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_ENERGY), BM_GETCHECK, 0, 0))
				t.damageType = DAMAGE_TYPE_ENERGY;
			else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PLASMA), BM_GETCHECK, 0, 0))
				t.damageType = DAMAGE_TYPE_PLASMA;
			else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PROJECTILE), BM_GETCHECK, 0, 0))
				t.damageType = DAMAGE_TYPE_PROJECTILE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_STUN), BM_GETCHECK, 0, 0))
				t.damageType = DAMAGE_TYPE_STUN;

			if(t.triggeringObjectTypes == 0)
			{
				::MessageBox(hWnd, "You have to select at least one triggering type for the trigger", "Error", MB_OK);
				break;
			}

			t.bMultitriggering = t.bActiveByDefault = t.bAEAtObject = FALSE;
			if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_MULTITRIGGERING), BM_GETCHECK, 0, 0))
				t.bMultitriggering = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_ACTIVE), BM_GETCHECK, 0, 0))
				t.bActiveByDefault = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_EFFECT_AT_OBJECT), BM_GETCHECK, 0, 0))
				t.bAEAtObject = TRUE;

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_TRIGGER_BROWSE_ACTION:
			if(p->m_selectedType == TRIGGER_TYPE_DIALOGUE)
			{
				if(CDatabaseUtilities::SelectDialogue(hWnd, temp))
				{
					sprintf(filename, "%i", temp);
					SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_TARGET), filename);
				}
			}
			else if(p->m_selectedType == TRIGGER_TYPE_TERMINAL)
			{
				if(CDatabaseUtilities::SelectTerminal(hWnd, lTemp))
				{
					sprintf(filename, "%i", lTemp);
					SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_TARGET), filename);
				}
			}
			else if(p->m_selectedType == TRIGGER_TYPE_ASSISTANTMSG)
			{
				if(CDatabaseUtilities::SelectAssistantMsg(hWnd, temp))
				{
					sprintf(filename, "%i", temp);
					SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_TARGET), filename);
				}
			}
			break;

		case IDC_TRIGGER_SET_EFFECT:
			p->GetTriggerEffect(hWnd);
			break;

		case IDC_TRIGGER_TYPE_DIALOGUE:
			if(HIWORD(wParam) == BN_CLICKED)
			{
				if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_DIALOGUE), BM_GETCHECK, 0, 0))
					p->SelectTriggerType(TRIGGER_TYPE_DIALOGUE, hWnd);
			}
			break;
		case IDC_TRIGGER_TYPE_TERMINAL:
			if(HIWORD(wParam) == BN_CLICKED)
			{
				if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_TERMINAL), BM_GETCHECK, 0, 0))
					p->SelectTriggerType(TRIGGER_TYPE_TERMINAL, hWnd);
			}
			break;
		case IDC_TRIGGER_TYPE_MESSAGE:
			if(HIWORD(wParam) == BN_CLICKED)
			{
				if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_MESSAGE), BM_GETCHECK, 0, 0))
					p->SelectTriggerType(TRIGGER_TYPE_ASSISTANTMSG, hWnd);
			}
			break;
		case IDC_TRIGGER_TYPE_EFFECT:
			if(HIWORD(wParam) == BN_CLICKED)
			{
				if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_EFFECT), BM_GETCHECK, 0, 0))
					p->SelectTriggerType(TRIGGER_TYPE_EFFECT, hWnd);
			}
			break;
		case IDC_TRIGGER_TYPE_DAMAGE:
			if(HIWORD(wParam) == BN_CLICKED)
			{
				if(SendMessage(GetDlgItem(hWnd, IDC_TRIGGER_TYPE_DAMAGE), BM_GETCHECK, 0, 0))
					p->SelectTriggerType(TRIGGER_TYPE_DAMAGES, hWnd);
			}
			break;
		}
		break;
	}

	return FALSE;
}

void CTriggerWnd::SelectTriggerType(TRIGGER_TYPE type, HWND hWnd)
{
	if(type == m_selectedType)
		return;

	if(type == TRIGGER_TYPE_DIALOGUE || type == TRIGGER_TYPE_TERMINAL || type == TRIGGER_TYPE_ASSISTANTMSG)
	{
		EnableWindow(GetDlgItem(hWnd, IDC_TRIGGER_BROWSE_ACTION), TRUE);
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_TARGET), "-1");
		m_template.ae.FillDefaults();

		EnableWindow(GetDlgItem(hWnd, IDC_TRIGGER_MESSAGE), FALSE);
		EnableWindow(GetDlgItem(hWnd, IDC_TRIGGER_BROWSE_SCRIPT), FALSE);
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_SCRIPT), "");
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_MESSAGE), "");
	}
	else if(type == TRIGGER_TYPE_EFFECT || type == TRIGGER_TYPE_DAMAGES)
	{
		EnableWindow(GetDlgItem(hWnd, IDC_TRIGGER_SET_EFFECT), TRUE);

		EnableWindow(GetDlgItem(hWnd, IDC_TRIGGER_BROWSE_ACTION), FALSE);
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_TARGET), "-1");
		EnableWindow(GetDlgItem(hWnd, IDC_TRIGGER_MESSAGE), FALSE);
		SetWindowText(GetDlgItem(hWnd, IDC_TRIGGER_MESSAGE), "");
	}

	m_selectedType = type;
}

void CTriggerWnd::GetTriggerEffect(HWND hWnd)
{
	CGetActionEventWnd wnd;
	wnd.OpenWindow(hWnd, m_template.ae, "Set trigger effect", true, true);
}

bool CTriggerWnd::OpenWindow(HWND parent, STriggerInfo &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TRIGGER), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
