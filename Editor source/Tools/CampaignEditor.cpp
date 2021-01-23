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

#include "..\\OtE\\OnTheEdge.h"
#include "..\\resource.h"
#include "..\\Databases\\DatabaseUtilities.h"
#include "CampaignEditor.h"
#include "CampaignBlockWnd.h"
#include "TerminalWnd.h"

CCampaignEditor *CCampaignEditor::m_pointer = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampaignEditor::CCampaignEditor() : m_camp(NULL), m_blocks(NULL), m_gadgets(NULL)
{
	m_pointer = this;
	m_bModified = false;
}

CCampaignEditor::~CCampaignEditor()
{
	m_pointer = NULL;
}

BOOL CALLBACK CCampaignEditor::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CCampaignEditor *p = CCampaignEditor::m_pointer;
	long ID;
	char szTemp[16];

	switch(iMessage)
	{
	case WM_INITDIALOG:
		p->UpdateFormInfo(hWnd);
		break;
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			p->UpdateCampaignInfo(hWnd);
			p->m_bModified = TRUE;

			EndDialog(hWnd, FALSE);
			break;
		case IDCANCEL:
			EndDialog(hWnd, FALSE);
			break;

		/**
		 * Block editing
		 **/
		case IDC_CE_ADDBLOCK:
			p->AddBlock(hWnd);
			break;
		case IDC_CE_DELETEBLOCK:
			p->DeleteBlock(hWnd);
			break;
		case IDC_CE_EDITBLOCK:
			p->EditBlock(hWnd);
			break;
		case IDC_CE_MOVEUP:
			p->MoveBlockUp(hWnd);
			break;
		case IDC_CE_MOVEDOWN:
			p->MoveBlockDown(hWnd);
			break;

		/**
		 * Gadget editing
		 **/
		case IDC_CE_ADDGADGET:
			p->AddGadget(hWnd);
			break;
		case IDC_CE_REMOVEGADGET:
			p->RemoveGadget(hWnd);
			break;
		case IDC_CE_EQUIPGADGET:
			p->EquipGadget(hWnd);
			break;
		case IDC_CE_REMOVEGADGET1:
			SetWindowText(GetDlgItem(hWnd, IDC_CE_GADGET1), "-1");
			break;
		case IDC_CE_REMOVEGADGET2:
			SetWindowText(GetDlgItem(hWnd, IDC_CE_GADGET2), "-1");
			break;

		/**
		 * Other
		 **/
		case IDC_CE_BROWSESHIP:
			ID = p->m_camp->GetStartShipID();
			CDatabaseUtilities::SelectShip(hWnd, ID);
			sprintf(szTemp, "%i", ID);
			SetWindowText(GetDlgItem(hWnd, IDC_CE_SHIPID), szTemp);
			break;

		case IDC_CE_BLOCKLIST:
			if(HIWORD(wParam) == LBN_DBLCLK)
			{
				p->EditBlock(hWnd);
			}
			break;

		case IDC_CE_PLAYER_TERMINAL:
			p->OpenPlayerTerminalSettings(hWnd);
			break;
		}
	}

	return FALSE;
}

void CCampaignEditor::OpenPlayerTerminalSettings(HWND hWnd)
{
	if(m_camp == NULL)
		return;

	CTerminalWnd wnd;
	wnd.OpenWindow(hWnd, m_playerTerminal);
}

void CCampaignEditor::UpdateCampaignInfo(HWND hWnd)
{
	if(m_camp == NULL)
		return;

	char szTemp[16];

	GetWindowText(GetDlgItem(hWnd, IDC_CE_GADGET1), szTemp, 8);
	m_camp->SetEquippedGadget(0, atoi(szTemp));
	GetWindowText(GetDlgItem(hWnd, IDC_CE_GADGET2), szTemp, 8);
	m_camp->SetEquippedGadget(1, atoi(szTemp));
	GetWindowText(GetDlgItem(hWnd, IDC_CE_SHIPID), szTemp, 8);
	m_camp->SetStartShipID(atoi(szTemp));

	GetWindowText(GetDlgItem(hWnd, IDC_CE_NAME), m_camp->GetName(), 63);
	GetWindowText(GetDlgItem(hWnd, IDC_CE_ENDMESSAGE), m_camp->GetEndMessage(), 2047);

	if(SendMessage(GetDlgItem(hWnd, IDC_CE_FORCESHIP), BM_GETCHECK, 0, 0))
		m_camp->SetIsShipForced(TRUE);
	else
		m_camp->SetIsShipForced(FALSE);

	*m_camp->GetPlayerTerminal() = m_playerTerminal;

	// Set block IDs
	m_camp->SetFirstBlockID(1);
	for(int i=0; i<m_blocks->size(); ++i)
	{
		(*m_blocks)[i].blockID = i+1;
		(*m_blocks)[i].nextBlockID = i+2;
	}
}

