
/*********************************
 *
 *		On the Edge
 * Copyright (2004) Azuris Productions
 * Jukka Tykkyläinen
 * http://azuris.ote-game.com
 *
 * WinMain - entry point for windows program
 *
 *********************************/

#include "OnTheEdge.h"
#include "SettingsFileParser.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
{
	COnTheEdge game;

	bool windowed = false, zbuffer = true, vsync = false;
	int ressX = 800, ressY = 600;

	// Parse command line parameters (temporary)
	char szTemp[32];
	char logNote[128];
	UINT i=0, k;
	while(i+1<strlen(lpCmdLine))
	{
		while(i+1<strlen(lpCmdLine) && lpCmdLine[i] != '-') ++i;
		if(lpCmdLine[i] != '-')
			break;
		++i;
		k=0;
		while(lpCmdLine[i] != ' ' && lpCmdLine[i] != '\0' && k<30)
		{
			szTemp[k] = lpCmdLine[i];
			++k;++i;
		}
		szTemp[k]='\0';
		sprintf(logNote, "Command line parameter: %s", szTemp);
		CPrinter::log(logNote);

		if(!strcmp(szTemp, "vsync"))
		{
			vsync = true;
			CPrinter::log("VSync enabled from command line");
		}
		else if(!strcmp(szTemp, "disablezbuffer"))
		{
			zbuffer = false;
			CPrinter::log("ZBuffer disabled from command line");
		}
		else if(!strcmp(szTemp, "parsemenudata"))
		{
			COnTheEdge::m_bParseMenuDataOnStartup = true;
		}
		else if(!strcmp(szTemp, "forcesynchhack"))
		{
			game.EnableCPUGPUSynchHack(true);
			game.EnableAutoSynchHack(false);
		}
		else if(!strcmp(szTemp, "disableautosynch"))
		{
			game.EnableCPUGPUSynchHack(false);
			game.EnableAutoSynchHack(false);
		}
	}

	//COnTheEdge::m_bParseMenuDataOnStartup = true;
	//game.EnableAutoSynchHack(false);

	CSettingsFileParser parser;
	parser.ParseFile("settings.ini");
	ressX = CGameSettings::GetSettingsObject()->GetSetting(GO_INT_SCREEN_WIDTH);
	ressY = CGameSettings::GetSettingsObject()->GetSetting(GO_INT_SCREEN_HEIGHT);
	windowed = !CGameSettings::GetSettingsObject()->GetSetting(GO_FULLSCREEN);

	if(FAILED(game.Create(ressX, ressY, 32, windowed, _T("On the Edge Intro 1.0"), 0, vsync, zbuffer)))
		return -1;

	return game.Run();
}
