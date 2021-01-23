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

#include "TerminalView.h"
#include "Animators\\FadeMenuAnimator.h"
#include "..\\OnTheEdge.h"
#include <sstream>

CTerminal *CTerminalView::m_terminal = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTerminalView::CTerminalView(IMenuManager *mgr)
: IMenuHandler(MENU_ID_TERMINAL, mgr), m_fadeDuration(0.7f), m_bMenuBackDown(true)
{
	m_bRequiresUpdate = TRUE;
	CGUIEnvironment *e = CGUIEnvironment::GetGUIEnvironment();

	//m_fontDejaVuSansMono12 = CResourceManager::GetManager().GetFont("Arial12", "Arial", 12, false, false);
}

CTerminalView::~CTerminalView()
{
}

void CTerminalView::CloseView()
{
	m_bClosing = true;
	if(m_window == NULL)
		return;

	m_window->AttachAnimationSequence(new CFadeMenuAnimator(m_window, false, m_fadeDuration, true));
}

bool CTerminalView::OnEvent(SEvent event)
{
	IGUIElement *e = (IGUIElement*)event.guiEvent.element;
	switch(event.type)
	{
	case EW_GUI_EVENT:
		switch(event.guiEvent.type)
		{
		case EWGUI_ELEMENT_CREATED:
			m_window = (CGUIWindow*)event.guiEvent.element;
			InitializeMenu();
			return true;
		case EWGUI_ELEMENT_DESTROYED:
			OnClose();
			return true;

		case EWGUI_WINDOW_ANIMATION_SEQUENCE_FINISHED:
			if(m_bClosing)
			{
				m_manager->ReportMenuClosed(this);
				SendCloseMessage();
			}
			else
				m_window->TabSelect(true);
			return true;

		case EWGUI_ELEMENT_HOVERED:
			if(e->IsTabStop() && e->GetVisible())
				PlayHoverSound();
			break;

		case EWGUI_ELEMENT_PRESSED:
			if(e->GetVisible())
				PlayActivationSound();
			switch(e->GetID())
			{
			case TV_CLOSE:
				//GetOTE()->UnpauseGame();
				//m_manager->CloseMenu();
				if(m_terminal != NULL)
					m_terminal->CloseTerminal();
				return TRUE;

			case TV_BUTTON1:
				PressButton(0);
				return TRUE;
			case TV_BUTTON2:
				PressButton(1);
				return TRUE;
			case TV_BUTTON3:
				PressButton(2);
				return TRUE;
			case TV_BUTTON4:
				PressButton(3);
				return TRUE;
			case TV_BUTTON5:
				PressButton(4);
				return TRUE;
			case TV_BUTTON6:
				PressButton(5);
				return TRUE;
			}
			break;
		}
		break;
	}

	return false;
}

bool CTerminalView::InitializeMenu()
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();
	CGUIWindow *win = m_window;
	if(win == NULL || m_terminal == NULL)
		return false;

	//CResourceManager &r = CResourceManager::GetManager();
	//r.SetTextureColorkey(D3DCOLOR_RGBA(0,0,0,255));

	GetOTE()->GetMenuDatabase()->ApplyMenuData(win, MENU_ID_TERMINAL, this);

	//r.RestoreDefaultColorkey();
	m_terminal->SetTerminalWindowHandler(this);

	win->AttachAnimationSequence(new CFadeMenuAnimator(win, true, m_fadeDuration, true));

	/*************
	 * Initialize terminal stuff
	 *************/

	// Set button names and visibilities
	GetOTE()->PauseGame();
	m_bClosing = false;

	UpdateCommandButton(0, (CGUIButton*)win->GetChild(TV_BUTTON1));
	UpdateCommandButton(1, (CGUIButton*)win->GetChild(TV_BUTTON2));
	UpdateCommandButton(2, (CGUIButton*)win->GetChild(TV_BUTTON3));
	UpdateCommandButton(3, (CGUIButton*)win->GetChild(TV_BUTTON4));
	UpdateCommandButton(4, (CGUIButton*)win->GetChild(TV_BUTTON5));
	UpdateCommandButton(5, (CGUIButton*)win->GetChild(TV_BUTTON6));

	CGUIButton *btn = (CGUIButton*)win->GetChild(TV_CLOSE);
	if(btn != NULL)
		btn->SetCaption("   Close");

	CGUILabel *lbl = (CGUILabel*)win->GetChild(TV_MESSAGE);
	if(lbl != NULL)
		lbl->SetCaption(m_terminal->GetWelcomeMessage().c_str());

	m_environment->SetCursorVisibility(TRUE);

	PlayMusic();
	m_bMenuBackDown = true;

	return true;
}

