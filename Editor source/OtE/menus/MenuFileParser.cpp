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

#include "MenuFileParser.h"
#include "IMenuManager.h"
#include "MainMenu.h"
#include "CreditsMenu.h"
#include "InventoryMenu.h"
#include "TerminalView.h"
#include "DialogueWindow.h"
#include "LoadSaveMenu.h"
#include "OptionsMenu.h"
#include "ControlsMenu.h"
#include "PlayerControlsMenu.h"
#include "GeneralControlsMenu.h"
#include "GadgetHotkeysMenu.h"
#include "AssistantView.h"
#include "MouseConfigMenu.h"
#include "LevelSelectionMenu.h"
#include "CampaignSelectionMenu.h"

#include <iostream>
#include <fstream>

using namespace std;

CMenuFileParser::CMenuFileParser() : m_currentColor(NULL)
{
}

bool CMenuFileParser::ParseFile(string filename, vector<CMenuDatabase::SMenuData> &outData)
{
	char command[32], param[128];
	char line[1024];

	std::ifstream in;
	in.open(filename.c_str());
	std::string errorMsg;

	CMenuDatabase::SMenuData menu;
	SMenuElement element;
	element.FillDefaults();
	bool menuFilled = false;
	bool elementFilled = false;
	bool fillingMenu = false;
	bool fillingElement = false;

	if(!in.is_open())
	{
		//ReportError("Failed opening menu data text file");
		CPrinter::logError("Failed opening menu data text file");
		return false;
	}

	while(in.peek() != EOF)
	{
		in.getline(line, 1024, '\n');
		if(!GetParams(command, param, line))
			continue;

		if(!strcmp(command, "MENU"))
		{
			if(menuFilled)
			{
				EndColor();
				if(elementFilled)
				{
					menu.elements.push_back(element);
					element.FillDefaults();
					elementFilled = false;
					fillingElement = false;
				}
				outData.push_back(menu);
				menu.elements.clear();
				menu.header.FillDefaults();
				menuFilled = false;
				fillingMenu = false;
			}

			if((menu.header.ID = GetMenuID(param)) == -1)
			{
				CPrinter::logError("Menu data parse: unknown menu id");
				continue;
			}

			fillingElement = false;
			fillingMenu = true;
			continue;
		}
		else if(!strcmp(command, "ELEMENT"))
		{
			if(!fillingMenu)
			{
				CPrinter::logError("Menu data parse: element before menu definition");
				continue;
			}

			if(elementFilled)
			{
				EndColor();
				menu.elements.push_back(element);
				element.FillDefaults();
				elementFilled = false;
				fillingElement = false;
			}

			element.ID = GetElementID(param);
			fillingElement = true;

			continue;
		}

		if(!fillingMenu)
		{
			CPrinter::logError("Menu data parse: command before menu definition");
			continue;
		}

		if(!fillingElement)
		{
			if(!ProcessHeaderCommand(command, param, menu.header))
			{
				errorMsg = "Menu data parse: invalid menu attribute: ";
				errorMsg += command;
				CPrinter::logError(errorMsg);
			}
			else
			{
				menuFilled = true;
				elementFilled = true;
			}
		}
		else
		{
			if(!ProcessCommand(command, param, element))
				CPrinter::logError("Menu data parse: invalid element attribute");
			else
			{
				menuFilled = true;
				elementFilled = true;
			}
		}
	}
	if(elementFilled)
		menu.elements.push_back(element);
	if(menuFilled)
		outData.push_back(menu);

	in.close();

	return true;
}

bool CMenuFileParser::GetParams(char *command, char *param, char *line)
{
	if(line[0] != '.')
		return false;

	int i=1, k=0;
	command[0] = '\0';
	param[0] = '\0';
	int len = (int)strlen(line);
	bool finished = false;
	for(; i<len; ++i, ++k)
	{
		if(line[i] == ' ')
		{
			command[k] = '\0';
			finished = true;
			break;
		}
		if(!isdigit(line[i]) && !isalpha(line[i]))
			return false;
		command[k] = line[i];
	}
	if(k == 0 || !finished)
		return false;

	++i;
	finished = false;
	for(k=0; i<len; ++i, ++k)
	{
		param[k] = line[i];
	}

	if(!finished)
		param[k] = '\0';

	return true;
}

