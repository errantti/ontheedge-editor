
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

#ifndef __CSETTINGSFILEPARSER_H__
#define __CSETTINGSFILEPARSER_H__

#include "GameSettings.h"
#include "Edgewalker\\IniFile.h"

class CSettingsFileParser
{
public:
	CSettingsFileParser();

	bool ParseFile(std::string filename);

private:
	bool GetLineInfo(char *command, char *param, const char *line);
	void ParseLine(const char *command, const char *param);
	BYTE GetControlValue(const std::string &section, const std::string &key, BYTE defaultValue);

	CIniFile m_file;
};

#endif // __CSETTINGSFILEPARSER_H__