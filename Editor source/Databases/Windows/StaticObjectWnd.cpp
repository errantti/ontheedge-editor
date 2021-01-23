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

#include "StaticObjectWnd.h"
#include "..\\..\\resource.h"
#include "..\\..\\Editor.h"
#include "..\\..\\Tools\\GetActionEventWnd.h"
#include "..\\..\\Tools\\GetVectorWnd.h"

CStaticObjectWnd *CStaticObjectWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaticObjectWnd::CStaticObjectWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CStaticObjectWnd::~CStaticObjectWnd()
{
	m_pointer = NULL;
}


BOOL CALLBACK CStaticObjectWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CStaticObjectWnd *p = CStaticObjectWnd::m_pointer;
	SStaticObjectTemplate &t = p->m_template;
	char filename[128];
	std::string str;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		if(!p->m_bNew)	// This has to be done before setting the target id or it will get erased
			p->SetSelectedType(hWnd, t.type);

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_SO_MESH), t.meshFilename);
		SetWindowText(GetDlgItem(hWnd, IDC_SO_NAME), t.name);

		sprintf(filename, "%f", t.bvCoefficient);
		SetWindowText(GetDlgItem(hWnd, IDC_SO_BVCOEFFICIENT), filename);
		sprintf(filename, "%i", t.targetID);
		SetWindowText(GetDlgItem(hWnd, IDC_SO_TARGET_ID), filename);
		sprintf(filename, "%i", t.templateID);
		SetWindowText(GetDlgItem(hWnd, IDC_SO_ID), filename);
		sprintf(filename, "%i", t.defaultParticleEffectID);
		SetWindowText(GetDlgItem(hWnd, IDC_SO_CONTROLLER), filename);
		sprintf(filename, "%f", t.collisionParam1);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_COLLISIONPARAM), filename);
		sprintf(filename, "%f", t.alpha);
		SetWindowText(GetDlgItem(hWnd, IDC_SO_ALPHA), filename);

		// radio boxes
		if(t.bCollidesWithObjects)
			SendMessage(GetDlgItem(hWnd, IDC_SO_COBJECTS), BM_SETCHECK, 1, 0);
		if(t.bDropWhenLoaded)
			SendMessage(GetDlgItem(hWnd, IDC_SO_DROPLOADED), BM_SETCHECK, 1, 0);

		if(t.collisionType == OBJECT_COLLISION_SPHERE)
			SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_SPHERE), BM_SETCHECK, 1, 0);
		else if(t.collisionType == OBJECT_COLLISION_BOX)
			SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_BOX), BM_SETCHECK, 1, 0);
		else if(t.collisionType == OBJECT_COLLISION_CYLINDER)
			SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_CYLINDER), BM_SETCHECK, 1, 0);

		if(!p->m_bNew)
		{
			// object type
			switch(t.type)
			{
			case STATIC_OBJECT_DEBRIS:
				SendMessage(GetDlgItem(hWnd, IDC_SO_DEBRIS), BM_SETCHECK, 1, 0);
				break;
			case STATIC_OBJECT_ENERGY_POD:
				SendMessage(GetDlgItem(hWnd, IDC_SO_ENERGY_POD), BM_SETCHECK, 1, 0);
				break;
			case STATIC_OBJECT_REPAIR_POD:
				SendMessage(GetDlgItem(hWnd, IDC_SO_REPAIR_POD), BM_SETCHECK, 1, 0);
				break;
			case STATIC_OBJECT_LEVEL_TELEPORT:
				SendMessage(GetDlgItem(hWnd, IDC_SO_LEVEL_TELEPORT), BM_SETCHECK, 1, 0);
				break;
			case STATIC_OBJECT_NORMAL_TELEPORT:
				SendMessage(GetDlgItem(hWnd, IDC_SO_NORMAL_TELEPORT), BM_SETCHECK, 1, 0);
				break;
			case STATIC_OBJECT_GADGET:
				SendMessage(GetDlgItem(hWnd, IDC_SO_GADGET), BM_SETCHECK, 1, 0);
				EnableWindow(GetDlgItem(hWnd, IDC_SO_BROWSE_TARGET), TRUE);
				break;
			case STATIC_OBJECT_GHOST_OBJECT:
				SendMessage(GetDlgItem(hWnd, IDC_SO_GHOSTOBJECT), BM_SETCHECK, 1, 0);
				break;
			case STATIC_OBJECT_PLAYER_STARTPOS:
				SendMessage(GetDlgItem(hWnd, IDC_SO_STARTPOS), BM_SETCHECK, 1, 0);
				break;
			case STATIC_OBJECT_PATCH:
				SendMessage(GetDlgItem(hWnd, IDC_SO_PATCH), BM_SETCHECK, 1, 0);
				EnableWindow(GetDlgItem(hWnd, IDC_SO_BROWSE_TARGET), TRUE);
				break;
			case STATIC_OBJECT_COMPONENT:
				SendMessage(GetDlgItem(hWnd, IDC_SO_COMPONENT), BM_SETCHECK, 1, 0);
				EnableWindow(GetDlgItem(hWnd, IDC_SO_BROWSE_TARGET), TRUE);
				break;
			case STATIC_OBJECT_QUESTITEM:
				SendMessage(GetDlgItem(hWnd, IDC_SO_QUESTITEM), BM_SETCHECK, 1, 0);
				EnableWindow(GetDlgItem(hWnd, IDC_SO_BROWSE_TARGET), TRUE);
				break;
			}
		}

		break;
	case WM_CLOSE:
		p->m_bModified = FALSE;
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_SO_CANCEL:
			p->m_bModified = FALSE;
			EndDialog(hWnd, FALSE);
			break;
		case ID_SO_OK:
			//::memset(&t, 0, sizeof(SStaticObjectTemplate));

			// Get the form information and return success
			GetWindowText(GetDlgItem(hWnd, IDC_SO_NAME), t.name, 64);
			if(strlen(t.name) < 3)
			{
				::MessageBox(hWnd, "Too short object name", "Error", MB_OK);
				break;
			}
			GetWindowText(GetDlgItem(hWnd, IDC_SO_MESH), t.meshFilename, 255);

			GetWindowText(GetDlgItem(hWnd, IDC_SO_BVCOEFFICIENT), filename, 8);
			t.bvCoefficient = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_SO_TARGET_ID), filename, 8);
			t.targetID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_SO_ID), filename, 8);
			t.templateID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_DO_COLLISIONPARAM), filename, 8);
			t.collisionParam1 = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_SO_CONTROLLER), filename, 8);
			t.defaultParticleEffectID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_SO_ALPHA), filename, 8);
			t.alpha = (float)atof(filename);

			t.bCollidesWithObjects = t.bDropWhenLoaded = false;
			if(SendMessage(GetDlgItem(hWnd, IDC_SO_COBJECTS), BM_GETCHECK, 0, 0))
				t.bCollidesWithObjects = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_SO_DROPLOADED), BM_GETCHECK, 0, 0))
				t.bDropWhenLoaded = TRUE;

			if(SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_SPHERE), BM_GETCHECK, 0, 0))
				t.collisionType = OBJECT_COLLISION_SPHERE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_BOX), BM_GETCHECK, 0, 0))
				t.collisionType = OBJECT_COLLISION_BOX;
			else if(SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_CYLINDER), BM_GETCHECK, 0, 0))
				t.collisionType = OBJECT_COLLISION_CYLINDER;

			if(SendMessage(GetDlgItem(hWnd, IDC_SO_DEBRIS), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_DEBRIS;
				strcpy(t.subCategory, "Debris");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_ENERGY_POD), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_ENERGY_POD;
				strcpy(t.subCategory, "Misc");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_REPAIR_POD), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_REPAIR_POD;
				strcpy(t.subCategory, "Misc");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_LEVEL_TELEPORT), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_LEVEL_TELEPORT;
				strcpy(t.subCategory, "Misc");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_NORMAL_TELEPORT), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_NORMAL_TELEPORT;
				strcpy(t.subCategory, "Misc");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_GADGET), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_GADGET;
				strcpy(t.subCategory, "Gadgets");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_GHOSTOBJECT), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_GHOST_OBJECT;
				strcpy(t.subCategory, "Misc");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_STARTPOS), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_PLAYER_STARTPOS;
				strcpy(t.subCategory, "Misc");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_PATCH), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_PATCH;
				strcpy(t.subCategory, "Ship patches");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_COMPONENT), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_COMPONENT;
				strcpy(t.subCategory, "Gadget components");
			}
			else if(SendMessage(GetDlgItem(hWnd, IDC_SO_QUESTITEM), BM_GETCHECK, 0, 0))
			{
				t.type = STATIC_OBJECT_QUESTITEM;
				strcpy(t.subCategory, "Quest items");
			}
			else
			{
				::MessageBox(hWnd, "You haven't selected object type!", "Error", MB_OK);
				break;
			}

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;
		case IDC_SO_BROWSE_MESH:
			// Open common dialogue to browse for mesh file
			if(CDatabaseUtilities::GetMeshFilename(hWnd, str) == FALSE)
				break;

			strcpy(p->m_template.meshFilename, str.c_str());
			SetWindowText(GetDlgItem(hWnd, IDC_SO_MESH), p->m_template.meshFilename);
			break;
		case IDC_SO_REMOVE_MESH:
			t.meshFilename[0] = '\0';
			SetWindowText(GetDlgItem(hWnd, IDC_SO_MESH), "");
			break;
		case IDC_SO_BROWSE_TARGET:
			p->SelectTarget(hWnd);
			break;

		// Type selection
		case IDC_SO_DEBRIS:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_DEBRIS);
			break;
		case IDC_SO_ENERGY_POD:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_ENERGY_POD);
			break;
		case IDC_SO_REPAIR_POD:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_REPAIR_POD);
			break;
		case IDC_SO_LEVEL_TELEPORT:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_LEVEL_TELEPORT);
			break;
		case IDC_SO_NORMAL_TELEPORT:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_NORMAL_TELEPORT);
			break;
		case IDC_SO_GHOSTOBJECT:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_GHOST_OBJECT);
			break;
		case IDC_SO_STARTPOS:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_PLAYER_STARTPOS);
			break;
		case IDC_SO_GADGET:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_GADGET);
			break;
		case IDC_SO_PATCH:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_PATCH);
			break;
		case IDC_SO_COMPONENT:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_COMPONENT);
			break;
		case IDC_SO_QUESTITEM:
			if(HIWORD(wParam) == BN_CLICKED) 
				p->SetSelectedType(hWnd, STATIC_OBJECT_QUESTITEM);
			break;

		case IDC_SO_BROWSE_PEFFECT:
			CDatabaseUtilities::SelectParticleEffect(hWnd, t.defaultParticleEffectID);
			sprintf(filename, "%i", t.defaultParticleEffectID);
			SetWindowText(GetDlgItem(hWnd, IDC_SO_CONTROLLER), filename);
			break;
		case IDC_SO_REMOVE_PEFFECT:
			SetWindowText(GetDlgItem(hWnd, IDC_SO_CONTROLLER), "-1");
			break;

		case IDC_SO_CUSTOMEA:
			p->GetActionEvent(p->m_template.customAction, hWnd, "custom");
			break;

		case IDC_SO_DEFAULT_EFFECT_POS:
			p->SetDefaultEffectPos(hWnd);
			break;
		}
		break;
	}

	return FALSE;
}

