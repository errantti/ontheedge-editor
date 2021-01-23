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

#include "EnemyObjectWnd.h"
#include "VesselObjectWnd.h"
#include "..\\..\\resource.h"
#include "..\\..\\Editor.h"
#include "..\\SelectionWnd.h"
#include "..\\DatabaseUtilities.h"

CEnemyObjectWnd *CEnemyObjectWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnemyObjectWnd::CEnemyObjectWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CEnemyObjectWnd::~CEnemyObjectWnd()
{
	m_pointer = NULL;
}

void CEnemyObjectWnd::OpenVesselProperties(HWND hWnd)
{
	CVesselObjectWnd vw;
	vw.OpenWindow(hWnd, m_template.vt, FALSE);
}

BOOL CALLBACK CEnemyObjectWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CEnemyObjectWnd *p = CEnemyObjectWnd::m_pointer;
	SEnemyTemplate &t = p->m_template;
	char filename[128];
	std::string str;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		// AI
		sprintf(filename, "%f", t.AI.shootingDistance);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_SHOOT_DIST), filename);
		sprintf(filename, "%f", t.AI.reactionDistance);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_REACTION_DIST), filename);
		sprintf(filename, "%f", t.AI.minimumDistance);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_MIN_DIST), filename);
		sprintf(filename, "%f", t.AI.aggroMaintainDistance);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_AGGROMAINTAIN), filename);
		sprintf(filename, "%f", t.AI.helpCallRadius);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_HELP_DIST), filename);
		sprintf(filename, "%f", t.AI.helpCallFrequency);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_HELPFREQUENCY), filename);

		// check boxes
		if(t.AI.bExplodesOnCollision)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_EXPLODES), BM_SETCHECK, 1, 0);
		if(t.AI.bMobile)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_MOBILE), BM_SETCHECK, 1, 0);
		if(t.AI.bShoots)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_SHOOTS), BM_SETCHECK, 1, 0);
		if(t.AI.bSuicide)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_SUICIDAL), BM_SETCHECK, 1, 0);
		if(t.AI.bBolted)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_BOLTED), BM_SETCHECK, 1, 0);
		if(t.AI.bCallsForHelp)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_CALLHELP), BM_SETCHECK, 1, 0);
		if(t.AI.bWaitsForGoodAngle)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_WAITANGLE), BM_SETCHECK, 1, 0);
		if(t.AI.customAIFlags & CUSTOM_AI_FLAG_CONTINUE_ROUTE)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_CONTINUE), BM_SETCHECK, 1, 0);
		if(t.AI.customAIFlags & CUSTOM_AI_FLAG_DONT_FOLLOW_ROUTE)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_DONT_FOLLOW), BM_SETCHECK, 1, 0);
		if(t.AI.customAIFlags & CUSTOM_AI_FLAG_USE_CUSTOM_ROUTE)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_USE_CUSTOM_ROUTE), BM_SETCHECK, 1, 0);
		if(t.AI.customAIFlags & CUSTOM_AI_FLAG_DISABLE_PATHFINDING)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_DISABLE_PATHFINDING), BM_SETCHECK, 1, 0);
		if(t.AI.customAIFlags & CUSTOM_AI_FLAG_ANTICIPATE_MOVEMENT)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_ANTICIPATE), BM_SETCHECK, 1, 0);

		if(t.AI.aggroResponse == ENEMY_AGGRO_RESPONSE_ATTACK)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AGGRO_ATTACK), BM_SETCHECK, 1, 0);
		else if(t.AI.aggroResponse == ENEMY_AGGRO_RESPONSE_ESCAPE)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AGGRO_ESCAPE), BM_SETCHECK, 1, 0);
		else if(t.AI.aggroResponse == ENEMY_AGGRO_RESPONSE_FOLLOWROUTE)
			SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AGGRO_ROUTE), BM_SETCHECK, 1, 0);

		// Gadgets
		sprintf(filename, "%i", t.gadgetID[0]);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_GADGET1), filename);
		sprintf(filename, "%i", t.gadgetID[1]);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_GADGET2), filename);

		sprintf(filename, "%f", t.AI.gadget2TriggerParams[0]);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_TRIGGERPARAM1), filename);
		sprintf(filename, "%f", t.AI.gadget2TriggerParams[1]);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_TRIGGERPARAM2), filename);

		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_TRIGGER_SCRIPT), t.AI.gadget2TriggerScript);

		if(t.AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_ACTIVE)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_ACTIVE), BM_SETCHECK, 1, 0);
		else if(t.AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_HEALTHLIMIT)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_HEALTH), BM_SETCHECK, 1, 0);
		else if(t.AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_DAMAGERESPONSE)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_DAMAGE), BM_SETCHECK, 1, 0);
		else if(t.AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_AGGROTIMELIMIT)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_AGGROTIME), BM_SETCHECK, 1, 0);
		else if(t.AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_SCRIPT)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_SCRIPT), BM_SETCHECK, 1, 0);

		if(t.AI.gadget2DisableGadget1)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_DISABLE_G1), BM_SETCHECK, 1, 0);

		// General settings
		if(t.alteredDamageType == DAMAGE_TYPE_ENERGY)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_TYPE_ENERGY), BM_SETCHECK, 1, 0);
		else if(t.alteredDamageType == DAMAGE_TYPE_PLASMA)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_TYPE_PLASMA), BM_SETCHECK, 1, 0);
		else if(t.alteredDamageType == DAMAGE_TYPE_PROJECTILE)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_TYPE_PROJECTILE), BM_SETCHECK, 1, 0);
		else if(t.alteredDamageType == DAMAGE_TYPE_STUN)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_TYPE_STUN2), BM_SETCHECK, 1, 0);

		sprintf(filename, "%f", t.alteredDamageTypeCoefficient);
		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_DAMAGE_COEF), filename);

		SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_ONAGGRO_SCRIPT), t.onAggroScript);

		if(t.bDisplayHealthBar)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_DISPLAY_HEALTH), BM_SETCHECK, 1, 0);
		if(t.bCanBePossessed)
			SendMessage(GetDlgItem(hWnd, IDC_VESSEL_POSSESSABLE), BM_SETCHECK, 1, 0);

		break;

	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_ENEMY_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_ENEMY_OK:
			// AI
			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_SHOOT_DIST), filename, 12);
			t.AI.shootingDistance = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_REACTION_DIST), filename, 12);
			t.AI.reactionDistance = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_MIN_DIST), filename, 12);
			t.AI.minimumDistance = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_AGGROMAINTAIN), filename, 12);
			t.AI.aggroMaintainDistance = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_HELP_DIST), filename, 12);
			t.AI.helpCallRadius = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_HELPFREQUENCY), filename, 12);
			t.AI.helpCallFrequency = (float)atof(filename);

			t.AI.bExplodesOnCollision = t.AI.bMobile = t.AI.bShoots = t.AI.bSuicide = FALSE;
			t.AI.bBolted = FALSE;
			t.AI.bCallsForHelp = t.AI.bWaitsForGoodAngle = FALSE;
			t.AI.bIsEnemy = TRUE;

			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_EXPLODES), BM_GETCHECK, 0, 0))
				t.AI.bExplodesOnCollision = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_MOBILE), BM_GETCHECK, 0, 0))
				t.AI.bMobile = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_SHOOTS), BM_GETCHECK, 0, 0))
				t.AI.bShoots = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_SUICIDAL), BM_GETCHECK, 0, 0))
				t.AI.bSuicide = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_BOLTED), BM_GETCHECK, 0, 0))
				t.AI.bBolted = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_CALLHELP), BM_GETCHECK, 0, 0))
				t.AI.bCallsForHelp = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_WAITANGLE), BM_GETCHECK, 0, 0))
				t.AI.bWaitsForGoodAngle = TRUE;

			t.AI.customAIFlags = 0;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_CONTINUE), BM_GETCHECK, 0, 0))
				t.AI.customAIFlags |= CUSTOM_AI_FLAG_CONTINUE_ROUTE;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_DONT_FOLLOW), BM_GETCHECK, 0, 0))
				t.AI.customAIFlags |= CUSTOM_AI_FLAG_DONT_FOLLOW_ROUTE;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_USE_CUSTOM_ROUTE), BM_GETCHECK, 0, 0))
				t.AI.customAIFlags |= CUSTOM_AI_FLAG_USE_CUSTOM_ROUTE;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_DISABLE_PATHFINDING), BM_GETCHECK, 0, 0))
				t.AI.customAIFlags |= CUSTOM_AI_FLAG_DISABLE_PATHFINDING;
			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AI_ANTICIPATE), BM_GETCHECK, 0, 0))
				t.AI.customAIFlags |= CUSTOM_AI_FLAG_ANTICIPATE_MOVEMENT;

			if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AGGRO_ATTACK), BM_GETCHECK, 0, 0))
				t.AI.aggroResponse = ENEMY_AGGRO_RESPONSE_ATTACK;
			else if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AGGRO_ESCAPE), BM_GETCHECK, 0, 0))
				t.AI.aggroResponse = ENEMY_AGGRO_RESPONSE_ESCAPE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_ENEMY_AGGRO_ROUTE), BM_GETCHECK, 0, 0))
				t.AI.aggroResponse = ENEMY_AGGRO_RESPONSE_FOLLOWROUTE;

			// Gadgets
			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_GADGET1), filename, 8);
			t.gadgetID[0] = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_GADGET2), filename, 8);
			t.gadgetID[1] = atoi(filename);

			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_TRIGGERPARAM1), filename, 8);
			t.AI.gadget2TriggerParams[0] = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_TRIGGERPARAM2), filename, 8);
			t.AI.gadget2TriggerParams[1] = (float)atof(filename);

			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_TRIGGER_SCRIPT), t.AI.gadget2TriggerScript, 31);

			if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_ACTIVE), BM_GETCHECK, 0, 0))
				t.AI.gadget2Trigger = ENEMY_GADGET2_TRIGGERTYPE_ACTIVE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_HEALTH), BM_GETCHECK, 0, 0))
				t.AI.gadget2Trigger = ENEMY_GADGET2_TRIGGERTYPE_HEALTHLIMIT;
			else if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_DAMAGE), BM_GETCHECK, 0, 0))
				t.AI.gadget2Trigger = ENEMY_GADGET2_TRIGGERTYPE_DAMAGERESPONSE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_SCRIPT), BM_GETCHECK, 0, 0))
				t.AI.gadget2Trigger = ENEMY_GADGET2_TRIGGERTYPE_SCRIPT;
			else if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_G2TRIGGER_AGGROTIME), BM_GETCHECK, 0, 0))
				t.AI.gadget2Trigger = ENEMY_GADGET2_TRIGGERTYPE_AGGROTIMELIMIT;

			t.AI.gadget2DisableGadget1 = false;
			if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_DISABLE_G1), BM_GETCHECK, 0, 0))
				t.AI.gadget2DisableGadget1 = true;

			// General settings
			if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_TYPE_ENERGY), BM_GETCHECK, 0, 0))
				t.alteredDamageType = DAMAGE_TYPE_ENERGY;
			else if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_TYPE_PLASMA), BM_GETCHECK, 0, 0))
				t.alteredDamageType = DAMAGE_TYPE_PLASMA;
			else if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_TYPE_PROJECTILE), BM_GETCHECK, 0, 0))
				t.alteredDamageType = DAMAGE_TYPE_PROJECTILE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_TYPE_STUN2), BM_GETCHECK, 0, 0))
				t.alteredDamageType = DAMAGE_TYPE_STUN;

			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_ONAGGRO_SCRIPT), t.onAggroScript, 31);

			t.bDisplayHealthBar = t.bCanBePossessed = false;
			if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_DISPLAY_HEALTH), BM_GETCHECK, 0, 0))
				t.bDisplayHealthBar = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_VESSEL_POSSESSABLE), BM_GETCHECK, 0, 0))
				t.bCanBePossessed = true;

			GetWindowText(GetDlgItem(hWnd, IDC_ENEMY_DAMAGE_COEF), filename, 12);
			t.alteredDamageTypeCoefficient = (float)atof(filename);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case ID_ENEMY_VESSEL:
			p->OpenVesselProperties(hWnd);
			break;

		case ID_ENEMY_BROWSE_GADGET1:
			CDatabaseUtilities::SelectGadget(hWnd, p->m_template.gadgetID[0]);
			sprintf(filename, "%i", t.gadgetID[0]);
			SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_GADGET1), filename);
			break;
		case ID_ENEMY_BROWSE_GADGET2:
			CDatabaseUtilities::SelectGadget(hWnd, p->m_template.gadgetID[1]);
			sprintf(filename, "%i", t.gadgetID[1]);
			SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_GADGET2), filename);
			break;

		case ID_ENEMY_BROWSE_TRIGGER_SCRIPT:
			if(CDatabaseUtilities::SelectGlobalScript(hWnd, str))
			{
				SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_TRIGGER_SCRIPT), str.c_str());
			}
			break;
		case ID_ENEMY_BROWSE_ONAGGRO_SCRIPT:
			if(CDatabaseUtilities::SelectGlobalScript(hWnd, str))
			{
				SetWindowText(GetDlgItem(hWnd, IDC_ENEMY_ONAGGRO_SCRIPT), str.c_str());
			}
			break;
		}
		break;
	}

	return FALSE;
}

bool CEnemyObjectWnd::OpenWindow(HWND parent, SEnemyTemplate &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ENEMY), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}

