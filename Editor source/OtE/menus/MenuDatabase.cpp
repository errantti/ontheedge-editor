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

#include "MenuDatabase.h"
#include "..\\Edgewalker\\Utilities.h"
#include "..\\OnTheEdge.h"

#include <fstream>

using namespace std;

bool CMenuDatabase::LoadDatabase(std::string filename)
{
	fstream f;
	f.open(filename.c_str(), std::ios::in | std::ios::binary);
	if(!f.is_open())
		return false;
	m_menus.clear();

	DWORD id = 0;

	f.read((char*)&id, sizeof(DWORD));
	if(id != MENU_DATABASE_BEGIN_IDENTIFIER)
	{
		CPrinter::logError("Menu database begin identifier mismatch");
		f.close();
		return false;
	}

	int menuCount;
	f.read((char*)&menuCount, sizeof(int));
	if(menuCount < 1 || menuCount > 50)
	{
		CPrinter::logError("Menu database read error: no menus to read or file is corrupt");
		f.close();
		return false;
	}

	SMenuData data;
	SMenuElement element;
	for(int i=0;i<menuCount;++i)
	{
		if(f.peek() == EOF)
		{
			CPrinter::logError("Menu database read error: unexpected end of file found");
			f.close();
			return false;
		}

		f.read((char*)&data.header, sizeof(SMenuHeader));

		if(data.header.elementCount < 1)
			continue;
		if(data.header.elementCount > 80)
		{
			return false;
		}

		for(int k=0;k<data.header.elementCount;++k)
		{
			f.read((char*)&element, sizeof(SMenuElement));
			data.elements.push_back(element);
		}

		m_menus.push_back(data);
		data.elements.clear();
	}
	f.close();

	return true;
}

bool CMenuDatabase::SaveDatabase(std::string filename)
{
	fstream f;
	f.open(filename.c_str(), std::ios::out | std::ios::binary);
	if(!f.is_open())
	{
		CPrinter::logError("Failed saving menu database: couldn't open file to write");
		return false;
	}

	DWORD identifier = MENU_DATABASE_BEGIN_IDENTIFIER;
	f.write((const char*)&identifier, sizeof(DWORD));

	int menuCount = (int)m_menus.size();
	f.write((const char*)&menuCount, sizeof(int));

	vector<SMenuData>::iterator it = m_menus.begin();
	for(; it != m_menus.end(); ++it)
	{
		it->header.elementCount = (int)it->elements.size();
		f.write((const char*)&it->header, sizeof(SMenuHeader));

		vector<SMenuElement>::iterator it2 = it->elements.begin();
		for(; it2 != it->elements.end(); ++it2)
			f.write((const char*)&(*it2), sizeof(SMenuElement));
	}

	f.close();

	return true;
}

bool CMenuDatabase::ApplyMenuData(CGUIWindow *window, int menuID, IMenuHandler *handler)
{
	SMenuData *m = NULL;

	vector<SMenuData>::iterator it = m_menus.begin();
	for(; it != m_menus.end(); ++it)
		if(it->header.ID == menuID)
		{
			m =&(*it);
			break;
		}

	if(m == NULL)
		return false;

	if(handler != NULL)
	{
		handler->SetOpenSound(m->header.openSound);
		handler->SetMusic(m->header.music);
		handler->SetActionSound(m->header.actionSound);
		handler->SetHoverSound(m->header.hoverSound);
	}

	// Position the window to the center
	int screenWidth = GetOTE()->GetWidth();
	int screenHeight = GetOTE()->GetHeight();
	int hwidth = (int)((float)screenWidth*0.5f) , hheight = (int)((float)screenHeight*0.5f);

	// Add menu controls
	//rect<int> area(m->header.x, m->header.x + m->header.width, m->header.y, m->header.y + m->header.height);
	rect<int> area;
	rect<int> childArea;
	if(m->header.position == WINDOW_POSITION_CENTER)
	{
		area.left = hwidth - (int)((float)m->header.width * 0.5f);
		area.top = hheight - (int)((float)m->header.height * 0.5f);
		area.right = area.left + m->header.width;
		area.bottom = area.top + m->header.height;
	}
	else if(m->header.position == WINDOW_POSITION_FULLSCREEN)
	{
		area.left = 0;
		area.right = 2*hwidth;
		area.top = 0;
		area.bottom = 2*hheight;
	}
	else
	{
		// If position is MANUAL, check alignment to figure up the actual coordinates
		if(m->header.vAlign == CONTROL_VALIGN_RIGHT)
		{
			area.left = screenWidth - m->header.x - m->header.width;
		}
		else if(m->header.vAlign == CONTROL_VALIGN_CENTER)
		{
			area.left = hwidth + m->header.x;
		}
		else //if (m->header.vAlign == CONTROL_VALIGN_LEFT)
		{
			area.left = m->header.x;
		}
		area.right = area.left + m->header.width;

		if(m->header.hAlign == CONTROL_HALIGN_BOTTOM)
		{
			area.top = screenHeight - m->header.y - m->header.height;
		}
		else if(m->header.hAlign == CONTROL_HALIGN_CENTER)
		{
			area.top = hheight + m->header.y;
		}
		else //if (m->header.vAlign == CONTROL_HALIGN_TOP)
		{
			area.top = m->header.y;
		}
		area.bottom = area.top + m->header.height;
	}

	int windowWidth = area.getWidth();
	int windowHeight = area.getHeight();

	window->SetElementRect(area);
	window->SetBackgroundImage(m->header.background);
	window->SetModColor(m->header.modColor);
	//if(m->header.modColor != 0xFFFFFFFF)
	//	CPrinter::logError("SDLFSKDLFSKL");
	
	vector<SMenuElement>::iterator i = m->elements.begin();
	for(; i != m->elements.end(); ++i)
	{
		FillControlRect(childArea, *i, windowWidth, windowHeight);
		if(i->type == ELEMENT_TYPE_BUTTON)
			AddButton(window, *i, childArea, m->header.modColor);
		else if(i->type == ELEMENT_TYPE_TEXT)
			AddTextControl(window, *i, childArea, m->header.modColor);
		else if(i->type == ELEMENT_TYPE_IMAGE)
			AddImageControl(window, *i, childArea, m->header.modColor);
		else if(i->type == ELEMENT_TYPE_SCROLLBAR)
			AddScrollbar(window, *i, childArea, m->header.modColor);
		else if(i->type == ELEMENT_TYPE_CHECKBOX)
			AddCheckbox(window, *i, childArea, m->header.modColor);
	}

	return true;
}

