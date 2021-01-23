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

#include "IMenuHandler.h"
#include "..\\OnTheEdge.h"
#include "MenuStackManager.h"

IMenuHandler::IMenuHandler(MENU_ID id, IMenuManager *mgr) : m_manager(mgr), m_ID(id), 
  m_bEscDown(TRUE), m_bEnterDown(FALSE), m_bDownDown(FALSE), m_bUpDown(FALSE),
  m_openSound(NULL), m_music(NULL), m_actionSound(NULL), m_hoverSound(NULL),
  m_window(NULL)
{
	m_environment = CGUIEnvironment::GetGUIEnvironment();
}

IMenuHandler::~IMenuHandler()
{
}

void IMenuHandler::HandleRoutineInput(SEvent event)
{
	IGUIElement *e = (IGUIElement*)event.guiEvent.element;
	switch(event.type)
	{
	case EW_GUI_EVENT:
		switch(event.guiEvent.type)
		{
		case EWGUI_ELEMENT_HOVERED:
			if(e->IsTabStop())
				PlayHoverSound();
			break;

		case EWGUI_ELEMENT_PRESSED:
			PlayActivationSound();
			break;
		}
		break;
	}
}

void IMenuHandler::SendCloseMessage()
{
	if(m_window == 0)
		return;
	SEvent e;
	e.type = EW_GUI_EVENT;
	e.guiEvent.type = EWGUI_ELEMENT_DESTROYED;
	m_window->OnEvent(e);
}

bool IMenuHandler::UpdateSelection(bool bConfirmWindow)
{
	if(bConfirmWindow)
		if(CMenuStackManager::GetManager().GetTopmostPointer() != m_window)
			return false;

	CGameSettings *s = CGameSettings::GetSettingsObject();

	if(s->IsMenuBackDown() && !m_bEscDown)
	{
		m_bEscDown = TRUE;
		if(OnMenuBackPressed())
			return true;
	}
	if(s->IsNavigateUpDown() && !m_bUpDown)
	{
		PlayHoverSound();
		m_window->TabSelect(false);
		m_bUpDown = true;
	}
	if(s->IsNavigateDownDown() && !m_bDownDown)
	{
		PlayHoverSound();
		m_window->TabSelect(true);
		m_bDownDown = true;
	}
	if(!s->IsMenuBackDown())
		m_bEscDown = FALSE;
	if(!s->IsNavigateUpDown())
		m_bUpDown = FALSE;
	if(!s->IsNavigateDownDown())
		m_bDownDown = FALSE;
	if(!s->IsGeneralOkDown())
		m_bEnterDown = FALSE;

	if(s->IsGeneralOkDown() && !m_bEnterDown)
	{
		m_bEnterDown = TRUE;
		IGUIElement *e = ((CGUIEnvironment*)m_environment)->GetFocusElement();
		if(e)
		{
			if(e->GetEnabled() && e->GetParent() == m_window)
			{
				PlayActivationSound();
				SEvent ev;
				ev.type = EW_GUI_EVENT;
				ev.guiEvent.type = EWGUI_ELEMENT_PRESSED;
				ev.guiEvent.element = e;
				e->OnEvent(ev);
				return false;
			}
		}
	}

	return false;
}

bool IMenuHandler::OnMenuBackPressed()
{
	return false;
}

void IMenuHandler::PlayMusic()
{
	GetOTE()->GetSoundManager()->PlayMusic(m_music, true, true);
}

void IMenuHandler::PlayOpenSound()
{
	if(m_openSound != NULL)
		m_openSound->Play(0, SOUND_TYPE_INTERFACE);
}

void IMenuHandler::PlayActivationSound()
{
	if(m_actionSound != NULL)
		m_actionSound->Play(0, SOUND_TYPE_INTERFACE);
}

void IMenuHandler::PlayHoverSound()
{
	if(m_hoverSound != NULL)
		m_hoverSound->Play(0, SOUND_TYPE_INTERFACE);
}

void IMenuHandler::SetOpenSound(const char *filename)
{
	if(filename != NULL)
		m_openSound = CSoundManager::GetManager().GetSound(filename, false, SOUND_TYPE_INTERFACE);
}

void IMenuHandler::SetMusic(const char *filename)
{
	if(filename != NULL)
		m_music = CSoundManager::GetManager().GetSound(filename, true, SOUND_TYPE_MUSIC);
}

void IMenuHandler::SetActionSound(const char *filename)
{
	if(filename != NULL)
		m_actionSound = CSoundManager::GetManager().GetSound(filename, false, SOUND_TYPE_INTERFACE);
}

void IMenuHandler::SetHoverSound(const char *filename)
{
	if(filename != NULL)
		m_hoverSound = CSoundManager::GetManager().GetSound(filename, false, SOUND_TYPE_INTERFACE);
}