bool CMenuFileParser::ProcessHeaderCommand(char *command, char *param, SMenuHeader &e)
{
	/** 
	 * Store the value
	 **/
	if(!strcmp(command, "X"))
	{
		e.x = atoi(param);
	}
	else if(!strcmp(command, "Y"))
	{
		e.y = atoi(param);
	}
	else if(!strcmp(command, "WIDTH"))
	{
		e.width = atoi(param);
	}
	else if(!strcmp(command, "HEIGHT"))
	{
		e.height = atoi(param);
	}
	else if(!strcmp(command, "POSITION"))
	{
		if(!strcmp(param, "MANUAL"))
			e.position = WINDOW_POSITION_MANUAL;
		else if(!strcmp(param, "CENTER"))
			e.position = WINDOW_POSITION_CENTER;
		else if(!strcmp(param, "FULLSCREEN"))
			e.position = WINDOW_POSITION_FULLSCREEN;
		else
		{
			CPrinter::logError("Unknown POSITION parameter");
			return false;
		}
	}
	else if(!strcmp(command, "VALIGN"))
	{
		if(!strcmp(param, "LEFT"))
			e.vAlign = CONTROL_VALIGN_LEFT;
		else if(!strcmp(param, "CENTER"))
			e.vAlign = CONTROL_VALIGN_CENTER;
		else if(!strcmp(param, "RIGHT"))
			e.vAlign = CONTROL_VALIGN_RIGHT;
		else
			CPrinter::logError("Menu data parser: invalid menu vertical align type");
	}
	else if(!strcmp(command, "HALIGN"))
	{
		if(!strcmp(param, "TOP"))
			e.hAlign = CONTROL_HALIGN_TOP;
		else if(!strcmp(param, "CENTER"))
			e.hAlign = CONTROL_HALIGN_CENTER;
		else if(!strcmp(param, "BOTTOM"))
			e.hAlign = CONTROL_HALIGN_BOTTOM;
		else
			CPrinter::logError("Menu data parser: invalid menu horizontal align type");
	}
	else if(!strcmp(command, "BACKGROUND"))
	{
		strncpy(e.background, param, 127);
	}
	else if(!strcmp(command, "MODCOLOR"))
	{
		BeginColor(e.modColor);
	}
	else if(!strcmp(command, "COLORA"))
	{
		m_a = atoi(param);
	}
	else if(!strcmp(command, "COLORR"))
	{
		m_r = atoi(param);
	}
	else if(!strcmp(command, "COLORG"))
	{
		m_g = atoi(param);
	}
	else if(!strcmp(command, "COLORB"))
	{
		m_b = atoi(param);
	}
	else if(!strcmp(command, "OPENSOUND"))
	{
		strncpy(e.openSound, param, 255);
	}
	else if(!strcmp(command, "MUSIC"))
	{
		strncpy(e.music, param, 255);
	}
	else if(!strcmp(command, "ACTIONSOUND"))
	{
		strncpy(e.actionSound, param, 255);
	}
	else if(!strcmp(command, "HOVERSOUND"))
	{
		strncpy(e.hoverSound, param, 255);
	}
	else
		return false;

	return true;
}

