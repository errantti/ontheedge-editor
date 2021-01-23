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

#ifndef __CSCRIPTWND_H__
#define __CSCRIPTWND_H__

#include <windows.h>

#include "..\\..\\OtE\\Edgewalker\\EWScript\\Script.h"
#include "..\\..\\OtE\\Edgewalker\\ILogWriter.h"

/**
 * Handles a window that lets the user create, edit, import
 * and export global and local scripts
 */
class CScriptWnd : public ILogWriter
{
public:
	CScriptWnd();
	~CScriptWnd();

	bool OpenWindow(HWND parent, CScript &rTemplate, bool bNew);

	void Log(const std::string text);
	void LogError(const std::string errorMsg);

private:
	bool Export(HWND hWnd);
	bool Import(HWND hWnd);
	bool Import(std::string filename, HWND hWnd, bool notifyFailure);
	bool ImportFromScript(HWND hWnd);
	bool SaveToFile(HWND hWnd);
	bool Validate(HWND hWnd, bool save = false);

	void SelectStatic(HWND hWnd);
	void SelectDynamic(HWND hWnd);
	void SelectEnemy(HWND hWnd);
	void SelectGadget(HWND hWnd);
	void SelectPatch(HWND hWnd);
	void SelectComponent(HWND hWnd);
	void SelectQuestItem(HWND hWnd);
	void SelectPEffect(HWND hWnd);
	void SelectLEffect(HWND hWnd);
	void SelectSound(HWND hWnd);
	void SelectLocaScript(HWND hWnd);
	void SelectGlobalScript(HWND hWnd);
	void SelectDialogue(HWND hWnd);
	void SelectTerminal(HWND hWnd);
	void SelectRoute(HWND hWnd);
	void SelectController(HWND hWnd);
	void SelectAssistantMsg(HWND hWnd);

	void ClearMessages();
	void AddMessage(const std::string msg);

	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static CScriptWnd *m_pointer;

	static bool m_bAutosaveScript;

	CScript *m_template;
	bool m_bModified, m_bNew, m_bSavedToFile;

	HWND m_hWnd;

	int m_numMessages;
};

#endif // #ifndef __CVESSELOBJECTWND_H__
