// Tournament.cpp: implementation of the CTournament class.
//
//////////////////////////////////////////////////////////////////////

#include "TournamentMenu.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTournamentMenu::CTournamentMenu(IMenuManager *mgr)
: IMenuHandler(MENU_TOURNAMENT , mgr)
{
	ship_player1 = 0;
	ship_player2 = 1;
	number_to_win = 2;
	number_map_rounds = 5;
	enable_ai_enemies = false;
	enable_pods = false;
	random_map_cycle = false;
	gadget1_player1 = 3;
	gadget2_player2 = 0;
	gadget1_player2 = 2;
	gadget2_player1 = 1;
	last_gadget = 12;
	strFilename_ship_player[0] = "imgs\\tournament\\ship1_tournament.bmp";
	strFilename_ship_player[1] = "imgs\\tournament\\ship2_tournament.bmp";
	strFilename_ship_player[2] = "imgs\\tournament\\ship3_tournament.bmp";
	
	strFilename_gadget[0] = "imgs\\tournament\\gadget\\FrontShield.bmp";
	strFilename_gadget[1] = "imgs\\tournament\\gadget\\BackShield.bmp";
	strFilename_gadget[2] = "imgs\\tournament\\gadget\\HeavyGenerator.bmp";
	strFilename_gadget[3] = "imgs\\tournament\\gadget\\MediumGenerator.bmp";
	strFilename_gadget[4] = "imgs\\tournament\\gadget\\Electrifier.bmp";
	strFilename_gadget[5] = "imgs\\tournament\\gadget\\Bouncer.bmp";
	strFilename_gadget[6] = "imgs\\tournament\\gadget\\Waver.bmp";
	strFilename_gadget[7] = "imgs\\tournament\\gadget\\HeavyLightener.bmp";
	strFilename_gadget[8] = "imgs\\tournament\\gadget\\TurningEngine.bmp";
	strFilename_gadget[9] = "imgs\\tournament\\gadget\\JumpEngine.bmp";
	strFilename_gadget[10] = "imgs\\tournament\\gadget\\Spikes.bmp";
	strFilename_gadget[11] = "imgs\\tournament\\gadget\\KineFist.bmp";
	strFilename_gadget[12] = "imgs\\tournament\\gadget\\PulseRifle.bmp";
/*	strName_gadget[0] = "Back Shield";
	strName_gadget[1] = "Bouncer";
	strName_gadget[2] = "Back Shield";
	strName_gadget[3] = "Back Shield";
	strName_gadget[4] = "Back Shield";
*/

}

CTournamentMenu::~CTournamentMenu()
{
}

