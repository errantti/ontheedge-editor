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

#include "DialogueWindow.h"
#include "..\\OnTheEdge.h"
#include "Animators\\VerticalScaleAnimator.h"
#include <sstream>

CDialogue *CDialogueWindow::m_dialogue = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialogueWindow::CDialogueWindow(IMenuManager *mgr)
: IMenuHandler(MENU_ID_DIALOGUE, mgr), m_bClosing(false), m_fadeDuration(0.7f)
{
	m_bRequiresUpdate = TRUE;
	CGUIEnvironment *e = CGUIEnvironment::GetGUIEnvironment();

	//m_fontDejaVuSansMono12 = CResourceManager::GetManager().GetFont("Arial12", "Arial", 12, false, false);
}

CDialogueWindow::~CDialogueWindow()
{
}

void CDialogueWindow::CloseView()
{
	m_bClosing = true;
	if(m_window == NULL)
		return;

	m_window->AttachAnimationSequence(new CVerticalScaleAnimator(m_window, false, m_fadeDuration, true));
}

bool CDialogueWindow::OnEvent(SEvent event)
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
				//m_manager->CloseMenu();
				m_manager->ReportMenuClosed(this);
				SendCloseMessage();
			}
			return true;

		case EWGUI_ELEMENT_PRESSED:
			switch(e->GetID())
			{
			case DW_SKIP:
				//GetOTE()->UnpauseGame();
				//m_manager->CloseMenu();
				if(m_dialogue != NULL)
					m_dialogue->CloseDialogue();
				return true;

			case DW_NEXT:
				if(m_dialogue != NULL)
					m_dialogue->NextComment();
				return true;
			}
			break;
		}
		break;
	}

	return false;
}

bool CDialogueWindow::InitializeMenu()
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();
	CGUIWindow *win = m_window;
	if(win == NULL || m_dialogue == NULL)
		return false;

	//CResourceManager &r = CResourceManager::GetManager();
	//r.SetTextureColorkey(D3DCOLOR_RGBA(0,0,0,255));

	GetOTE()->GetMenuDatabase()->ApplyMenuData(win, MENU_ID_DIALOGUE, this);

	//r.RestoreDefaultColorkey();
	m_dialogue->SetDialogueWindowHandler(this);
	win->AttachAnimationSequence(new CVerticalScaleAnimator(win, true, m_fadeDuration, true));

	m_bClosing = false;

	m_dialogue->NextComment();

	CGameSettings &s = *CGameSettings::GetSettingsObject();
	IGUIElement *e = m_window->GetChild(DW_SKIP);
	if(e)
	{
		std::string str("  Skip (");
		str += s.GetControlSymbolOrCode(s.GetGeneralKeyboardControlSet().menuBack);
		str += ')';
		e->SetCaption(str);
	}
	e = m_window->GetChild(DW_NEXT);
	if(e)
	{
		std::string str("  Next (");
		str += s.GetControlSymbolOrCode(s.GetGeneralKeyboardControlSet().generalOk);
		str += ')';
		e->SetCaption(str);
	}

	/*************
	 * Initialize dialogue stuff
	 *************/

	// Pause game and display first comment
	GetOTE()->PauseGame();

	m_environment->SetCursorVisibility(TRUE);

	return true;
}

void CDialogueWindow::OnClose()
{
	m_environment->ForceFocusElement(NULL);
	GetOTE()->GetGUIEnvironment()->SetCursorVisibility(FALSE);
	GetOTE()->UnpauseGame();
	if(m_dialogue)
		m_dialogue->ReportDialogueWindowClosed();
	m_dialogue = NULL;
}

void CDialogueWindow::Update()
{
	static bool bSpaceDown = false, b0Down = false;

	CGameSettings *s = CGameSettings::GetSettingsObject();

	if(!bSpaceDown && s->IsGeneralOkDown())
	{
		bSpaceDown = true;
		if(m_dialogue != NULL)
			m_dialogue->NextComment();
	}
	if(!s->IsGeneralOkDown())
		bSpaceDown = false;

	if(!b0Down && s->IsMenuBackDown())
	{
		b0Down = true;
		if(m_dialogue != NULL)
			m_dialogue->CloseDialogue();
	}
	if(!s->IsMenuBackDown())
		b0Down = false;
}

void CDialogueWindow::UpdateDialogueComment(std::string message, std::string speaker)
{
	CGUILabel *lbl = (CGUILabel*)m_window->GetChild(DW_TEXT);
	if(lbl != NULL)
		lbl->SetCaption(message.c_str());

	lbl = (CGUILabel*)m_window->GetChild(DW_SPEAKER);
	if(lbl != NULL)
		lbl->SetCaption(speaker.c_str());
}