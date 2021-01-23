/*********************************
 *
 *		On the Edge
 * Copyright (2004) Azuris Productions
 * Jukka Tykkyläinen & Ismo Huhtiniemi
 * http://koti.mbnet.fi/jtykky/stuff/azuris
 *
 *********************************/

#ifndef __MULTIPLAYERSELECTIONMENU_H__
#define __MULTIPLAYERSELECTIONMENU_H__



#include "IMenu.h"

enum MULTIPLAYER_SELECTION_MENU_ELEMENTS{
	MPS_BG = 10,
	MPS_QUICK_GAME,
	MPS_TOURNAMENT,
	MPS_CLOSE,
	MPS_COUNT
};

class CMultiPlayerSelectionMenu : public IMenuHandler  
{
public:
	void OnClose();
	bool InitializeMenu();
	bool OnEvent(SEvent event);
	CMultiPlayerSelectionMenu(IMenuManager *mgr);
	virtual ~CMultiPlayerSelectionMenu();

};

#endif // ifndef __MULTIPLAYERSELECTIONMENU_H__