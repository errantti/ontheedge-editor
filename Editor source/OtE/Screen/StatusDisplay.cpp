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

#include "..\\OnTheEdge.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusDisplay::CStatusDisplay() : m_lifePercentage(1.f), m_energyPercentage(1.f),
	m_pl2Life(1.f), m_pl2Energy(1.f), m_bShowFPS(FALSE), m_bYDown(TRUE), m_timeFromFPSUpdate(0.f)
{
	m_gadgetModColor[0] = m_gadgetModColor[1] = 0xFFFFFFFF;
	m_gadgetPercentage[0] = m_gadgetPercentage[1] = 1.f;
	m_pl2gadget[0] = m_pl2gadget[1] = 1.f;
}

CStatusDisplay::~CStatusDisplay()
{
}

bool CStatusDisplay::LoadDefaultGraphics()
{
	int failed = 0;

	if(m_background.LoadTexture("imgs\\Status\\status.png", 0) == FALSE)
		++failed;

	if(m_lifeBar.LoadTexture("imgs\\Status\\shields.png", 0) == FALSE)
		++failed;

	if(m_energyBar.LoadTexture("imgs\\Status\\energy.png", 0) == FALSE)
		++failed;

	//if(m_gadgetLoadBar.LoadTexture("imgs\\Status\\GadgetLoad.tga", 0xFF000000) == FALSE)
	//	++failed;

	if(failed > 0)
	{
		CPrinter::logError("Failed loading one or more of the status display images");
		return FALSE;
	}

	return TRUE;
}

