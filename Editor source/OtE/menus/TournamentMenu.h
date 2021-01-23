// Tournament.h: interface for the CTournament class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __TOURNAMENT_H__
#define __TOURNAMENT_H__

#include "IMenu.h"
#include <string>
#include <sstream>


enum TOURNAMENT_MENU_ELEMENTS {
	TOU_BG = 10,
	TOU_CLOSE,
	TOU_PLAY,
	TOU_QM_SHIP_PL1,
	TOU_SHIP_PL1,
	TOU_QM_SHIP_PL2,
	TOU_SHIP_PL2,
	TOU_GADGET1_PL1,
	TOU_GADGET2_PL1,
	TOU_GADGET1_PL2,
	TOU_GADGET2_PL2,
	TOU_LA_SHIP_PL1,
	TOU_RA_SHIP_PL1,
	TOU_LA_SHIP_PL2,
	TOU_RA_SHIP_PL2,
	TOU_LA_GADGET1_PL1,
	TOU_RA_GADGET1_PL1,
	TOU_LA_GADGET2_PL1,
	TOU_RA_GADGET2_PL1,
	TOU_LA_GADGET1_PL2,
	TOU_RA_GADGET1_PL2,
	TOU_LA_GADGET2_PL2,
	TOU_RA_GADGET2_PL2,
	TOU_RAND_MAPS,
	TOU_ENABLE_PODS,
	TOU_ENABLE_AI_ENEMIES,
	TOU_NUMBER_TO_WIN,
	TOU_NUMBER_MAP_ROUNDS,
	TOU_COUNT
};
class CTournamentMenu : public IMenuHandler   
{
public:
	bool OnEvent(struct SEvent);
	void OnClose();
	bool InitializeMenu();
	CTournamentMenu(IMenuManager *mgr);
	virtual ~CTournamentMenu();
private:
	int	ship_player1, ship_player2;
	int	gadget1_player1, gadget2_player1;
	int	gadget1_player2, gadget2_player2;
	int number_to_win, number_map_rounds;
	int last_gadget;
	std::string strFilename_ship_player[3];
	bool random_map_cycle, enable_pods, enable_ai_enemies;
	std::stringstream strnumber_to_win;
	std::string strFilename_gadget[13];
	char buffer [33];

};

#endif // ifndef __TOURNAMENT_H__
