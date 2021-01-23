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

#include "GameSettings.h"
#include "Edgewalker\\Edgewalker.h"
#include "Edgewalker\\IniFile.h"

#include <sstream>

CGameSettings *CGameSettings::m_this = NULL;
std::vector<std::pair<BYTE, std::string> > CGameSettings::m_keymap;
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameSettings::CGameSettings() : m_pad(NULL), m_keyboard(NULL)
{
	int i;
	for(i=0;i<GO_B_COUNT;++i)
		m_bOptions[i] = FALSE;
	for(i=0;i<GO_INT_COUNT;++i)
		m_iOptions[i] = 0;
	for(i=0;i<GO_F_COUNT;++i)
		m_fOptions[i] = 0.f;

	// Custom default settings
	m_bOptions[GO_FULLSCREEN] = true;
	m_bOptions[GO_STATIC_SHADOWS_ENABLED] = true;
	m_bOptions[GO_MOUSE_ENABLED] = true;
	m_bOptions[GO_FPS_LIMIT_ENABLED] = true;
	m_bOptions[GO_ANIME_SMILEYS_ENABLED] = true;
	m_iOptions[GO_INT_SCREEN_WIDTH] = 800;
	m_iOptions[GO_INT_SCREEN_HEIGHT] = 600;

	m_fOptions[GO_SOUND_VOLUME] = 1.f;
	m_fOptions[GO_MUSIC_VOLUME] = 0.5f;
	m_fOptions[GO_MOUSE_SENSITIVITY] = 1.f;

	m_gadgetHotkeys[0] = DIK_1;
	m_gadgetHotkeys[1] = DIK_2;
	m_gadgetHotkeys[2] = DIK_3;
	m_gadgetHotkeys[3] = DIK_4;
	m_gadgetHotkeys[4] = DIK_5;

	m_playerCameraSettings.FillDefaults();

	// TODO: change default value
	m_defaultCampaign = "campaigns\\aIntro Easy.cpg";

	m_controlSets[0].FillDefaulKeyboard1();
	m_controlSets[1].FillDefaultKeyboard2();

	m_generalKeyboard.FillDefaultKeyboard();
	m_generalPad.FillDefaultPad();

	m_this = this;

	FillKeymap();
}

CGameSettings::~CGameSettings()
{
	m_this = NULL;
}

void CGameSettings::LoadDefaultSettings()
{
}

void CGameSettings::FillKeymap()
{
	if(!m_keymap.empty())
		return;

	m_keymap.push_back(pair<BYTE, string>	(255, "--")	);

	m_keymap.push_back(pair<BYTE, string>	(DIK_A, "A")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_B, "B")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_C, "C")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_D, "D")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_E, "E")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F, "F")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_G, "G")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_H, "H")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_I, "I")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_J, "J")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_K, "K")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_L, "L")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_M, "M")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_N, "N")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_O, "O")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_P, "P")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_Q, "Q")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_R, "R")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_S, "S")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_T, "T")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_U, "U")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_V, "V")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_W, "W")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_X, "X")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_Y, "Y")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_Z, "Z")	);

	m_keymap.push_back(pair<BYTE, string>	(DIK_1, "'1'")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_2, "'2'")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_3, "'3'")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_4, "'4'")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_5, "'5'")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_6, "'6'")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_7, "'7'")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_8, "'8'")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_9, "'9'")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_0, "'0'")	);

	m_keymap.push_back(pair<BYTE, string>	(DIK_F1, "F1")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F2, "F2")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F3, "F3")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F4, "F4")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F5, "F5")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F6, "F6")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F7, "F7")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F8, "F8")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F9, "F9")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F10, "F10")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F11, "F11")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_F12, "F12")	);

	m_keymap.push_back(pair<BYTE, string>	(DIK_SPACE, "Space")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_RETURN, "Enter")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_COLON, "Colon")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_ADD, "Add")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_BACK, "Back")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_COMMA, "Comma")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_ESCAPE, "Esc")	);

	m_keymap.push_back(pair<BYTE, string>	(DIK_DOWN,		"Down")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_UP,		"Up")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_RIGHT,		"Right")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_LEFT,		"Left")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_RSHIFT, "RShift")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_LSHIFT, "LShift")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_LCONTROL, "LControl")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_RCONTROL, "RControl")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_TAB, "Tab")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_CAPSLOCK, "Capslock")	);

	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD0, "Numpad0")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD1, "Numpad1")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD2, "Numpad2")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD3, "Numpad3")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD4, "Numpad4")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD5, "Numpad5")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD6, "Numpad6")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD7, "Numpad7")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD8, "Numpad8")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPAD9, "Numpad9")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPADENTER, "NumpadEnter")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPADCOMMA, "NumpadComma")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPADEQUALS, "NumpadEquals")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPADMINUS, "NumpadMinus")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPADPLUS, "NumpadPlus")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPADSTAR, "NumpadStar")	);
	m_keymap.push_back(pair<BYTE, string>	(DIK_NUMPADSLASH, "NumpadSlash")	);
}

