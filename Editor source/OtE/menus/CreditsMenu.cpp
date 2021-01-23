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

#include "CreditsMenu.h"
#include "..\\OnTheEdge.h"
#include "MenuStackManager.h"

#define FIRST_TEXT_ID		200

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCreditsMenu::CCreditsMenu(IMenuManager *mgr)
: CFlowMenu(MENU_ID_CREDITS, mgr, MENU_ID_MAIN)
{
}

CCreditsMenu::~CCreditsMenu()
{
}

void CCreditsMenu::CustomInit()
{
	// Create text controls
	ResetTextControls();

	LoadCreditsFromFile("ingame_credits.txt");

	if(m_creditsText.empty())
		return;

	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();

	rect<int> pos;
	pos.left = 10;
	pos.right = 277;
	pos.bottom = 160;
	pos.top = 120;

	m_timeFromLastText = 0.f;
	m_nextTextIndex = 0;

	for(int i=0; i<NUM_ROLLING_TEXT_CONTROLS; ++i)
	{
		m_textControls[i].control = (CGUILabel*)env->AddLabel(m_window, pos, FIRST_TEXT_ID+i);
		m_textControls[i].control->SetModColor(D3DCOLOR_RGBA(150, 150, 150, 0));
	}
}

bool CCreditsMenu::OnEvent(SEvent event)
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
			case CRE_CLOSE:
				CloseFlowMenu();
				return true;
			}
			break;
		}
		break;
	}

	return false;
}

void CCreditsMenu::OnClose()
{
	return;
}

bool CCreditsMenu::LoadCreditsFromFile(const std::string filename)
{
	// Check whether the credits file has already been loaded
	if(!m_creditsText.empty())
		return true;

	ifstream in(filename.c_str());
	if(!in.is_open())
		return false;

	char buf[1024];
	while(in.peek() != EOF)
	{
		in.getline(buf, 1023);
		buf[1023] = '\0';
		m_creditsText.push_back(buf);
	}

	in.close();

	return true;
}

void CCreditsMenu::ResetTextControls()
{
	for(int i=0; i<NUM_ROLLING_TEXT_CONTROLS; ++i)
	{
		if(m_textControls[i].control != NULL)
		{
			//m_textControls[i].control->Remove();
			m_textControls[i].control = NULL;
		}
		m_textControls[i].enabled = false;
	}
}

int CCreditsMenu::GetNextFreeText()
{
	bool found = false;
	for(int i = 0; i<NUM_ROLLING_TEXT_CONTROLS; ++i)	
	{
		if(m_textControls[i].enabled)
			found = true;
		else if(found)
			return i;
	}
	if(m_textControls[0].enabled)
		return -1;
	return 0;
}

void CCreditsMenu::UpdateRollingText(float frameTime)
{
	if(m_creditsText.empty())
		return;

	int i;
	float speed = frameTime * 15.f;
	int startBottom = 160;

	DWORD alpha;
	
	//float fadeMultiplier = 255.f / 20.f;
	float fadeMultiplier = 255.f * 0.05f;

	D3DCOLOR textColor = D3DCOLOR_RGBA(200, 200, 200, 0);
	D3DCOLOR textColorFull = D3DCOLOR_RGBA(200, 200, 200, 255);
	D3DCOLOR textColorNull = D3DCOLOR_RGBA(200, 200, 200, 0);

	if(speed == 0)
		speed = 1;
	rect<int> pos;

	if(m_timeFromLastText <= 0.f)
	{
		if(m_nextTextIndex >= (int)m_creditsText.size())
		{
			m_timeFromLastText = 5.f;
			m_nextTextIndex = 0;
		}
		else
		{
			int index = GetNextFreeText();
			if(index != -1)
			{
				m_textControls[index].enabled = true;
				pos = m_textControls[index].control->GetElementRect();
				pos.bottom = startBottom;
				pos.top = startBottom-20;
				m_textControls[index].control->SetElementRect(pos);
				m_textControls[index].control->SetModColor(textColorNull);
				m_textControls[index].control->SetVisible(true);
				m_textControls[index].control->SetCaption(m_creditsText[m_nextTextIndex++]);
				m_textControls[index].time = 0.f;

				m_timeFromLastText = 1.0f;
			}
		}
	}
	else
		m_timeFromLastText -= frameTime;

	for(i=0; i<NUM_ROLLING_TEXT_CONTROLS; ++i)
	{
		if(!m_textControls[i].enabled)
			continue;

		m_textControls[i].time += speed;

		pos = m_textControls[i].control->GetElementRect();
		pos.bottom = startBottom - (int)m_textControls[i].time;
		pos.top = pos.bottom - 20;
		m_textControls[i].control->SetElementRect(pos);

		if(m_textControls[i].time > 110.f)
		{
			m_textControls[i].control->SetVisible(false);
			m_textControls[i].enabled = false;
		}
		else if(m_textControls[i].time > 90.f)
		{
			alpha = ((int)((110.f-m_textControls[i].time) * fadeMultiplier))&0xFF;
			textColor = (textColor&0x00FFFFFF) | (alpha<<24);
			m_textControls[i].control->SetModColor(textColor);
			//m_textControls[i].control->SetText
		}
		else if(m_textControls[i].time < 20.f)
		{
			alpha = ((int)(m_textControls[i].time * fadeMultiplier))&0xFF;
			textColor = (textColor&0x00FFFFFF) | (alpha<<24);
			m_textControls[i].control->SetModColor(textColor);
		}
		else
			m_textControls[i].control->SetModColor(textColorFull);
	}
}

void CCreditsMenu::Update()
{
	// Update rolling texts
	UpdateRollingText(GetOTE()->GetFrameTime());

	// The usual update
	CFlowMenu::Update();
}