void CCampaignEditor::UpdateFormInfo(HWND hWnd)
{
	if(m_camp == NULL)
		return;

	char szTemp[16];

	// Update blocks / levels and gadgets
	UpdateBlockList(hWnd);
	UpdateGadgetList(hWnd);

	sprintf(szTemp, "%i", m_camp->GetEquippedGadget(0));
	SetWindowText(GetDlgItem(hWnd, IDC_CE_GADGET1), szTemp);
	sprintf(szTemp, "%i", m_camp->GetEquippedGadget(1));
	SetWindowText(GetDlgItem(hWnd, IDC_CE_GADGET2), szTemp);
	sprintf(szTemp, "%i", m_camp->GetStartShipID());
	SetWindowText(GetDlgItem(hWnd, IDC_CE_SHIPID), szTemp);

	SetWindowText(GetDlgItem(hWnd, IDC_CE_NAME), m_camp->GetName());
	SetWindowText(GetDlgItem(hWnd, IDC_CE_ENDMESSAGE), m_camp->GetEndMessage());

	m_playerTerminal = *m_camp->GetPlayerTerminal();

	if(m_camp->IsShipForced())
		SendMessage(GetDlgItem(hWnd, IDC_CE_FORCESHIP), BM_SETCHECK, 1, 0);
}

void CCampaignEditor::UpdateBlockList(HWND hWnd, bool activateSelection, int selection)
{
	HWND list = GetDlgItem(hWnd, IDC_CE_BLOCKLIST);
	SendMessage(list, LB_RESETCONTENT, 0, 0);

	for(int i=0; i<m_blocks->size(); ++i)
		SendMessage(list, LB_INSERTSTRING, i, (LPARAM)(*m_blocks)[i].levelFilename);

	if(activateSelection)
	{
		SendMessage(GetDlgItem(hWnd, IDC_CE_BLOCKLIST), LB_SETCURSEL, (WPARAM)selection, 0);
	}
}

void CCampaignEditor::UpdateGadgetList(HWND hWnd)
{
	HWND list = GetDlgItem(hWnd, IDC_CE_GADGETLIST);
	SendMessage(list, LB_RESETCONTENT, 0, 0);
	std::string name;
	CGadgetDatabase *db = GetOTE()->GetGadgetDatabase();

	char szTemp[128];
	for(int i=0; i<m_gadgets->size(); ++i)
	{
		if(db->GetGadgetName((*m_gadgets)[i], name))
			sprintf(szTemp, "%i - %s", (*m_gadgets)[i], name.c_str());
		else
			sprintf(szTemp, "%i -", (*m_gadgets)[i]);
		SendMessage(list, LB_INSERTSTRING, i, (LPARAM)szTemp);
	}
}

/******* 
 * Block edit methods
 *******/

void CCampaignEditor::EditBlock(HWND hWnd)
{
	int selection;
	CCampaignBlockWnd w;
	if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_CE_BLOCKLIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
	{
		w.OpenWindow(hWnd, (*m_blocks)[selection], FALSE);
	}
}

void CCampaignEditor::MoveBlockDown(HWND hWnd)
{
	int selection;
	if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_CE_BLOCKLIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
	{
		if(selection >= m_blocks->size()-1 || m_blocks->size() < 2)
			return;
		CCampaign::SCampaignBlock s;
		s = (*m_blocks)[selection];
		(*m_blocks)[selection] = (*m_blocks)[selection+1];
		(*m_blocks)[selection+1] = s;
		UpdateBlockList(hWnd, true, selection+1);
	}
}