BYTE CGameSettings::GetKeyCode(std::string key)
{
	vector<std::pair<BYTE, std::string> >::iterator it = m_keymap.begin();
	for(; it != m_keymap.end(); ++it)
		if(it->second == key)
			return it->first;
	return 0;
}

std::string CGameSettings::GetKeyString(BYTE code)
{
	vector<std::pair<BYTE, std::string> >::iterator it = m_keymap.begin();
	for(; it != m_keymap.end(); ++it)
		if(it->first == code)
			return it->second;
	return "";
}

bool CGameSettings::IsMenuBackDown()
{
	if(m_keyboard)
		if(m_keyboard->GetButtonDown(m_generalKeyboard.menuBack))
			return true;
	if(m_pad)
		if(m_pad->GetButtonDown(m_generalPad.menuBack))
			return true;
	return false;
}

bool CGameSettings::IsGeneralOkDown()
{
	if(m_keyboard)
		if(m_keyboard->GetButtonDown(m_generalKeyboard.generalOk))
			return true;
	if(m_pad)
		if(m_pad->GetButtonDown(m_generalPad.generalOk))
			return true;
	return false;
}

bool CGameSettings::IsOpenInventoryDown()
{
	if(m_keyboard)
		if(m_keyboard->GetButtonDown(m_generalKeyboard.openInventory))
			return true;
	if(m_pad)
		if(m_pad->GetButtonDown(m_generalPad.openInventory))
			return true;
	return false;
}

bool CGameSettings::IsOpenPlayerTerminalDown()
{
	if(m_keyboard)
		if(m_keyboard->GetButtonDown(m_generalKeyboard.openPlayerTerminal))
			return true;
	if(m_pad)
		if(m_pad->GetButtonDown(m_generalPad.openPlayerTerminal))
			return true;
	return false;
}

bool CGameSettings::IsNavigateRightDown()
{
	if(m_keyboard)
	{
		if(m_controlSets[0].keyboard)
			if(m_keyboard->GetButtonDown(m_controlSets[0].right))
				return true;
		if(m_keyboard->GetButtonDown(DIK_RIGHT))
			return true;
	}
	if(m_pad)
		if(m_pad->GetState()->lX > 0)
			return true;
	return false;
}

bool CGameSettings::IsNavigateLeftDown()
{
	if(m_keyboard)
	{
		if(m_controlSets[0].keyboard)
			if(m_keyboard->GetButtonDown(m_controlSets[0].left))
				return true;
		if(m_keyboard->GetButtonDown(DIK_LEFT))
			return true;
	}
	if(m_pad)
		if(m_pad->GetState()->lX < 0)
			return true;
	return false;
}

