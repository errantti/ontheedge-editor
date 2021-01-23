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

#include "LevelPropertiesWnd.h"
#include "..\\resource.h"
#include "..\\Databases\\DatabaseUtilities.h"
#include "Get4DVectorWnd.h"
#include "GetVectorWnd.h"

CLevelPropertiesWnd *CLevelPropertiesWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevelPropertiesWnd::CLevelPropertiesWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CLevelPropertiesWnd::~CLevelPropertiesWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CLevelPropertiesWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CLevelPropertiesWnd *p = CLevelPropertiesWnd::m_pointer;
	CLevel::SLevelFileHeader &t = p->m_template;
	char filename[128];
	std::string str;
	SCROLLINFO scroll;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		// Edit fields
		SetWindowText(GetDlgItem(hWnd, IDC_LP_NAME), t.name);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_DESCRIPTION), t.description);
		sprintf(filename, "%f", t.energyPodRegeneration);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_ENERGYPOD), filename);
		sprintf(filename, "%f", t.repairPodRegeneration);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_REPAIRPOD), filename);
		sprintf(filename, "%f", t.gravity);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_GRAVITY), filename);
		sprintf(filename, "%f", t.density);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_DENSITY), filename);
		sprintf(filename, "%f", t.friction);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_FRICTION), filename);

		// skybox textures
		SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_TOP), t.skyboxTexture[0]);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_BOTTOM), t.skyboxTexture[1]);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_FRONT), t.skyboxTexture[2]);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_BACK), t.skyboxTexture[3]);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_LEFT), t.skyboxTexture[4]);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_RIGHT), t.skyboxTexture[5]);

		// Check box
		if(t.bSinglePlayerMap)
			SendMessage(GetDlgItem(hWnd, IDC_LP_SINGLE_PLAYER), BM_SETCHECK, 1, 0);

		// Scroll bars
		// set the scroll bar positions
		t.lightDirection.x /= -t.lightDirection.y;
		t.lightDirection.z /= -t.lightDirection.y;
		t.lightDirection.y = -1.f;

		// set scroll info
		scroll.cbSize = sizeof(SCROLLINFO);
		scroll.fMask = SIF_POS | SIF_RANGE;
		scroll.nMax = 100;
		scroll.nMin = -100;
		scroll.nPos = (int)(t.lightDirection.x * 100.f);
		SendMessage(GetDlgItem(hWnd, IDC_LP_X_DIR), SBM_SETSCROLLINFO, (WPARAM)0, (LPARAM)&scroll);
		scroll.nPos = (int)(t.lightDirection.z * 100.f);
		SendMessage(GetDlgItem(hWnd, IDC_LP_Z_DIR), SBM_SETSCROLLINFO, (WPARAM)0, (LPARAM)&scroll);

		// Music
		sprintf(filename, "%i", t.musicID[0]);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_MUSIC1), filename);
		sprintf(filename, "%i", t.musicID[1]);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_MUSIC2), filename);
		sprintf(filename, "%i", t.musicID[2]);
		SetWindowText(GetDlgItem(hWnd, IDC_LP_MUSIC3), filename);
		if(t.bHasBossMusicTheme)
			SendMessage(GetDlgItem(hWnd, IDC_LP_AUTOPLAY), BM_SETCHECK, 1, 0);
		if(t.bLoopMusic)
			SendMessage(GetDlgItem(hWnd, IDC_LP_LOOP), BM_SETCHECK, 1, 0);

		// Camera
		if(t.cameraType == CAMERA_TYPE_FIXED_FOLLOW)
			SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE1), BM_SETCHECK, 1, 0);
		else if(t.cameraType == CAMERA_TYPE_FREE_FOLLOW)
			SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE2), BM_SETCHECK, 1, 0);
		else if(t.cameraType == CAMERA_TYPE_FOLLOW_BEHIND)
			SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE3), BM_SETCHECK, 1, 0);
		else if(t.cameraType == CAMERA_TYPE_FIRST_PERSON)
			SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE4), BM_SETCHECK, 1, 0);
		else if(t.cameraType == CAMERA_TYPE_FPS)
			SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE5), BM_SETCHECK, 1, 0);

		if(t.bForceLevelCamera)
			SendMessage(GetDlgItem(hWnd, IDC_LP_FORCE_CAMERA), BM_SETCHECK, 1, 0);

		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_LP_OK:
			// Gather form information
			GetWindowText(GetDlgItem(hWnd, IDC_LP_NAME), t.name, 63);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_DESCRIPTION), t.description, 1023);

			// skybox textures
			GetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_TOP), t.skyboxTexture[0], 127);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_BOTTOM), t.skyboxTexture[1], 127);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_FRONT), t.skyboxTexture[2], 127);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_BACK), t.skyboxTexture[3], 127);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_LEFT), t.skyboxTexture[4], 127);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_RIGHT), t.skyboxTexture[5], 127);

			GetWindowText(GetDlgItem(hWnd, IDC_LP_ENERGYPOD), filename, 12);
			t.energyPodRegeneration = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_REPAIRPOD), filename, 12);
			t.repairPodRegeneration = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_GRAVITY), filename, 12);
			t.gravity = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_DENSITY), filename, 12);
			t.density = (float)atof(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_FRICTION), filename, 12);
			t.friction = (float)atof(filename);

			t.bSinglePlayerMap = FALSE;
			if(SendMessage(GetDlgItem(hWnd, IDC_LP_SINGLE_PLAYER), BM_GETCHECK, 0, 0))
				t.bSinglePlayerMap = TRUE;

			// Get scrollbar position
			scroll.cbSize = sizeof(SCROLLINFO);
			scroll.fMask = SIF_TRACKPOS;
			SendMessage(GetDlgItem(hWnd, IDC_LP_X_DIR), SBM_GETSCROLLINFO, 0, (LPARAM)&scroll);
			t.lightDirection.x = scroll.nTrackPos * 0.01f;
			SendMessage(GetDlgItem(hWnd, IDC_LP_Z_DIR), SBM_GETSCROLLINFO, 0, (LPARAM)&scroll);
			t.lightDirection.z = scroll.nTrackPos * 0.01f;

			// Music
			GetWindowText(GetDlgItem(hWnd, IDC_LP_MUSIC1), filename, 8);
			t.musicID[0] = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_MUSIC2), filename, 8);
			t.musicID[1] = atoi(filename);
			GetWindowText(GetDlgItem(hWnd, IDC_LP_MUSIC3), filename, 8);
			t.musicID[2] = atoi(filename);

			t.bHasBossMusicTheme = t.bLoopMusic = FALSE;
			if(SendMessage(GetDlgItem(hWnd, IDC_LP_AUTOPLAY), BM_GETCHECK, 0, 0))
				t.bHasBossMusicTheme = TRUE;
			if(SendMessage(GetDlgItem(hWnd, IDC_LP_LOOP), BM_GETCHECK, 0, 0))
				t.bLoopMusic = TRUE;

			// Camera
			if(SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE1), BM_GETCHECK, 0, 0))
				t.cameraType = CAMERA_TYPE_FIXED_FOLLOW;
			else if(SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE2), BM_GETCHECK, 0, 0))
				t.cameraType = CAMERA_TYPE_FREE_FOLLOW;
			else if(SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE3), BM_GETCHECK, 0, 0))
				t.cameraType = CAMERA_TYPE_FOLLOW_BEHIND;
			else if(SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE4), BM_GETCHECK, 0, 0))
				t.cameraType = CAMERA_TYPE_FIRST_PERSON;
			else if(SendMessage(GetDlgItem(hWnd, IDC_LP_CAMERA_TYPE5), BM_GETCHECK, 0, 0))
				t.cameraType = CAMERA_TYPE_FPS;

			t.bForceLevelCamera = FALSE;
			if(SendMessage(GetDlgItem(hWnd, IDC_LP_FORCE_CAMERA), BM_GETCHECK, 0, 0))
				t.bForceLevelCamera = TRUE;

			p->m_bModified = TRUE;

			EndDialog(hWnd, FALSE);
			break;
		case ID_LP_CANCEL:
			EndDialog(hWnd, FALSE);
			break;

		case ID_LP_SET_COLOR:
			CDatabaseUtilities::SelectD3DCOLOR(hWnd, t.lightColor);
			break;
		case ID_LP_SET_AMBIENT_COLOR:
			CDatabaseUtilities::SelectD3DCOLOR(hWnd, t.ambientColor);
			break;

		case IDC_LP_BROWSEMUSIC1:
			p->SelectMusic(hWnd, 0, IDC_LP_MUSIC1);
			break;
		case IDC_LP_BROWSEMUSIC2:
			p->SelectMusic(hWnd, 1, IDC_LP_MUSIC2);
			break;
		case IDC_LP_BROWSEMUSIC3:
			p->SelectMusic(hWnd, 2, IDC_LP_MUSIC3);
			break;

		case IDC_LP_SET_CAMERA_PARAM:
			p->SelectCameraParam(hWnd);
			break;

		// Skybox textures
		case IDC_LP_CMD_TOP:
			if(CDatabaseUtilities::GetSkyboxTextureFilename(hWnd, str) == FALSE)
				break;

			SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_TOP), str.c_str());
			break;
		case IDC_LP_CMD_BOTTOM:
			if(CDatabaseUtilities::GetSkyboxTextureFilename(hWnd, str) == FALSE)
				break;

			SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_BOTTOM), str.c_str());
			break;
		case IDC_LP_CMD_LEFT:
			if(CDatabaseUtilities::GetSkyboxTextureFilename(hWnd, str) == FALSE)
				break;

			SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_LEFT), str.c_str());
			break;
		case IDC_LP_CMD_RIGHT:
			if(CDatabaseUtilities::GetSkyboxTextureFilename(hWnd, str) == FALSE)
				break;

			SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_RIGHT), str.c_str());
			break;
		case IDC_LP_CMD_FRONT:
			if(CDatabaseUtilities::GetSkyboxTextureFilename(hWnd, str) == FALSE)
				break;

			SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_FRONT), str.c_str());
			break;
		case IDC_LP_CMD_BACK:
			if(CDatabaseUtilities::GetSkyboxTextureFilename(hWnd, str) == FALSE)
				break;

			SetWindowText(GetDlgItem(hWnd, IDC_LP_EDIT_BACK), str.c_str());
			break;
		}
		break;
	case WM_HSCROLL:
		if((HWND)lParam == GetDlgItem(hWnd, IDC_LP_X_DIR))
		{
			scroll.cbSize = sizeof(SCROLLINFO);
			scroll.fMask = SIF_TRACKPOS;
			SendMessage(GetDlgItem(hWnd, IDC_LP_X_DIR), SBM_GETSCROLLINFO, 0, (LPARAM)&scroll);
			SendMessage(GetDlgItem(hWnd, IDC_LP_X_DIR), SBM_SETPOS, (WPARAM)scroll.nTrackPos, (LPARAM)1);
		}
		else if((HWND)lParam == GetDlgItem(hWnd, IDC_LP_Z_DIR))
		{
			scroll.cbSize = sizeof(SCROLLINFO);
			scroll.fMask = SIF_TRACKPOS;
			SendMessage(GetDlgItem(hWnd, IDC_LP_Z_DIR), SBM_GETSCROLLINFO, 0, (LPARAM)&scroll);
			SendMessage(GetDlgItem(hWnd, IDC_LP_Z_DIR), SBM_SETPOS, (WPARAM)scroll.nTrackPos, (LPARAM)1);
		}
		break;
	}

	return FALSE;
}

void CLevelPropertiesWnd::SelectCameraParam(HWND hWnd)
{
	CGetVectorWnd wnd;
	wnd.OpenWindow(hWnd, m_template.cameraParam, "Set camera param");
}

void CLevelPropertiesWnd::SelectMusic(HWND hWnd, int index, int dlgItem)
{
	char buf[16];
	if(CDatabaseUtilities::SelectSound(hWnd, m_template.musicID[index]))
	{
		sprintf(buf, "%i", m_template.musicID[index]);
		SetWindowText(GetDlgItem(hWnd, dlgItem), buf);
	}
}

bool CLevelPropertiesWnd::Open(HWND parent, CLevel::SLevelFileHeader &h, bool bModal)
{
	m_template = h;

	if(bModal)
	{
		DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LEVEL_PROPERTIES), parent, WndProc);
	}
	else
	{
		CreateDialog(NULL, MAKEINTRESOURCE(IDD_LEVEL_PROPERTIES), parent, WndProc);
	}

	if(m_bModified)
	{
		h = m_template;
		return TRUE;
	}
	else
		return FALSE;
}