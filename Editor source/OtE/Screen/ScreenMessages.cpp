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

#include "ScreenMessages.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenMessages::CScreenMessages() : m_halfMessageHeight(10), m_defaultTextColor(0xFFFFFFFF)
{
}

CScreenMessages::~CScreenMessages()
{
}

void CScreenMessages::Update(float frameTime)
{
	vector<SScreenMessage>::iterator it = m_detailedMessages.begin();
	for(; it != m_detailedMessages.end();)
	{
		if(it->bDelete)
		{
			if(it->time < 0.1f)
			{
				it = m_detailedMessages.erase(it);
				continue;
			}
			it->time -= frameTime;
		}
		++it;
	}
}

void CScreenMessages::Render()
{
	CFont *font = GetEdgewalkerApp()->GetGUIEnvironment()->GetDefaultFont();

	vector<SScreenMessage>::iterator it = m_detailedMessages.begin();
	D3DCOLOR c = D3DCOLOR_RGBA(255, 255, 255, 10);
	DWORD alpha;
	for(; it != m_detailedMessages.end(); ++it)
	{
		//font->Print(it->message.c_str(), it->area, it->textColor, it->flags);
		if(it->time < it->fadeTime)
		{
			alpha = ((int)(it->time/it->fadeTime*255.f))&0xFF;
			it->textColor = (it->textColor&0x00FFFFFF) | (alpha<<24);
		}
		//it->textColor = (it->textColor&0x00FFFFFF) | ((50)&0xFF)<<24;
		font->Print(it->message.c_str(), it->area, it->textColor, it->flags);
	}
}

void CScreenMessages::AddDetailedMessage(SScreenMessage s)
{
	if((s.time < 0.001f && s.bDelete) || s.message == "")
		return;

	if(s.fadeTime > s.time)
		s.fadeTime = s.time;
	if(s.fadeTime <= 0.f)
		s.bFades = FALSE;

	m_detailedMessages.push_back(s);
}

bool CScreenMessages::UpdateDetailedMessage(int ID, string newMessage)
{
	if(ID == -1)
		return false;

	vector<SScreenMessage>::iterator it = m_detailedMessages.begin();
	for(; it != m_detailedMessages.end(); ++it)
		if(it->ID == ID)
		{
			it->message = newMessage;
			return true;
		}
	return false;
}

bool CScreenMessages::RemoveDetailedMessage(int ID)
{
	if(ID == -1)
		return false;

	vector<SScreenMessage>::iterator it = m_detailedMessages.begin();
	for(; it != m_detailedMessages.end(); ++it)
		if(it->ID == ID)
		{
			m_detailedMessages.erase(it);
			return true;
		}

	return false;
}

void CScreenMessages::AddMessage(std::string msg, float time)
{
	AddMessage(msg, time, m_defaultTextColor);
}

void CScreenMessages::AddMessage(std::string msg, float time, D3DCOLOR color)
{
	if(time < 0.5f || msg.size() < 1)
		return;

	SScreenMessage m;
	m.FillBasicMessage();

	m.message = msg;
	m.time = time;
	m.textColor = color;
	m.flags = DT_VCENTER;

	// Position the text to the center
	/*int hheight = (int)((float)GetDXApp()->GetHeight()*0.5f);
	int hwidth = (int)((float)GetDXApp()->GetWidth()*0.5f);

	m.area.left = hwidth - 300;
	m.area.right = hwidth + 300;
	m.area.bottom = hheight + m_halfMessageHeight;
	m.area.top = hheight - m_halfMessageHeight;*/

	m.area.left = 10;
	m.area.right = 350;
	m.area.top = 130 - m_halfMessageHeight;
	m.area.bottom = 130 + m_halfMessageHeight;

	BumpBasicMessages();

	m_detailedMessages.push_back(m);
}

void CScreenMessages::BumpBasicMessages()
{
	int height = 2*m_halfMessageHeight;
	vector<SScreenMessage>::iterator it = m_detailedMessages.begin();
	for(; it != m_detailedMessages.end();)
	{
		if(it->bAutoPushback)
		{
			it->area.bottom -= height;
			it->area.top -= height;

			if(it->area.top < 30)
			{
				it = m_detailedMessages.erase(it);
				continue;
			}
		}
		++it;
	}
}