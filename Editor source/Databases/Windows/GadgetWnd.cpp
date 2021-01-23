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

#include "GadgetWnd.h"
#include "..\\..\\resource.h"
#include "..\\DatabaseUtilities.h"

#include "..\\..\\OtE\\Gadgets\\WeaponGadget.h"
#include "..\\..\\OtE\\Gadgets\\KineticGadget.h"
#include "..\\..\\OtE\\Gadgets\\HoverGadget.h"
#include "..\\..\\OtE\\Gadgets\\PossessGadget.h"
#include "..\\..\\OtE\\Gadgets\\ShieldGadget.h"
#include "..\\..\\OtE\\Gadgets\\StealthGadget.h"

CGadgetWnd *CGadgetWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGadgetWnd::CGadgetWnd() : m_bAmmoWeapon(true)
{
	m_pointer = this;
}

CGadgetWnd::~CGadgetWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CGadgetWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CGadgetWnd *p = CGadgetWnd::m_pointer;
	SGadgetTemplate &g = p->m_template.first;
	SCustomGadgetTemplate *c = p->m_template.second;

	std::string str;
	long temp;
	char szTemp[128];

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		if(!p->m_bNew)
		{
			p->DisableTypeSelection(hWnd);
			p->FillFormInfo(hWnd);
			p->m_bTypeSelected = TRUE;
		}

		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_GADGET_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_GADGET_OK:
			GetWindowText(GetDlgItem(hWnd, IDC_GADGET_NAME), szTemp, GADGET_NAME_LENGTH-1);
			if(strlen(szTemp) < 3)
			{
				::MessageBox(hWnd, "Too short gadget name", "Error", MB_OK);
				break;
			}
			// Get the form information and return success
			p->FillTemplateFromForm(hWnd);

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		/**
		 * Browse buttons
		 **/
		case IDC_GADGET_BROWSEICON:
			CDatabaseUtilities::GetGadgetIconFilename(hWnd, str);
			SetWindowText(GetDlgItem(hWnd, IDC_GADGET_ICON), str.c_str());
			break;
		case IDC_GADGET_REMOVEICON:
			SetWindowText(GetDlgItem(hWnd, IDC_GADGET_ICON), "");
			break;
		case IDC_GADGET_WBROWSEAMMO:
			if(p->m_bAmmoWeapon)
			{
				if(!CDatabaseUtilities::SelectAmmo(hWnd, temp))
					break;
			}
			else
			{
				if(!CDatabaseUtilities::SelectParticleEffect(hWnd, temp))
					break;
			}
			sprintf(szTemp, "%i", temp);
			SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WAMMOID), szTemp);
			break;
		case IDC_GADGET_WPEFFECTBROWSE:
			if(!CDatabaseUtilities::SelectParticleEffect(hWnd, temp))
				break;
			sprintf(szTemp, "%i", temp);
			SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WPEFFECT), szTemp);
			break;
		case IDC_GADGET_PBROWSEPEFFECT:
			if(!CDatabaseUtilities::SelectParticleEffect(hWnd, temp))
				break;
			sprintf(szTemp, "%i", temp);
			SetWindowText(GetDlgItem(hWnd, IDC_GADGET_PPEFFECT), szTemp);
			break;

		case IDC_GADGET_HITSCRIPTBROWSE:
			if(CDatabaseUtilities::SelectGlobalScript(hWnd, str))
				SetWindowText(GetDlgItem(hWnd, IDC_GADGET_HITSCRIPT), str.c_str());
			break;

		case IDC_GADGET_HITSOUNDBROWSE:
			if(CDatabaseUtilities::SelectSound(hWnd, temp))
			{
				sprintf(szTemp, "%i", temp);
				SetWindowText(GetDlgItem(hWnd, IDC_GADGET_HITSOUND), szTemp);
			}
			break;

		case IDC_GADGET_BROWSESCRIPT:
			if(CDatabaseUtilities::SelectGlobalScript(hWnd, str))
				SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SCRIPT), str.c_str());
			break;
		case IDC_GADGET_REMOVESCRIPT:
			SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SCRIPT), "");
			break;
		case IDC_GADGET_BROWSESOUND:
			if(CDatabaseUtilities::SelectSound(hWnd, temp))
			{
				sprintf(szTemp, "%i", temp);
				SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SOUND), szTemp);
			}
			break;
		case IDC_GADGET_REMOVESOUND:
			SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SOUND), "-1");
			break;

		case IDC_GADGET_SEFFECTBROWSE:
			if(!CDatabaseUtilities::SelectParticleEffect(hWnd, temp))
				break;
			sprintf(szTemp, "%i", temp);
			SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SPEFFECT), szTemp);
			break;

		/**
		 * Gadget type selection
		 * These are called only when the gadget selection is made (other information hasn't been filled)
		 * When the selection is made the type option buttons will be disabled and the form info filled
		 * with the default info for the selected type of gadget
		 **/
		case IDC_GADGET_TYPE_WEAPON:
			//p->m_template.FillDefaultWeapon();
			p->m_template.second = new SWeaponGadgetTemplate();
			p->m_template.second->FillDefaults();
			g.FillDefaults();
			g.gadgetType = GADGET_TYPE_WEAPON;
			p->DisableTypeSelection(hWnd);
			p->FillFormInfo(hWnd);
			p->m_bTypeSelected = TRUE;
			break;
		case IDC_GADGET_TYPE_KINETIC:
			//p->m_template.FillDefaultPush();
			p->m_template.second = new SKineticGadgetTemplate();
			p->m_template.second->FillDefaults();
			g.FillDefaults();
			g.gadgetType = GADGET_TYPE_KINETIC;
			p->DisableTypeSelection(hWnd);
			p->FillFormInfo(hWnd);
			p->m_bTypeSelected = TRUE;
			break;
		case IDC_GADGET_TYPE_HOVER:
			p->m_template.second = new SHoverGadgetTemplate();
			p->m_template.second->FillDefaults();
			g.FillDefaults();
			g.gadgetType = GADGET_TYPE_HOVER;
			p->DisableTypeSelection(hWnd);
			p->FillFormInfo(hWnd);
			p->m_bTypeSelected = TRUE;
			break;
		case IDC_GADGET_TYPE_SHIELD:
			p->m_template.second = new SShieldGadgetTemplate();
			p->m_template.second->FillDefaults();
			g.FillDefaults();
			g.gadgetType = GADGET_TYPE_SHIELD;
			p->DisableTypeSelection(hWnd);
			p->FillFormInfo(hWnd);
			p->m_bTypeSelected = TRUE;
			break;
		case IDC_GADGET_TYPE_STEALTH:
			p->m_template.second = new SStealthGadgetTemplate();
			p->m_template.second->FillDefaults();
			g.FillDefaults();
			g.gadgetType = GADGET_TYPE_STEALTH;
			p->DisableTypeSelection(hWnd);
			p->FillFormInfo(hWnd);
			p->m_bTypeSelected = TRUE;
			break;
		case IDC_GADGET_TYPE_MIND_CONTROL:
			p->m_template.second = new SPossessGadgetTemplate();
			p->m_template.second->FillDefaults();
			g.FillDefaults();
			g.gadgetType = GADGET_TYPE_POSSESS;
			p->DisableTypeSelection(hWnd);
			p->FillFormInfo(hWnd);
			p->m_bTypeSelected = TRUE;
			break;

		case IDC_GADGET_WFORWARDA:
		case IDC_GADGET_WNOVA:
		case IDC_GADGET_WNOVAA:
		case IDC_GADGET_WBACKWARD:
		case IDC_GADGET_WFORWARD:
			p->SelectWeaponType(WEAPON_MODE_FORWARD, hWnd, false);
			break;
		case IDC_GADGET_WTHROWER:
			p->SelectWeaponType(WEAPON_MODE_THROWER, hWnd, false);
			break;
		}
		break;
	}

	return FALSE;
}

