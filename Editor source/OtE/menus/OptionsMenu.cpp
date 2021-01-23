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

#include "OptionsMenu.h"
#include "..\\OnTheEdge.h"
#include "MenuStackManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COptionsMenu::COptionsMenu(IMenuManager *mgr)
: CFlowMenu(MENU_ID_OPTIONS , mgr, MENU_ID_MAIN), m_currentCampaignIndex(-1)
{
}

COptionsMenu::~COptionsMenu()
{
}

void COptionsMenu::CustomInit()
{
	/*char buffer[256];
	IGUIElement *e = m_window->GetChild(OPT_CAMPAIGN);
	char *campaignName = GetOTE()->GetCampaign()->GetName();

	sprintf(buffer, "Campaign: %s", campaignName);
	if(e != NULL)
	{
		e->SetCaption(buffer);

		// Restriction: campaign can only be changed when level isn't loaded
		if(!GetOTE()->IsGameRunning())
		{
			CCampaign::EnumerateCampaigns(m_campaigns, "campaigns\\");
			std::vector<std::pair<std::string,std::string> >::iterator it = m_campaigns.begin();
			int index = 0;
			m_currentCampaignIndex = -1;
			std::string filename = GetOTE()->GetCampaign()->GetCampaignFilename();
			for(; it != m_campaigns.end(); ++it)
			{
				if(it->first.compare(filename) == 0)
				{
					m_currentCampaignIndex = index;
					break;
				}
				++index;
			}
			if(m_currentCampaignIndex == -1 && !m_campaigns.empty())
				m_currentCampaignIndex = 0;
			e->SetEnabled(true);
		}
		else
		{
			e->SetEnabled(false);
			m_currentCampaignIndex = -1;
			m_campaigns.clear();
		}

		// This option is disabled for now because campaign selection is done through
		// Start campaign -> campaign selection
		e->SetEnabled(false);
	}*/

	IGUIElement *e;
	CGameSettings &s = *GetOTE()->GetSettings();
	if(s.GetSetting(GO_SOUND_VOLUME) < 0.01f)
	{
		e = m_window->GetChild(OPT_SOUND);
		if(e)
			e->SetCaption("Sounds: Off");
	}
	if(s.GetSetting(GO_MUSIC_VOLUME) < 0.01f)
	{
		e = m_window->GetChild(OPT_MUSIC);
		if(e)
			e->SetCaption("Music: Off");
	}

	e = m_window->GetChild(OPT_ENEMY_EMOTES);
	if(e != NULL)
	{
		if(s.GetSetting(GO_ANIME_SMILEYS_ENABLED))
			e->SetCaption("Enemy emotes: On");
		else
			e->SetCaption("Enemy emotes: Off");
	}
}

bool COptionsMenu::OnEvent(SEvent event)
{
	if(CFlowMenu::OnEvent(event))
		return true;

	IGUIElement *e = (IGUIElement*)event.guiEvent.element;
	switch(event.type)
	{
	case EW_GUI_EVENT:
		switch(event.guiEvent.type)
		{
		case EWGUI_ELEMENT_PRESSED:
			switch(e->GetID())
			{
			case OPT_CLOSE:
				CloseFlowMenu();
				//OnClose();
				return true;
			case OPT_CONTROLS:
				m_manager->OpenMenu(MENU_ID_CONTROLS, false);
				return true;
			/*case OPT_CAMPAIGN:
				SelectNextCampaign();
				return true;*/
			case OPT_ENEMY_EMOTES:
				SetEnemyEmotes();
				return true;
			case OPT_SOUND:
				SetSound();
				return true;
			case OPT_MUSIC:
				SetMusic();
				return true;
			case OPT_TUTORIALS:
				return true;
			}
			break;
		}
	}

	return false;
}

void COptionsMenu::OnClose()
{
	if(!GetOTE()->IsGameRunning() && m_currentCampaignIndex >= 0)
	{
		// Load the selected campaign (if it's already loaded, nothing will happen)
		GetOTE()->LoadCampaign(m_campaigns[m_currentCampaignIndex].first);

		// Make this the default campaign to be loaded next time the game is ran
		CGameSettings::GetSettingsObject()->SetDefaultCampaign(m_campaigns[m_currentCampaignIndex].first);
	}

	CGameSettings::GetSettingsObject()->WriteSettingsToIni("settings.ini");
}

void COptionsMenu::SelectNextCampaign()
{
	// If there are no campaign info there's nothing we can do
	if(m_currentCampaignIndex == -1 || m_campaigns.empty())
		return;

	++m_currentCampaignIndex;
	if(m_currentCampaignIndex >= (int)m_campaigns.size())
		m_currentCampaignIndex = 0;

	IGUIElement *e = m_window->GetChild(OPT_CAMPAIGN);
	std::string str("Campaign: ");
	str += m_campaigns[m_currentCampaignIndex].second;
	if(e != NULL)
	{
		e->SetCaption(str);
	}
}

void COptionsMenu::SetSound()
{
	CGameSettings &s = *GetOTE()->GetSettings();
	std::string str("Sounds: On");
	if(s.GetSetting(GO_SOUND_VOLUME) < 0.01f)
	{
		s.SetSetting(GO_SOUND_VOLUME, 1.f);
	}
	else
	{
		s.SetSetting(GO_SOUND_VOLUME, 0.f);
		str = "Sounds: Off";
	}

	IGUIElement *e = m_window->GetChild(OPT_SOUND);
	if(e)
		e->SetCaption(str);
}

void COptionsMenu::SetMusic()
{
	CGameSettings &s = *GetOTE()->GetSettings();
	std::string str("Music: On");
	if(s.GetSetting(GO_MUSIC_VOLUME) < 0.01f)
	{
		s.SetSetting(GO_MUSIC_VOLUME, 0.5f);
	}
	else
	{
		s.SetSetting(GO_MUSIC_VOLUME, 0.f);
		str = "Music: Off";
	}

	IGUIElement *e = m_window->GetChild(OPT_MUSIC);
	if(e)
		e->SetCaption(str);
}

void COptionsMenu::SetEnemyEmotes()
{
	CGameSettings &s = *GetOTE()->GetSettings();
	std::string str("Enemy emotes: On");
	if(s.GetSetting(GO_ANIME_SMILEYS_ENABLED))
	{
		s.SetSetting(GO_ANIME_SMILEYS_ENABLED, false);
		str = "Enemy emotes: Off";
	}
	else
		s.SetSetting(GO_ANIME_SMILEYS_ENABLED, true);

	GetOTE()->AddScreenMessage("Option will be applied at level start", 6.f);

	IGUIElement *e = m_window->GetChild(OPT_ENEMY_EMOTES);
	if(e)
		e->SetCaption(str);
}