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

#ifndef __CTERMINALVIEW_H__
#define __CTERMINALVIEW_H__

#include "IMenuHandler.h"
#include "..\\Screen\\Terminal.h"

enum TERMINALVIEW_ELEMENTS
{
	TV_BUTTON1 = 1100,
	TV_BUTTON2,
	TV_BUTTON3,
	TV_BUTTON4,
	TV_BUTTON5,
	TV_BUTTON6,
	TV_CLOSE,
	TV_MESSAGE,
	TV_NAME
};

/**
 * Terminal view window handler
 */
class CTerminalView : public IMenuHandler  
{
public:
	CTerminalView(IMenuManager *mgr);
	virtual ~CTerminalView();

	static void SetTerminalPointer(CTerminal *terminal) {m_terminal = terminal;}

	virtual bool InitializeMenu();
	virtual void OnClose();

	virtual bool OnEvent(SEvent event);

	virtual void Update();

	void UpdateTerminalMessage(std::string message);

	void CloseView();

	void SetCommandVisibility(int index, bool bVisible);

protected:
	virtual bool OnMenuBackPressed();

private:
	void PressButton(int index);
	void UpdateCommandButton(int index, CGUIButton *control);

	D3DCOLOR m_modColor;
	CFont *m_fontDejaVuSansMono12;

	bool m_bClosing;
	bool m_bMenuBackDown;

	float m_fadeDuration;

	static CTerminal *m_terminal;
};

#endif // #ifndef __CINVENTORYMENU_H__