bool CGadgetWnd::OpenWindow(HWND parent, std::pair<SGadgetTemplate, SCustomGadgetTemplate*> &rTemplate, bool bNew)
{
	//m_bNew = bNew;
	m_bTypeSelected = FALSE;

	if(rTemplate.second == NULL)
		m_bNew = true;
	else
		m_bNew = false;

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GADGET), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
	{
		if(m_bNew && m_template.second)
			delete m_template.second;
		return FALSE;
	}
}

void CGadgetWnd::DisableTypeSelection(HWND hWnd)
{
	// Disable type selection...
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_TYPE_WEAPON), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_TYPE_KINETIC), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_TYPE_MIND_CONTROL), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_TYPE_STEALTH), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_TYPE_SHIELD), FALSE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_TYPE_HOVER), FALSE);

	// ...and enable general settings and ok-button
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_DESCRIPTION), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_NAME), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_BROWSEICON), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_REMOVEICON), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_MULTIPLAYER), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_ONEUSE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_ENEMY), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_MASS), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_BROWSESOUND), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_REMOVESOUND), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_BROWSESCRIPT), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_REMOVESCRIPT), TRUE);
	EnableWindow(GetDlgItem(hWnd, ID_GADGET_OK), TRUE);
}

void CGadgetWnd::FillFormInfo(HWND hWnd)
{
	FillGeneralFormInfo(hWnd);

	SGadgetTemplate &g = m_template.first;
	SCustomGadgetTemplate *c = m_template.second;

	if(c == NULL)
	{
		CPrinter::logError("Trying to fill form from unselected gadget type");
		return;
	}

	SHoverGadgetTemplate *hover = (SHoverGadgetTemplate*)c;
	SStealthGadgetTemplate *stealth = (SStealthGadgetTemplate*)c;
	SPossessGadgetTemplate *possess = (SPossessGadgetTemplate*)c;

	char szTemp[128];
	switch(g.gadgetType)
	{
	case GADGET_TYPE_WEAPON:
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_TYPE_WEAPON), BM_SETCHECK, 1, 0);
		FillWeaponFormInfo(hWnd);
		g.bActivateable = false;
		break;

	case GADGET_TYPE_KINETIC:
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_TYPE_KINETIC), BM_SETCHECK, 1, 0);
		FillKineticGunFormInfo(hWnd);
		g.bActivateable = false;
		break;

	case GADGET_TYPE_SHIELD:
		g.bActivateable = true;
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_TYPE_SHIELD), BM_SETCHECK, 1, 0);
		FillShieldFormInfo(hWnd);
		break;

	case GADGET_TYPE_HOVER:
		g.bActivateable = false;
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_TYPE_HOVER), BM_SETCHECK, 1, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_GADGET_HFORCE), TRUE);
		EnableWindow(GetDlgItem(hWnd, IDC_GADGET_HEC), TRUE);

		sprintf(szTemp, "%f", hover->force);
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_HFORCE), szTemp);
		sprintf(szTemp, "%f", hover->energyConsumption);
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_HEC), szTemp);
		break;

	case GADGET_TYPE_STEALTH:
		g.bActivateable = true;
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_TYPE_ENGINE), BM_SETCHECK, 1, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_GADGET_STEALTH_COEF), TRUE);
		EnableWindow(GetDlgItem(hWnd, IDC_GADGET_STEALTH_ENERGY), TRUE);

		sprintf(szTemp, "%f", stealth->coefficient);
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_STEALTH_COEF), szTemp);
		sprintf(szTemp, "%f", stealth->energyConsumption);
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_STEALTH_ENERGY), szTemp);
		break;

	case GADGET_TYPE_POSSESS:
		g.bActivateable = true;
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_TYPE_MIND_CONTROL), BM_SETCHECK, 1, 0);
		EnableWindow(GetDlgItem(hWnd, IDC_GADGET_POSEC), TRUE);
		EnableWindow(GetDlgItem(hWnd, IDC_GADGET_POSRADIUS), TRUE);
		EnableWindow(GetDlgItem(hWnd, IDC_GADGET_POSDIST), TRUE);

		sprintf(szTemp, "%f", possess->energyConsumption);
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_POSEC), szTemp);
		sprintf(szTemp, "%f", possess->effectRadius);
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_POSRADIUS), szTemp);
		sprintf(szTemp, "%f", possess->maintainDistance);
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_POSDIST), szTemp);
		break;
	}
}

