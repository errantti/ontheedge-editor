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

#include "HealthBar.h"
#include "..\\OnTheEdge.h"


#define HEALTH_BAR_FADE_TIME	0.4f	// time it takes for the health bar to fade in/out
#define HEALTH_BAR_KILL_DELAY	3		// If the class doesn't get health update in 3 frames then it will fade out
#define HEALTH_BAR_DROP_RATE	0.15f	// The rate at which the health bar shortens when health status changes (1.f = 100% per second)

#define HPBAR_IMAGE_WIDTH		142
#define HPBAR_IMAGE_HEIGHT		23
#define HPBAR_IMAGE_WIDTHF		142.f
#define HPBAR_IMAGE_HEIGHTF		23.f


int CHealthBar::m_instCount = 0;
bool CHealthBar::m_bFadingEnabled = true;
CTexture *CHealthBar::m_background = NULL;
CTexture *CHealthBar::m_healthTexture = NULL;

CHealthBar::CHealthBar() : m_framesToDeath(0), m_bActive(false)
{
	m_instCount++;
}
CHealthBar::~CHealthBar()
{
	m_instCount--;
	if(m_instCount == 0)
		UnloadImages();
}

void CHealthBar::UnloadImages()
{
	if(m_background)
	{
		delete m_background;
		m_background = NULL;
	}
	if(m_healthTexture)
	{
		delete m_healthTexture;
		m_healthTexture = NULL;
	}
}

void CHealthBar::LoadImages(char *background, char *hpbar)
{
	if(m_background == NULL)
	{
		m_background = new CTexture;
		if(m_background->LoadTexture(background, 0xFF000000) == FALSE)
		{
			delete m_background;
			m_background = NULL;
			CPrinter::log("Failed loading health bar background image", LOG_ERROR);
		}
	}
	if(m_healthTexture == NULL)
	{
		m_healthTexture = new CTexture;
		if(m_healthTexture->LoadTexture(hpbar, 0xFF000000) == FALSE)
		{
			CPrinter::log("Failed loading health bar image", LOG_ERROR);
			delete m_healthTexture;
			m_healthTexture = NULL;
		}
	}
}

void CHealthBar::Update(float frametime)
{
	if(!m_bActive)
		return;
	if(m_framesToDeath <=0)
	{
		if(m_state == DIALOGUE_STATE_DIALOGUE)
		{
			m_fadeTimer = HEALTH_BAR_FADE_TIME;
			m_state = DIALOGUE_STATE_FADE_OUT;
			if(!m_bFadingEnabled)
			{
				m_bActive = false;
				return;
			}
		}
		else if(m_state == DIALOGUE_STATE_FADE_OUT)
		{
			m_fadeTimer -= frametime;
			if(m_fadeTimer < 0.f)
			{
				m_bActive = false;
				return;
			}
		}
		else
			m_bActive = false;
		return;
	}

	if(m_state == DIALOGUE_STATE_FADE_IN)
	{
		m_fadeTimer += frametime;
		if(m_fadeTimer > HEALTH_BAR_FADE_TIME)
			m_state = DIALOGUE_STATE_DIALOGUE;
	}

	m_framesToDeath--;

	if(m_healthPercentage > m_targetPercentage)
	{
		m_healthPercentage -= frametime;
		if(m_healthPercentage < m_targetPercentage)
			m_healthPercentage = m_targetPercentage;
	}
	else
		m_healthPercentage = m_targetPercentage;
}

void CHealthBar::Render()
{
	if(!m_bActive)
		return;

	LPD3DXSPRITE sprite = GetDXApp()->GetSpriteInterface();
	if(sprite == NULL || m_background == NULL || m_healthTexture == NULL)
		return;

	D3DXMATRIX mat, id;
	float temp;
	D3DXMatrixIdentity(&mat);
	D3DXMatrixIdentity(&id);
	D3DXVECTOR3 center(HPBAR_IMAGE_WIDTHF * 0.5f, HPBAR_IMAGE_HEIGHTF * 0.5f, 0.f);

	int width = GetOTE()->GetWidth() , height = GetOTE()->GetHeight();
	int halfWidth = (int)(width * 0.5), halfHeight = (int)(height * 0.5);
	D3DXVECTOR3 position((float)halfWidth, 20.f, 0.f);

	RECT r;
	r.left = 0;
	r.top = 0;

	if(m_state == DIALOGUE_STATE_FADE_IN || m_state == DIALOGUE_STATE_FADE_OUT)
	{
		//temp = (m_fadeTimer+0.2f)/(DIALOGUE_FADE_TIME+0.2f);
		temp = m_fadeTimer/HEALTH_BAR_FADE_TIME;
		mat._11 = temp;
		//mat._42 = 256.f - temp * 256.f;
		mat._41 = halfWidth - temp * halfWidth;
		sprite->SetTransform(&mat);
		sprite->Draw(m_background->GetTexture(), NULL, &center, &position, 0xFFFFFFFF);
		sprite->SetTransform(&id);
	}
	else
	{
		sprite->Draw(m_background->GetTexture(), NULL, &center, &position, 0xFFFFFFFF);

		// Render health
		r.bottom = HPBAR_IMAGE_HEIGHT;
		r.right = (int)(HPBAR_IMAGE_WIDTHF * m_healthPercentage);
		position.x += 10.f;
		sprite->Draw(m_healthTexture->GetTexture(), &r, &center, &position, 0xFFFFFFFF);
	}
}

void CHealthBar::SetHealth(float hitPoints, float maxHitpoins)
{
	m_targetPercentage = hitPoints / maxHitpoins;
	m_framesToDeath = HEALTH_BAR_KILL_DELAY;
	if(!m_bActive)
	{
		// Display fade in 'animation' and activate the health bar
		m_bActive = true;
		m_healthPercentage = 1.f;
		m_fadeTimer = 0.f;
		if(m_bFadingEnabled)
			m_state = DIALOGUE_STATE_FADE_IN;
		else
			m_state = DIALOGUE_STATE_DIALOGUE;
	}
	else if(m_state == DIALOGUE_STATE_FADE_OUT)
		m_state = DIALOGUE_STATE_FADE_IN;
}