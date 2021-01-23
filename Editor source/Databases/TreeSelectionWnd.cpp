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

#include "TreeSelectionWnd.h"
#include "..\\resource.h"
//#include "..\\Editor.h"

#include <commctrl.h>
#include <algorithm>

using namespace std;

CTreeSelectionWnd *CTreeSelectionWnd::m_pointer = NULL;
vector<CTreeSelectionWnd*> CTreeSelectionWnd::m_instances;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTreeSelectionWnd::CTreeSelectionWnd() : m_hWnd(NULL), m_selectedID(-1)
{
	m_pointer = this;
	m_instances.push_back(this);
}

CTreeSelectionWnd::~CTreeSelectionWnd()
{
	m_pointer = NULL;
	vector<CTreeSelectionWnd*>::iterator it = m_instances.begin();
	for(; it != m_instances.end(); ++it)
		if((*it) == this)
		{
			m_instances.erase(it);
			break;
		}
	if(m_instances.size()>0)
		m_pointer = m_instances[m_instances.size()-1];
}

CTreeSelectionWnd *CTreeSelectionWnd::GetInstanceByHWND(HWND hWnd)
{
	vector<CTreeSelectionWnd*>::iterator it = m_instances.begin();
	for(; it != m_instances.end(); ++it)
		if((*it)->m_hWnd == hWnd)
			return (*it);
	return NULL;
}


/****
 * Selection window callback
 ****/
BOOL CALLBACK CTreeSelectionWnd::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	CTreeSelectionWnd *p = CTreeSelectionWnd::m_pointer;

	bool bActive = false;
	if(p->m_hWnd == hWnd)
		bActive = true;
	else
	{
		// Get the correct instance
		CTreeSelectionWnd *inst = GetInstanceByHWND(hWnd);
		if(inst == NULL)
		{
			if(p->m_hWnd == NULL)
				p->m_hWnd = hWnd;
			else
			{
				CPrinter::logError("Weird error: couldn't find tree selection window instance by window handle");
				return FALSE;
			}
		}
	}
	// TODO: implement bActive checks

	long id;
	HWND list = NULL;
	p->m_hWnd = hWnd;

	switch(iMessage)
	{
	case WM_INITDIALOG:
		InitCommonControls();
		p->UpdateList();

		SetWindowText(hWnd, p->m_caption.c_str());

		p->m_bSelected = FALSE;
		break;

	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_SELECTION_CANCEL:
			EndDialog(hWnd, FALSE);
			break;

		case ID_SELECTION_SELECT:
			if((id = p->GetSelectedID(true, true)) != -1)
			{
				p->m_selectedID = id;
				p->m_bSelected = TRUE;
				EndDialog(hWnd, FALSE);
			}
			break;
		}
		break;
	case WM_NOTIFY:
		switch(LOWORD(wParam))
		{
		case IDC_TREE1:
			if(((LPNMHDR)lParam)->code == NM_DBLCLK)
			{
				if((id = p->GetSelectedID(true, true)) != -1)
				{
					p->m_selectedID = id;
					p->m_bSelected = TRUE;
					EndDialog(hWnd, FALSE);
				}
			}
			break;
		}
		break;
	}

	return FALSE;
}

void CTreeSelectionWnd::UpdateList()
{
	if(m_hWnd == NULL)
		return;

	ClearTree(m_hWnd);

	TV_INSERTSTRUCT tv;
	ZeroMemory(&tv, sizeof(TV_INSERTSTRUCT));

	HTREEITEM root = NULL;

	// Sort entries by category
	sort(m_entries.begin(), m_entries.end());

	char category[64] = "";
	bool firstRootCreated = false;
	category[0] = '\0';

	vector<SSelectionEntry>::iterator it = m_entries.begin();
	for(; it != m_entries.end(); ++it)
	{
		if(!firstRootCreated || strncmp(category, it->category, 63))
		{
			// Create new root for a new category
			tv.hParent = NULL;
			tv.hInsertAfter = TVI_ROOT;
			tv.item.pszText = it->category;
			tv.item.mask = TVIF_TEXT;
			tv.item.iImage = -1;
			tv.item.iSelectedImage = -1;

			strncpy(category, it->category, 63);
			//strcpy(category, "muh");

			firstRootCreated = true;

			root = (HTREEITEM)SendDlgItemMessage(m_hWnd, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)&tv);
		}

		if(root == NULL)
		{
			CPrinter::logError("Internal error while populating tree selection view: null root");
			return;
		}

		tv.hParent = root;
		tv.hInsertAfter = TVI_LAST;
		tv.item.pszText = it->name;
		tv.item.mask = TVIF_TEXT;
		// A nice hack to make extracting id out of an HTREEITEM easier after selection
		//tv.item.iImage = (int)it->id;
		SendDlgItemMessage(m_hWnd, IDC_TREE1, TVM_INSERTITEM, 0, (LPARAM)&tv);
	}
}

void CTreeSelectionWnd::ClearTree(HWND hWnd)
{
	int treeCount = TreeView_GetCount(GetDlgItem(hWnd, IDC_TREE1));
	for(int i=0;i<=treeCount;++i)
		TreeView_DeleteAllItems(GetDlgItem(hWnd, IDC_TREE1));
}

long CTreeSelectionWnd::GetIDByEntryName(char *text)
{
	vector<SSelectionEntry>::iterator it = m_entries.begin();
	for(; it != m_entries.end(); ++it)
		if(!strncmp(text, it->name, 63))
			return it->id;
	return -1;
}

long CTreeSelectionWnd::GetSelectedID(bool bNotifyError, bool bSetSelectedText)
{
	TV_ITEM tv;
	HTREEITEM selected = NULL;
	char text[64] = "";

	memset(&tv, 0, sizeof(tv));
	selected = (HTREEITEM)SendDlgItemMessage(m_hWnd, IDC_TREE1, TVM_GETNEXTITEM, TVGN_CARET, (LPARAM)selected);

	if(selected == NULL)
	{
		if(bNotifyError)
			::MessageBox(m_hWnd, "You haven't selected any item!", "Error!", MB_OK);
		return -1;
	}

	TreeView_EnsureVisible(m_hWnd, selected);
	SendDlgItemMessage(m_hWnd, IDC_TREE1, TVM_SELECTITEM, TVGN_CARET, (LPARAM)selected);

	tv.mask = TVIF_TEXT;
	tv.pszText = text;
	tv.cchTextMax = 63;
	tv.hItem = selected;

	if(SendDlgItemMessage(m_hWnd, IDC_TREE1, TVM_GETITEM, TVGN_CARET, (LPARAM)&tv))
	{
		if(tv.cChildren == 0)
		{
			if(bSetSelectedText)
				m_selectedText = text;
			return GetIDByEntryName(text);
		}
	}

	return -1;
}

bool CTreeSelectionWnd::OpenWindow(HWND parent, long &selectedID, std::string caption, std::string *text)
{
	m_caption = "Select ";
	m_caption += caption;

	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_TREE_SELECTION), parent, WndProc);

	if(m_bSelected)
	{
		selectedID = m_selectedID;
		if(text != NULL)
			*text = m_selectedText;
		return TRUE;
	}

	return FALSE;
}
