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

#include "ObjectPropertiesWnd.h"
#include "..\\resource.h"
#include "..\\Databases\\DatabaseUtilities.h"
#include "GetVectorWnd.h"
#include "..\\Databases\\SelectionWnd.h"
#include "..\\OtE\\OnTheEdge.h"

CObjectPropertiesWnd *CObjectPropertiesWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectPropertiesWnd::CObjectPropertiesWnd()
{
	m_pointer = this;
}

CObjectPropertiesWnd::~CObjectPropertiesWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CObjectPropertiesWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CObjectPropertiesWnd *p = CObjectPropertiesWnd::m_pointer;
	SObjectInstanceInfo &i = p->m_template;
	char filename[128];

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->SetObjectType(hWnd);

		// Edit fields
		sprintf(filename, "%i", i.id);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_ID), filename);
		sprintf(filename, "%i", i.targetID);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_TARGETID), filename);
		sprintf(filename, "%i", i.particleEffectID);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_PEFFECT), filename);
		sprintf(filename, "%i", i.lightEffectID);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_LEFFECT2), filename);
		sprintf(filename, "%i", i.controllerID);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_CONTROLLER), filename);

		sprintf(filename, "%f", i.controllerParams[0]);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_CPARAM1), filename);
		sprintf(filename, "%f", i.controllerParams[1]);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_CPARAM2), filename);

		if(i.scriptOverrideType == SCRIPT_OVERRIDE_ONDEATH)
			SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE1), BM_SETCHECK, 1, 0);
		else if(i.scriptOverrideType == SCRIPT_OVERRIDE_ONAGGRO)
			SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE2), BM_SETCHECK, 1, 0);
		else if(i.scriptOverrideType == SCRIPT_OVERRIDE_ONDAMAGE)
			SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE3), BM_SETCHECK, 1, 0);
		else if(i.scriptOverrideType == SCRIPT_OVERRIDE_ONCOLLISION)
			SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE4), BM_SETCHECK, 1, 0);
		else if(i.scriptOverrideType == SCRIPT_OVERRIDE_CUSTOM)
			SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE5), BM_SETCHECK, 1, 0);
		else if(i.scriptOverrideType == SCRIPT_OVERRIDE_NONE)
			SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE6), BM_SETCHECK, 1, 0);

		SetWindowText(GetDlgItem(hWnd, IDC_OP_REFNAME), i.refName);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_SCRIPT), i.scriptOverride);

		if(i.bRequiresConstantUpdate)
			SendMessage(GetDlgItem(hWnd, IDC_OP_REQUIRES_UPDATE), BM_SETCHECK, 1, 0);
		if(i.bVisibleInGame)
			SendMessage(GetDlgItem(hWnd, IDC_OP_VISIBLE), BM_SETCHECK, 1, 0);

		break;
	case WM_CLOSE:
		p->m_bModified = FALSE;
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			GetWindowText(GetDlgItem(hWnd, IDC_OP_ID), filename, 8);
			i.id = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_OP_TARGETID), filename, 8);
			i.targetID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_OP_PEFFECT), filename, 8);
			i.particleEffectID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_OP_LEFFECT2), filename, 8);
			i.lightEffectID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_OP_CONTROLLER), filename, 8);
			i.controllerID = atoi(filename);

			GetWindowText(GetDlgItem(hWnd, IDC_OP_CPARAM1), filename, 12);
			i.controllerParams[0] = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_OP_CPARAM2), filename, 12);
			i.controllerParams[1] = (float)atof(filename);

			if(SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE1), BM_GETCHECK, 0, 0))
				i.scriptOverrideType = SCRIPT_OVERRIDE_ONDEATH;
			else if(SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE2), BM_GETCHECK, 0, 0))
				i.scriptOverrideType = SCRIPT_OVERRIDE_ONAGGRO;
			else if(SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE3), BM_GETCHECK, 0, 0))
				i.scriptOverrideType = SCRIPT_OVERRIDE_ONDAMAGE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE4), BM_GETCHECK, 0, 0))
				i.scriptOverrideType = SCRIPT_OVERRIDE_ONCOLLISION;
			else if(SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE5), BM_GETCHECK, 0, 0))
				i.scriptOverrideType = SCRIPT_OVERRIDE_CUSTOM;
			else if(SendMessage(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE6), BM_GETCHECK, 0, 0))
				i.scriptOverrideType = SCRIPT_OVERRIDE_NONE;

			GetWindowText(GetDlgItem(hWnd, IDC_OP_REFNAME), i.refName, 31);
			GetWindowText(GetDlgItem(hWnd, IDC_OP_SCRIPT), i.scriptOverride, 31);

			i.bRequiresConstantUpdate = i.bVisibleInGame = false;
			if(SendMessage(GetDlgItem(hWnd, IDC_OP_REQUIRES_UPDATE), BM_GETCHECK, 0, 0))
				i.bRequiresConstantUpdate = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_OP_VISIBLE), BM_GETCHECK, 0, 0))
				i.bVisibleInGame = true;

			p->m_bModified = TRUE;

			EndDialog(hWnd, FALSE);
			break;
		case IDCANCEL:
			p->m_bModified = FALSE;
			EndDialog(hWnd, FALSE);
			break;
		case IDC_OP_BROWSE_TARGET:
			p->BrowseTarget(hWnd);
			break;
		case IDC_OP_BROWSE_SCRIPT:
			p->BrowseScript(hWnd);
			break;
		case IDC_OP_BROWSE_PEFFECT:
			p->BrowsePEffect(hWnd);
			break;
		case IDC_OP_BROWSE_LEFFECT:
			p->BrowseLEffect(hWnd);
			break;
		case IDC_OP_BROWSE_CONTROLLER:
			p->BrowseController(hWnd);
			break;
		case IDC_OP_EFFECT_POS:
			p->SetEffectPos(hWnd);
			break;
		}
	}

	return FALSE;
}

