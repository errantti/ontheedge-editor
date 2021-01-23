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

#include "SelectionWnd.h"
#include "..\\resource.h"
#include "..\\Editor.h"

using namespace std;

CSelectionWnd *CSelectionWnd::m_pointer = NULL;
vector<CSelectionWnd*> CSelectionWnd::m_instances;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSelectionWnd::CSelectionWnd() : m_selected(-1), m_hWnd(NULL)
{
	m_pointer = this;
	m_instances.push_back(this);
}

CSelectionWnd::~CSelectionWnd()
{
	m_pointer = NULL;
	vector<CSelectionWnd*>::iterator it = m_instances.begin();
	for(; it != m_instances.end(); ++it)
		if((*it) == this)
		{
			m_instances.erase(it);
			break;
		}
	if(m_instances.size()>0)
		m_pointer = m_instances[m_instances.size()-1];
}

CSelectionWnd *CSelectionWnd::GetInstanceByHWND(HWND hWnd)
{
	vector<CSelectionWnd*>::iterator it = m_instances.begin();
	for(; it != m_instances.end(); ++it)
		if((*it)->m_hWnd == hWnd)
			return (*it);
	return NULL;
}


/****
 * Selection window callback
 ****/
BOOL CALLBACK CSelectionWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CSelectionWnd *p = CSelectionWnd::m_pointer;

	bool bActive = false;
	if(p->m_hWnd == hWnd)
		bActive = true;
	else
	{
		// Get the correct instance
		CSelectionWnd *inst = GetInstanceByHWND(hWnd);
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
	// TODO: implement bActive checks

	int selection = -1, i;
	HWND list = NULL;
	p->m_hWnd = hWnd;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		SetWindowText(hWnd, p->m_caption.c_str());
		list = GetDlgItem(hWnd, IDC_SELECTION_LIST);
		for(i=0;i<p->m_entries.size();i++)
			SendMessage(list, LB_INSERTSTRING, i, (LPARAM)p->m_entries[i].name);
		break;
	case WM_CLOSE:
		p->m_selected = -1;
		EndDialog(hWnd, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_SELECTION_CANCEL:
			p->m_selected = -1;
			EndDialog(hWnd, FALSE);
			break;
		case ID_SELECTION_SELECT:
			if((selection = (int)SendMessage(GetDlgItem(hWnd, IDC_SELECTION_LIST), LB_GETCURSEL, 0, 0)) != LB_ERR)
			{
				p->m_selected = selection;
				EndDialog(hWnd, FALSE);
			}
			else
				::MessageBox(hWnd, "You haven't selected any item!", "Error!", MB_OK);
			break;
		case IDC_SELECTION_LIST:
			if(HIWORD(wParam) == LBN_DBLCLK)
			{
				if((selection = (int)SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0)) != LB_ERR)
				{
					p->m_selected = selection;
					EndDialog(hWnd, FALSE);
				}
			}
			break;
		}
		break;
	}

	return FALSE;
}

void CSelectionWnd::UpdateList()
{
	if(m_hWnd == NULL)
		return;

	for(int i=0;i<m_entries.size();i++)
		SendMessage(GetDlgItem(m_hWnd, IDC_SELECTION_LIST), LB_INSERTSTRING, i, (LPARAM)m_entries[i].name);
}


bool CSelectionWnd::OpenWindow(HWND parent, long &selectedID, std::string caption, std::string *text)
{
	m_caption = "Select ";
	m_caption += caption;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECTION), parent, WndProc);

	selectedID = 0;

	if(m_selected<0 || m_selected >= m_entries.size())
		return FALSE;

	selectedID = m_entries[m_selected].id;
	if(text != NULL)
		*text = m_entries[m_selected].name;
	return TRUE;
}