bool CTournamentMenu::InitializeMenu()
{
	/*CPrinter::log("CTournament InitializeMenu");
	m_environment = env;
	if(env == NULL)
		return false;
	CTexture *bg_tournament = env->LoadImage("imgs\\tournament\\bg_tournament.jpg", D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(0,800,0,600),bg_tournament,TOU_BG);
	
	CGUIButton *b_close;
	CTexture *close_tournament_h = env->AddImage("imgs\\tournament\\close_tournament_h.bmp", D3DCOLOR_RGBA(0,0,0,255));
	b_close = env->AddButton(rect<int>(58, 314, 486, 550), "", TOU_CLOSE);
	b_close->SetImageHover(close_tournament_h);
	
	CGUIButton *b_play;
	CTexture *play_tournament_h = env->AddImage("imgs\\tournament\\play_tournament_h.bmp", D3DCOLOR_RGBA(0,0,0,255));
	b_play = env->AddButton(rect<int>(492, 748, 486, 550), "", TOU_PLAY);
	b_play->SetImageHover(play_tournament_h);

	CTexture *ship_pl1_tournament = env->AddImage(strFilename_ship_player[ship_player1], D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(102,166,253,317),ship_pl1_tournament,TOU_SHIP_PL1);
	
	CTexture *ship_pl2_tournament = env->AddImage(strFilename_ship_player[ship_player2], D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(449,513,253,317),ship_pl2_tournament,TOU_SHIP_PL2);


	CTexture *gadget1_tournament_pl1 = env->AddImage(strFilename_gadget[gadget1_player1], D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(206,270,253,317),gadget1_tournament_pl1,TOU_GADGET1_PL1);
	
	CTexture *gadget2_tournament_pl1= env->AddImage(strFilename_gadget[gadget2_player1], D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(277,341,253,317),gadget2_tournament_pl1,TOU_GADGET2_PL1);

	CTexture *gadget1_tournament_pl2 = env->AddImage(strFilename_gadget[gadget1_player2], D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(553,617,253,317),gadget1_tournament_pl2,TOU_GADGET1_PL2);
	
	CTexture *gadget2_tournament_pl2= env->AddImage(strFilename_gadget[gadget2_player2], D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(624,688,253,317),gadget2_tournament_pl2,TOU_GADGET2_PL2);

/*
	CTexture *qm_tournament = new CTexture;
	qm_tournament->LoadTexture("imgs\\tournament\\qm_tournament.bmp", D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(102,166,253,317),qm_tournament,TOU_QM_SHIP_PL1);
*/
	
	/*CGUIButton *b_la_ship_pl1;
	CTexture *leftarrow_tournament_h = env->AddImage("imgs\\tournament\\leftarrow_tournament_h.bmp", D3DCOLOR_RGBA(0,0,0,255));
	b_la_ship_pl1 = env->AddButton(rect<int>(102, 134, 318, 334), "", TOU_LA_SHIP_PL1);
	b_la_ship_pl1->SetImageHover(leftarrow_tournament_h);
	
	CGUIButton *b_ra_ship_pl1;
	CTexture *rightarrow_tournament_h = env->AddImage("imgs\\tournament\\rightarrow_tournament_h.bmp", D3DCOLOR_RGBA(0,0,0,255));
	b_ra_ship_pl1 = env->AddButton(rect<int>(134, 166, 318, 334), "", TOU_RA_SHIP_PL1);
	b_ra_ship_pl1->SetImageHover(rightarrow_tournament_h);
	
	CGUIButton *b_la_gadget1_pl1;
	b_la_gadget1_pl1 = env->AddButton(rect<int>(205, 238, 318, 334), "", TOU_LA_GADGET1_PL1);
	b_la_gadget1_pl1->SetImageHover(leftarrow_tournament_h);
	
	CGUIButton *b_ra_gadget1_pl1;
	b_ra_gadget1_pl1 = env->AddButton(rect<int>(238, 270, 318, 334), "", TOU_RA_GADGET1_PL1);
	b_ra_gadget1_pl1->SetImageHover(rightarrow_tournament_h);

	CGUIButton *b_la_gadget2_pl1;
	b_la_gadget2_pl1 = env->AddButton(rect<int>(278, 310, 318, 334), "", TOU_LA_GADGET2_PL1);
	b_la_gadget2_pl1->SetImageHover(leftarrow_tournament_h);
	
	CGUIButton *b_ra_gadget2_pl1;
	b_ra_gadget2_pl1 = env->AddButton(rect<int>(310, 342, 318, 334), "", TOU_RA_GADGET2_PL1);
	b_ra_gadget2_pl1->SetImageHover(rightarrow_tournament_h);

	int xplus = 347;
		
	CGUIButton *b_la_ship_pl2;
	b_la_ship_pl2 = env->AddButton(rect<int>(102+xplus, 134+xplus, 318, 334), "", TOU_LA_SHIP_PL2);
	b_la_ship_pl2->SetImageHover(leftarrow_tournament_h);
	
	CGUIButton *b_ra_ship_pl2;
	b_ra_ship_pl2 = env->AddButton(rect<int>(134+xplus, 166+xplus, 318, 334), "", TOU_RA_SHIP_PL2);
	b_ra_ship_pl2->SetImageHover(rightarrow_tournament_h);
	
	CGUIButton *b_la_gadget1_pl2;
	b_la_gadget1_pl2 = env->AddButton(rect<int>(205+xplus, 238+xplus, 318, 334), "", TOU_LA_GADGET1_PL2);
	b_la_gadget1_pl2->SetImageHover(leftarrow_tournament_h);
	
	CGUIButton *b_ra_gadget1_pl2;
	b_ra_gadget1_pl2 = env->AddButton(rect<int>(238+xplus, 270+xplus, 318, 334), "", TOU_RA_GADGET1_PL2);
	b_ra_gadget1_pl2->SetImageHover(rightarrow_tournament_h);

	CGUIButton *b_la_gadget2_pl2;
	b_la_gadget2_pl2 = env->AddButton(rect<int>(278+xplus, 310+xplus, 318, 334), "", TOU_LA_GADGET2_PL2);
	b_la_gadget2_pl2->SetImageHover(leftarrow_tournament_h);
	
	CGUIButton *b_ra_gadget2_pl2;
	b_ra_gadget2_pl2 = env->AddButton(rect<int>(310+xplus, 342+xplus, 318, 334), "", TOU_RA_GADGET2_PL2);
	b_ra_gadget2_pl2->SetImageHover(rightarrow_tournament_h);
	

	CGUIButton *b_x_random_map;
	CTexture *x_tournament_n = env->AddImage("imgs\\tournament\\x_tournament_n.bmp", D3DCOLOR_RGBA(0,0,0,255));
	CTexture *x_tournament_h = env->AddImage("imgs\\tournament\\x_tournament_h.bmp", D3DCOLOR_RGBA(0,0,0,255));
	b_x_random_map = env->AddButton(rect<int>(152, 168, 444, 460), "", TOU_RAND_MAPS);
	b_x_random_map->SetImagePressed(x_tournament_n);


	CGUIButton *b_enable_pods;
	b_enable_pods = env->AddButton(rect<int>(344,360, 444, 460), "", TOU_ENABLE_PODS);
	b_enable_pods->SetImage(x_tournament_n);
	//b_enable_pods->SetImageHover(x_tournament_h);

	CGUIButton *b_enable_ai_enemies;
	b_enable_ai_enemies = env->AddButton(rect<int>(506,522, 444, 460), "", TOU_ENABLE_AI_ENEMIES);
	b_enable_ai_enemies->SetImage(x_tournament_n);
	
	CGUIButton *b_number_points_to_win;
	b_number_points_to_win = env->AddButton(rect<int>(153, 169, 408,424 ), itoa(number_to_win,buffer,10), TOU_NUMBER_TO_WIN);
		
	CGUIButton *b_number_map_rounds;
	b_number_map_rounds = env->AddButton(rect<int>(345, 361, 408,424 ), itoa(number_map_rounds,buffer,10), TOU_NUMBER_MAP_ROUNDS);

/*
	CGUILabel *lbl_save0;
	strnumber_to_win << number_to_win;
	lbl_save0 = env->AddLabel(rect<int>(101, 343, 191, 217), (char*)number_to_win, TOU_NUMBER_TO_WIN);
*/
	
//env->AddImage(ship1_pl1_tournament);
	return true;
}

