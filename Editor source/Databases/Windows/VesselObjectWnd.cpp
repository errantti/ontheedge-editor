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

#include "VesselObjectWnd.h"
#include "..\\..\\resource.h"
#include "..\\..\\Editor.h"
#include "DynamicObjectWnd.h"
#include "..\\..\\Tools\\GetActionEventWnd.h"
#include "..\\..\\Tools\\GetVectorWnd.h"

CVesselObjectWnd *CVesselObjectWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVesselObjectWnd::CVesselObjectWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CVesselObjectWnd::~CVesselObjectWnd()
{
	m_pointer = NULL;
}

void CVesselObjectWnd::OpenDynamicObjectProperties(HWND hWnd)
{
	CDynamicObjectWnd dw;
	dw.OpenWindow(hWnd, m_template.dt, FALSE);
}

BOOL CALLBACK CVesselObjectWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CVesselObjectWnd *p = CVesselObjectWnd::m_pointer;
	SVesselTemplate &t = p->m_template;
	char filename[128];
	std::string str;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		sprintf(filename, "%f", t.thrust);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_THRUST), filename);
		sprintf(filename, "%f", t.levitationForce);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_LFORCE), filename);
		sprintf(filename, "%f", t.zeroDistance);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_ZERO_DIST), filename);
		sprintf(filename, "%f", t.levitationCoefficient);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_LCOEF), filename);
		sprintf(filename, "%f", t.turnSpeed);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_TURN_SPEED), filename);
		sprintf(filename, "%f", t.baseEnergy);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_ENERGY), filename);
		sprintf(filename, "%f", t.baseEnergyRegeneration);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_ENERGY_REG), filename);

		// Explosion
		sprintf(filename, "%f", t.explosion.damage);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_EXP_DAMAGE), filename);
		sprintf(filename, "%f", t.explosion.force);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_EXP_FORCE), filename);
		sprintf(filename, "%f", t.explosion.radius);
		SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_EXP_RADIUS), filename);

		break;

	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_VESSEL_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_VESSEL_OK:
			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_THRUST), filename, 12);
			t.thrust = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_LFORCE), filename, 12);
			t.levitationForce = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_ZERO_DIST), filename, 12);
			t.zeroDistance = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_LCOEF), filename, 12);
			t.levitationCoefficient = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_TURN_SPEED), filename, 12);
			t.turnSpeed = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_ENERGY), filename, 12);
			t.baseEnergy = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_ENERGY_REG), filename, 12);
			t.baseEnergyRegeneration = (float)atof(filename);

			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_EXP_DAMAGE), filename, 12);
			t.explosion.damage = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_EXP_FORCE), filename, 12);
			t.explosion.force = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_VESSEL_EXP_RADIUS), filename, 12);
			t.explosion.radius = (float)atof(filename);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case ID_VESSEL_DYNAMIC_OBJECT:
			p->OpenDynamicObjectProperties(hWnd);
			break;

		case IDC_VESSEL_AMMOSPAWN:
			p->GetAmmoSpawnAdjustment(hWnd);
			break;

		case IDC_VESSEL_ACCELERATE:
			p->GetPartialActionEvent(p->m_template.peIDAccelerate, p->m_template.soundIDAccelerate, hWnd, "acceleration");
			break;
		case IDC_VESSEL_BRAKE:
			p->GetPartialActionEvent(p->m_template.peIDBrake, p->m_template.soundIDBreak, hWnd, "braking");
			break;
		case IDC_VESSEL_TURN:
			p->GetPartialActionEvent(p->m_template.peIDTurn, p->m_template.soundIDTurn, hWnd, "turning");
			break;

		/*case ID_VESSEL_BROWSE_EFFECT:
			id = p->m_template.explosionTypeID;
			CDatabaseUtilities::SelectParticleEffect(hWnd,id);
			sprintf(filename, "%i", id);
			SetWindowText(GetDlgItem(hWnd, IDC_VESSEL_EXP_EFFECT_ID), filename);
			break;*/
		}
		break;
	}

	return FALSE;
}

void CVesselObjectWnd::GetPartialActionEvent(long &partID, long &soundID, HWND hWnd, std::string caption)
{
	bool fields[3] = {true, true, false};
	SActionEvent ae;
	ae.FillDefaults();
	ae.particleEffectID = partID;
	ae.soundID = soundID;
	CGetActionEventWnd wnd;
	if(wnd.OpenWindow(hWnd, ae, caption, true, fields))
	{
		partID = ae.particleEffectID;
		soundID = ae.soundID;
	}
}

void CVesselObjectWnd::GetAmmoSpawnAdjustment(HWND hWnd)
{
	CGetVectorWnd wnd;
	wnd.OpenWindow(hWnd, m_template.ammoSpawnAdjustment, "Set ammo spawn position adjustment");
}

bool CVesselObjectWnd::OpenWindow(HWND parent, SVesselTemplate &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VESSEL), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
