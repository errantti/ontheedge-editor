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

#include "DialogueWnd.h"
#include "DialogueLineWnd.h"
#include "..\\resource.h"
#include "..\\Editor.h"
#include "..\\Databases\\DatabaseUtilities.h"

CDialogueWnd *CDialogueWnd::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialogueWnd::CDialogueWnd() : m_bModified(FALSE), m_hWnd(NULL)
{
	m_pointer = this;
}

CDialogueWnd::~CDialogueWnd()
{
	m_pointer = NULL;
}

void CDialogueWnd::OpenDialogueLineProperties(int index)
{
	CDialogueLineWnd dw;
	std::vector<CDialogue::SComment> *v = m_template.GetComments();
	if(dw.OpenWindow(m_hWnd, (*v)[index], FALSE))
		UpdateList();
}

void CDialogueWnd::DeleteComment(int index)
{
	std::vector<CDialogue::SComment>::iterator it = m_template.GetComments()->begin()+index;
	/*for(int i=0;i!=index;i++)
	{
		it++;
		// Backdoor
		if(i>10000)
			return;
	}*/
	if(it == m_template.GetComments()->end())
		return;
	m_template.GetComments()->erase(it);
}

void CDialogueWnd::AddComment()
{
	CDialogue::SComment cNew;
	CDialogueLineWnd w;
	if(w.OpenWindow(m_hWnd, cNew, TRUE))
	{
		m_template.GetComments()->push_back(cNew);
		UpdateList();
	}
}

void CDialogueWnd::MoveUp(int index)
{
	std::vector<CDialogue::SComment> *v = m_template.GetComments();

	if(index <= 0 || index>= v->size())
		return;

	CDialogue::SComment c = (*v)[index-1];
	(*v)[index-1] = (*v)[index];
	(*v)[index] = c;
}

void CDialogueWnd::MoveDown(int index)
{
	std::vector<CDialogue::SComment> *v = m_template.GetComments();

	if(index < 0 || index>= v->size()-1)
		return;

	CDialogue::SComment c = (*v)[index+1];
	(*v)[index+1] = (*v)[index];
	(*v)[index] = c;
}

/*****
 * Database selection window callback
 *****/
BOOL CALLBACK CDialogueWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CDialogueWnd *p = CDialogueWnd::m_pointer;
	int selection = -1;
	p->m_hWnd = hWnd;
	char szTemp[MAX_MISSION_OBJECTIVE_LENGTH];

	switch(iMessage)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hWnd, IDC_D_NAME), p->m_template.GetName().c_str());
		SetWindowText(GetDlgItem(hWnd, IDC_D_OBJECTIVE), p->m_template.GetNewMissionObjective().c_str());
		SetWindowText(GetDlgItem(hWnd, IDC_D_SCRIPT), p->m_template.GetOnDialogueEndScriptName());

		sprintf(szTemp, "%i", p->m_template.GetMissionPriority());
		SetWindowText(GetDlgItem(hWnd, IDC_D_PRIORITY), szTemp);

		if(p->m_template.GetUpdatesMissionObjective())
			SendMessage(GetDlgItem(hWnd, IDC_D_UPDATE), BM_SETCHECK, 1, 0);

		p->m_bModified = FALSE;
		p->UpdateList();
		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_D_CANCEL:
			EndDialog(hWnd, FALSE);
			break;

		case ID_D_OK:
			GetWindowText(GetDlgItem(hWnd, IDC_D_NAME), szTemp, 31);
			if(strlen(szTemp) < 3)
			{
				::MessageBox(hWnd, "Too short dialogue name", "Error", MB_OK);
				break;
			}
			p->m_template.SetName(szTemp);

			GetWindowText(GetDlgItem(hWnd, IDC_D_OBJECTIVE), szTemp, MAX_MISSION_OBJECTIVE_LENGTH-1);
			p->m_template.SetNewMissionObjective(szTemp);

			GetWindowText(GetDlgItem(hWnd, IDC_D_SCRIPT), szTemp, 31);
			p->m_template.SetOnDialogueEndScriptName(szTemp);

			GetWindowText(GetDlgItem(hWnd, IDC_D_PRIORITY), szTemp, 6);
			p->m_template.SetMissionPriority(atoi(szTemp));

			if(SendMessage(GetDlgItem(hWnd, IDC_D_UPDATE), BM_GETCHECK, 0, 0))
				p->m_template.SetUpdatesMissionObjective(true);
			else 
				p->m_template.SetUpdatesMissionObjective(false);

			EndDialog(hWnd, FALSE);
			p->m_bModified = TRUE;
			break;

		case ID_D_DELETE:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_D_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				p->DeleteComment(selection);
				p->UpdateList();
				break;
			}
			else
				::MessageBox(hWnd, "You haven't selected any item!", "Error!", MB_OK);
			break;

		case ID_D_EDIT:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_D_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				p->OpenDialogueLineProperties(selection);
				break;
			}
			else
				::MessageBox(hWnd, "You haven't selected any item!", "Error!", MB_OK);
			break;

		case ID_D_ADD:
			p->AddComment();
			break;

		case ID_D_MOVE_UP:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_D_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				p->MoveUp(selection);
				p->UpdateList();
			}
			break;

		case ID_D_MOVE_DOWN:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_D_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				p->MoveDown(selection);
				p->UpdateList();
			}
			break;

		case IDC_D_LIST:
			if(HIWORD(wParam) == LBN_DBLCLK)
			{
				if((selection = (int)SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0)) != LB_ERR)
				{
					p->OpenDialogueLineProperties(selection);
				}
			}
			break;

		case IDC_D_BROWSE_SCRIPT:
			p->SelectScript();
			break;
		}
		break;
	}

	return FALSE;
}

void CDialogueWnd::UpdateList()
{
	if(m_hWnd == NULL)
		return;

	SendMessage(GetDlgItem(m_hWnd, IDC_D_LIST), LB_RESETCONTENT, 0, 0);

	std::string s,c;
	std::vector<CDialogue::SComment> *v = m_template.GetComments();
	char szTemp[128];

	for(int i=0;i<v->size();i++)
	{
		s = (*v)[i].speaker;
		s+= " - ";
		c = (*v)[i].comment;
		if(c.size() > 30)
		{
			c = c.substr(0, 30);
			s += c + "...";
		}
		else
			s += c;

		strcpy(szTemp, s.c_str());
		SendMessage(GetDlgItem(m_hWnd, IDC_D_LIST), LB_INSERTSTRING, i, (LPARAM)szTemp);
	}
}

void CDialogueWnd::SelectScript()
{
	std::string script;
	if(CDatabaseUtilities::SelectLocalScript(m_hWnd, script))
	{
		SetWindowText(GetDlgItem(m_hWnd, IDC_D_SCRIPT), script.c_str());
	}
}

bool CDialogueWnd::OpenWindow(HWND parent, CDialogue &rTemplate, bool bNew)
{
	m_template = rTemplate;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOGUE), parent, WndProc);

	if(m_bModified)
	{
		rTemplate = m_template;
		return TRUE;
	}
	else
		return FALSE;
}
