// SinglePlayerMenu.cpp: implementation of the CSinglePlayerMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "SinglePlayerMenu.h"
#include "..\\OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSinglePlayerMenu::CSinglePlayerMenu(IMenuManager *mgr)
: IMenu(MENU_SINGLEPLAYER , mgr), m_selectedShip(0), m_bEscDown(TRUE)
{
	m_bRequiresUpdate = TRUE;

	m_shipIcon[0] = "imgs\\ships\\ship1.bmp";
	m_shipIcon[1] = "imgs\\ships\\ship2.bmp";
	m_shipIcon[2] = "imgs\\ships\\ship3.bmp";
	m_shipIcon[3] = "imgs\\ships\\ship4.bmp";

	m_shipDetails[0] = "'Cab'\nA light vessel which got its name from becoming known as a ship for \
		the skilled pilots becouse of its speed and manoeuvering abilities. Downside is that it can \
		carry a very limited weight and has light armoring but still has a moderately powerful \
		energy reactor. Remember that cab's strength is its speed and manouverability. Cab is made \
		for running from enemies rather destroying them.";
	m_shipDetails[1] = "Medium class fightervessel \nA ship with avarage speed and good energy reactor but \
		cannot carry heavy loads. As 'medium' has an avarage plating it's good for fighting every now and \
		then but don't try to brind down too big an enemy mob at once, running is an option as well becouse \
		'medium' is fast enough to do that when required.";
	m_shipDetails[2] = "Tank\nA heavy surface fighter which can carry loads of gadgets, has a powerful \
		reactor and good armoring. Due to the weight of the ship the engine cannot boost this ship \
		type to great speeds. True combat powerhouse whose main strategy is to deliver more damage then \
		it gets. It's important for tank to use the best armament you can find.";
	m_shipDetails[3] = "Vanquisher\nA hybrid vessel which on the other hand has good engine and can reach \
		good speeds and on the other also has moderately heavy plating. Downsides are weak energy reactor \
		and below-avarage manoeuvering.";

	for(int i=0;i<4;++i)
		m_shipIDs[i] = i+1;
}

CSinglePlayerMenu::~CSinglePlayerMenu()
{

}

bool CSinglePlayerMenu::InitializeMenu(CGUIEnvironment *env)
{
	/*
	144,658,72,584 bg_single
	144,+256,460,508 close_single
	400,+256,460,508 play_single
	199,231,296,312 leftarrow_single_h
	231,263,296,312 rightarrow_single_h*/
	m_environment = env;
	if(env == NULL)
		return false;
	CTexture *bg_singleplayer = env->LoadImage("imgs\\singleplayer\\bg_single.tga", D3DCOLOR_RGBA(0,0,0,255));
	env->AddImageElement(rect<int>(144,800,72,600),bg_singleplayer,SING_BG);

	CGUIButton *b_close;
	CTexture *close_singleplayer_h = env->LoadImage("imgs\\singleplayer\\close_single_h.jpg", D3DCOLOR_RGBA(0,0,0,255));
	b_close = env->AddButton(rect<int>(144, 400, 460, 508), "", SING_CLOSE);
	b_close->SetImageHover(close_singleplayer_h);
	
	CGUIButton *b_play;
	CTexture *play_singleplayer_h = env->LoadImage("imgs\\singleplayer\\play_single_h.jpg", D3DCOLOR_RGBA(0,0,0,255));
	b_play = env->AddButton(rect<int>(400, 656, 460, 508), "", SING_PLAY);
	b_play->SetImageHover(play_singleplayer_h);

	env->AddImageElement(rect<int>(199,263,232,296), NULL, SING_SHIP);
	
	CGUIButton *leftArrow;
	CTexture *leftArrowTex = env->LoadImage("imgs\\singleplayer\\leftarrow_single_h.tga", D3DCOLOR_RGBA(0,0,0,255));
	leftArrow = env->AddButton(rect<int>(199, 231, 296, 312), "", SING_LEFT);
	leftArrow->SetImageHover(leftArrowTex);
	
	CGUIButton *rightArrow;
	CTexture *rightarrowTex = env->AddImage("imgs\\singleplayer\\rightarrow_single_h.tga", D3DCOLOR_RGBA(0,0,0,255));
	rightArrow = env->AddButton(rect<int>(231, 263, 296, 312), "", SING_RIGHT);
	rightArrow->SetImageHover(rightarrowTex);

	// Details label control (relative coordinates: 162, 158 Size: 300x200)
	env->AddLabel(rect<int>(306, 606, 230, 430), NULL, SING_DETAILS);

	m_selectedShip = 2; // Make Tank the default selection
	for(int i=0; i<4; ++i)
		m_ships[i] = env->LoadImage(m_shipIcon[i].c_str(), 0xFF000000);

	UpdateShipIcon();
	m_bEscDown = TRUE;
	
	return true;
}

bool CSinglePlayerMenu::OnEvent(SEvent event)
{
	
	IGUIElement *e = (IGUIElement*)event.guiEvent.element;
	switch(event.type)
	{
	case EW_GUI_EVENT:
		switch(event.guiEvent.type)
		{
	
		case EWGUI_ELEMENT_PRESSED:
			switch(e->GetID())
			{
			case SING_CLOSE:
				m_manager->OpenMenu(MENU_ID_MAIN, true);
				return true;
			case SING_PLAY:
				
				return true;

			case SING_LEFT:
				--m_selectedShip;
				UpdateShipIcon();
				break;

			case SING_RIGHT:
				++m_selectedShip;
				UpdateShipIcon();
				break;
			}
			break;
		}
	break;
	}
		


	return false;
}


bool CSinglePlayerMenu::RemoveMenuElements()
{
	for(int i=SING_BG; i< SING_COUNT;i++)
		m_environment->RemoveElement(i);
	m_environment->RemoveImages();
	return true;

}

void CSinglePlayerMenu::UpdateShipIcon()
{
	if(m_selectedShip < 0)
		m_selectedShip = 3;
	if(m_selectedShip > 3)
		m_selectedShip = 0;

	// Update ship icon and details
	CGUIImage *i = (CGUIImage*)m_environment->GetElement(SING_SHIP);
	if(i)
	{
		i->SetIndependentImage(m_ships[m_selectedShip]);
	}
	CGUILabel *l = (CGUILabel*)m_environment->GetElement(SING_DETAILS);
	if(l)
	{
		l->SetCaption(m_shipDetails[m_selectedShip].c_str());
	}
}

void CSinglePlayerMenu::Update()
{
	if(KEY_DOWN(VK_ESCAPE) && !m_bEscDown)
	{
		m_bEscDown = TRUE;
		m_manager->OpenMenu(MENU_ID_MAIN);
		return;
	}
	if(!KEY_DOWN(VK_ESCAPE))
		m_bEscDown = FALSE;
}