void CCampaignEditor::MoveBlockUp(HWND hWnd)
{
	int selection;
	if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_CE_BLOCKLIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
	{
		if(selection <= 0 || m_blocks->size() < 2)
			return;
		CCampaign::SCampaignBlock s;
		s = (*m_blocks)[selection];
		(*m_blocks)[selection] = (*m_blocks)[selection-1];
		(*m_blocks)[selection-1] = s;
		UpdateBlockList(hWnd);
		UpdateBlockList(hWnd, true, selection-1);
	}
}

void CCampaignEditor::AddBlock(HWND hWnd)
{
	CCampaignBlockWnd w;
	CCampaign::SCampaignBlock b;
	if(w.OpenWindow(hWnd, b, TRUE))
	{
		m_blocks->push_back(b);
		UpdateBlockList(hWnd);
	}
}

void CCampaignEditor::DeleteBlock(HWND hWnd)
{
	int selection, i=0;
	if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_CE_BLOCKLIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
	{
		std::vector<CCampaign::SCampaignBlock>::iterator it = m_blocks->begin();
		for(;it != m_blocks->end(); ++it)
		{
			if(i == selection)
			{
				m_blocks->erase(it);
				UpdateBlockList(hWnd);
				return;
			}
			++i;
		}
	}
}


/********
 * Gadget edit methods
 ********/

void CCampaignEditor::AddGadget(HWND hWnd)
{
	long ID = -1;
	CDatabaseUtilities::SelectGadget(hWnd, ID);
	if(ID != -1)
	{
		m_gadgets->push_back(ID);
		UpdateGadgetList(hWnd);
	}
}

void CCampaignEditor::RemoveGadget(HWND hWnd)
{
	int selection, i=0;
	if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_CE_GADGETLIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
	{
		std::vector<long>::iterator it = m_gadgets->begin();
		for(;it != m_gadgets->end(); ++it)
		{
			if(i == selection)
			{
				m_gadgets->erase(it);
				UpdateGadgetList(hWnd);
				return;
			}
			++i;
		}
	}
}

void CCampaignEditor::EquipGadget(HWND hWnd)
{
	int selection;
	char szTemp[16];
	if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_CE_GADGETLIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
	{
		std::pair<SGadgetTemplate, SCustomGadgetTemplate*> *p = GetOTE()->GetGadgetDatabase()->GetGadgetTemplate((*m_gadgets)[selection]);
		if(p != NULL)
		{
			if(p->first.bActivateable)
			{
				sprintf(szTemp, "%i", (*m_gadgets)[selection]);
				SetWindowText(GetDlgItem(hWnd, IDC_CE_GADGET2), szTemp);
			}
			else
			{
				sprintf(szTemp, "%i", (*m_gadgets)[selection]);
				SetWindowText(GetDlgItem(hWnd, IDC_CE_GADGET1), szTemp);
			}
		}
	}
}


/*** 
 * 
 ***/

void CCampaignEditor::Open(HWND parent, std::string filename)
{
	m_camp = GetOTE()->GetCampaign();
	if(m_camp == NULL)
	{
		MessageBox(parent, "Very weird error: can't find campaign object", "Error", MB_OK);
		return;
	}

	m_filename = m_camp->GetCampaignFilename();

	m_blocks = m_camp->GetBlocks();
	m_gadgets = m_camp->GetStartGadgetIDs();

	// Load the campaign - NO MORE! Campaign should be already loaded
	/*if(m_camp->LoadCampaign(filename.c_str()) == FALSE)
	{
		MessageBox(parent, "Failed loading campaign", "Error", MB_OK);
		CPrinter::log("Failed loading campaign - default campaign initiated", LOG_ERROR);
		m_camp->InitDefaultCampaign();
	}*/

	// Open the window
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CAMPAIGN_EDITOR), parent, WndProc);

	if(m_bModified)
	{
		// Save campaign
		if(m_camp->SaveCampaign(m_filename.c_str()) == FALSE)
		{
			MessageBox(parent, "Failed saving campaign", "Error", MB_OK);
			CPrinter::log("Failed saving campaign", LOG_ERROR);
			return;
		}
		else
			MessageBox(parent, "Campaign succesfully saved", "OtE Editor", MB_OK);
	}
}