void CTerminalView::UpdateCommandButton(int index, CGUIButton *control)
{
	if(control == NULL)
		return;
	if(index < 0 || index > 5)
		return;

	char buffer[256];

	if(m_terminal->IsCommandVisible(index))
	{
		sprintf(buffer, "   %s", m_terminal->GetCommandName(index).c_str());
		control->SetCaption(buffer);
		control->SetEnabled(m_terminal->IsCommandEnabled(index));
	}
	else
	{
		if(m_environment->GetFocusElement() == control)
		{
			// If the element in question is focused (=selected), select another
			// item in the view
			m_window->TabSelect(true);
		}
		//if(control->Is
		control->SetVisible(false);
	}
}

void CTerminalView::SetCommandVisibility(int index, bool bVisible)
{
	if(index < 0 || index > 5)
		return;

	UpdateCommandButton(index, (CGUIButton*)m_window->GetChild(TV_BUTTON1 + index));
}

void CTerminalView::OnClose()
{
	m_environment->ForceFocusElement(NULL);
	GetOTE()->GetGUIEnvironment()->SetCursorVisibility(FALSE);
	GetOTE()->UnpauseGame();
	if(m_terminal)
		m_terminal->ReportTerminalViewClosed();
	m_terminal = NULL;
}

void CTerminalView::Update()
{
	CGameSettings *s = CGameSettings::GetSettingsObject();
	if(s->IsMenuBackDown() && !m_bMenuBackDown)
	{
		m_bMenuBackDown = true;
		if(m_terminal != NULL)
			m_terminal->CloseTerminal();
		return;
	}
	if(!s->IsMenuBackDown())
		m_bMenuBackDown = false;

	UpdateSelection(false);
	/*CGameSettings *s = CGameSettings::GetSettingsObject();

	if(s->IsNavigateUpDown() && !m_bUpArrowDown)
	{
		m_window->TabSelect(false);
		m_bUpArrowDown = true;
	}
	if(s->IsNavigateDownDown() && !m_bDownArrowDown)
	{
		m_window->TabSelect(true);
		m_bDownArrowDown = true;
	}
	if(!s->IsNavigateUpDown())
		m_bUpArrowDown = FALSE;
	if(!s->IsNavigateDownDown())
		m_bDownArrowDown = FALSE;
	if(!s->IsGeneralOkDown())
		m_bEnterDown = false;

	if(s->IsGeneralOkDown() && !m_bEnterDown)
	{
		IGUIElement *e = ((CGUIEnvironment*)m_environment)->GetFocusElement();
		m_bEnterDown = true;
		if(e)
		{
			SEvent ev;
			ev.type = EW_GUI_EVENT;
			ev.guiEvent.type = EWGUI_ELEMENT_PRESSED;
			ev.guiEvent.element = e;
			e->OnEvent(ev);
			return;
		}
	}*/
}

bool CTerminalView::OnMenuBackPressed()
{
	return false;
}

void CTerminalView::PressButton(int index)
{
	if(m_terminal == NULL)
		return;

	m_terminal->ExecuteCommand(index);
}

void CTerminalView::UpdateTerminalMessage(std::string message)
{
	CGUILabel *lbl = (CGUILabel*)m_window->GetChild(TV_MESSAGE);
	if(lbl != NULL)
		lbl->SetCaption(message.c_str());
}