void CStatusDisplay::Render()
{
	LPDIRECT3DTEXTURE9 tex = m_background.GetTexture();
	LPD3DXSPRITE sprite = GetDXApp()->GetSpriteInterface();

	if(sprite == NULL)
		return;

	RECT r;
	r.left = 0;
	r.top = 0;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 center;

	IGadget *g;
	D3DXMATRIX mat, id;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixIdentity(&id);

	float width = (float)GetOTE()->GetWidth() , height = (float)GetOTE()->GetHeight();
	char buf[8];

	CGUIEnvironment *env = GetOTE()->GetGUIEnvironment();
	CFont *f = env->GetDefaultFont();

	/*** 
	 * Single player mode - player 1
	 ***/
	if(tex)
	{
		// Render the player one background image
		// Background image is 400x70 and is located in the lower right corner of the screen
		// The coordinates of the upper left corner of the status display are (400, 530)
		//pos = D3DXVECTOR3(400.f, 530.f, 0.f);
		//pos = D3DXVECTOR3(width - 400.f, height - 70.f, 0.f);
		pos = D3DXVECTOR3(0.f, 0.f, 0.f);
		sprite->Draw(tex, NULL, NULL, &pos, 0xFFFFFFFF);
	}

	tex = m_lifeBar.GetTexture();
	if(tex)
	{
		r.bottom = 15;
		r.right = (int)(142 * m_lifePercentage);
		// The relative coordinates of the health bar image are (65, 10)
		//pos = D3DXVECTOR3(width - 335.f, height - 60.f, 0.f);
		pos = D3DXVECTOR3(145.f, 4.f, 0.f);
		sprite->Draw(tex, &r, NULL, &pos, 0xFFFFFFFF);
	}

	sprintf(buf, "%i%%", (int)(m_lifePercentage*100.f));
	f->Print(buf, rect<int>(97, 142, 3, 17), 0xFFFFFFFF, DT_RIGHT);

	tex = m_energyBar.GetTexture();
	if(tex)
	{
		r.bottom = 15;
		r.right = (int)(142 * m_energyPercentage);
		// The relative coordinates of the energy bar image are (37, 40)
		//pos = D3DXVECTOR3(width - 363.f, height - 30.f, 0.f);
		pos = D3DXVECTOR3(145.f, 23.f, 0.f);
		sprite->Draw(tex, &r, NULL, &pos, 0xFFFFFFFF);
	}

	sprintf(buf, "%i%%", (int)(m_energyPercentage*100.f));
	f->Print(buf, rect<int>(97, 142, 22, 39), 0xFFFFFFFF, DT_RIGHT);

	/*tex = m_gadgetLoadBar.GetTexture();
	if(tex)
	{
		r.right = (int)(120.f*m_gadgetPercentage[0]);
		r.bottom = 9;
		// The relative coordinates of the first gadget load bar are (223, 16)
		pos = D3DXVECTOR3(width - 400.f + 223.f, height - 70.f + 16.f, 0.f);
		sprite->Draw(tex, &r, NULL, &pos, m_gadgetModColor[0]);

		r.right = (int)(120.f*m_gadgetPercentage[1]);
		// The relative coordinates of the second gadget load bar are (196, 45)
		pos = D3DXVECTOR3(width - 400.f + 196.f, height - 70.f + 45.f, 0.f);
		sprite->Draw(tex, &r, NULL, &pos, m_gadgetModColor[1]);
	}*/

	// Draw the gadget icons
	//CShip *s = GetOTE()->GetPlayer(0)->GetShip();
	CPlayer *p1 = GetOTE()->GetPlayer(0);
	if(p1->GetShip() != NULL)
	{
		//mat._11 = mat._22 = 0.5f;
		//sprite->SetTransform(&mat);

		bool changing = false;
		if((g = p1->GetShipGadget(1, changing)) != NULL)
		{
			//GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(760.f, 535.f));
			//GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(1520.f, 1070.f));
			//GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2((width - 40.f)*2.f, (height - 65.f)*2.f));
			//g->
			if(changing)
				GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(9.f, 2.f), 0x6AFFBBFF);
			else if(g->IsActive())
				GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(9.f, 2.f), 0xFFBBBBFF);
			else
				GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(9.f, 2.f));
		}
		if((g = p1->GetShipGadget(0, changing)) != NULL)
		{
			//GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(738.f, 568.f));
			//GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(1476.f, 1136.f));
			//GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2((width - 62.f)*2.f, (height - 32.f)*2.f));
			if(changing)
				GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(53.f, 2.f), 0x6AFFBBFF);
			else
				GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(53.f, 2.f));
		}
		sprite->SetTransform(&id);
	}

	/*** 
	 * Multiplayer mode - player 2 status
	 ***
	if(GetOTE()->IsMultiplayer())
	{
		mat._11 = -1.f;
		mat._22 = 1.f;
		sprite->SetTransform(&mat);
		tex = m_pl1Background.GetTexture();
		if(tex)
		{
			// Render the player two background image
			// Background image is 400x70 and is located in the lower right corner of the screen
			// The coordinates of the upper left corner of the status display are (400, 530)
			pos = D3DXVECTOR3(-400.f, 530.f, 0.f);
			sprite->Draw(tex, NULL, NULL, &pos, 0xFFFFFFFF);
		}
		//sprite->SetTransform(&id);

		tex = m_lifeBar.GetTexture();
		if(tex)
		{
			r.bottom = 19;
			r.right = (int)(151.f * m_pl2Life);
			// The relative coordinates of the health bar image are (65, 10)
			pos = D3DXVECTOR3(-334.f, 540.f, 0.f);
			sprite->Draw(tex, &r, NULL, &pos, 0xFFFFFFFF);
		}

		tex = m_energyBar.GetTexture();
		if(tex)
		{
			r.bottom = 19;
			r.right = (int)(151.f * m_pl2Energy);
			// The relative coordinates of the energy bar image are (37, 40)
			pos = D3DXVECTOR3(-362.f, 570.f, 0.f);
			sprite->Draw(tex, &r, NULL, &pos, 0xFFFFFFFF);
		}

		tex = m_gadgetLoadBar.GetTexture();
		if(tex)
		{
			r.right = (int)(120.f*m_pl2gadget[0]);
			r.bottom = 9;
			// The relative coordinates of the first gadget load bar are (223, 16)
			pos = D3DXVECTOR3(-176.f, 546.f, 0.f);
			sprite->Draw(tex, &r, NULL, &pos, 0xFFFFFFFF);

			r.right = (int)(120.f*m_pl2gadget[1]);
			// The relative coordinates of the second gadget load bar are (196, 45)
			pos = D3DXVECTOR3(-202.f, 575.f, 0.f);
			sprite->Draw(tex, &r, NULL, &pos, 0xFFFFFFFF);
		}

		s = GetOTE()->GetPlayer(1)->GetShip();

		if(s)
		{
			mat._11 = mat._22 = 0.5f;
			sprite->SetTransform(&mat);

			if((g = s->GetEquippedGadget(0)) != NULL)
			{
				//GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(760.f, 535.f));
				GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(16.f, 1070.f));
			}
			if((g = s->GetEquippedGadget(1)) != NULL)
			{
				//GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(738.f, 568.f));
				GetDXApp()->Draw2DImage(g->GetIcon(), D3DXVECTOR2(60.f, 1136.f));
			}
		}
		sprite->SetTransform(&id);
	}*/
}

