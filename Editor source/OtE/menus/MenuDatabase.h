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

#ifndef __CMENUDATABASE_H__
#define __CMENUDATABASE_H__

#include "IMenuManager.h"

#define MENU_DATABASE_BEGIN_IDENTIFIER	0xFB361FAC

enum MENU_ELEMENT_TYPE {
	ELEMENT_TYPE_BUTTON,
	ELEMENT_TYPE_TEXT,
	ELEMENT_TYPE_IMAGE,
	ELEMENT_TYPE_SCROLLBAR,
	ELEMENT_TYPE_CHECKBOX,
	ELEMENT_TYPE_FORCEDWORD = 0xFFFFFFFF
};

enum CONTROL_VALIGN {
	CONTROL_VALIGN_LEFT = 0,
	CONTROL_VALIGN_CENTER,
	CONTROL_VALIGN_RIGHT = 0xFFFFFFFF
};

enum CONTROL_HALIGN {
	CONTROL_HALIGN_TOP = 0,
	CONTROL_HALIGN_CENTER,
	CONTROL_HALIGN_BOTTOM = 0xFFFFFFFF
};

enum WINDOW_POSITION {
	WINDOW_POSITION_MANUAL = 0,
	WINDOW_POSITION_CENTER,
	WINDOW_POSITION_FULLSCREEN = 0xFFFFFFFF
};

/**
 * Contains the essential information of a single
 * menu window instance
 */
struct SMenuHeader
{
	int ID;
	bool useTextureAlpha;
	WINDOW_POSITION position;
	int x, y, width, height;
	char background[128];
	int elementCount;
	D3DCOLOR modColor;
	CONTROL_VALIGN vAlign;
	CONTROL_HALIGN hAlign;
	char openSound[256];
	char music[256];
	char actionSound[256];
	char hoverSound[256];

	void FillDefaults()
	{
		ID = -1;
		x = y = 0;
		useTextureAlpha = false;
		position = WINDOW_POSITION_MANUAL;
		modColor = D3DCOLOR_RGBA(255, 255, 255, 255);
		width = height = 10;
		background[0] = '\0';
		elementCount = 0;
		vAlign = CONTROL_VALIGN_LEFT;
		hAlign = CONTROL_HALIGN_TOP;

		openSound[0] = '\0';
		music[0] = '\0';
		actionSound[0] = '\0';
		hoverSound[0] = '\0';
	}
};

/**
 * Contains the essential information of a single
 * menu element instance
 */
struct SMenuElement
{
	int ID;
	MENU_ELEMENT_TYPE type;
	int x,y,width,height;
	int tabstopNum;
	char background[4][128];
	char text[128];
	bool enabled, visible, tabstop, usesTextureAlpha;
	D3DCOLOR textColor;
	char font[64];
	float minValue, maxValue;
	DWORD textAlign;
	CONTROL_VALIGN vAlign;
	CONTROL_HALIGN hAlign;

	void FillDefaults()
	{
		ID = -1;
		type = ELEMENT_TYPE_IMAGE;
		x = y = 0;
		tabstopNum = 0;
		width = height = 10;
		for(int i=0;i<4;++i)
			background[i][0] = '\0';
		text[0] = '\0';
		enabled = visible = true;
		tabstop = false;
		usesTextureAlpha = false;
		textColor = textColor = D3DCOLOR_RGBA(255, 255, 255, 255);
		font[0] = '\0';
		minValue = 0;
		maxValue = 100;
		textAlign = DT_CENTER | DT_VCENTER;
		vAlign = CONTROL_VALIGN_LEFT;
		hAlign = CONTROL_HALIGN_TOP;
	}
};

/**
 * Stores the templates of game menus
 */
class CMenuDatabase
{
public:
	struct SMenuData
	{
		std::vector<SMenuElement> elements;
		SMenuHeader header;
	};

	bool LoadDatabase(std::string filename);
	bool SaveDatabase(std::string filename);

	bool ApplyMenuData(CGUIWindow *window, int menuID, IMenuHandler *handler);

	std::vector<SMenuData> &GetMenuData() {return m_menus;}

private:
	void AddButton(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor);
	void AddTextControl(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor);
	void AddImageControl(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor);
	void AddScrollbar(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor);
	void AddCheckbox(CGUIWindow *parent, SMenuElement &data, rect<int> &area, D3DCOLOR modColor);

	void FillControlRect(rect<int> &area, SMenuElement &data, int parentWidth, int parentHeight);

	std::vector<SMenuData> m_menus;
};

#endif __CMENUDATABASE_H__