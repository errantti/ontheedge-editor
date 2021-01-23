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

#ifndef __IMENU_H__
#define __IMENU_H__

#include "IMenuManager.h"

class IMenuHandler : public IEventReceiver
{
public:
	IMenuHandler(MENU_ID id, IMenuManager *mgr) : m_manager(mgr), m_ID(id)
	{
		m_environment = CGUIEnvironment::GetGUIEnvironment();
	}

	virtual ~IMenuHandler() {}

	// InitializeMenu
	// Loads the menu controls to the gui environment
	virtual bool InitializeMenu() = 0;
	// removes the menu controls from the gui environment
	virtual void OnClose() = 0;

	// handler for messages for this menu
	virtual bool OnEvent(SEvent event) = 0;

	void SetWindowHandle(CGUIWindow *window) {m_window = window;}

	MENU_ID GetID() {return m_ID;}

	void SendCloseMessage()
	{
		if(m_window == 0)
			return;
		SEvent e;
		e.type = EW_GUI_EVENT;
		e.guiEvent.type = EWGUI_ELEMENT_DESTROYED;
		m_window->OnEvent(e);
	}

protected:
	CGUIEnvironment *m_environment;
	IMenuManager *m_manager;

	CGUIWindow *m_window;

	MENU_ID m_ID;
};


#endif // #ifndef __IMENU_H__