void CGadgetWnd::FillGeneralFormInfo(HWND hWnd)
{
	char szTemp[128];

	SGadgetTemplate &g = m_template.first;

	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_NAME), g.name);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_ICON), g.icon);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_DESCRIPTION), g.description);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SCRIPT), g.activationScript);

	sprintf(szTemp, "%i", g.ID);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_ID), szTemp);
	sprintf(szTemp, "%i", g.activationSoundID);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SOUND), szTemp);

	sprintf(szTemp, "%f", g.capacityRequirement);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_MASS), szTemp);

	if(g.bMultiplayer)
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_MULTIPLAYER), BM_SETCHECK, 1, 0);
	if(g.bOneUse)
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_ONEUSE), BM_SETCHECK, 1, 0);
	if(g.useFlags & GADGET_USABLE_ENEMY)
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_ENEMY), BM_SETCHECK, 1, 0);
}

void CGadgetWnd::FillWeaponFormInfo(HWND hWnd)
{
	char szTemp[128];

	SWeaponGadgetTemplate *w = (SWeaponGadgetTemplate*)m_template.second;

	// Enable all weapon controls
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WFORWARD), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WFORWARDA), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WNOVA), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WNOVAA), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WBACKWARD), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WTHROWER), TRUE);

	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WDAMAGE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WEXPRADIUS), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WBEXPLODES), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WBEXPLODES2), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_HITPEFFECT), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_HITPEFFECTBROWSE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_HITSOUND), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_HITSOUNDBROWSE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_HITSCRIPT), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_HITSCRIPTBROWSE), TRUE);

	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WEC), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WFIREPACE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WSPEED), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WANGLE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WLIFETIME), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WAMMOCOUNT), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WAMMOID), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_WBROWSEAMMO), TRUE);

	// Fill the values
	switch(w->mode)
	{
	case WEAPON_MODE_FORWARD:
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_WFORWARD), BM_SETCHECK, 1, 0);
		break;
	case WEAPON_MODE_FORWARD_ALTERNATING:
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_WFORWARDA), BM_SETCHECK, 1, 0);
		break;
	case WEAPON_MODE_NOVA:
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_WNOVA), BM_SETCHECK, 1, 0);
		break;
	case WEAPON_MODE_NOVA_ALTERNATING:
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_WNOVAA), BM_SETCHECK, 1, 0);
		break;
	case WEAPON_MODE_BACKWARD:
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_WBACKWARD), BM_SETCHECK, 1, 0);
		break;
	case WEAPON_MODE_THROWER:
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_WTHROWER), BM_SETCHECK, 1, 0);
		break;
	case WEAPON_MODE_RAY:
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_WBEAM), BM_SETCHECK, 1, 0);
		break;
	}

	SelectWeaponType(w->mode, hWnd, true);

	sprintf(szTemp, "%f", w->damage);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WDAMAGE), szTemp);
	sprintf(szTemp, "%f", w->explosion.radius);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WEXPRADIUS), szTemp);
	sprintf(szTemp, "%i", w->hitEffect);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_HITPEFFECT), szTemp);
	sprintf(szTemp, "%i", w->hitSound);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_HITSOUND), szTemp);

	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_HITSCRIPT), w->hitScript);

	if(w->bExplosive)
	{
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_WBEXPLODES), BM_SETCHECK, 1, 0);
		sprintf(szTemp, "%f", w->explosion.damage);
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WDAMAGE), szTemp);
	}
	if(w->bExplodeOnTimerEnd)
		SendMessage(GetDlgItem(hWnd, IDC_GADGET_WBEXPLODES2), BM_SETCHECK, 1, 0);

	sprintf(szTemp, "%f", w->energyConsumption);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WEC), szTemp);
	sprintf(szTemp, "%f", w->firingPace);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WFIREPACE), szTemp);
	sprintf(szTemp, "%f", w->force);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WSPEED), szTemp);
	sprintf(szTemp, "%f", w->lifetime);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WLIFETIME), szTemp);
	sprintf(szTemp, "%f", w->angle);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WANGLE), szTemp);

	sprintf(szTemp, "%i", w->numBullets);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WAMMOCOUNT), szTemp);
	sprintf(szTemp, "%i", w->ammoTemplateID);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WAMMOID), szTemp);
}