bool CGameSettings::IsNavigateUpDown()
{
	if(m_keyboard)
	{
		if(m_controlSets[0].keyboard)
			if(m_keyboard->GetButtonDown(m_controlSets[0].forward))
				return true;
		if(m_keyboard->GetButtonDown(DIK_UP))
			return true;
	}
	if(m_pad)
		if(m_pad->GetState()->lY < 0)
			return true;
	return false;
}

bool CGameSettings::IsNavigateDownDown()
{
	if(m_keyboard)
	{
		if(m_controlSets[0].keyboard)
			if(m_keyboard->GetButtonDown(m_controlSets[0].back))
				return true;
		if(m_keyboard->GetButtonDown(DIK_DOWN))
			return true;
	}
	if(m_pad)
		if(m_pad->GetState()->lY > 0)
			return true;
	return false;
}

bool CGameSettings::WriteSettingsToIni(const std::string filename)
{
	CIniFile file;

	string section("General");
	file.WriteBoolValue(section, "fullscreen", m_bOptions[GO_FULLSCREEN]);
	file.WriteBoolValue(section, "staticShadowsEnabled", m_bOptions[GO_STATIC_SHADOWS_ENABLED]);
	file.WriteBoolValue(section, "tutorialsEnabled", m_bOptions[GO_TUTORIALS_ENABLED]);
	file.WriteBoolValue(section, "enemyEmotes", m_bOptions[GO_ANIME_SMILEYS_ENABLED]);
	file.WriteIntValue(section, "screenWidth", m_iOptions[GO_INT_SCREEN_WIDTH]);
	file.WriteIntValue(section, "screenHeight", m_iOptions[GO_INT_SCREEN_HEIGHT]);
	file.WriteDoubleValue(section, "musicVolume", m_fOptions[GO_MUSIC_VOLUME]);
	file.WriteDoubleValue(section, "effectVolume", m_fOptions[GO_SOUND_VOLUME]);
	file.WriteStringValue(section, "defaultCampaign", m_defaultCampaign);
	file.WriteBoolValue(section, "fpsLimitEnabled", m_bOptions[GO_FPS_LIMIT_ENABLED]);

	section = "Player 1 controls";

	for(int i=0; i<2; ++i)
	{
		if(i==1)
			section = "Player 2 controls";

		SPlayerControlSet set = GetControlSet(i);

		file.WriteBoolValue(section, "useKeyboard", set.keyboard);
		file.WriteStringValue(section, "accelerate", GetSymbolOrCodeEx(set.forward, set.keyboard));
		file.WriteStringValue(section, "brake", GetSymbolOrCodeEx(set.back, set.keyboard));
		file.WriteStringValue(section, "turnLeft", GetSymbolOrCodeEx(set.left, set.keyboard));
		file.WriteStringValue(section, "turnRight", GetSymbolOrCodeEx(set.right, set.keyboard));
		file.WriteStringValue(section, "thrustLeft", GetSymbolOrCodeEx(set.thrustLeft, set.keyboard));
		file.WriteStringValue(section, "thrustRight", GetSymbolOrCodeEx(set.thrustRight, set.keyboard));
		file.WriteStringValue(section, "gadget1", GetSymbolOrCodeEx(set.gadget1, set.keyboard));
		file.WriteStringValue(section, "gadget2", GetSymbolOrCodeEx(set.gadget2, set.keyboard));
		file.WriteStringValue(section, "boost", GetSymbolOrCodeEx(set.boost, set.keyboard));
	}

	section = "General keyboard controls";
	for(int i=0; i<2; ++i)
	{
		SGeneralControlSet set;
		if(i==1)
		{
			set = GetGeneralPadControlSet();
			section = "General pad controls";
		}
		else
			set = GetGeneralKeyboardControlSet();

		file.WriteStringValue(section, "menuBack", GetSymbolOrCodeEx(set.menuBack, !(bool)i));
		file.WriteStringValue(section, "generalOk", GetSymbolOrCodeEx(set.generalOk, !(bool)i));
		file.WriteStringValue(section, "openInventory", GetSymbolOrCodeEx(set.openInventory, !(bool)i));
		file.WriteStringValue(section, "openPlayerTerminal", GetSymbolOrCodeEx(set.openPlayerTerminal, !(bool)i));
		file.WriteStringValue(section, "takeScreenshot", GetSymbolOrCodeEx(set.takeScreenshot, !(bool)i));
		file.WriteStringValue(section, "quickLoad", GetSymbolOrCodeEx(set.quickLoad, !(bool)i));
		file.WriteStringValue(section, "quickSave", GetSymbolOrCodeEx(set.quickSave, !(bool)i));
		file.WriteStringValue(section, "displayFPS", GetSymbolOrCodeEx(set.displayFPS, !(bool)i));
	}

	section = "Mouse controls";
	file.WriteBoolValue(section, "mouseEnabled", m_bOptions[GO_MOUSE_ENABLED]);
	file.WriteDoubleValue(section, "sensitivity", m_fOptions[GO_MOUSE_SENSITIVITY]);

	section = "Gadget hotkeys";
	file.WriteStringValue(section, "gadget1", GetSymbolOrCodeEx(m_gadgetHotkeys[0], true));
	file.WriteStringValue(section, "gadget2", GetSymbolOrCodeEx(m_gadgetHotkeys[1], true));
	file.WriteStringValue(section, "gadget3", GetSymbolOrCodeEx(m_gadgetHotkeys[2], true));
	file.WriteStringValue(section, "gadget4", GetSymbolOrCodeEx(m_gadgetHotkeys[3], true));
	file.WriteStringValue(section, "gadget5", GetSymbolOrCodeEx(m_gadgetHotkeys[4], true));

	// Don't let users to edit this unless they know exactly what they're doing
	section = "Player camera";
	file.WriteBoolValue(section, "useFollowCam", m_playerCameraSettings.bUseFollowCam);
	file.WriteDoubleValue(section, "distance", m_playerCameraSettings.distance);
	file.WriteDoubleValue(section, "smoothingFactor", m_playerCameraSettings.smoothingFactor);
	file.WriteDoubleValue(section, "angle", m_playerCameraSettings.angle);
	file.WriteDoubleValue(section, "farClipPlane", m_playerCameraSettings.farClipPlane);
	file.WriteDoubleValue(section, "nearClipPlane", m_playerCameraSettings.nearClipPlane);

	if(file.WriteFile(filename))
	{
		CPrinter::log("Settings succesfully written to settings.ini");
		return true;
	}
	else
	{
		CPrinter::logError("Failed writing settings to settings.ini");
		return false;
	}
}

std::string CGameSettings::GetControlSymbolOrCode(BYTE code)
{
	string s(GetKeyString(code));
	if(s.empty())
	{
		stringstream ss;
		ss << (int)code;
		return ss.str();
	}
	return s;
}

std::string CGameSettings::GetSymbolOrCodeEx(BYTE code, bool bKeyboard)
{
	if(bKeyboard)
		return GetControlSymbolOrCode(code);
	stringstream ss;
	ss << (int)code;
	return ss.str();
}

void CGameSettings::FillDefaultKeyboardGadgetHotkeys()
{
	m_gadgetHotkeys[0] = DIK_6;
	m_gadgetHotkeys[1] = DIK_7;
	m_gadgetHotkeys[2] = DIK_8;
	m_gadgetHotkeys[3] = DIK_9;
	m_gadgetHotkeys[4] = DIK_0;
}

void CGameSettings::FillDefaultKeybMouseGadgetHotkeys()
{
	m_gadgetHotkeys[0] = DIK_Q;
	m_gadgetHotkeys[1] = DIK_E;
	m_gadgetHotkeys[2] = DIK_1;
	m_gadgetHotkeys[3] = DIK_3;
	m_gadgetHotkeys[4] = DIK_4;
}