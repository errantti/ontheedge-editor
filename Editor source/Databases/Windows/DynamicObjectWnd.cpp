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

#include "DynamicObjectWnd.h"
#include "..\\..\\resource.h"
#include "..\\..\\Editor.h"
#include "..\\..\\Tools\\GetActionEventWnd.h"
#include "..\\..\\Tools\\GetVectorWnd.h"

CDynamicObjectWnd *CDynamicObjectWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDynamicObjectWnd::CDynamicObjectWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CDynamicObjectWnd::~CDynamicObjectWnd()
{
	m_pointer = NULL;
}


BOOL CALLBACK CDynamicObjectWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CDynamicObjectWnd *p = CDynamicObjectWnd::m_pointer;
	SDynamicObjectTemplate &t = p->m_template;
	char filename[128];
	std::string str;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_DO_MESH), t.meshFilename);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_NAME), t.name);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_SUBCATEGORY), t.subCategory);

		sprintf(filename, "%f", t.mass);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_MASS), filename);
		sprintf(filename, "%f", t.bvCoefficient);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_BCCOEFFICIENT), filename);
		sprintf(filename, "%f", t.dragCoefficient);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_DRAGCOEFFICIENT), filename);
		sprintf(filename, "%i", t.hitPoints);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_HITPOINTS), filename);
		sprintf(filename, "%i", t.templateID);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_ID), filename);
		sprintf(filename, "%i", t.defaultParticleEffectID);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_DEFAULTPEFFECT), filename);
		sprintf(filename, "%f", t.collisionParam1);
		SetWindowText(GetDlgItem(hWnd, IDC_DO_COLLISIONPARAM), filename);

		// radio boxes
		if(t.bIndestructible)
			SendMessage(GetDlgItem(hWnd, IDC_DO_INDESTRUCTIBLE), BM_SETCHECK, 1, 0);
		if(t.bAffectedByDrag)
			SendMessage(GetDlgItem(hWnd, IDC_DO_DRAG), BM_SETCHECK, 1, 0);
		if(t.bAffectedByGravity)
			SendMessage(GetDlgItem(hWnd, IDC_DO_GRAVITY), BM_SETCHECK, 1, 0);
		if(t.bCollidesWithExternalObjects)
			SendMessage(GetDlgItem(hWnd, IDC_DO_CEXTERNAL), BM_SETCHECK, 1, 0);
		if(t.bCollidesWithObjects)
			SendMessage(GetDlgItem(hWnd, IDC_DO_COBJECTS), BM_SETCHECK, 1, 0);
		if(t.bRequiresCollisionDetermination)
			SendMessage(GetDlgItem(hWnd, IDC_DO_REQUIRESC), BM_SETCHECK, 1, 0);

		if(t.bDropWhenLoaded)
			SendMessage(GetDlgItem(hWnd, IDC_DO_DROPWHENLOADED), BM_SETCHECK, 1, 0);

		if(t.collisionType == OBJECT_COLLISION_SPHERE)
			SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_SPHERE), BM_SETCHECK, 1, 0);
		else if(t.collisionType == OBJECT_COLLISION_BOX)
			SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_BOX), BM_SETCHECK, 1, 0);
		else if(t.collisionType == OBJECT_COLLISION_CYLINDER)
			SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_CYLINDER), BM_SETCHECK, 1, 0);
		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_DO_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_DO_OK:
			//::memset(&t, 0, sizeof(SDynamicObjectTemplate));

			// Get the form information and return success
			GetWindowText(GetDlgItem(hWnd, IDC_DO_NAME), t.name, 64);
			if(strlen(t.name) < 3)
			{
				::MessageBox(hWnd, "Too short object name", "Error", MB_OK);
				break;
			}
			GetWindowText(GetDlgItem(hWnd, IDC_DO_MESH), t.meshFilename, 128);
			GetWindowText(GetDlgItem(hWnd, IDC_DO_SUBCATEGORY), t.subCategory, 32);

			GetWindowText(GetDlgItem(hWnd, IDC_DO_MASS), filename, 12);
			t.mass = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_DO_BCCOEFFICIENT), filename, 8);
			t.bvCoefficient = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_DO_DRAGCOEFFICIENT), filename, 8);
			t.dragCoefficient = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_DO_HITPOINTS), filename, 8);
			t.hitPoints = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_DO_ID), filename, 8);
			t.templateID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_DO_DEFAULTPEFFECT), filename, 8);
			t.defaultParticleEffectID = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_DO_COLLISIONPARAM), filename, 12);
			t.collisionParam1 = (float)atof(filename);

			t.bAffectedByDrag = t.bAffectedByGravity = t.bCollidesWithExternalObjects =
				t.bCollidesWithObjects = t.bIndestructible = t.bRequiresCollisionDetermination = FALSE;

			if(SendMessage(GetDlgItem(hWnd, IDC_DO_INDESTRUCTIBLE), BM_GETCHECK, 0, 0))
				t.bIndestructible = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_DO_DRAG), BM_GETCHECK, 0, 0))
				t.bAffectedByDrag = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_DO_GRAVITY), BM_GETCHECK, 0, 0))
				t.bAffectedByGravity = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_DO_CEXTERNAL), BM_GETCHECK, 0, 0))
				t.bCollidesWithExternalObjects = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_DO_COBJECTS), BM_GETCHECK, 0, 0))
				t.bCollidesWithObjects = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_DO_REQUIRESC), BM_GETCHECK, 0, 0))
				t.bRequiresCollisionDetermination = TRUE;

			if(SendMessage(GetDlgItem(hWnd, IDC_DO_DROPWHENLOADED), BM_GETCHECK, 0, 0))
				t.bDropWhenLoaded = TRUE;

			if(SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_SPHERE), BM_GETCHECK, 0, 0))
				t.collisionType = OBJECT_COLLISION_SPHERE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_BOX), BM_GETCHECK, 0, 0))
				t.collisionType = OBJECT_COLLISION_BOX;
			else if(SendMessage(GetDlgItem(hWnd, IDC_DO_COLLISION_CYLINDER), BM_GETCHECK, 0, 0))
				t.collisionType = OBJECT_COLLISION_CYLINDER;

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;
		case IDC_DO_BROWSE_MESH:
			// Open common dialogue to browse for mesh file
			if(CDatabaseUtilities::GetMeshFilename(hWnd, str) == FALSE)
				break;

			strcpy(p->m_template.meshFilename, str.c_str());
			SetWindowText(GetDlgItem(hWnd, IDC_DO_MESH), p->m_template.meshFilename);
			break;
		case IDC_DO_REMOVE_MESH:
			t.meshFilename[0] = '\0';
			SetWindowText(GetDlgItem(hWnd, IDC_DO_MESH), "");
			break;

		case IDC_DO_BROWSE_DEFAULTPEFFECT:
			CDatabaseUtilities::SelectParticleEffect(hWnd, t.defaultParticleEffectID);
			sprintf(filename, "%i", t.defaultParticleEffectID);
			SetWindowText(GetDlgItem(hWnd, IDC_DO_DEFAULTPEFFECT), filename);
			break;

		case IDC_DO_EA_DEATH:
			p->GetActionEvent(p->m_template.onDestruction, hWnd, "on death");
			break;
		case IDC_DO_EA_DAMAGE:
			p->GetActionEvent(p->m_template.onDamageTaken, hWnd, "on damage taken");
			break;
		case IDC_DO_EA_COLLISION:
			p->GetActionEvent(p->m_template.onCollision, hWnd, "on collision");
			break;

		case IDC_DO_DEFAULT_EFFECT_POS:
			p->SetDefaultEffectPos(hWnd);
			break;
		}
		break;
	}

	return FALSE;
}

void CDynamicObjectWnd::GetActionEvent(SActionEvent &ea, HWND hWnd, std::string caption)
{
	CGetActionEventWnd wnd;
	wnd.OpenWindow(hWnd, ea, caption, true, false);
}

void CDynamicObjectWnd::SetDefaultEffectPos(HWND hWnd)
{
	CGetVectorWnd wnd;
	wnd.OpenWindow(hWnd, m_template.defaultEffectPos, "Set default effect position");
}

bool CDynamicObjectWnd::OpenWindow(HWND parent, SDynamicObjectTemplate &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DYNAMIC_OBJECT), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