void CObjectPropertiesWnd::SetObjectType(HWND hWnd)
{
	m_type = m_object->GetType();

	if(m_type == STATIC_OBJECT_ID)
	{
		m_soType = ((CStaticObject*)m_object)->GetStaticObjectType();

		SetWindowText(GetDlgItem(hWnd, IDC_OP_OBJECTTYPE), "Static");
		switch(m_soType)
		{
		case STATIC_OBJECT_DEBRIS:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Debris");
			DisableScriptSelection(hWnd);
			break;
		case STATIC_OBJECT_ENERGY_POD:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Energy pod");
			EnableTargetSelection(hWnd, "Energy capacity", true, false);
			DisableScriptSelection(hWnd);
			break;
		case STATIC_OBJECT_REPAIR_POD:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Repair pod");
			EnableTargetSelection(hWnd, "Repair capacity", true, false);
			DisableScriptSelection(hWnd);
			break;
		case STATIC_OBJECT_LEVEL_TELEPORT:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Level teleport");
			DisableScriptSelection(hWnd);
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SCRIPT_MSG), "Target level");
			EnableWindow(GetDlgItem(hWnd, IDC_OP_BROWSE_SCRIPT), TRUE);
			EnableTargetSelection(hWnd, "Target startpos object ID", true, false);
			break;
		case STATIC_OBJECT_NORMAL_TELEPORT:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Teleport");
			DisableScriptSelection(hWnd);
			EnableTargetSelection(hWnd, "Target ID", true, false);
			break;
		case STATIC_OBJECT_GHOST_OBJECT:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Ghost object");
			DisableScriptSelection(hWnd);
			break;
		case STATIC_OBJECT_PLAYER_STARTPOS:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Starting position");
			DisableScriptSelection(hWnd);
			break;
		case STATIC_OBJECT_GADGET:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Gadget");
			EnableTargetSelection(hWnd, "Gadget type ID", true, true);
			break;
		case STATIC_OBJECT_PATCH:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Ship patch");
			EnableTargetSelection(hWnd, "Patch type ID", true, true);
			break;
		case STATIC_OBJECT_COMPONENT:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Gadget component");
			EnableTargetSelection(hWnd, "Component type ID", true, true);
			break;
		case STATIC_OBJECT_QUESTITEM:
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SUBTYPE), "Quest item");
			EnableTargetSelection(hWnd, "Item type ID", true, true);
			break;
		}
	}
	else if(m_type == DYNAMIC_OBJECT_ID)
	{
		SetWindowText(GetDlgItem(hWnd, IDC_OP_OBJECTTYPE), "Dynamic");
	}
	else if(m_type == ENEMY_OBJECT_ID)
	{
		SetWindowText(GetDlgItem(hWnd, IDC_OP_OBJECTTYPE), "Enemy");

		EnableTargetSelection(hWnd, "Route");
	}
}

void CObjectPropertiesWnd::DisableScriptSelection(HWND hWnd)
{
	EnableWindow(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE1), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE2), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE3), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE4), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE5), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_OP_SCRIPTTYPE6), FALSE);

	EnableWindow(GetDlgItem(hWnd, IDC_OP_SCRIPT), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_OP_BROWSE_SCRIPT), FALSE);

	m_template.scriptOverrideType = SCRIPT_OVERRIDE_NONE;
}

