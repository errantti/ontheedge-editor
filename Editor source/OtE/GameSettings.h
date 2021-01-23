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

#ifndef __CGAMESETTINGS_H__
#define __CGAMESETTINGS_H__

#include "Controllers\\Player.h"
#include "Edgewalker\\Input\\InputKeyboard.h"
#include "Edgewalker\\Input\\InputJoystick.h"

enum BOOLEAN_GAME_OPTION
{
	// User adjustable options
	GO_SOUNDS_ENABLED=0,
	GO_MUSIC_ENABLED,
	GO_SHOW_DIALOGUES_ALWAYS,
	GO_DEV_MODE,
	GO_ANIME_SMILEYS_ENABLED,

	// Non-user adjustable in-game options
	GO_SHOW_DIALOGUES,		// When false, no dialogues are initiated

	GO_FULLSCREEN,

	GO_STATIC_SHADOWS_ENABLED,

	GO_TUTORIALS_ENABLED,

	GO_MOUSE_ENABLED,

	GO_FPS_LIMIT_ENABLED,

	GO_B_COUNT,
	GO_FORCEDWORD=0xFFFFFFFF
};

enum INTEGER_GAME_OPTION
{
	GO_INT_DEF=0,
	GO_INT_SCREEN_WIDTH,
	GO_INT_SCREEN_HEIGHT,
	GO_INT_COUNT,
	GO_INT_FORCEDWORD=0xFFFFFFFF
};

enum FLOAT_GAME_OPTION
{
	GO_SOUND_VOLUME=0,
	GO_MUSIC_VOLUME,
	GO_MOUSE_SENSITIVITY,
	GO_F_COUNT,
	GO_F_FORCEDWORD=0xFFFFFFFF
};

#define NUM_CONTROL_SETS	2

struct SGeneralControlSet
{
	// Open main menu / go back one menu level / skip dialogue, default Esc
	BYTE menuBack;
	// Execute selected procedure / next dialogue item / restart level, default Enter
	BYTE generalOk;
	// Open inventory
	BYTE openInventory;
	// Open player terminal
	BYTE openPlayerTerminal;
	BYTE takeScreenshot;
	BYTE quickLoad;
	BYTE quickSave;
	BYTE displayFPS;

	void FillDefaultKeyboard()
	{
		menuBack = DIK_ESCAPE;
		generalOk = DIK_RETURN;
		openInventory = DIK_T;
		openPlayerTerminal = DIK_U;
		takeScreenshot = DIK_F12;
		quickLoad = DIK_F9;
		quickSave = DIK_F5;
		displayFPS = DIK_Y;
	}

	void FillDefaultKeyboardMouse()
	{
		menuBack = DIK_ESCAPE;
		generalOk = DIK_SPACE;
		openInventory = DIK_T;
		openPlayerTerminal = DIK_G;
		takeScreenshot = DIK_F12;
		quickLoad = DIK_F9;
		quickSave = DIK_F5;
		displayFPS = DIK_Y;
	}

	void FillDefaultPad()
	{
		menuBack = 4;
		generalOk = 6;
		openInventory = 5;
		openPlayerTerminal = 7;
		takeScreenshot = 100;
		quickLoad = 100;
		quickSave = 100;
		displayFPS = 100;
	}
};

struct SPlayerCameraSettings
{
	bool bUseFollowCam;
	float distance;
	float smoothingFactor;
	float angle;
	float farClipPlane;
	float nearClipPlane;

	void FillDefaults()
	{
		bUseFollowCam = true;
		distance = 240.f;
		smoothingFactor = 2.f;
		angle = 1.1f;
		farClipPlane = 1000.f;
		nearClipPlane = 1.f;
	}
};

#define NUM_GADGET_HOTKEYS		5

class CGameSettings
{
public:
	CGameSettings();
	~CGameSettings();

	inline bool GetSetting(BOOLEAN_GAME_OPTION option) {return m_bOptions[option];}
	inline int GetSetting(INTEGER_GAME_OPTION option) {return m_iOptions[option];}
	inline float GetSetting(FLOAT_GAME_OPTION option) {return m_fOptions[option];}

	inline void SetSetting(BOOLEAN_GAME_OPTION option, bool value) {m_bOptions[option] = value;}
	inline void SetSetting(INTEGER_GAME_OPTION option, int value) {m_iOptions[option] = value;}
	inline void SetSetting(FLOAT_GAME_OPTION option, float value) {m_fOptions[option] = value;}

	inline SPlayerControlSet GetControlSet(int player) {return m_controlSets[player];}
	inline void SetControlSet(SPlayerControlSet set, int player) {m_controlSets[player] = set;}

	bool LoadSettingsFromFile(const char *filename);
	bool SaveSettingsToFile(const char *filename);

	void LoadDefaultSettings();

	SGeneralControlSet GetGeneralKeyboardControlSet() {return m_generalKeyboard;}
	SGeneralControlSet GetGeneralPadControlSet() {return m_generalPad;}

	void SetGeneralKeyboardControlSet(SGeneralControlSet set) {m_generalKeyboard = set;}
	void SetGeneralPadControlSet(SGeneralControlSet set) {m_generalPad = set;}

	SPlayerCameraSettings GetPlayerCameraSettings() {return m_playerCameraSettings;}
	void SetPlayerCameraSettings(SPlayerCameraSettings settings) {m_playerCameraSettings = settings;}

	void SetDefaultCampaign(std::string filename) {m_defaultCampaign = filename;}
	std::string GetDefaultCampaign() {return m_defaultCampaign;}

	void FillDefaultKeyboardGadgetHotkeys();
	void FillDefaultKeybMouseGadgetHotkeys();

	bool WriteSettingsToIni(const std::string filename);

	BYTE *GetGadgetShortcuts() {return m_gadgetHotkeys;}

	// General controls
	bool IsMenuBackDown();
	bool IsGeneralOkDown();
	bool IsOpenInventoryDown();
	bool IsOpenPlayerTerminalDown();
	bool IsNavigateRightDown();
	bool IsNavigateLeftDown();
	bool IsNavigateUpDown();
	bool IsNavigateDownDown();

	void SetKeyboardObject(CInputKeyboard *keyboard) {m_keyboard = keyboard;}
	void SetJoystickObject(CInputJoystick *pad) {m_pad = pad;}

	std::string GetControlSymbolOrCode(BYTE code);

	static BYTE GetKeyCode(std::string key);
	static std::string GetKeyString(BYTE code);

	static CGameSettings *GetSettingsObject() {return m_this;}

private:
	void FillKeymap();
	std::string GetSymbolOrCodeEx(BYTE code, bool bKeyboard);

	bool m_bOptions[GO_B_COUNT];
	int m_iOptions[GO_INT_COUNT];
	float m_fOptions[GO_F_COUNT];

	SPlayerControlSet m_controlSets[NUM_CONTROL_SETS];
	SGeneralControlSet m_generalKeyboard;
	SGeneralControlSet m_generalPad;

	SPlayerCameraSettings m_playerCameraSettings;

	BYTE m_gadgetHotkeys[NUM_GADGET_HOTKEYS];

	std::string m_defaultCampaign;

	CInputKeyboard *m_keyboard;
	CInputJoystick *m_pad;

	static CGameSettings *m_this;

	static std::vector<std::pair<BYTE, std::string> > m_keymap;
};

#endif // #ifndef __CGAMESETTINGS_H__
