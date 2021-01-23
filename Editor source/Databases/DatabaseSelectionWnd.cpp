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

#include "DatabaseSelectionWnd.h"
#include "..\\resource.h"
#include "..\\Editor.h"
#include <algorithm>

using namespace std;

CDatabaseSelectionWnd *CDatabaseSelectionWnd::m_pointer = NULL;
vector<CDatabaseSelectionWnd*> CDatabaseSelectionWnd::m_instances;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDatabaseSelectionWnd::CDatabaseSelectionWnd() : m_database(NULL), 
	m_hWnd(NULL), m_bSaved(TRUE)
{
	m_pointer = this;
	m_instances.push_back(this);
}

CDatabaseSelectionWnd::~CDatabaseSelectionWnd()
{
	m_pointer = NULL;
	vector<CDatabaseSelectionWnd*>::iterator it = m_instances.begin();
	for(; it != m_instances.end(); ++it)
		if((*it) == this)
		{
			m_instances.erase(it);
			break;
		}
	if(m_instances.size()>0)
		m_pointer = m_instances[m_instances.size()-1];
}

CDatabaseSelectionWnd *CDatabaseSelectionWnd::GetInstanceByHWND(HWND hWnd)
{
	vector<CDatabaseSelectionWnd*>::iterator it = m_instances.begin();
	for(; it != m_instances.end(); ++it)
		if((*it)->m_hWnd == hWnd)
			return (*it);
	return NULL;
}


/*****
 * Database selection window callback
 *****/
BOOL CALLBACK CDatabaseSelectionWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CDatabaseSelectionWnd *p = CDatabaseSelectionWnd::m_pointer;

	bool bActive = false;
	if(p->m_hWnd == hWnd)
		bActive = true;
	else
	{
		// Get the correct instance
		CDatabaseSelectionWnd *inst = GetInstanceByHWND(hWnd);
		if(inst == NULL)
		{
			if(p->m_hWnd == NULL)
				p->m_hWnd = hWnd;
			else
			{
				CPrinter::logError("Weird error: couldn't find selection window instance by window handle");
				return FALSE;
			}
		}
	}

	int selection = -1;
	HWND list = NULL;
	p->m_hWnd = hWnd;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		SetWindowText(hWnd, p->m_caption.c_str());
		SetWindowText(GetDlgItem(hWnd, ID_SELECTION_CANCEL), "Delete");
		SetWindowText(GetDlgItem(hWnd, ID_SELECTION_SELECT), "Edit");
		ShowWindow(GetDlgItem(hWnd, ID_SELECTION_ADD), TRUE);
		//ShowWindow(GetDlgItem(hWnd, ID_SELECTION_SAVE), TRUE);

		if(p->m_bShowClone)
			ShowWindow(GetDlgItem(hWnd, ID_SELECTION_CLONE), TRUE);
		if(p->m_bShowImportExport)
		{
			ShowWindow(GetDlgItem(hWnd, ID_SELECTION_IMPORT), TRUE);
			ShowWindow(GetDlgItem(hWnd, ID_SELECTION_EXPORT), TRUE);
		}
		if(p->m_bShowSave)
			ShowWindow(GetDlgItem(hWnd, ID_SELECTION_SAVE), TRUE);

		p->m_bSaved = TRUE;
		p->UpdateList();
		break;
	case WM_CLOSE:
		if(p->m_bSaved == FALSE && p->m_bMakeSureIsSaved)
		{
			if(::MessageBox(hWnd, "You haven't saved the changes to the database. Are you sure you want exit?", "OtE Editor database", MB_YESNO) == IDYES)
				EndDialog(hWnd, FALSE);
		}
		else
			EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_SELECTION_CANCEL:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_SELECTION_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				if(p->m_database)
					p->m_database->RemoveTemplate(p->m_entries[selection].id, p);
				p->m_bSaved = FALSE;
			}
			else
				::MessageBox(hWnd, "You haven't selected any item!", "Error!", MB_OK);
			break;

		case ID_SELECTION_SELECT:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_SELECTION_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				if(p->m_database)
					p->m_database->EditTemplate(p->m_entries[selection].id, p, hWnd);
				p->m_bSaved = FALSE;
			}
			else
				::MessageBox(hWnd, "You haven't selected any item!", "Error!", MB_OK);
			break;

		case ID_SELECTION_ADD:
			if(p->m_database)
				p->m_database->AddNewTemplate(p, hWnd);
			p->m_bSaved = FALSE;
			break;

		case ID_SELECTION_SAVE:
			if(p->m_database)
				p->m_database->SaveDatabase();
			p->m_bSaved = TRUE;
			break;

		case ID_SELECTION_IMPORT:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_SELECTION_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				if(p->m_database)
					p->m_database->ImportTemplate(p->m_entries[selection].id, p, hWnd);
			}
			else if(p->m_database)
				p->m_database->ImportTemplate(-1, p, hWnd);
			p->m_bSaved = FALSE;
			break;

		case ID_SELECTION_EXPORT:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_SELECTION_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				if(p->m_database)
					p->m_database->ExportTemplate(p->m_entries[selection].id, hWnd);
			}
			else
				::MessageBox(hWnd, "You haven't selected any item!", "Error!", MB_OK);
			break;

		case ID_SELECTION_CLONE:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_SELECTION_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				if(p->m_database)
					p->m_database->CloneTemplate(p->m_entries[selection].id, p);
			}
			else
				::MessageBox(hWnd, "You haven't selected any item!", "Error!", MB_OK);
			break;

		case IDC_SELECTION_LIST:
			if(HIWORD(wParam) == LBN_DBLCLK)
			{
				if((selection = (int)SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0)) != LB_ERR)
				{
					if(p->m_database)
						p->m_database->EditTemplate(p->m_entries[selection].id, p, hWnd);
					p->m_bSaved = FALSE;
				}
			}
			break;
		}
		break;
	}

	return FALSE;
}

void CDatabaseSelectionWnd::UpdateList()
{
	if(m_hWnd == NULL)
		return;

	sort(m_entries.begin(), m_entries.end());

	SendMessage(GetDlgItem(m_hWnd, IDC_SELECTION_LIST), LB_RESETCONTENT, 0, 0);

	for(int i=0;i<m_entries.size();i++)
		SendMessage(GetDlgItem(m_hWnd, IDC_SELECTION_LIST), LB_INSERTSTRING, i, (LPARAM)m_entries[i].name);
}

bool CDatabaseSelectionWnd::OpenWindow(HWND parent, std::string caption, IDatabaseHandler *database, bool bMakeSureIsSaved,
									   bool bShowSave, bool bShowImportExport, bool bShowClone)
{
	m_caption = caption;
	m_database = database;
	m_bMakeSureIsSaved = bMakeSureIsSaved;

	m_bShowClone = bShowClone;
	m_bShowImportExport = bShowImportExport;
	m_bShowSave = bShowSave;

	if(m_database)
	{
		m_database->LoadDatabase();
		m_database->UpdateSelectionList(this);
	}
	else
		CPrinter::log("Internal error: couldn't open database", LOG_ERROR);

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECTION), parent, WndProc);

	return TRUE;
}