void CStaticObjectWnd::GetActionEvent(SActionEvent &ea, HWND hWnd, std::string caption)
{
	CGetActionEventWnd wnd;
	wnd.OpenWindow(hWnd, ea, caption, true, false);
}

void CStaticObjectWnd::SelectTarget(HWND hWnd)
{
	MessageBox(hWnd, "Disabled - set gadget or item type from object properties", "OtE Editor error", MB_OK);
	return;

	/*long id;
	bool succeeded = false;
	if(m_selectedType == STATIC_OBJECT_GADGET)
		succeeded = CDatabaseUtilities::SelectGadget(hWnd, id);
	else if(m_selectedType == STATIC_OBJECT_PATCH)
		succeeded = CDatabaseUtilities::SelectShipPatch(hWnd, id);
	else if(m_selectedType == STATIC_OBJECT_COMPONENT)
		succeeded = CDatabaseUtilities::SelectComponent(hWnd, id);
	else if(m_selectedType == STATIC_OBJECT_QUESTITEM)
		succeeded = CDatabaseUtilities::SelectQuestItem(hWnd, id);

	char buf[16];
	if(succeeded)
	{
		m_template.targetID = id;
		sprintf(buf, "%i", id);
		SetWindowText(GetDlgItem(hWnd, IDC_SO_TARGET_ID), buf);
	}*/
}

