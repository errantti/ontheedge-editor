// MultiPlayerSelectionMenu.cpp: implementation of the CMultiPlayerSelectionMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "MultiPlayerSelectionMenu.h"
#include "..\\OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiPlayerSelectionMenu::CMultiPlayerSelectionMenu(IMenuManager *mgr)
: IMenuHandler(MENU_MULTIPLAYER_SELECTION , mgr)
{

}

CMultiPlayerSelectionMenu::~CMultiPlayerSelectionMenu()
{

}

bool CMultiPlayerSelectionMenu::InitializeMenu()
{
	/*CPrinter::log("CMultiPlayerSelectionMenu InitializeMenu entered");
	m_environment = env;
	if(env == NULL)
		return false;

	CTexture *bg_multis = env->LoadImage("imgs\\multiselection\\bg_multis.jpg", D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(0, 800, 0, 600), bg_multis, MPS_BG);
	

	CGUIButton *b_quickmatch;
	CTexture *quickmatch_multis_h = env->LoadImage("imgs\\multiselection\\quickmatch_multis_h.bmp", D3DCOLOR_RGBA(0,0,0,255));
	b_quickmatch =env->AddButton(rect<int>(239, 751, 223, 287), "", MPS_QUICK_GAME);
	b_quickmatch->SetImageHover(quickmatch_multis_h);	

	CGUIButton *b_tournament;
	CTexture *tournament_multis_h = env->LoadImage("imgs\\multiselection\\tournament_multis_h.bmp", D3DCOLOR_RGBA(0,0,0,255));
	b_tournament = env->AddButton(rect<int>(239, 751, 271, 335), "", MPS_TOURNAMENT);
	b_tournament->SetImageHover(tournament_multis_h);

	CGUIButton *b_close;
	CTexture *close_multis_h = env->LoadImage("imgs\\multiselection\\back_multis_h.bmp", D3DCOLOR_RGBA(0,0,0,255));
	
	b_close = env->AddButton(rect<int>(239, 751, 367, 431), "", MPS_CLOSE);
	b_close->SetImageHover(close_multis_h);*/
	return true;

}

bool CMultiPlayerSelectionMenu::OnEvent(SEvent event)
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
			case MPS_QUICK_GAME:
				m_manager->CloseMenu();
				GetOTE()->StartMultiplayer();
				m_environment->SetCursorVisibility(FALSE);
				return false;
				break;
			case MPS_TOURNAMENT:
				//m_manager->OpenMenu(MENU_TOURNAMENT, true);
				//CPrinter::log(" MPS_TOURNAMENT valittu");
				return false;
				break;
			case MPS_CLOSE:
				m_manager->OpenMenu(MENU_ID_MAIN, true);
				return false;
				break;
			}
			break;
		}

	break;
	}*/
	return false;

}


void CMultiPlayerSelectionMenu::OnClose()
{
	return;
}
