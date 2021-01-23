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

#include "CampaignBlockWnd.h"
#include "..\\resource.h"
#include "..\\OtE\\OnTheEdge.h"
#include "..\\Databases\\DatabaseUtilities.h"

CCampaignBlockWnd *CCampaignBlockWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampaignBlockWnd::CCampaignBlockWnd() : m_bModified(FALSE)
{
	m_pointer = this;
}

CCampaignBlockWnd::~CCampaignBlockWnd()
{
	m_pointer = NULL;
}


BOOL CALLBACK CCampaignBlockWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CCampaignBlockWnd *p = CCampaignBlockWnd::m_pointer;
	CCampaign::SCampaignBlock &t = p->m_template;
	char szTemp[16];
	std::string str;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->m_bModified = FALSE;

		// Set form information
		SetWindowText(GetDlgItem(hWnd, IDC_CB_LEVEL), t.levelFilename);
		SetWindowText(GetDlgItem(hWnd, IDC_CB_MISSIONOBJECTIVES), t.introductionText);

		sprintf(szTemp, "%i", t.targetID);
		SetWindowText(GetDlgItem(hWnd, IDC_CB_TARGETID), szTemp);

		// radio boxes
		if(t.bShowIntroduction)
			SendMessage(GetDlgItem(hWnd, IDC_CB_SHOWOBJECTIVES), BM_SETCHECK, 1, 0);

		if(t.endType == LEVEL_END_TARGET_DESTROYED)
			SendMessage(GetDlgItem(hWnd, IDC_CB_TARGETDEAD), BM_SETCHECK, 1, 0);
		else if(t.endType == LEVEL_END_ENEMIES_KILLED)
			SendMessage(GetDlgItem(hWnd, IDC_CB_ALLDEAD), BM_SETCHECK, 1, 0);
		else if(t.endType == LEVEL_END_SCRIPT)
			SendMessage(GetDlgItem(hWnd, IDC_CB_SCRIPT), BM_SETCHECK, 1, 0);
		else if(t.endType == LEVEL_END_NONE)
			SendMessage(GetDlgItem(hWnd, IDC_CB_NONE), BM_SETCHECK, 1, 0);

		SetWindowText(GetDlgItem(hWnd, IDC_CB_SCRIPT_NAME), t.scriptName);

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
			::memset(&t, 0, sizeof(CCampaign::SCampaignBlock));

			// Get the form information and return success
			GetWindowText(GetDlgItem(hWnd, IDC_CB_LEVEL), t.levelFilename, 127);
			if(!strcmp(t.levelFilename, ""))
			{
				MessageBox(hWnd, "You haven't selected level for this block!", "Error!", MB_OK);
				return FALSE;
			}
			GetWindowText(GetDlgItem(hWnd, IDC_CB_MISSIONOBJECTIVES), t.introductionText, 511);
			GetWindowText(GetDlgItem(hWnd, IDC_CB_SCRIPT_NAME), t.scriptName, 31);

			GetWindowText(GetDlgItem(hWnd, IDC_CB_TARGETID), szTemp, 8);
			t.targetID = atoi(szTemp);

			if(SendMessage(GetDlgItem(hWnd, IDC_CB_SHOWOBJECTIVES), BM_GETCHECK, 0, 0))
				t.bShowIntroduction = TRUE;
			else
				t.bShowIntroduction = FALSE;

			t.endType = LEVEL_END_NONE;
			if(SendMessage(GetDlgItem(hWnd, IDC_CB_TARGETDEAD), BM_GETCHECK, 0, 0))
				t.endType = LEVEL_END_TARGET_DESTROYED;
			else if (SendMessage(GetDlgItem(hWnd, IDC_CB_ALLDEAD), BM_GETCHECK, 0, 0))
				t.endType = LEVEL_END_ENEMIES_KILLED;
			else if (SendMessage(GetDlgItem(hWnd, IDC_CB_SCRIPT), BM_GETCHECK, 0, 0))
				t.endType = LEVEL_END_SCRIPT;

			p->m_bModified = TRUE;
			EndDialog(hWnd, FALSE);
			break;
		case IDC_CB_BROWSE:
			p->BrowseLevel(hWnd);
			break;

		case IDC_CB_BROWSE_SCRIPT:
			p->BrowseScript(hWnd);
			break;
		}
		break;
	}

	return FALSE;
}

void CCampaignBlockWnd::BrowseLevel(HWND hWnd)
{
	OPENFILENAME ofn;
	char filename[128];

	char folder[128];
	char tempFile[256] = "";
	sprintf(folder, "%slevels\\", GetOTE()->GetCurrentCampaignFolder().c_str());

	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = tempFile;
	ofn.lpstrFileTitle = filename;
	ofn.lpstrFilter = "On the Edge level (*.oel)\0*.OEL\0\0";
	ofn.lpstrDefExt = "oel";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = 127;
	ofn.nMaxFileTitle = 63;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = folder;

	if(!GetOpenFileName(&ofn))
		return;

	SetWindowText(GetDlgItem(hWnd, IDC_CB_LEVEL), filename);
}

void CCampaignBlockWnd::BrowseScript(HWND hWnd)
{
	std::string str;
	if(CDatabaseUtilities::SelectGlobalScript(hWnd, str))
		SetWindowText(GetDlgItem(hWnd, IDC_CB_SCRIPT_NAME), str.c_str());
}

bool CCampaignBlockWnd::OpenWindow(HWND parent, CCampaign::SCampaignBlock &rTemplate, bool bNew)
{
	if(bNew)
		rTemplate.FillDefaults();

	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CAMPAIGN_BLOCK), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