bool CTournamentMenu::OnEvent(SEvent event)
{
	
	/*IGUIElement *e = (IGUIElement*)event.guiEvent.element;
	switch(event.type)
	{
	case EW_GUI_EVENT:
		switch(event.guiEvent.type)
		{
	
		case EWGUI_ELEMENT_PRESSED:
			switch(e->GetID())
			{
			case TOU_CLOSE:
				m_manager->OpenMenu(MENU_ID_MAIN, true);
				return true;
			case TOU_PLAY:
				m_manager->CloseMenu();
				return true;
			case TOU_NUMBER_TO_WIN:
				if (number_to_win < 9){
					number_to_win++;
				}else {
					number_to_win = 1;
				}
				e->SetCaption(itoa(number_to_win,buffer,10));
				return true;
			case TOU_NUMBER_MAP_ROUNDS:
				if (number_map_rounds < 9){
					number_map_rounds++;
				}else {
					number_map_rounds = 3;
				}
				e->SetCaption(itoa(number_map_rounds,buffer,10));
				return true;
			case TOU_RAND_MAPS:
				if(random_map_cycle)
				{
					random_map_cycle = false;
				}else{
					random_map_cycle = true;
				}
				e->Draw();
				return true;
			case TOU_ENABLE_PODS:
				if(enable_pods)
				{
					enable_pods = false;
				}else{
					enable_pods = true;
				}
				e->Draw();
				return true;
			case TOU_ENABLE_AI_ENEMIES:
				if(enable_ai_enemies)
				{
					enable_ai_enemies = false;
				}else{
					enable_ai_enemies = true;
				}
				e->Draw();
				return true;

			// selecting ships
			case TOU_RA_SHIP_PL1:
				switch(ship_player1)
				{
				case 0:
					ship_player1 = 1;
					break;
				case 1:
					ship_player1 = 2;
					break;
				case 2:
					ship_player1 = 0;
					break;
				}
				
				break;
			case TOU_LA_SHIP_PL1:
				switch(ship_player1)
				{
				case 0:
					ship_player1 = 2;
					break;
				case 1:
					ship_player1 = 0;
					break;
				case 2:
					ship_player1 = 1;
					break;
				}
				break;

			case TOU_RA_SHIP_PL2:
				switch(ship_player2)
				{
				case 0:
					ship_player2 = 1;
					break;
				case 1:
					ship_player2 = 2;
					break;
				case 2:
					ship_player2 = 0;
					break;
				}
				break;
			case TOU_LA_SHIP_PL2:
				switch(ship_player2)
				{
				case 0:
					ship_player2 = 2;
					break;
				case 1:
					ship_player2 = 0;
					break;
				case 2:
					ship_player2 = 1;
					break;
				}
				break;

			// selecting gadgets player 1
			case TOU_RA_GADGET1_PL1:
				if(gadget1_player1 == last_gadget)
				{
					gadget1_player1 = 0;
				}else{
					gadget1_player1++;
				}
				if (gadget1_player1 == gadget2_player1)
						gadget1_player1++;
				break;
			case TOU_LA_GADGET1_PL1:
				if(gadget1_player1 == 0)
				{
					gadget1_player1 = last_gadget;
					
				}else{
					gadget1_player1--;
				}
				if (gadget1_player1 == gadget2_player1)
						gadget1_player1--;
				break;
			case TOU_RA_GADGET2_PL1:
				if(gadget2_player1 == last_gadget)
				{
					gadget2_player1 = 0;
				}else{
					gadget2_player1++;
				}
				if (gadget1_player1 == gadget2_player1)
						gadget2_player1++;
				break;
			case TOU_LA_GADGET2_PL1:
				if(gadget2_player1 == 0)
				{
					gadget2_player1 = last_gadget;
				}else{
					gadget2_player1--;
				}
				if (gadget1_player1 == gadget2_player1)
						gadget2_player1--;
				break;;

			// selecting gadgets player 2
			case TOU_RA_GADGET1_PL2:
				if(gadget1_player2 == last_gadget)
				{
					gadget1_player2 = 0;
				}else{
					gadget1_player2++;
				}
				if (gadget1_player2 == gadget2_player2)
						gadget1_player2++;
				break;
			case TOU_LA_GADGET1_PL2:
				if(gadget1_player2 == 0)
				{
					gadget1_player2 = last_gadget;
				}else{
					gadget1_player2--;
				}
				if (gadget1_player2 == gadget2_player2)
						gadget1_player2--;
				break;
			case TOU_RA_GADGET2_PL2:
				if(gadget2_player2 == last_gadget)
				{
					gadget2_player2 = 0;
				}else{
					gadget2_player2++;
				}
				if (gadget1_player2 == gadget2_player2)
						gadget2_player2++;
				break;
			case TOU_LA_GADGET2_PL2:
				if(gadget2_player2 == 0)
				{
					gadget2_player2 = last_gadget;
				}else{
					gadget2_player2--;
				}
				if (gadget1_player2 == gadget2_player2)
						gadget2_player2--;
				break;
			
							
			}
			break;

			

		case EWGUI_ELEMENT_HOVERED:
			switch(e->GetID())
			{
			case TOU_CLOSE:
				e->Draw();
				return true;
			case TOU_PLAY:
				e->Draw();
				return true;
			}
		break;
		}
	break;
	}
	CGUIImage *ship1 = (CGUIImage*)m_environment->GetElement(TOU_SHIP_PL1);
	ship1->SetImage(m_environment->AddImage(strFilename_ship_player[ship_player1], D3DCOLOR_RGBA(0,0,0,255)));

	CGUIImage *ship2 = (CGUIImage*)m_environment->GetElement(TOU_SHIP_PL2);
	ship2->SetImage(m_environment->AddImage(strFilename_ship_player[ship_player2], D3DCOLOR_RGBA(0,0,0,255)));

	CGUIImage *gadget1_pl1 = (CGUIImage*)m_environment->GetElement(TOU_GADGET1_PL1);
	gadget1_pl1->SetImage(m_environment->AddImage(strFilename_gadget[gadget1_player1], D3DCOLOR_RGBA(0,0,0,255)));
	
	CGUIImage *gadget2_pl1 = (CGUIImage*)m_environment->GetElement(TOU_GADGET2_PL1);
	gadget2_pl1->SetImage(m_environment->AddImage(strFilename_gadget[gadget2_player1], D3DCOLOR_RGBA(0,0,0,255)));

	CGUIImage *gadget1_pl2 = (CGUIImage*)m_environment->GetElement(TOU_GADGET1_PL2);
	gadget1_pl2->SetImage(m_environment->AddImage(strFilename_gadget[gadget1_player2], D3DCOLOR_RGBA(0,0,0,255)));
	
	CGUIImage *gadget2_pl2 = (CGUIImage*)m_environment->GetElement(TOU_GADGET2_PL2);
	gadget2_pl2->SetImage(m_environment->AddImage(strFilename_gadget[gadget2_player2], D3DCOLOR_RGBA(0,0,0,255)));

	if (random_map_cycle){
		CGUIImage *rand_maps = (CGUIImage*)m_environment->GetElement(TOU_RAND_MAPS);
		rand_maps->SetImage(m_environment->AddImage("imgs\\tournament\\x_tournament_p.bmp", D3DCOLOR_RGBA(0,0,0,255)));
	} else {
		CGUIImage *rand_maps = (CGUIImage*)m_environment->GetElement(TOU_RAND_MAPS);
		rand_maps->SetImage(m_environment->AddImage("imgs\\tournament\\x_tournament_n.bmp", D3DCOLOR_RGBA(0,0,0,255)));

	}

	if (enable_pods){
		CGUIImage *en_pods = (CGUIImage*)m_environment->GetElement(TOU_ENABLE_PODS);
		en_pods->SetImage(m_environment->AddImage("imgs\\tournament\\x_tournament_p.bmp", D3DCOLOR_RGBA(0,0,0,255)));
	} else {
		CGUIImage *en_pods = (CGUIImage*)m_environment->GetElement(TOU_ENABLE_PODS);
		en_pods->SetImage(m_environment->AddImage("imgs\\tournament\\x_tournament_n.bmp", D3DCOLOR_RGBA(0,0,0,255)));

	}
	if (enable_ai_enemies){
		CGUIImage *en_ai_enemies = (CGUIImage*)m_environment->GetElement(TOU_ENABLE_AI_ENEMIES);
		en_ai_enemies->SetImage(m_environment->AddImage("imgs\\tournament\\x_tournament_p.bmp", D3DCOLOR_RGBA(0,0,0,255)));
	} else {
		CGUIImage *en_ai_enemies = (CGUIImage*)m_environment->GetElement(TOU_ENABLE_AI_ENEMIES);
		en_ai_enemies->SetImage(m_environment->AddImage("imgs\\tournament\\x_tournament_n.bmp", D3DCOLOR_RGBA(0,0,0,255)));

	}*/

	return false;
}

void CTournamentMenu::OnClose()
{
}