bool CMenuFileParser::ProcessCommand(char *command, char *param, SMenuElement &e)
{
	bool paramBoolean = false;
	bool booleanValue = false;

	if(!strcmp(param, "TRUE"))
	{
		paramBoolean = true;
		booleanValue = true;
	}
	else if(!strcmp(param, "FALSE"))
	{
		paramBoolean = true;
		booleanValue = false;
	}

	/** 
	 * Store the value
	 **/
	if(!strcmp(command, "TYPE"))
	{
		if(!strcmp(param, "BUTTON"))
			e.type = ELEMENT_TYPE_BUTTON;
		else if(!strcmp(param, "TEXT"))
			e.type = ELEMENT_TYPE_TEXT;
		else if(!strcmp(param, "IMAGE"))
			e.type = ELEMENT_TYPE_IMAGE;
		else if(!strcmp(param, "SCROLLBAR"))
			e.type = ELEMENT_TYPE_SCROLLBAR;
		else if(!strcmp(param, "CHECKBOX"))
			e.type = ELEMENT_TYPE_CHECKBOX;
		else
			CPrinter::logError("Menu data parser: invalid element type");
	}
	else if(!strcmp(command, "VALIGN"))
	{
		if(!strcmp(param, "LEFT"))
			e.vAlign = CONTROL_VALIGN_LEFT;
		else if(!strcmp(param, "CENTER"))
			e.vAlign = CONTROL_VALIGN_CENTER;
		else if(!strcmp(param, "RIGHT"))
			e.vAlign = CONTROL_VALIGN_RIGHT;
		else
			CPrinter::logError("Menu data parser: invalid element vertical align type");
	}
	else if(!strcmp(command, "HALIGN"))
	{
		if(!strcmp(param, "TOP"))
			e.hAlign = CONTROL_HALIGN_TOP;
		else if(!strcmp(param, "CENTER"))
			e.hAlign = CONTROL_HALIGN_CENTER;
		else if(!strcmp(param, "BOTTOM"))
			e.hAlign = CONTROL_HALIGN_BOTTOM;
		else
			CPrinter::logError("Menu data parser: invalid element horizontal align type");
	}
	else if(!strcmp(command, "X"))
	{
		e.x = atoi(param);
	}
	else if(!strcmp(command, "Y"))
	{
		e.y = atoi(param);
	}
	else if(!strcmp(command, "WIDTH"))
	{
		e.width = atoi(param);
	}
	else if(!strcmp(command, "HEIGHT"))
	{
		e.height = atoi(param);
	}
	else if(!strcmp(command, "TABORDER"))
	{
		e.tabstopNum = atoi(param);
	}
	else if(!strcmp(command, "BACKGROUND1"))
	{
		strncpy(e.background[0], param, 127);
	}
	else if(!strcmp(command, "BACKGROUND2"))
	{
		strncpy(e.background[1], param, 127);
	}
	else if(!strcmp(command, "BACKGROUND3"))
	{
		strncpy(e.background[2], param, 127);
	}
	else if(!strcmp(command, "BACKGROUND4"))
	{
		strncpy(e.background[3], param, 127);
	}
	else if(!strcmp(command, "USETEXTUREALPHA"))
	{
		if(paramBoolean)
			e.usesTextureAlpha = booleanValue;
		else
			CPrinter::logError("Menu data parser: nonboolean value given to USETEXTUREALPHA attribute");
	}
	else if(!strcmp(command, "TEXTALIGN"))
	{
		if(!strcmp(param, "CENTER"))
			e.textAlign = DT_CENTER | DT_VCENTER;
		else if(!strcmp(param, "RIGHT"))
			e.textAlign = DT_RIGHT | DT_VCENTER;
		else if(!strcmp(param, "LEFT"))
			e.textAlign = DT_LEFT | DT_VCENTER;
		else if(!strcmp(param, "LEFTWORDBREAK"))
			e.textAlign = DT_LEFT | DT_WORDBREAK;
		else if(!strcmp(param, "CENTERWORDBREAK"))
			e.textAlign = DT_CENTER | DT_WORDBREAK;
	}
	else if(!strcmp(command, "TEXT"))
	{
		strncpy(e.text, param, 127);
	}
	else if(!strcmp(command, "FONT"))
	{
		strncpy(e.font, param, 63);
	}
	else if(!strcmp(command, "ENABLED"))
	{
		if(paramBoolean)
			e.enabled = booleanValue;
		else
			CPrinter::logError("Menu data parse: ENABLED has non-boolean value");
	}
	else if(!strcmp(command, "VISIBLE"))
	{
		if(paramBoolean)
			e.visible = booleanValue;
		else
			CPrinter::logError("Menu data parse: VISIBLE has non-boolean value");
	}
	else if(!strcmp(command, "TABSTOP"))
	{
		if(paramBoolean)
			e.tabstop = booleanValue;
		else
			CPrinter::logError("Menu data parse: TABSTOP has non-boolean value");
	}
	else if(!strcmp(command, "TEXTCOLOR"))
	{
		BeginColor(e.textColor);
	}
	else if(!strcmp(command, "COLORA"))
	{
		m_a = atoi(param);
	}
	else if(!strcmp(command, "COLORR"))
	{
		m_r = atoi(param);
	}
	else if(!strcmp(command, "COLORG"))
	{
		m_g = atoi(param);
	}
	else if(!strcmp(command, "COLORB"))
	{
		m_b = atoi(param);
	}
	else if(!strcmp(command, "MINVALUE"))
	{
		e.minValue = (float)atof(param);
	}
	else if(!strcmp(command, "MAXVALUE"))
	{
		e.maxValue = (float)atof(param);
	}
	else
		return false;

	return true;
}