void CStatusDisplay::Update(float frameTime)
{
	CPlayer *p1 = GetOTE()->GetPlayer(0);
	CShip *s;
	if(p1 != NULL)
	{
		s = p1->GetShip();
		if(s)
		{
			m_lifePercentage = s->GetHitpoints()/s->GetMaxHitPoints();
			m_energyPercentage = s->GetEnergy() / s->GetMaxEnergy();

			// Make sure that the life won't go to 0% even though player is still alive
			if(m_lifePercentage < 0.01f)
				m_lifePercentage = 0.01f;
		}
		else
		{
			// Assume the player has been destroyed
			m_lifePercentage = m_energyPercentage = 0.f;
		}

		m_gadgetModColor[0] = m_gadgetModColor[1] = 0xFFFFFFFF;
		m_gadgetPercentage[0] = m_gadgetPercentage[1] = 1.f;
		// Gadgets
		/*if(p1->IsChangingGadget())
		{
			int s = p1->GetNewGadgetSlot();
			m_gadgetModColor[s] = 0xFF00A0FF;
			m_gadgetPercentage[s] = (GADGET_CHANGE_DELAY-p1->GetGadgetChangeCounter()) / GADGET_CHANGE_DELAY;
		}*/
	}

	// Update player 2 if in multiplayer mode
	CPlayer *p2 = GetOTE()->GetPlayer(1);
	if(p2 != NULL && GetOTE()->IsMultiplayer())
	{
		s = p2->GetShip();
		if(s)
		{
			m_pl2Life = s->GetHitpoints()/s->GetMaxHitPoints();
			m_pl2Energy = s->GetEnergy() / s->GetMaxEnergy();
		}
		else
		{
			// Assume the player has been destroyed
			m_pl2Life = m_pl2Energy = 0.f;
		}
		m_pl2gadget[0] = m_pl2gadget[1] = 1.f;
	}

	bool bFPSUpdated = FALSE;
	CScreenMessages::SScreenMessage msg;
	stringstream stream;

	CInputKeyboard *k = GetOTE()->GetKeyboard();

	static int framesSinceLastUpdate = 0;
	BYTE code = CGameSettings::GetSettingsObject()->GetGeneralKeyboardControlSet().displayFPS;
	if(k->GetButtonDown(code) && !m_bYDown)
	{
		m_bYDown = TRUE;
		m_bShowFPS = !m_bShowFPS;
		framesSinceLastUpdate = FPS_UPDATE_FREQUENCY;
		m_timeFromFPSUpdate = FPS_UPDATE_FREQUENCY;
		if(m_bShowFPS)
		{
			msg.FillDefaults();
			msg.area = rect<int>(600, 800, 10, 50);
			stream << 1.f / frameTime;
			stream >> msg.message;
			msg.message = "FPS: " + msg.message + "\nAvarage FPS: ?";
			msg.ID = 213;
			msg.bDelete = FALSE;
			if(!GetOTE()->GetScreenMessageInterface()->UpdateDetailedMessage(msg.ID, msg.message))
				GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);
		}
		else
		{
			GetOTE()->GetScreenMessageInterface()->RemoveDetailedMessage(213);
		}
		bFPSUpdated = TRUE;
	}

	if(!k->GetButtonDown(code))
		m_bYDown = FALSE;

	if(!bFPSUpdated && m_bShowFPS)
	{
		m_timeFromFPSUpdate -= frameTime;
		++framesSinceLastUpdate;
		if(m_timeFromFPSUpdate < 0.f)
		{
			stream << 1.f / frameTime;
			stream >> msg.message;
			msg.message = "FPS: " + msg.message + "\nAvarage FPS: ";
			stringstream s2;
			s2 << ((float)framesSinceLastUpdate / FPS_UPDATE_FREQUENCY);
			string avarage;
			s2 >> avarage;
			msg.message += avarage;
			GetOTE()->GetScreenMessageInterface()->UpdateDetailedMessage(213, msg.message);
			m_timeFromFPSUpdate = FPS_UPDATE_FREQUENCY;
			framesSinceLastUpdate = 0;
		}
	}
}