void CGadgetWnd::FillKineticGunFormInfo(HWND hWnd)
{
	char szTemp[128];

	SKineticGadgetTemplate *k = (SKineticGadgetTemplate*)m_template.second;

	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_PFORCE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_PEC), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_PFIREPACE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_PANGLE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_PRADIUS), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_PPEFFECT), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_PBROWSEPEFFECT), TRUE);

	sprintf(szTemp, "%f", k->force);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_PFORCE), szTemp);
	sprintf(szTemp, "%f", k->energyConsumption);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_PEC), szTemp);
	sprintf(szTemp, "%f", k->firingPace);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_PFIREPACE), szTemp);
	sprintf(szTemp, "%f", k->angle);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_PANGLE), szTemp);
	sprintf(szTemp, "%f", k->radius);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_PRADIUS), szTemp);

	sprintf(szTemp, "%i", k->particleEffectID);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_PPEFFECT), szTemp);
}

void CGadgetWnd::FillShieldFormInfo(HWND hWnd)
{
	char szTemp[128];

	SShieldGadgetTemplate *s = (SShieldGadgetTemplate*)m_template.second;

	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_SEC), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_SDC), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_SPEFFECT), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_GADGET_SEFFECTBROWSE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_AMMO_TYPE_ENERGY), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_AMMO_TYPE_PROJECTILE), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_AMMO_TYPE_STUN), TRUE);
	EnableWindow(GetDlgItem(hWnd, IDC_AMMO_TYPE_PLASMA), TRUE);

	sprintf(szTemp, "%f", s->coefficient);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SDC), szTemp);
	sprintf(szTemp, "%f", s->energyConsumption);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SEC), szTemp);
	sprintf(szTemp, "%i", s->particleEffectID);
	SetWindowText(GetDlgItem(hWnd, IDC_GADGET_SPEFFECT), szTemp);

	switch(s->type)
	{
	case DAMAGE_TYPE_ENERGY:
		SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_ENERGY), BM_SETCHECK, 1, 0);
		break;
	case DAMAGE_TYPE_PROJECTILE:
		SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PROJECTILE), BM_SETCHECK, 1, 0);
		break;
	case DAMAGE_TYPE_STUN:
		SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_STUN), BM_SETCHECK, 1, 0);
		break;
	case DAMAGE_TYPE_PLASMA:
		SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PLASMA), BM_SETCHECK, 1, 0);
		break;
	}
}


