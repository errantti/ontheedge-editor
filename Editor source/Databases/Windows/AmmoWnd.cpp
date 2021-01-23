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

#include "AmmoWnd.h"
#include "..\\..\\resource.h"
#include "..\\..\\OtE\\OnTheEdge.h"
#include "..\\DatabaseUtilities.h"

CAmmoWnd *CAmmoWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAmmoWnd::CAmmoWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CAmmoWnd::~CAmmoWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CAmmoWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CAmmoWnd *p = CAmmoWnd::m_pointer;
	SAmmoTemplate &t = p->m_template;
	char szTemp[128];
	std::string str;
	long id;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_AMMO_NAME), t.name);
		SetWindowText(GetDlgItem(hWnd, IDC_AMMO_MESH), t.meshFilename);

		sprintf(szTemp, "%f", t.mass);
		SetWindowText(GetDlgItem(hWnd, IDC_AMMO_MASS), szTemp);

		sprintf(szTemp, "%i", t.particleEffectID);
		SetWindowText(GetDlgItem(hWnd, IDC_AMMO_PEFFECT), szTemp);
		sprintf(szTemp, "%i", t.ID);
		SetWindowText(GetDlgItem(hWnd, IDC_AMMO_ID), szTemp);
		sprintf(szTemp, "%i", t.controllerID);
		SetWindowText(GetDlgItem(hWnd, IDC_AMMO_CONTROLLER), szTemp);

		if(t.bAffectedByGravity)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_GRAVITY), BM_SETCHECK, 1, 0);
		if(t.bCollidesWithStaticObjects)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_COLLIDESSTATIC), BM_SETCHECK, 1, 0);

		if(t.damageType == DAMAGE_TYPE_ENERGY)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_ENERGY), BM_SETCHECK, 1, 0);
		else if(t.damageType == DAMAGE_TYPE_PLASMA)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PLASMA), BM_SETCHECK, 1, 0);
		else if(t.damageType == DAMAGE_TYPE_PROJECTILE)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PROJECTILE), BM_SETCHECK, 1, 0);
		else if(t.damageType == DAMAGE_TYPE_STUN)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_STUN), BM_SETCHECK, 1, 0);
		else if(t.damageType == DAMAGE_TYPE_KINETIC)
			SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_KINETIC), BM_SETCHECK, 1, 0);
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
			GetWindowText(GetDlgItem(hWnd, IDC_AMMO_NAME), t.name, 63);
			if(strlen(t.name) < 3)
			{
				::MessageBox(hWnd, "Too short name", "Error", MB_OK);
				break;
			}
			GetWindowText(GetDlgItem(hWnd, IDC_AMMO_MESH), t.meshFilename, 127);

			GetWindowText(GetDlgItem(hWnd, IDC_AMMO_MASS), szTemp, 12);
			t.mass = (float)atof(szTemp);

			GetWindowText(GetDlgItem(hWnd, IDC_AMMO_PEFFECT), szTemp, 8);
			t.particleEffectID = atoi(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_AMMO_ID), szTemp, 8);
			t.ID = atoi(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_AMMO_CONTROLLER), szTemp, 8);
			t.controllerID = atoi(szTemp);

			t.bAffectedByGravity = t.bCollidesWithStaticObjects = FALSE;
			if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_GRAVITY), BM_GETCHECK, 0, 0))
				t.bAffectedByGravity = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_COLLIDESSTATIC), BM_GETCHECK, 0, 0))
				t.bCollidesWithStaticObjects = TRUE;

			if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_ENERGY), BM_GETCHECK, 0, 0))
				t.damageType = DAMAGE_TYPE_ENERGY;
			else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PLASMA), BM_GETCHECK, 0, 0))
				t.damageType = DAMAGE_TYPE_PLASMA;
			else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PROJECTILE), BM_GETCHECK, 0, 0))
				t.damageType = DAMAGE_TYPE_PROJECTILE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_STUN), BM_GETCHECK, 0, 0))
				t.damageType = DAMAGE_TYPE_STUN;
			else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_KINETIC), BM_GETCHECK, 0, 0))
				t.damageType = DAMAGE_TYPE_KINETIC;

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_AMMO_BROWSEMESH:
			if(CDatabaseUtilities::GetMeshFilename(hWnd, str) == FALSE)
				break;

			strcpy(p->m_template.meshFilename, str.c_str());
			SetWindowText(GetDlgItem(hWnd, IDC_AMMO_MESH), p->m_template.meshFilename);
			break;
		case IDC_AMMO_REMOVEMESH:
			SetWindowText(GetDlgItem(hWnd, IDC_AMMO_MESH), "");
			break;

		case IDC_AMMO_BROWSEPEFFECT:
			if(!CDatabaseUtilities::SelectParticleEffect(hWnd, id))
				break;
			sprintf(szTemp, "%i", id);
			SetWindowText(GetDlgItem(hWnd, IDC_AMMO_PEFFECT), szTemp);
			break;
		case IDC_AMMO_REMOVEPEFFECT:
			SetWindowText(GetDlgItem(hWnd, IDC_AMMO_PEFFECT), "-1");
			break;

		case IDC_AMMO_BROWSECONTROLLER:
			// TODO
			break;
		case IDC_AMMO_REMOVECONTROLLER:
			SetWindowText(GetDlgItem(hWnd, IDC_AMMO_CONTROLLER), "-1");
			break;
		}
		break;
	}

	return FALSE;
}

bool CAmmoWnd::OpenWindow(HWND parent, SAmmoTemplate &rTemplate, bool bNew)
{
	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_AMMO), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