void CObjectPropertiesWnd::EnableTargetSelection(HWND hWnd, std::string name, bool editable, bool enableBrowse)
{
	if(enableBrowse)
		EnableWindow(GetDlgItem(hWnd, IDC_OP_BROWSE_TARGET), TRUE);

	if(editable)
		EnableWindow(GetDlgItem(hWnd, IDC_OP_TARGETID), TRUE);

	if(name.size() > 0)
		SetWindowText(GetDlgItem(hWnd, IDC_OP_TARGETID_MSG), name.c_str());
}

void CObjectPropertiesWnd::BrowseTarget(HWND hWnd)
{
	int temp;
	long id;
	char buf[16];
	if(m_type == STATIC_OBJECT_ID)
	{
		switch(m_soType)
		{
		case STATIC_OBJECT_GADGET:
			if(CDatabaseUtilities::SelectGadget(hWnd, id))
			{
				sprintf(buf, "%i", id);
				SetWindowText(GetDlgItem(hWnd, IDC_OP_TARGETID), buf);
			}
			break;
		case STATIC_OBJECT_PATCH:
			if(CDatabaseUtilities::SelectShipPatch(hWnd, id))
			{
				sprintf(buf, "%i", id);
				SetWindowText(GetDlgItem(hWnd, IDC_OP_TARGETID), buf);
			}
			break;
		case STATIC_OBJECT_COMPONENT:
			if(CDatabaseUtilities::SelectComponent(hWnd, id))
			{
				sprintf(buf, "%i", id);
				SetWindowText(GetDlgItem(hWnd, IDC_OP_TARGETID), buf);
			}
			break;
		case STATIC_OBJECT_QUESTITEM:
			if(CDatabaseUtilities::SelectQuestItem(hWnd, id))
			{
				sprintf(buf, "%i", id);
				SetWindowText(GetDlgItem(hWnd, IDC_OP_TARGETID), buf);
			}
			break;
		}
	}
	else if(m_type == ENEMY_OBJECT_ID)
	{
		if(CDatabaseUtilities::SelectWaypointSet(hWnd, temp))
		{
			sprintf(buf, "%i", temp);
			SetWindowText(GetDlgItem(hWnd, IDC_OP_TARGETID), buf);
		}
	}
}

void CObjectPropertiesWnd::BrowseController(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectController(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_CONTROLLER), buf);
	}
}

void CObjectPropertiesWnd::BrowseLEffect(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectLightEffect(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_LEFFECT2), buf);
	}
}

void CObjectPropertiesWnd::BrowsePEffect(HWND hWnd)
{
	long temp;
	char buf[16];
	if(CDatabaseUtilities::SelectParticleEffect(hWnd, temp))
	{
		sprintf(buf, "%i", temp);
		SetWindowText(GetDlgItem(hWnd, IDC_OP_PEFFECT), buf);
	}
}

void CObjectPropertiesWnd::BrowseScript(HWND hWnd)
{
	std::string str;
	long temp;
	if(m_type == STATIC_OBJECT_ID && m_soType == STATIC_OBJECT_LEVEL_TELEPORT)
	{
		CSelectionWnd wnd;
		vector<CCampaign::SCampaignBlock> *blocks = GetOTE()->GetCampaign()->GetBlocks();
		vector<CCampaign::SCampaignBlock>::iterator it = blocks->begin();
		for(; it != blocks->end(); ++it)
			wnd.AddEntry(SSelectionEntry(it->levelFilename, 0));

		if(wnd.OpenWindow(hWnd, temp, "level", &str))
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SCRIPT), str.c_str());
	}
	else
	{
		if(CDatabaseUtilities::SelectLocalScript(hWnd, str))
		{
			SetWindowText(GetDlgItem(hWnd, IDC_OP_SCRIPT), str.c_str());
		}
	}
}

void CObjectPropertiesWnd::SetEffectPos(HWND hWnd)
{
	CGetVectorWnd wnd;
	wnd.OpenWindow(hWnd, m_template.effectPos, "Set effect position");
}

bool CObjectPropertiesWnd::Open(HWND parent, SObjectInstanceInfo *info, IGameObject *object)
{
	if(object == NULL || info == NULL)
		return false;

	m_object = object;
	m_template = *info;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OBJECT_PROPERTIES), parent, WndProc);

	if(m_bModified)
	{
		*info = m_template;
		return TRUE;
	}
	else
		return FALSE;
}