/**************** 
 * Filling the template structure from form
 ****************/

void CGadgetWnd::FillTemplateFromForm(HWND hWnd)
{
	char szTemp[128];

	SGadgetTemplate &g = m_template.first;
	SCustomGadgetTemplate *c = m_template.second;

	if(c == NULL)
	{
		CPrinter::logError("Couldn't save gadget changes: gadget type dependent data didn't exist");
		return;
	}

	SHoverGadgetTemplate *hover = (SHoverGadgetTemplate*)c;
	SStealthGadgetTemplate *stealth = (SStealthGadgetTemplate*)c;
	SPossessGadgetTemplate *possess = (SPossessGadgetTemplate*)c;

	FillGeneralTemplateInfo(hWnd);

	// Don't attempt reading the option box selection becouse it can't be changed
	// after being set for the first time. If it differs from g.gadgetType then
	// there's something wrong anyway
	switch(g.gadgetType)
	{
	case GADGET_TYPE_WEAPON:
		FillWeaponTemplateInfo(hWnd);
		break;

	case GADGET_TYPE_KINETIC:
		FillKineticGunTemplateInfo(hWnd);
		break;

	case GADGET_TYPE_SHIELD:
		FillShieldTemplateInfo(hWnd);
		break;

	case GADGET_TYPE_STEALTH:
		GetWindowText(GetDlgItem(hWnd, IDC_GADGET_STEALTH_COEF), szTemp, 12);
		stealth->coefficient = (float)atof(szTemp);
		GetWindowText(GetDlgItem(hWnd, IDC_GADGET_STEALTH_ENERGY), szTemp, 12);
		stealth->energyConsumption = (float)atof(szTemp);
		break;

	case GADGET_TYPE_HOVER:
		GetWindowText(GetDlgItem(hWnd, IDC_GADGET_HFORCE), szTemp, 12);
		hover->force = (float)atof(szTemp);
		GetWindowText(GetDlgItem(hWnd, IDC_GADGET_HEC), szTemp, 12);
		hover->energyConsumption = (float)atof(szTemp);
		break;

	case GADGET_TYPE_POSSESS:
		GetWindowText(GetDlgItem(hWnd, IDC_GADGET_POSEC), szTemp, 12);
		possess->energyConsumption = (float)atof(szTemp);
		GetWindowText(GetDlgItem(hWnd, IDC_GADGET_POSRADIUS), szTemp, 12);
		possess->effectRadius = (float)atof(szTemp);
		GetWindowText(GetDlgItem(hWnd, IDC_GADGET_POSDIST), szTemp, 12);
		possess->maintainDistance = (float)atof(szTemp);
		break;
	}
}

