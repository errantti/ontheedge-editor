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

#ifndef __CDATABASESELECTIONWND_H__
#define __CDATABASESELECTIONWND_H__

#include "SelectionWnd.h"
#include "IDatabaseHandler.h"

class CDatabaseSelectionWnd : public ISelectionList
{
public:
	CDatabaseSelectionWnd();
	virtual ~CDatabaseSelectionWnd();

	bool OpenWindow(HWND parent, std::string caption, IDatabaseHandler *database, bool bMakeSureIsSaved = true, 
		bool bShowSave = TRUE, bool bShowImportExport = FALSE, bool bShowClone = FALSE);

	inline void AddEntry(SSelectionEntry e) {m_entries.push_back(e);}
	inline void ClearEntries() {m_entries.clear();}
	inline void SetEntries(std::vector<SSelectionEntry> &entries) {m_entries = entries;}

	virtual void UpdateList();

private:
	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static CDatabaseSelectionWnd *GetInstanceByHWND(HWND hWnd);

	static CDatabaseSelectionWnd *m_pointer;
	static std::vector<CDatabaseSelectionWnd*> m_instances;

	std::vector<SSelectionEntry> m_entries;
	int m_selected;
	std::string m_caption;

	bool m_bShowSave, m_bShowImportExport, m_bShowClone;

	IDatabaseHandler *m_database;
	HWND m_hWnd;

	bool m_bSaved, m_bMakeSureIsSaved;
};

#endif // #ifndef __CDATABASESELECTIONWND_H__