void CMenuFileParser::EndColor()
{
	bool colorFull = true;
	if(m_currentColor)
	{
		if(m_a < 0 || m_a > 255)
		{
			colorFull = false;
			m_a = 255;
		}
		if(m_r < 0 || m_r > 255)
		{
			colorFull = false;
			m_r = 255;
		}
		if(m_g < 0 || m_g > 255)
		{
			colorFull = false;
			m_g = 255;
		}
		if(m_b < 0 || m_b > 255)
		{
			colorFull = false;
			m_b = 255;
		}

		*m_currentColor = D3DCOLOR_RGBA(m_r, m_g, m_b, m_a);
		m_currentColor = NULL;

		if(!colorFull)
			CPrinter::logError("Menu data parse: not all color components were given or they were incorrect");
	}
}

void CMenuFileParser::BeginColor(D3DCOLOR &color)
{
	EndColor();

	m_a = m_b = m_r = m_g = -1;
	m_currentColor = &color;
}

int CMenuFileParser::GetMenuID(char *name)
{
	if(!strcmp(name, "MAIN_MENU"))
		return MENU_ID_MAIN;
	else if(!strcmp(name, "OPTIONS"))
		return MENU_ID_OPTIONS;
	else if(!strcmp(name, "CONTROLS"))
		return MENU_ID_CONTROLS;
	else if(!strcmp(name, "PLAYER_CONTROLS"))
		return MENU_ID_PLAYER_CONTROLS;
	else if(!strcmp(name, "GENERAL_CONTROLS"))
		return MENU_ID_GENERAL_CONTROLS;
	else if(!strcmp(name, "LOADSAVE"))
		return MENU_ID_LOADSAVE;
	else if(!strcmp(name, "INVENTORY"))
		return MENU_ID_INVENTORY;
	else if(!strcmp(name, "CREDITS"))
		return MENU_ID_CREDITS;
	else if(!strcmp(name, "INVENTORY"))
		return MENU_ID_INVENTORY;
	else if(!strcmp(name, "INVENTORYBOX1"))
		return MENU_ID_INVENTORY_BOX1;
	else if(!strcmp(name, "INVENTORYBOX2"))
		return MENU_ID_INVENTORY_BOX2;
	else if(!strcmp(name, "INVENTORYBOX3"))
		return MENU_ID_INVENTORY_BOX3;
	else if(!strcmp(name, "INVENTORYBOX4"))
		return MENU_ID_INVENTORY_BOX4;
	else if(!strcmp(name, "INVENTORYBOX5"))
		return MENU_ID_INVENTORY_BOX5;
	else if(!strcmp(name, "TERMINAL"))
		return MENU_ID_TERMINAL;
	else if(!strcmp(name, "DIALOGUE"))
		return MENU_ID_DIALOGUE;
	else if(!strcmp(name, "LOGO"))
		return MENU_ID_LOGO;
	else if(!strcmp(name, "GADGET_HOTKEYS"))
		return MEDU_ID_GADGET_HOTKEYS;
	else if(!strcmp(name, "ASSISTANT"))
		return MENU_ID_ASSISTANT_VIEW;
	else if(!strcmp(name, "MOUSE_CONFIGURATION"))
		return MENU_ID_MOUSE_CONFIGURATION;
	else if(!strcmp(name, "LEVEL_SELECTION"))
		return MENU_ID_LEVEL_SELECTION;
	else if(!strcmp(name, "CAMPAIGN_SELECTION"))
		return MENU_ID_CAMPAIGN_SELECTION;
	else
	{
		std::string str = "Menu data parser: menu id not found: ";
		str += name;
		CPrinter::logError(str);
	}

	return -1;
}