void CGadgetWnd::FillGeneralTemplateInfo(HWND hWnd)
{
	char szTemp[128];

	SGadgetTemplate &g = m_template.first;

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_NAME), g.name, GADGET_NAME_LENGTH-1);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_ICON), g.icon, 127);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_DESCRIPTION), g.description, GADGET_DESCRIPTION_LENGTH-1);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_SCRIPT), g.activationScript, 31);

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_ID), szTemp, 8);
	g.ID = atoi(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_SOUND), szTemp, 8);
	g.activationSoundID = atoi(szTemp);

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_MASS), szTemp, 12);
	g.capacityRequirement = (float)atof(szTemp);

	g.bOneUse = g.bMultiplayer = FALSE;
	g.useFlags = GADGET_USABLE_SINGLEPLAYER;
	if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_ONEUSE), BM_GETCHECK, 0, 0))
		g.bOneUse = TRUE;
	if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_MULTIPLAYER), BM_GETCHECK, 0, 0))
		g.bMultiplayer = TRUE;
	if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_ENEMY), BM_GETCHECK, 0, 0))
		g.useFlags = GADGET_USABLE_ENEMY;
}

void CGadgetWnd::FillWeaponTemplateInfo(HWND hWnd)
{
	char szTemp[128];
	SWeaponGadgetTemplate *w = (SWeaponGadgetTemplate*)m_template.second;

	if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_WFORWARD), BM_GETCHECK, 0, 0))
		w->mode = WEAPON_MODE_FORWARD;
	else if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_WFORWARDA), BM_GETCHECK, 0, 0))
		w->mode = WEAPON_MODE_FORWARD_ALTERNATING;
	else if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_WNOVA), BM_GETCHECK, 0, 0))
		w->mode = WEAPON_MODE_NOVA;
	else if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_WNOVAA), BM_GETCHECK, 0, 0))
		w->mode = WEAPON_MODE_NOVA_ALTERNATING;
	else if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_WBACKWARD), BM_GETCHECK, 0, 0))
		w->mode = WEAPON_MODE_BACKWARD;
	else if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_WTHROWER), BM_GETCHECK, 0, 0))
		w->mode = WEAPON_MODE_THROWER;
	else if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_WBEAM), BM_GETCHECK, 0, 0))
		w->mode = WEAPON_MODE_RAY;

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_WDAMAGE), szTemp, 12);
	w->explosion.damage = w->damage = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_WEXPRADIUS), szTemp, 12);
	w->explosion.radius = (float)atof(szTemp);

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_HITPEFFECT), szTemp, 12);
	w->hitEffect = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_HITSOUND), szTemp, 12);
	w->hitSound = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_HITSCRIPT), w->hitScript, 31);

	if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_WBEXPLODES), BM_GETCHECK, 0, 0))
	{
		w->bExplosive = TRUE;
	}
	else
		w->bExplosive = FALSE;

	if(SendMessage(GetDlgItem(hWnd, IDC_GADGET_WBEXPLODES2), BM_GETCHECK, 0, 0))
		w->bExplodeOnTimerEnd = TRUE;
	else
		w->bExplodeOnTimerEnd = FALSE;

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_WEC), szTemp, 12);
	w->energyConsumption = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_WFIREPACE), szTemp, 12);
	w->firingPace = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_WSPEED), szTemp, 12);
	w->force = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_WLIFETIME), szTemp, 12);
	w->lifetime = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_WANGLE), szTemp, 12);
	w->angle = (float)atof(szTemp);

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_WAMMOCOUNT), szTemp, 8);
	w->numBullets = atoi(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_WAMMOID), szTemp, 8);
	w->ammoTemplateID = atoi(szTemp);
}

