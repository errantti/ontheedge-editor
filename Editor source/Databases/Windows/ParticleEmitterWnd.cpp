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

#include "ParticleEmitterWnd.h"
#include "..\\..\\resource.h"
#include "..\\DatabaseUtilities.h"
#include "..\\..\\Tools\\GetColorWnd.h"

CParticleEmitterWnd *CParticleEmitterWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticleEmitterWnd::CParticleEmitterWnd()
{
	m_pointer = this;
}

CParticleEmitterWnd::~CParticleEmitterWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CParticleEmitterWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CParticleEmitterWnd *p = CParticleEmitterWnd::m_pointer;
	sEMITTER_PARAMS &i = p->m_template;
	char szTemp[128];
	std::string str;

	p->m_hWnd = hWnd;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information

		// Emitter parameters
		sprintf(szTemp, "%i", i.maxParticleCount);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_MAX_PARTICLES), szTemp);
		sprintf(szTemp, "%i", i.particleCount);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_EMIT_COUNT), szTemp);
		sprintf(szTemp, "%f", i.emitFrequency);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_FREQUENCY), szTemp);
		sprintf(szTemp, "%f", i.startTime);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_START_DELAY), szTemp);

		if(i.autoEmit)
			SendMessage(GetDlgItem(hWnd, IDC_PE_AUTOEMIT), BM_SETCHECK, 1, 0);

		SetWindowText(GetDlgItem(hWnd, IDC_PE_NAME), i.name);


		// Particle parameters
		SetWindowText(GetDlgItem(hWnd, IDC_PE_TEXTURE), i.resourceFilename);

		p->m_bUsingMesh = i.hasMesh;
		if(i.hasMesh)
		{
			SendMessage(GetDlgItem(hWnd, IDC_PE_HASMESH), BM_SETCHECK, 1, 0);
			SetWindowText(GetDlgItem(hWnd, IDC_PE_LABEL_RESOURCE), "Mesh:");
		}
		if(i.inheritParentSpeed)
			SendMessage(GetDlgItem(hWnd, IDC_PE_INHERITSPEED), BM_SETCHECK, 1, 0);
		if(i.isBillboard)
			SendMessage(GetDlgItem(hWnd, IDC_PE_BILLBOARD), BM_SETCHECK, 1, 0);
		if(i.additiveBlend)
			SendMessage(GetDlgItem(hWnd, IDC_PE_ADDITIVEBLEND), BM_SETCHECK, 1, 0);
		if(i.exactSpeedDetermination)
			SendMessage(GetDlgItem(hWnd, IDC_PE_EXACTSPEED), BM_SETCHECK, 1, 0);
		if(i.replaceOldParticles)
			SendMessage(GetDlgItem(hWnd, IDC_PE_REPLACEOLD), BM_SETCHECK, 1, 0);

		sprintf(szTemp, "%f", i.timeToLive);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_TTL), szTemp);

		sprintf(szTemp, "%f", i.scaleStart);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_SIZE_START), szTemp);
		sprintf(szTemp, "%f", i.scaleEnd);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_SIZE_END), szTemp);
		sprintf(szTemp, "%f", i.speedMin);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_MINSPEED), szTemp);
		sprintf(szTemp, "%f", i.speedMax);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_MAXSPEED), szTemp);
		sprintf(szTemp, "%f", i.weightMin);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_MINWEIGHT), szTemp);
		sprintf(szTemp, "%f", i.weightMax);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_MAXWEIGHT), szTemp);


		// Gravity
		if(i.hasGravity)
			SendMessage(GetDlgItem(hWnd, IDC_PE_GRAVITY), BM_SETCHECK, 1, 0);
		if(i.useExternalGravity)
			SendMessage(GetDlgItem(hWnd, IDC_PE_EXTERNAL_GRAVITY), BM_SETCHECK, 1, 0);


		// Mesh matrix rotation
		if(i.useRotationMatrices)
			SendMessage(GetDlgItem(hWnd, IDC_PE_ROTMAT_ENABLED), BM_SETCHECK, 1, 0);
		if(i.inheritParentRotation)
			SendMessage(GetDlgItem(hWnd, IDC_PE_INHERIT_ROTMAT), BM_SETCHECK, 1, 0);
		sprintf(szTemp, "%i", i.numRotationMatrices);
		SetWindowText(GetDlgItem(hWnd, IDC_PE_ROTMAT_COUNT), szTemp);

		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_PE_CANCEL:
			EndDialog(hWnd, FALSE);
			break;
		case ID_PE_OK:
			// Get the form information and return success

			GetWindowText(GetDlgItem(hWnd, IDC_PE_NAME), i.name, 63);
			if(strlen(i.name) < 3)
			{
				::MessageBox(hWnd, "Too short emitter name", "Error", MB_OK);
				break;
			}

			// Emitter parameters
			GetWindowText(GetDlgItem(hWnd, IDC_PE_MAX_PARTICLES), szTemp, 5);
			i.maxParticleCount = atoi(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PE_EMIT_COUNT), szTemp, 5);
			i.particleCount = atoi(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PE_FREQUENCY), szTemp, 8);
			i.emitFrequency = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PE_START_DELAY), szTemp, 8);
			i.startTime = (float)atof(szTemp);

			i.autoEmit = false;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_AUTOEMIT), BM_GETCHECK, 0, 0))
				i.autoEmit = true;


			// Particle parameters
			GetWindowText(GetDlgItem(hWnd, IDC_PE_TEXTURE), i.resourceFilename, 127);

			i.hasMesh = i.inheritParentSpeed = i.isBillboard = false;
			i.additiveBlend = i.exactSpeedDetermination = i.replaceOldParticles = false;

			if(SendMessage(GetDlgItem(hWnd, IDC_PE_HASMESH), BM_GETCHECK, 0, 0))
				i.hasMesh = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_INHERITSPEED), BM_GETCHECK, 0, 0))
				i.inheritParentSpeed = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_BILLBOARD), BM_GETCHECK, 0, 0))
				i.isBillboard = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_ADDITIVEBLEND), BM_GETCHECK, 0, 0))
				i.additiveBlend = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_EXACTSPEED), BM_GETCHECK, 0, 0))
				i.exactSpeedDetermination = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_REPLACEOLD), BM_GETCHECK, 0, 0))
				i.replaceOldParticles = true;

			GetWindowText(GetDlgItem(hWnd, IDC_PE_TTL), szTemp, 8);
			i.timeToLive = (float)atof(szTemp);

			GetWindowText(GetDlgItem(hWnd, IDC_PE_SIZE_START), szTemp, 8);
			i.scaleStart = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PE_SIZE_END), szTemp, 8);
			i.scaleEnd = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PE_MINSPEED), szTemp, 8);
			i.speedMin = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PE_MAXSPEED), szTemp, 8);
			i.speedMax = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PE_MINWEIGHT), szTemp, 8);
			i.weightMin = (float)atof(szTemp);
			GetWindowText(GetDlgItem(hWnd, IDC_PE_MAXWEIGHT), szTemp, 8);
			i.weightMax = (float)atof(szTemp);


			// Gravity
			i.hasGravity = i.useExternalGravity = false;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_GRAVITY), BM_GETCHECK, 0, 0))
				i.hasGravity = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_EXTERNAL_GRAVITY), BM_GETCHECK, 0, 0))
				i.useExternalGravity = true;


			// Mesh matrix rotation
			i.useRotationMatrices = i.inheritParentRotation = false;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_ROTMAT_ENABLED), BM_GETCHECK, 0, 0))
				i.useRotationMatrices = true;
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_INHERIT_ROTMAT), BM_GETCHECK, 0, 0))
				i.inheritParentRotation = true;

			GetWindowText(GetDlgItem(hWnd, IDC_PE_ROTMAT_COUNT), szTemp, 8);


			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;


		case IDC_PE_BROWSE_TEXTURE:
			// Open common dialogue to browse for texture or mesh file
			if(p->m_bUsingMesh)
			{
				if(CDatabaseUtilities::GetMeshFilename(hWnd, str) == FALSE)
					break;
			}
			else
			{
				if(CDatabaseUtilities::GetTextureFilename(hWnd, str) == FALSE)
					break;
			}

			//strcpy(i.resourceFilename, str.c_str());
			SetWindowText(GetDlgItem(hWnd, IDC_PE_TEXTURE), str.c_str());
			break;
		case IDC_PE_REMOVE_TEXTURE:
			//p->m_template.resourceFilename[0] = '\0';
			SetWindowText(GetDlgItem(hWnd, IDC_PE_TEXTURE), "");
			break;

		case IDC_PE_POSITION:
			p->m_getVectorWnd.OpenWindow(hWnd, i.position, "Set relative start position");
			break;
		case IDC_PE_POSITION_SPREAD:
			p->m_getVectorWnd.OpenWindow(hWnd, i.positionSpread, "Set relative start position spread");
			break;
		case IDC_PE_DIRECTION:
			p->m_getVectorWnd.OpenWindow(hWnd, i.direction, "Set relative direction");
			break;
		case IDC_PE_DIRECTION_SPREAD:
			p->m_getVectorWnd.OpenWindow(hWnd, i.directionSpread, "Set relative direction spread");
			break;

		case IDC_PE_COLOR_START:
			/*vec.x = i.colorStart.a;
			vec.y = i.colorStart.r;
			vec.z = i.colorStart.g;
			vec.w = i.colorStart.b;
			p->m_getColorWnd.OpenWindow(hWnd, vec, "Set color at start", fields);
			i.colorStart.a = vec.x;
			i.colorStart.r = vec.y;
			i.colorStart.g = vec.z;
			i.colorStart.b = vec.w;*/
			p->SelectColor(hWnd, i.colorStart);
			break;
		case IDC_PE_COLOR_END:
			/*vec.x = i.colorEnd.a;
			vec.y = i.colorEnd.r;
			vec.z = i.colorEnd.g;
			vec.w = i.colorEnd.b;
			p->m_getColorWnd.OpenWindow(hWnd, vec, "Set color at end", fields);
			i.colorEnd.a = vec.x;
			i.colorEnd.r = vec.y;
			i.colorEnd.g = vec.z;
			i.colorEnd.b = vec.w;*/
			p->SelectColor(hWnd, i.colorEnd);
			break;

		case IDC_PE_HASMESH:
			if(SendMessage(GetDlgItem(hWnd, IDC_PE_HASMESH), BM_GETCHECK, 0, 0))
				p->SetResourceType(true);
			else
				p->SetResourceType(false);
			break;

		case IDC_PE_GRAVITY_VECTOR:
			p->m_getVectorWnd.OpenWindow(hWnd, i.gravity, "Set gravity vector");
			break;

		case IDC_PE_SET_MIN_ROTMAT:
			p->m_getVectorWnd.OpenWindow(hWnd, i.minRotationSpeed, "Set minimum rotation speed");
			break;
		case IDC_PE_SET_MAX_ROTMAT:
			p->m_getVectorWnd.OpenWindow(hWnd, i.maxRotationSpeed, "Set maximum rotation speed");
			break;
		}
		break;
	}

	return FALSE;
}

void CParticleEmitterWnd::SelectColor(HWND hWnd, D3DXCOLOR &color)
{
	CGetColorWnd colorWnd;
	colorWnd.OpenWindow(hWnd, color);
}

void CParticleEmitterWnd::SetResourceType(bool value)
{
	if(m_hWnd == NULL)
		return;

	m_bUsingMesh = value;
	if(value)
	{
		SetWindowText(GetDlgItem(m_hWnd, IDC_PE_LABEL_RESOURCE), "Mesh:");
		SetWindowText(GetDlgItem(m_hWnd, IDC_PE_TEXTURE), "");
	}
	else
	{
		SetWindowText(GetDlgItem(m_hWnd, IDC_PE_LABEL_RESOURCE), "Texture:");
		SetWindowText(GetDlgItem(m_hWnd, IDC_PE_TEXTURE), "");
	}
}

bool CParticleEmitterWnd::OpenWindow(HWND parent, sEMITTER_PARAMS &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PARTICLE_EFFECT), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
