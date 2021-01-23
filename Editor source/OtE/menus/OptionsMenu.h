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

#ifndef __OPTIONSMENU_H__
#define __OPTIONSMENU_H__

#include "FlowMenu.h"

enum OPTIONS_MENU_ELEMENTS{
	OPT_CLOSE=100,
	OPT_MUSIC,
	OPT_SOUND,
	OPT_CONTROLS,
	OPT_CAMPAIGN,
	OPT_TUTORIALS,
	OPT_ENEMY_EMOTES
};

/**
 * Handles options menu window
 */
class COptionsMenu : public CFlowMenu  
{
public:
	COptionsMenu(IMenuManager *mgr);
	virtual ~COptionsMenu();

	void CustomInit();

	void OnClose();
	bool OnEvent(SEvent event);

private:
	void SelectNextCampaign();
	void SetSound();
	void SetMusic();
	void SetEnemyEmotes();

	std::vector<std::pair<std::string,std::string> > m_campaigns;
	int m_currentCampaignIndex;
};

#endif // ifndef __OPTIONSMENU_H__
