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

#include "CampaignSelectionMenu.h"
#include "..\\OnTheEdge.h"
#include "MenuStackManager.h"
#include "..\\Edgewalker\\IniFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampaignSelectionMenu::CCampaignSelectionMenu(IMenuManager *mgr)
: CFlowMenu(MENU_ID_CAMPAIGN_SELECTION, mgr, MENU_ID_MAIN),	m_firstCampaignIndex(0)
{
}

CCampaignSelectionMenu::~CCampaignSelectionMenu()
{
}

void CCampaignSelectionMenu::CustomInit()
{
	IGUIElement *element;

	element = m_window->GetChild(CS_PREV);
	if(element != NULL)
		element->SetVisible(false);

	CCampaign::EnumerateCampaigns(m_campaigns, "campaigns\\");

	int numCampaigns = (int)m_campaigns.size();
	if(numCampaigns <= 5)
	{
		element = m_window->GetChild(CS_NEXT);
		if(element != NULL)
			element->SetVisible(false);
	}

	m_firstCampaignIndex = 0;

	UpdateCampaignList();
}

bool CCampaignSelectionMenu::OnEvent(SEvent event)
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
			case CS_CLOSE:
				CloseFlowMenu();
				return true;
			case CS_CAMPAIGN1:
				SelectCampaign(0);
				return true;
			case CS_CAMPAIGN2:
				SelectCampaign(1);
				return true;
			case CS_CAMPAIGN3:
				SelectCampaign(2);
				return true;
			case CS_CAMPAIGN4:
				SelectCampaign(3);
				return true;
			case CS_CAMPAIGN5:
				SelectCampaign(4);
				return true;
			case CS_PREV:
				Prev();
				return true;
			case CS_NEXT:
				Next();
				return true;
			}
			break;
		}
		break;
	}

	return false;
}

void CCampaignSelectionMenu::OnClose()
{
}

void CCampaignSelectionMenu::UpdateCampaignList()
{
	int numCampaigns = (int)m_campaigns.size();

	IGUIElement *element;

	int i=0;
	long id = CS_CAMPAIGN1;
	for(; i < 5 && i + m_firstCampaignIndex < numCampaigns; ++i, ++id)
	{
		element = m_window->GetChild(id);
		if(element != NULL)
		{
			element->SetVisible(true);
			element->SetCaption(m_campaigns[i + m_firstCampaignIndex].second);
		}
	}
	// Set the remaining autosave elements invisible
	for(; i < 5; ++i, ++id)
	{
		element = m_window->GetChild(id);
		if(element != NULL)
			element->SetVisible(false);
	}
}

void CCampaignSelectionMenu::SelectCampaign(int slot)
{
	if(GetOTE()->LoadCampaign(m_campaigns[m_firstCampaignIndex + slot].first))
	{
		// Make this campaign the new default campaign so when the player starts
		// the game next time he can load the game from where he left off without
		// having to select the campaign again
		// This involves parsing the settings.ini, changing the default campaign
		// filename and saving the change
		CIniFile f;
		if(f.ReadFile("settings.ini"))
		{
			f.WriteStringValue("General", "defaultCampaign", m_campaigns[m_firstCampaignIndex + slot].first);
			if(!f.WriteFile("settings.ini"))
				CPrinter::logError("Failed setting default campaign: failed writing settings.ini");
		}
		else
			CPrinter::logError("Failed setting default campaign: failed reading settings.ini");/**/

		// Aaand let's roll!
		CloseAllMenus();
		GetOTE()->StartCampaign(1);
	}
}

void CCampaignSelectionMenu::Prev()
{
	IGUIElement *element;

	if(m_firstCampaignIndex > 0)
	{
		m_firstCampaignIndex -= 5;
		if(m_firstCampaignIndex < 1)
		{
			m_firstCampaignIndex = 0;

			element = m_window->GetChild(CS_PREV);
			if(element != NULL)
				element->SetVisible(false);
		}

		element = m_window->GetChild(CS_NEXT);
		if(element != NULL)
			element->SetVisible(true);
	}

	UpdateCampaignList();
}

void CCampaignSelectionMenu::Next()
{
	IGUIElement *element;

	int numCampaigns = (int)m_campaigns.size();
	if(numCampaigns > m_firstCampaignIndex + 5)
	{
		m_firstCampaignIndex += 5;

		element = m_window->GetChild(CS_PREV);
		if(element != NULL)
			element->SetVisible(true);

		UpdateCampaignList();
	}

	if(numCampaigns <= m_firstCampaignIndex + 5)
	{
		IGUIElement *element = m_window->GetChild(CS_NEXT);
		if(element != NULL)
			element->SetVisible(false);
	}
}