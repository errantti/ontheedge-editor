
/*********************************
 *
 *		On the Edge
 * Copyright (2006) Azuris Productions
 * Jukka Tykkyläinen
 * http://azuris.ote-game.com
 *
 * CSettingsFileParser
 *
 *********************************/

#include "SettingsFileParser.h"

#include <iostream>
#include <fstream>
#include <dinput.h>

using namespace std;

CSettingsFileParser::CSettingsFileParser()
{
}

void ClampFloat(float &f1, float min, float max)
{
	if(f1 < min)
		f1 = min;
	if(f1 > max)
		f1 = max;
}

bool CSettingsFileParser::ParseFile(string filename)
{
	if(!m_file.ReadFile(filename))
	{
		CPrinter::logError("Failed opening settings.ini - using default settings");
		return false;
	}

	CGameSettings &s = *(CGameSettings::GetSettingsObject());

	int width = m_file.ReadIntValue("General", "screenWidth", 800);
	int height = m_file.ReadIntValue("General", "screenHeight", 600);
	bool fullscreen = m_file.ReadBoolValue("General", "fullscreen", true);
	bool staticShadowsEnabled = m_file.ReadBoolValue("General", "staticShadowsEnabled", true);
	bool tutorialsEnabled = m_file.ReadBoolValue("General", "tutorialsEnabled", true);
	bool enemyEmotes = m_file.ReadBoolValue("General", "enemyEmotes", false);
	float musicVolume = (float)m_file.ReadDoubleValue("General", "musicVolume", 0.8);
	float effectVolume = (float)m_file.ReadDoubleValue("General", "effectVolume", 1.0);
	bool bFPSLimit = m_file.ReadBoolValue("General", "fpsLimitEnabled", true);
	// TODO: change default value
	std::string defaultCampaign = m_file.ReadStringValue("General", "defaultCampaign", "campaigns\\OtE intro.cpg");

	s.SetDefaultCampaign(defaultCampaign);

	s.SetSetting(GO_FULLSCREEN, fullscreen);
	s.SetSetting(GO_STATIC_SHADOWS_ENABLED, staticShadowsEnabled);
	s.SetSetting(GO_TUTORIALS_ENABLED, tutorialsEnabled);
	s.SetSetting(GO_ANIME_SMILEYS_ENABLED, enemyEmotes);
	s.SetSetting(GO_FPS_LIMIT_ENABLED, bFPSLimit);

	bool mouseEnabled = m_file.ReadBoolValue("Mouse controls", "mouseEnabled", true);
	float sensitivity = (float)m_file.ReadDoubleValue("Mouse controls", "sensitivity", s.GetSetting(GO_MOUSE_SENSITIVITY));
	s.SetSetting(GO_MOUSE_ENABLED, mouseEnabled);
	s.SetSetting(GO_MOUSE_SENSITIVITY, sensitivity);

	ClampFloat(musicVolume, 0.f, 1.f);
	ClampFloat(effectVolume, 0.f, 1.f);
	s.SetSetting(GO_MUSIC_VOLUME, musicVolume);
	if(musicVolume == 0.f)
		s.SetSetting(GO_MUSIC_ENABLED, false);
	s.SetSetting(GO_SOUND_VOLUME, effectVolume);
	if(effectVolume == 0.f)
		s.SetSetting(GO_SOUND_VOLUME, false);

	if(width < 800)
	{
		CPrinter::logError("Settings.ini error: too small or invalid screen width, defaulting to 800");
		width = 800;
	}
	
	s.SetSetting(GO_INT_SCREEN_WIDTH, width);

	if(height < 600)
	{
		CPrinter::logError("Settings.ini error: too small or invalid screen height, defaulting to 600");
		height = 600;
	}
	s.SetSetting(GO_INT_SCREEN_HEIGHT, height);

	// Controls

	string section("Player 1 controls");

	for(int i=0; i<2; ++i)
	{
		if(i==1)
			section = "Player 2 controls";

		SPlayerControlSet set = s.GetControlSet(i);

		set.keyboard = m_file.ReadBoolValue(section, "useKeyboard", true);
		set.forward = GetControlValue(section, "accelerate", set.forward);
		set.back = GetControlValue(section, "brake", set.back);
		set.left = GetControlValue(section, "turnLeft", set.left);
		set.right = GetControlValue(section, "turnRight", set.right);
		set.thrustLeft = GetControlValue(section, "thrustLeft", set.thrustLeft);
		set.thrustRight = GetControlValue(section, "thrustRight", set.thrustRight);
		set.gadget1 = GetControlValue(section, "gadget1", set.gadget1);
		set.gadget2 = GetControlValue(section, "gadget2", set.gadget2);
		set.boost = GetControlValue(section, "boost", set.boost);

		s.SetControlSet(set, i);
	}

	section = "General keyboard controls";
	for(int i=0; i<2; ++i)
	{
		SGeneralControlSet set;
		if(i==1)
		{
			set = s.GetGeneralPadControlSet();
			section = "General pad controls";
		}
		else
			set = s.GetGeneralKeyboardControlSet();

		set.menuBack = GetControlValue(section, "menuBack", set.menuBack);
		set.generalOk = GetControlValue(section, "generalOk", set.generalOk);
		set.openInventory = GetControlValue(section, "openInventory", set.openInventory);
		set.openPlayerTerminal = GetControlValue(section, "openPlayerTerminal", set.openPlayerTerminal);
		set.takeScreenshot = GetControlValue(section, "takeScreenshot", set.takeScreenshot);
		set.quickLoad = GetControlValue(section, "quickLoad", set.quickLoad);
		set.quickSave = GetControlValue(section, "quickSave", set.quickSave);
		set.displayFPS = GetControlValue(section, "displayFPS", set.displayFPS);

		if(i==1)
			s.SetGeneralPadControlSet(set);
		else
			s.SetGeneralKeyboardControlSet(set);
	}

	BYTE *hotkeys = s.GetGadgetShortcuts();
	section = "Gadget hotkeys";
	hotkeys[0] = GetControlValue(section, "gadget1", DIK_6);
	hotkeys[1] = GetControlValue(section, "gadget2", DIK_7);
	hotkeys[2] = GetControlValue(section, "gadget3", DIK_8);
	hotkeys[3] = GetControlValue(section, "gadget4", DIK_9);
	hotkeys[4] = GetControlValue(section, "gadget5", DIK_0);

	SPlayerCameraSettings cams;
	cams.FillDefaults();
	section = "Player camera";
	cams.bUseFollowCam = m_file.ReadBoolValue(section, "useFollowCam", true);
	cams.distance = (float)m_file.ReadDoubleValue(section, "distance", cams.distance);
	cams.smoothingFactor = (float)m_file.ReadDoubleValue(section, "smoothingFactor", cams.smoothingFactor);
	cams.angle = (float)m_file.ReadDoubleValue(section, "angle", cams.angle);
	cams.nearClipPlane = (float)m_file.ReadDoubleValue(section, "nearClipPlane", cams.nearClipPlane);
	cams.farClipPlane = (float)m_file.ReadDoubleValue(section, "farClipPlane", cams.farClipPlane);
	s.SetPlayerCameraSettings(cams);

	return true;
}

BYTE CSettingsFileParser::GetControlValue(const std::string &section, const std::string &key, BYTE defaultValue)
{
	string str = m_file.ReadStringValue(section, key, "");
	if(str == "")
		return defaultValue;

	BYTE byteTemp = CGameSettings::GetKeyCode(str);
	int iTemp;
	if(byteTemp == 0)
	{
		iTemp = atoi(str.c_str());
		/*if(iTemp <= 0 || iTemp > 255)
		{
			string errorMsg("Settings.ini error: invalid control key value: ");
			errorMsg += str;
			CPrinter::logError(errorMsg);
			return defaultValue;
		}*/
		byteTemp = (BYTE)iTemp;
	}

	return byteTemp;
}