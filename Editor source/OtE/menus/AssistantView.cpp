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

#include "AssistantView.h"
#include "..\\OnTheEdge.h"
#include "Animators\\VerticalScaleAnimator.h"
#include "Animators\\DropdownAnimator.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAssistantView::CAssistantView(IMenuManager *mgr)
: IMenuHandler(MENU_ID_ASSISTANT_VIEW, mgr), m_bClosing(false), m_fadeDuration(0.5f)
{
	m_bRequiresUpdate = TRUE;
	CGUIEnvironment *e = CGUIEnvironment::GetGUIEnvironment();

	//m_fontDejaVuSansMono12 = CResourceManager::GetManager().GetFont("Arial12", "Arial", 12, false, false);
}

CAssistantView::~CAssistantView()
{
}

void CAssistantView::CloseView()
{
	m_bClosing = true;
	if(m_window == NULL)
		return;

	//m_window->AttachAnimationSequence(new CVerticalScaleAnimator(m_window, false, m_fadeDuration, true));
	m_window->AttachAnimationSequence(new CDropdownAnimator(m_window, false, m_fadeDuration, true));
}

bool CAssistantView::OnEvent(SEvent event)
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
				SendCloseMessage();
			}
			return true;

		case EWGUI_ELEMENT_PRESSED:
			CAssistantMessages::GetIntance()->Close();
			return true;
		}
		break;
	}

	return false;
}

bool CAssistantView::InitializeMenu()
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();
	CGUIWindow *win = m_window;
	if(win == NULL)
		return false;

	GetOTE()->GetMenuDatabase()->ApplyMenuData(win, MENU_ID_ASSISTANT_VIEW, this);

	//win->AttachAnimationSequence(new CVerticalScaleAnimator(win, true, m_fadeDuration, true));
	win->AttachAnimationSequence(new CDropdownAnimator(win, true, m_fadeDuration, true));

	CAssistantMessages::GetIntance()->SetAssistantViewObject(this);
	UpdateText(CAssistantMessages::GetIntance()->GetCurrentMessage(), "");

	m_bClosing = false;

	return true;
}

void CAssistantView::OnClose()
{
	CAssistantMessages::GetIntance()->ReportViewClosed();
}

void CAssistantView::Update()
{
	static bool bSpaceDown = false, b0Down = false;

	CGameSettings *s = CGameSettings::GetSettingsObject();

	bool pressed = false;
	if(!bSpaceDown && s->IsGeneralOkDown())
	{
		bSpaceDown = true;
		pressed = true;
	}
	if(!s->IsGeneralOkDown())
		bSpaceDown = false;

	if(m_window->GetVisible() && pressed)
		CAssistantMessages::GetIntance()->Close();
}

void CAssistantView::UpdateText(std::string *message, std::string header)
{
	CGUILabel *lbl;
	if(message)
	{
		lbl = (CGUILabel*)m_window->GetChild(AW_TEXT);
		if(lbl != NULL)
			lbl->SetCaption(message->c_str());
	}

	lbl = (CGUILabel*)m_window->GetChild(AW_HEADER);
	if(lbl != NULL)
		lbl->SetCaption(header.c_str());
}

void CAssistantView::Hide()
{
	m_window->SetVisible(false);
}

void CAssistantView::Reveal()
{
	m_window->SetVisible(true);
}