void CGadgetWnd::FillKineticGunTemplateInfo(HWND hWnd)
{
	char szTemp[128];
	SKineticGadgetTemplate *k = (SKineticGadgetTemplate*)m_template.second;

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_PFORCE), szTemp, 12);
	k->force = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_PEC), szTemp, 12);
	k->energyConsumption = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_PFIREPACE), szTemp, 12);
	k->firingPace = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_PANGLE), szTemp, 12);
	k->angle = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_PRADIUS), szTemp, 12);
	k->radius = (float)atof(szTemp);

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_PPEFFECT), szTemp, 8);
	k->particleEffectID = atoi(szTemp);
}

void CGadgetWnd::FillShieldTemplateInfo(HWND hWnd)
{
	char szTemp[128];
	SShieldGadgetTemplate *s = (SShieldGadgetTemplate*)m_template.second;

	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_SDC), szTemp, 12);
	s->coefficient = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_SEC), szTemp, 12);
	s->energyConsumption = (float)atof(szTemp);
	GetWindowText(GetDlgItem(hWnd, IDC_GADGET_SPEFFECT), szTemp, 5);
	s->particleEffectID = atoi(szTemp);

	if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_ENERGY), BM_GETCHECK, 0, 0))
		s->type = DAMAGE_TYPE_ENERGY;
	else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PLASMA), BM_GETCHECK, 0, 0))
		s->type = DAMAGE_TYPE_PLASMA;
	else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_PROJECTILE), BM_GETCHECK, 0, 0))
		s->type = DAMAGE_TYPE_PROJECTILE;
	else if(SendMessage(GetDlgItem(hWnd, IDC_AMMO_TYPE_STUN), BM_GETCHECK, 0, 0))
		s->type = DAMAGE_TYPE_STUN;
}

void CGadgetWnd::SelectWeaponType(WEAPON_MODE mode, HWND hWnd, bool bFirst)
{
	switch(mode)
	{
	case WEAPON_MODE_THROWER:
		m_bAmmoWeapon = false;
		break;
	case WEAPON_MODE_FORWARD:
	case WEAPON_MODE_FORWARD_ALTERNATING:
	case WEAPON_MODE_NOVA:
	case WEAPON_MODE_NOVA_ALTERNATING:
	case WEAPON_MODE_BACKWARD:
	default:
		m_bAmmoWeapon = true;
		break;
	}

	if(!bFirst)
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_WAMMOID), "-1");

	if(m_bAmmoWeapon)
	{
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_LBLSPEED), "Speed");
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_LBLAMMOID), "Ammo ID");
	}
	else
	{
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_LBLSPEED), "Range");
		SetWindowText(GetDlgItem(hWnd, IDC_GADGET_LBLAMMOID), "EffectID");
	}
}