void CStaticObjectWnd::SetSelectedType(HWND hWnd, STATIC_OBJECT_TYPE type)
{
	m_selectedType = type;

	switch(type)
	{
	case STATIC_OBJECT_DEBRIS:
	case STATIC_OBJECT_ENERGY_POD:
	case STATIC_OBJECT_REPAIR_POD:
	case STATIC_OBJECT_LEVEL_TELEPORT:
	case STATIC_OBJECT_NORMAL_TELEPORT:
	case STATIC_OBJECT_GHOST_OBJECT:
	case STATIC_OBJECT_PLAYER_STARTPOS:
		EnableWindow(GetDlgItem(hWnd, IDC_SO_BROWSE_TARGET), FALSE);
		SetWindowText(GetDlgItem(hWnd, IDC_SO_TARGET_ID), "-1");
		break;
	case STATIC_OBJECT_GADGET:
	case STATIC_OBJECT_PATCH:
	case STATIC_OBJECT_COMPONENT:
	case STATIC_OBJECT_QUESTITEM:
		EnableWindow(GetDlgItem(hWnd, IDC_SO_BROWSE_TARGET), TRUE);
		SetWindowText(GetDlgItem(hWnd, IDC_SO_TARGET_ID), "-1");
		break;
	}

	// Disable type selection
	DisableTypeSelection(hWnd);
}

void CStaticObjectWnd::DisableTypeSelection(HWND hWnd)
{
	EnableWindow(GetDlgItem(hWnd, IDC_SO_DEBRIS), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_REPAIR_POD), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_ENERGY_POD), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_LEVEL_TELEPORT), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_NORMAL_TELEPORT), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_GHOSTOBJECT), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_STARTPOS), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_GADGET), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_PATCH), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_COMPONENT), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_SO_QUESTITEM), FALSE);
}

void CStaticObjectWnd::SetDefaultEffectPos(HWND hWnd)
{
	CGetVectorWnd wnd;
	wnd.OpenWindow(hWnd, m_template.defaultEffectPos, "Set default effect position");
}

bool CStaticObjectWnd::OpenWindow(HWND parent, SStaticObjectTemplate &rTemplate, bool bNew)
{
	m_bNew = bNew;
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_STATIC_OBJECT), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
