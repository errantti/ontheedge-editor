/*********************************
 *
 *		On the Edge
 * Copyright (2005) Azuris Productions
 * Jukka Tykkyläinen & Ismo Huhtiniemi
 * http://koti.mbnet.fi/jtykky
 *
 * SinglePlayerMenu - handles single player (ship selection)
 * menu functionalities and inizialiting
 *
 *********************************/

#ifndef __CSINGLEPLAYERMENU_H__
#define __CSINGLEPLAYERMENU_H__

#include "IMenu.h"

enum SINGLEPLAYER_MENU_ELEMENTS {
	SING_BG = 500,
	SING_CLOSE,
	SING_PLAY,
	SING_SHIP,
	SING_LEFT,
	SING_RIGHT,
	SING_DETAILS,
	SING_COUNT
};
class CSinglePlayerMenu : public IMenu  
{
public:
	CSinglePlayerMenu(IMenuManager *mgr);
	~CSinglePlayerMenu();

	bool RemoveMenuElements();
	bool InitializeMenu(CGUIEnvironment *env);
	bool OnEvent(SEvent event);

	inline void SetShipDetails(int shipIndex, string details) {m_shipDetails[shipIndex] = details;}
	inline void SetShipIcon(int shipIndex, string filename) {m_shipIcon[shipIndex] = filename;}
	inline void SetShipID(int shipIndex, long ID) {m_shipIDs[shipIndex] = ID;}

	virtual void Update();

private:
	void UpdateShipIcon();

	int m_selectedShip;
	string m_shipIcon[4];
	string m_shipDetails[4];
	long m_shipIDs[4];
	CTexture *m_ships[4];

	bool m_bEscDown;
};

#endif // #ifndef __CSINGLEPLAYERMENU_H__
