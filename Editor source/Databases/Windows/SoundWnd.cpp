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

#include "SoundWnd.h"
#include "..\\..\\resource.h"
#include "..\\..\\OtE\\OnTheEdge.h"
#include "..\\DatabaseUtilities.h"

CSoundWnd *CSoundWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundWnd::CSoundWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CSoundWnd::~CSoundWnd()
{
	m_pointer = NULL;
}

BOOL CALLBACK CSoundWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CSoundWnd *p = CSoundWnd::m_pointer;
	CSoundManager::SSoundTemplate &t = p->m_template;
	char szTemp[128];
	std::string str;
	long id;
	SCROLLINFO scroll;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set the scrollbar info
		scroll.cbSize = sizeof(SCROLLINFO);
		scroll.fMask = SIF_POS | SIF_RANGE;
		scroll.nMax = 100;
		scroll.nMin = 0;
		scroll.nPos = (int)(t.volume * 100.f);
		SendMessage(GetDlgItem(hWnd, IDC_SOUND_VOLUME), SBM_SETSCROLLINFO, (WPARAM)0, (LPARAM)&scroll);

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_SOUND_NAME), t.name);
		SetWindowText(GetDlgItem(hWnd, IDC_SOUND_FILENAME), t.filename);

		if(strlen(t.filename)>1)
			EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);

		if(t.stream)
			SendMessage(GetDlgItem(hWnd, IDC_SOUND_STREAM), BM_SETCHECK, 1, 0);

		sprintf(szTemp, "%i", t.id);
		SetWindowText(GetDlgItem(hWnd, IDC_SOUND_ID), szTemp);

		if(t.soundType == SOUND_TYPE_MUSIC)
			SendMessage(GetDlgItem(hWnd, IDC_RADIO1), BM_SETCHECK, 1, 0);
		else if(t.soundType == SOUND_TYPE_INTERFACE)
			SendMessage(GetDlgItem(hWnd, IDC_RADIO2), BM_SETCHECK, 1, 0);
		else if(t.soundType == SOUND_TYPE_EXPLOSION)
			SendMessage(GetDlgItem(hWnd, IDC_RADIO3), BM_SETCHECK, 1, 0);
		else if(t.soundType == SOUND_TYPE_PLAYER_GUN)
			SendMessage(GetDlgItem(hWnd, IDC_RADIO4), BM_SETCHECK, 1, 0);
		else if(t.soundType == SOUND_TYPE_PLAYER_MISC)
			SendMessage(GetDlgItem(hWnd, IDC_RADIO5), BM_SETCHECK, 1, 0);
		else if(t.soundType == SOUND_TYPE_ENEMY_GUN)
			SendMessage(GetDlgItem(hWnd, IDC_RADIO6), BM_SETCHECK, 1, 0);
		else if(t.soundType == SOUND_TYPE_ENEMY_MISC)
			SendMessage(GetDlgItem(hWnd, IDC_RADIO7), BM_SETCHECK, 1, 0);
		else if(t.soundType == SOUND_TYPE_ENVIRONMENT_MISC)
			SendMessage(GetDlgItem(hWnd, IDC_RADIO8), BM_SETCHECK, 1, 0);

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
			GetWindowText(GetDlgItem(hWnd, IDC_SOUND_NAME), t.name, 31);
			if(strlen(t.name) < 3)
			{
				::MessageBox(hWnd, "Too short sound name", "Error", MB_OK);
				break;
			}
			GetWindowText(GetDlgItem(hWnd, IDC_SOUND_FILENAME), t.filename, 127);

			if(SendMessage(GetDlgItem(hWnd, IDC_SOUND_STREAM), BM_GETCHECK, 0, 0))
				t.stream = true;
			else
				t.stream = false;

			if(SendMessage(GetDlgItem(hWnd, IDC_RADIO1), BM_GETCHECK, 0, 0))
				t.soundType = SOUND_TYPE_MUSIC;
			else if(SendMessage(GetDlgItem(hWnd, IDC_RADIO2), BM_GETCHECK, 0, 0))
				t.soundType = SOUND_TYPE_INTERFACE;
			else if(SendMessage(GetDlgItem(hWnd, IDC_RADIO3), BM_GETCHECK, 0, 0))
				t.soundType = SOUND_TYPE_EXPLOSION;
			else if(SendMessage(GetDlgItem(hWnd, IDC_RADIO4), BM_GETCHECK, 0, 0))
				t.soundType = SOUND_TYPE_PLAYER_GUN;
			else if(SendMessage(GetDlgItem(hWnd, IDC_RADIO5), BM_GETCHECK, 0, 0))
				t.soundType = SOUND_TYPE_PLAYER_MISC;
			else if(SendMessage(GetDlgItem(hWnd, IDC_RADIO6), BM_GETCHECK, 0, 0))
				t.soundType = SOUND_TYPE_ENEMY_GUN;
			else if(SendMessage(GetDlgItem(hWnd, IDC_RADIO7), BM_GETCHECK, 0, 0))
				t.soundType = SOUND_TYPE_ENEMY_MISC;
			else if(SendMessage(GetDlgItem(hWnd, IDC_RADIO8), BM_GETCHECK, 0, 0))
				t.soundType = SOUND_TYPE_ENVIRONMENT_MISC;

			// Get scrollbar position
			scroll.cbSize = sizeof(SCROLLINFO);
			scroll.fMask = SIF_TRACKPOS;
			SendMessage(GetDlgItem(hWnd, IDC_SOUND_VOLUME), SBM_GETSCROLLINFO, 0, (LPARAM)&scroll);
			t.volume = (float)scroll.nTrackPos * 0.01f;

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;

		case IDC_SOUND_BROWSE:
			if(CDatabaseUtilities::GetSoundFilename(hWnd, str, "sounds\\") == FALSE)
				break;

			SetWindowText(GetDlgItem(hWnd, IDC_SOUND_FILENAME), str.c_str());
			EnableWindow(GetDlgItem(hWnd, IDOK), TRUE);
			break;

		case IDC_SOUND_LISTEN:
			p->Listen(hWnd);
			break;
		}
		break;

	case WM_HSCROLL:
		if((HWND)lParam == GetDlgItem(hWnd, IDC_SOUND_VOLUME))
		{
			scroll.cbSize = sizeof(SCROLLINFO);
			scroll.fMask = SIF_TRACKPOS;
			SendMessage(GetDlgItem(hWnd, IDC_SOUND_VOLUME), SBM_GETSCROLLINFO, 0, (LPARAM)&scroll);
			SendMessage(GetDlgItem(hWnd, IDC_SOUND_VOLUME), SBM_SETPOS, (WPARAM)scroll.nTrackPos, (LPARAM)1);
		}
		break;
	}

	return FALSE;
}

void CSoundWnd::Listen(HWND hWnd)
{
	char buf[128];
	GetWindowText(GetDlgItem(hWnd, IDC_SOUND_FILENAME), buf, 127);
	std::string path = "sounds\\";
	path += buf;
	CSound *s = new CSound(CSoundManager::GetManager().GetSoundSystem(), -1);
	if(s->LoadSound(buf, true, SOUND_TYPE_MUSIC))
		s->Play();
	else
	{
		::MessageBox(hWnd, "Failed loading supplied sound", "Error", MB_OK);
	}
	delete s;
}

bool CSoundWnd::OpenWindow(HWND parent, CSoundManager::SSoundTemplate &rTemplate, bool bNew)
{
	m_template = rTemplate;

	if(bNew)
		rTemplate.FillDefaults();

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SOUND), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