int CMenuFileParser::GetElementID(char *name)
{
	if(!strcmp(name, "0"))
		return -1;
	// CREDITS MENU
	else if(!strcmp(name, "CREDITS_CLOSE"))
		return CRE_CLOSE;

	// LOAD/SAVE MENU
	else if(!strcmp(name, "LSE_CLOSE"))
		return LSE_CLOSE;
	else if(!strcmp(name, "LSE_HEADER"))
		return LSE_HEADER;
	else if(!strcmp(name, "LSE_SAVE1"))
		return LSE_SAVE1;
	else if(!strcmp(name, "LSE_SAVE2"))
		return LSE_SAVE2;
	else if(!strcmp(name, "LSE_SAVE3"))
		return LSE_SAVE3;
	else if(!strcmp(name, "LSE_SAVE4"))
		return LSE_SAVE4;
	else if(!strcmp(name, "LSE_SAVE5"))
		return LSE_SAVE5;
	else if(!strcmp(name, "LSE_AUTOSAVE"))
		return LSE_AUTOSAVE;
	else if(!strcmp(name, "LSE_CAMPAIGN"))
		return LSE_CAMPAIGN;

	// MAIN MENU
	else if(!strcmp(name, "MME_SINGLE"))
		return MME_SINGLE;
	else if(!strcmp(name, "MME_MULTI"))
		return MME_MULTI;
	else if(!strcmp(name, "MME_LOAD"))
		return MME_LOAD;
	else if(!strcmp(name, "MME_SAVE"))
		return MME_SAVE;
	else if(!strcmp(name, "MME_CREDITS"))
		return MME_CREDITS;
	else if(!strcmp(name, "MME_OPTIONS"))
		return MME_OPTIONS;
	else if(!strcmp(name, "MME_EXIT"))
		return MME_EXIT;

	// OPTIONS
	else if(!strcmp(name, "OPT_CLOSE"))
		return OPT_CLOSE;
	else if(!strcmp(name, "OPT_CONTROLS"))
		return OPT_CONTROLS;
	else if(!strcmp(name, "OPT_ENEMY_EMOTES"))
		return OPT_ENEMY_EMOTES;
	else if(!strcmp(name, "OPT_MUSIC"))
		return OPT_MUSIC;
	else if(!strcmp(name, "OPT_SOUND"))
		return OPT_SOUND;
	else if(!strcmp(name, "OPT_TUTORIALS"))
		return OPT_TUTORIALS;

	// CONTROLS
	else if(!strcmp(name, "CTR_CLOSE"))
		return CTR_CLOSE;
	else if(!strcmp(name, "CTR_PLAYER1"))
		return CTR_PLAYER1;
	else if(!strcmp(name, "CTR_PLAYER2"))
		return CTR_PLAYER2;
	else if(!strcmp(name, "CTR_MOUSE"))
		return CTR_MOUSE;
	else if(!strcmp(name, "CTR_GENERAL_KEYBOARD"))
		return CTR_GENERAL_KEYBOARD;
	else if(!strcmp(name, "CTR_GENERAL_PAD"))
		return CTR_GENERAL_PAD;
	else if(!strcmp(name, "CTR_GADGET_HOTKEYS"))
		return CTR_GADGET_HOTKEYS;

	// PLAYER CONTROLS
	else if(!strcmp(name, "PCE_CLOSE"))
		return PCE_CLOSE;
	else if(!strcmp(name, "PCE_HEADER"))
		return PCE_HEADER;
	else if(!strcmp(name, "PCE_USE_KEYBOARD"))
		return PCE_USE_KEYBOARD;
	else if(!strcmp(name, "PCE_ACCELERATE"))
		return PCE_ACCELERATE;
	else if(!strcmp(name, "PCE_BRAKE"))
		return PCE_BRAKE;
	else if(!strcmp(name, "PCE_TURN_LEFT"))
		return PCE_TURN_LEFT;
	else if(!strcmp(name, "PCE_TURN_RIGHT"))
		return PCE_TURN_RIGHT;
	else if(!strcmp(name, "PCE_THRUST_LEFT"))
		return PCE_THRUST_LEFT;
	else if(!strcmp(name, "PCE_THRUST_RIGHT"))
		return PCE_THRUST_RIGHT;
	else if(!strcmp(name, "PCE_GADGET1"))
		return PCE_GADGET1;
	else if(!strcmp(name, "PCE_GADGET2"))
		return PCE_GADGET2;
	else if(!strcmp(name, "PCE_BOOST"))
		return PCE_BOOST;

	// MOUSE CONFIGURATION
	else if(!strcmp(name, "MC_CLOSE"))
		return MC_CLOSE;
	else if(!strcmp(name, "MC_ENABLED"))
		return MC_ENABLED;
	else if(!strcmp(name, "MC_SENSITIVITY"))
		return MC_SENSITIVITY;

	// GENERAL CONTROLS
	else if(!strcmp(name, "GCE_CLOSE"))
		return GCE_CLOSE;
	else if(!strcmp(name, "GCE_HEADER"))
		return GCE_HEADER;
	else if(!strcmp(name, "GCE_MENU_BACK"))
		return GCE_MENU_BACK;
	else if(!strcmp(name, "GCE_GENERAL_OK"))
		return GCE_GENERAL_OK;
	else if(!strcmp(name, "GCE_OPEN_INVENTORY"))
		return GCE_OPEN_INVENTORY;
	else if(!strcmp(name, "GCE_PLAYER_TERMINAL"))
		return GCE_PLAYER_TERMINAL;
	else if(!strcmp(name, "GCE_TAKE_SCREENSHOT"))
		return GCE_TAKE_SCREENSHOT;
	else if(!strcmp(name, "GCE_QUICKLOAD"))
		return GCE_QUICKLOAD;
	else if(!strcmp(name, "GCE_QUICKSAVE"))
		return GCE_QUICKSAVE;
	else if(!strcmp(name, "GCE_DISPLAY_FPS"))
		return GCE_DISPLAY_FPS;

	// GADGET HOTLEYS
	else if(!strcmp(name, "GH_CLOSE"))
		return GH_CLOSE;
	else if(!strcmp(name, "GH_HOTKEY1"))
		return GH_HOTKEY1;
	else if(!strcmp(name, "GH_HOTKEY2"))
		return GH_HOTKEY2;
	else if(!strcmp(name, "GH_HOTKEY3"))
		return GH_HOTKEY3;
	else if(!strcmp(name, "GH_HOTKEY4"))
		return GH_HOTKEY4;
	else if(!strcmp(name, "GH_HOTKEY5"))
		return GH_HOTKEY5;

	// INVENTORY
	else if(!strcmp(name, "INV_DETAILIMG"))
		return INV_DETAILIMG;
	else if(!strcmp(name, "INV_GADGETNAME"))
		return INV_GADGETNAME;
	else if(!strcmp(name, "INV_GADGETDETAILS"))
		return INV_GADGETDETAILS;
	else if(!strcmp(name, "INV_GADGET1"))
		return INV_GADGET1;
	else if(!strcmp(name, "INV_GADGET2"))
		return INV_GADGET2;
	else if(!strcmp(name, "INV_SELECTION"))
		return INV_SELECTION;
	else if(!strcmp(name, "INV_EXIT"))
		return INV_EXIT;

	// TERMINAL
	else if(!strcmp(name, "TV_BUTTON1"))
		return TV_BUTTON1;
	else if(!strcmp(name, "TV_BUTTON2"))
		return TV_BUTTON2;
	else if(!strcmp(name, "TV_BUTTON3"))
		return TV_BUTTON3;
	else if(!strcmp(name, "TV_BUTTON4"))
		return TV_BUTTON4;
	else if(!strcmp(name, "TV_BUTTON5"))
		return TV_BUTTON5;
	else if(!strcmp(name, "TV_BUTTON6"))
		return TV_BUTTON6;
	else if(!strcmp(name, "TV_CLOSE"))
		return TV_CLOSE;
	else if(!strcmp(name, "TV_NAME"))
		return TV_NAME;
	else if(!strcmp(name, "TV_MESSAGE"))
		return TV_MESSAGE;

	// DIALOGUE
	else if(!strcmp(name, "DW_TEXT"))
		return DW_TEXT;
	else if(!strcmp(name, "DW_SPEAKER"))
		return DW_SPEAKER;
	else if(!strcmp(name, "DW_SKIP"))
		return DW_SKIP;
	else if(!strcmp(name, "DW_NEXT"))
		return DW_NEXT;

	// ASSISTANT
	else if(!strcmp(name, "AW_TEXT"))
		return AW_TEXT;
	else if(!strcmp(name, "AW_HEADER"))
		return AW_HEADER;

	// CAMPAIGN SELECTION
	else if(!strcmp(name, "CS_CLOSE"))
		return CS_CLOSE;
	else if(!strcmp(name, "CS_CAMPAIGN1"))
		return CS_CAMPAIGN1;
	else if(!strcmp(name, "CS_CAMPAIGN2"))
		return CS_CAMPAIGN2;
	else if(!strcmp(name, "CS_CAMPAIGN3"))
		return CS_CAMPAIGN3;
	else if(!strcmp(name, "CS_CAMPAIGN4"))
		return CS_CAMPAIGN4;
	else if(!strcmp(name, "CS_CAMPAIGN5"))
		return CS_CAMPAIGN5;
	else if(!strcmp(name, "CS_NEXT"))
		return CS_NEXT;
	else if(!strcmp(name, "CS_PREV"))
		return CS_PREV;

	// LEVEL SELECTION
	else if(!strcmp(name, "LS_CLOSE"))
		return LS_CLOSE;
	else if(!strcmp(name, "LS_LEVEL1"))
		return LS_LEVEL1;
	else if(!strcmp(name, "LS_LEVEL2"))
		return LS_LEVEL2;
	else if(!strcmp(name, "LS_LEVEL3"))
		return LS_LEVEL3;
	else if(!strcmp(name, "LS_LEVEL4"))
		return LS_LEVEL4;
	else if(!strcmp(name, "LS_LEVEL5"))
		return LS_LEVEL5;
	else if(!strcmp(name, "LS_NEXT"))
		return LS_NEXT;
	else if(!strcmp(name, "LS_PREV"))
		return LS_PREV;

	else
	{
		std::string str = "Menu data parser: element id not found: ";
		str += name;
		CPrinter::logError(str);
	}

	return -1;
}