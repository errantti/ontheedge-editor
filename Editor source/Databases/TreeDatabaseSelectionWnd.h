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

#ifndef __CTREEDATABASESELECTIONWND_H__
#define __CTREEDATABASESELECTIONWND_H__

#include <windows.h>
#include <string>
#include "ISelectionList.h"
#include "IDatabaseHandler.h"

/**
 * CTreeDatabaseSelectionWnd is a 'template' selection window class that
 * takes several parameters and lets user select an item from
 * a tree view ordered by category
 */
class CTreeDatabaseSelectionWnd : public ISelectionList
{
public:
	CTreeDatabaseSelectionWnd();
	~CTreeDatabaseSelectionWnd();

	bool OpenWindow(HWND parent, std::string caption, IDatabaseHandler *database, bool bModal = TRUE, bool bMakeSureIsSaved = true);

	inline void AddEntry(SSelectionEntry e) {m_entries.push_back(e);}
	inline void ClearEntries() {m_entries.clear();}
	inline void SetEntries(std::vector<SSelectionEntry> &entries) {m_entries = entries;}

	virtual void UpdateList();

	HWND GetWindowHandle() {return m_hWnd;}

private:
	void ClearTree(HWND hWnd);
	long GetIDByEntryName(char *text);
	long GetSelectedID(bool bNotifyError);

	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	/// Topmost selection window
	static CTreeDatabaseSelectionWnd *m_pointer;
	/// Stack of selection windows
	static std::vector<CTreeDatabaseSelectionWnd*> m_instances;

	static CTreeDatabaseSelectionWnd *GetInstanceByHWND(HWND hWnd);

	std::vector<SSelectionEntry> m_entries;
	int m_selected;
	std::string m_caption;

	IDatabaseHandler *m_database;
	HWND m_hWnd;

	bool m_bSaved, m_bMakeSureIsSaved;
};

#endif // #ifndef __CSELECTIONWND_H__
