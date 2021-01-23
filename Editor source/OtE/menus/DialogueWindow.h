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

#ifndef __CDIALOGUEWINDOW_H__
#define __CDIALOGUEWINDOW_H__

#include "IMenuHandler.h"
#include "..\\Screen\\Dialogue.h"

enum DIALOGUEWINDOW_ELEMENTS
{
	DW_TEXT = 1200,
	DW_SPEAKER,
	DW_SKIP,
	DW_NEXT
};

/**
 * Dialogue view window handler
 */
class CDialogueWindow : public IMenuHandler  
{
public:
	CDialogueWindow(IMenuManager *mgr);
	virtual ~CDialogueWindow();

	static void SetDialoguePointer(CDialogue *dialogue) {m_dialogue = dialogue;}

	virtual bool InitializeMenu();
	virtual void OnClose();

	virtual bool OnEvent(SEvent event);

	virtual void Update();

	void UpdateDialogueComment(std::string message, std::string speaker);

	void CloseView();

private:
	D3DCOLOR m_modColor;
	CFont *m_fontDejaVuSansMono12;

	bool m_bSkipDown, m_bClosing;

	float m_fadeDuration;

	static CDialogue *m_dialogue;
};

#endif // #ifndef __CINVENTORYMENU_H__