void CMenuDatabase::FillControlRect(rect<int> &area, SMenuElement &data, int parentWidth, int parentHeight)
{
	if(data.vAlign == CONTROL_VALIGN_RIGHT)
	{
		area.left = parentWidth - data.x - data.width;
	}
	else if(data.vAlign == CONTROL_VALIGN_CENTER)
	{
		area.left = (int)(parentWidth * 0.5f) + data.x;
	}
	else //if (data.vAlign == CONTROL_VALIGN_LEFT)
	{
		area.left = data.x;
	}
	area.right = area.left + data.width;

	if(data.hAlign == CONTROL_HALIGN_BOTTOM)
	{
		area.top = parentHeight - data.y - data.height;
	}
	else if(data.hAlign == CONTROL_HALIGN_CENTER)
	{
		area.top = (int)(parentHeight * 0.5f) + data.y;
	}
	else //if (data.vAlign == CONTROL_HALIGN_TOP)
	{
		area.top = data.y;
	}
	area.bottom = area.top + data.height;
}

void CMenuDatabase::AddButton(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor)
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();

	CGUIButton *b = (CGUIButton*)env->AddButton(parent, area, data.ID, data.text);

	if(data.usesTextureAlpha)
	{
		CPrinter::log("MUSDFSKDFKSLDK");
		CResourceManager::GetManager().SetTextureColorkey(0);
	}

	CTexture *tex = CResourceManager::GetManager().GetTexture(data.background[0]);
	b->SetImage(tex);
	tex = CResourceManager::GetManager().GetTexture(data.background[1]);
	b->SetImageHover(tex);

	if(data.usesTextureAlpha)
		CResourceManager::GetManager().RestoreDefaultColorkey();

	b->SetModColor(modColor);
	//b->SetModColor(0xFFFFFFFF);
	b->SetTextColor(data.textColor);
	b->SetVisible(data.visible);
	b->SetEnabled(data.enabled);
	b->SetIsTabStop(data.tabstop);
	b->SetTabOrder(data.tabstopNum);
	b->SetTextAlign(data.textAlign);
}

void CMenuDatabase::AddTextControl(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor)
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();

	CGUILabel *b = (CGUILabel*)env->AddLabel(parent, area, data.ID, data.text);

	b->SetModColor(modColor);
	b->SetVisible(data.visible);
}

void CMenuDatabase::AddImageControl(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor)
{
	CGUIEnvironment *env = CGUIEnvironment::GetGUIEnvironment();

	if(data.usesTextureAlpha)
		CResourceManager::GetManager().SetTextureColorkey(0);

	CGUIImage *b = (CGUIImage*)env->AddImage(parent, area, data.ID, data.background[0]);

	if(data.usesTextureAlpha)
		CResourceManager::GetManager().RestoreDefaultColorkey();

	b->SetModColor(modColor);
	b->SetVisible(data.visible);
}

void CMenuDatabase::AddScrollbar(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor)
{
	// TODO:
}

void CMenuDatabase::AddCheckbox(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor)
{
	// TODO:
}