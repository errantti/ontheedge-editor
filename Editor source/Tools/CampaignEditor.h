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

#ifndef __CCAMPAIGNEDITOR_H__
#define __CCAMPAIGNEDITOR_H__

#include <windows.h>
#include "..\\OtE\\Campaign.h"

class CCampaignEditor
{
public:
	CCampaignEditor();
	~CCampaignEditor();

	void Open(HWND parent, std::string filename);

private:
	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam); 

	static CCampaignEditor *m_pointer;

	void UpdateFormInfo(HWND hWnd);
	void UpdateCampaignInfo(HWND hWnd);

	void UpdateGadgetList(HWND hWnd);
	void UpdateBlockList(HWND hWnd, bool activateSelection = false, int selection = 0);

	void EditBlock(HWND hWnd);
	void MoveBlockDown(HWND hWnd);
	void MoveBlockUp(HWND hWnd);
	void AddBlock(HWND hWnd);
	void DeleteBlock(HWND hWnd);

	void AddGadget(HWND hWnd);
	void RemoveGadget(HWND hWnd);
	void EquipGadget(HWND hWnd);

	void OpenPlayerTerminalSettings(HWND hWnd);

	bool m_bModified;
	std::string m_filename;
	CCampaign *m_camp;
	CTerminal m_playerTerminal;

	std::vector<long> *m_gadgets;
	std::vector<CCampaign::SCampaignBlock> *m_blocks;
};

#endif // #ifndef __CCAMPAIGNEDITOR_H__
