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

#include "OnTheEdge.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCampaign::CCampaign() : m_currentBlockID(-1), m_bLevelCompleted(FALSE), m_currentLevelIndex(0),
	m_bCampaignLoaded(FALSE), m_nextStartposID(-1), m_playerStartposID(-1), m_bStartLevelDefault(TRUE),
	m_campaignStartTime(-1)
{
	m_name[0] = '\0';
	m_endMessage[0] = '\0';
	m_playerTerminal.FillDefaults();
	m_bForceStartShip = true;
	m_firstBlockID = -1;
	m_equippedGadgets[0] = -1;
	m_equippedGadgets[1] = -1;
	m_startShipID = -1;
	int i;
	for(i=0;i<MAX_MISSION_OBJECTIVES;++i)
		m_missionObjectives[i] = "No mission objective.";
	for(i=0;i<5;++i)
		m_gadgetHotkeyIDs[i] = -1;
}

CCampaign::~CCampaign()
{
}

void CCampaign::InitDefaultCampaign()
{
	m_blocks.clear();
	m_startGadgetIDs.clear();

	/**
	 * Add default blocks
	 **/
	SCampaignBlock b;
	b.blockID = 1;
	b.bShowIntroduction = TRUE;
	b.endType = LEVEL_END_ENEMIES_KILLED;
	strcpy_s(b.introductionText, 511, "Your objective is to destroy all the opposing fighters. \
		You're equipped with a far superior vessel but the enemies are numerous. Use arrow-keys \
		to turn and accelerate, L to shoot and I to boost your speed. Watch out for the blue ships' explosions.");
	strcpy_s(b.levelFilename, 127, "level1.oel");
	b.nextBlockID = 2;
	b.targetID = -1;
	AddBlock(b);

	b.blockID = 2;
	b.nextBlockID = 3;
	strcpy_s(b.introductionText, 511, "Muksista vaan");
	strcpy_s(b.levelFilename, 127, "level1Original.oel");
	AddBlock(b);

	/**
	 * Begin settings
	 **/
	m_startGadgetIDs.push_back(2);
	m_startGadgetIDs.push_back(4);
	m_startGadgetIDs.push_back(5);
	m_startGadgetIDs.push_back(6);
	m_startGadgetIDs.push_back(7);
	m_startGadgetIDs.push_back(8);

	m_equippedGadgets[0] = 2;
	m_equippedGadgets[1] = 8;

	m_bForceStartShip = FALSE;
	m_startShipID = 0;
	m_firstBlockID = 1;

	m_bCampaignLoaded = TRUE;
	strcpy_s(m_name, 63, "Default campaign");
	strcpy_s(m_endMessage, 2047, "Congratulations! You have completed the default campaign!");

	m_campaignFilename = "";
}

/**
 * EndLevel
 * Function is called any time a level ending criteria _might_ be met
 * This function checks if this is the case and acts accordingly
 **/
bool CCampaign::EndLevel(LEVEL_END_TYPE endType, long targetID)
{
	if(m_bLevelCompleted)	// Should never actually happen but if the level has already been reported completed
		return TRUE;		// there's no need to check again

	if(m_blocks.size() == 0)
		return FALSE;

	if(endType == m_blocks[m_currentLevelIndex].endType)
	{
		if(endType == LEVEL_END_FORCEDWORD || endType == LEVEL_END_NONE)
			return FALSE;
		if(endType == LEVEL_END_TARGET_DESTROYED && m_blocks[m_currentLevelIndex].targetID != targetID)
			return FALSE;
		if(endType == LEVEL_END_TARGET_DESTROYED && targetID == -1)
			return FALSE;

		// The level end condition has been met
		m_bLevelCompleted = TRUE;

		GetOTE()->AddScreenMessage("Level completed");
		DisplayLevelTime();

		m_levelStart.StoreCurrentSituation();
		GetOTE()->GetPlayer(0)->StopChangingGadgets();
		GetOTE()->EndLevel(TRUE, 2.5f);

		m_bStartLevelDefault = TRUE;

		// Start from default
		m_nextStartposID = -1;

		return TRUE;
	}

	return FALSE;
}

void CCampaign::ForceEndLevel(std::string levelFilename, long startposID)
{
	if(levelFilename.length() == 0)
	{
		// TODO: this is temp solution, I was lazy
		m_nextLevelIndex = 10000;
		m_bLevelCompleted = TRUE;
		m_bStartLevelDefault = FALSE;
		GetOTE()->GetPlayer(0)->StopChangingGadgets();
		GetOTE()->EndLevel(TRUE, 2.5f);
		return;
	}

	// Check whether player is alive, dead player can't be forced to complete level
	CPlayer *p1 = GetOTE()->GetPlayer(0);
	if(p1->GetShip() == NULL)
		return;

	// Check whether the level exists
	bool found = false;
	vector<SCampaignBlock>::iterator it = m_blocks.begin();
	int i;
	for(i=0; it != m_blocks.end(); ++it, ++i)
		if(!levelFilename.compare(it->levelFilename))
		{
			found = true;
			break;
		}

	if(!found)
	{
		// Level not found, don't end level
		CPrinter::logError("Internal error: required level not found in the campaign");
		return;
	}

	m_bStartLevelDefault = FALSE;

	m_nextLevelIndex = i;
	m_nextStartposID = startposID;

	m_bLevelCompleted = TRUE;

	m_levelStart.StoreCurrentSituation();
	GetOTE()->GetPlayer(0)->StopChangingGadgets();
	DisplayLevelTime();
	GetOTE()->EndLevel(TRUE, 2.5f);
}

void CCampaign::RestartLevel()
{
	m_bLevelCompleted = FALSE;
	GetOTE()->GetPlayer(0)->StopChangingGadgets();
	GetOTE()->EndLevel(TRUE, 2.5f);
}

void CCampaign::NextLevel()
{
	// Dead player can't complete a level
	//if(GetOTE()->GetPlayer(0)->GetShip() == NULL)
	//	m_bLevelCompleted = FALSE;

	if(m_bLevelCompleted)
	{
		GetOTE()->GetSettings()->SetSetting(GO_SHOW_DIALOGUES, TRUE);

		if(m_bStartLevelDefault)
		{
			// Find the next block
			//vector<SCampaignBlock>::iterator it = 
			bool bFound = FALSE;
			int i;
			for(i=0; i<(int)m_blocks.size(); ++i)
				if(m_blocks[i].blockID == m_blocks[m_currentLevelIndex].nextBlockID)
				{
					m_currentLevelIndex = i;
					bFound = TRUE;
					m_currentBlockID = m_blocks[i].blockID;
					break;
				}

			if(!bFound)
			{
				// Either these has been a bug or a mistake or the campaign is over
				DisplayEndingText();
				DisplayLevelTime();
				DisplayCampaignTime();

				GetOTE()->CampaignCompleted();
				return;
			}
		}
		else
		{
			// TODO: this is temp solution, I was lazy
			if(m_nextLevelIndex == 10000)
			{
				// Either these has been a bug or a mistake or the campaign is over
				DisplayEndingText();
				DisplayLevelTime();
				DisplayCampaignTime();
				GetOTE()->CampaignCompleted();
				return;
			}

			m_currentBlockID = m_blocks[m_nextLevelIndex].blockID;
			m_currentLevelIndex = m_nextLevelIndex;
		}

		m_playerStartposID = m_nextStartposID;
		m_levelStart.SetPlayerStartPosID(m_playerStartposID);

		// Load the next level
		StartLevel();
	}
	else
	{
		GetOTE()->GetSettings()->SetSetting(GO_SHOW_DIALOGUES, FALSE);
		StartLevel();
		m_levelStart.ApplyGadgetsAndItems();
		m_globalVariables.clear();
		m_globalVariables = m_levelStart.GetGlobalVariables();
		
		long hotkeys[5];
		m_levelStart.GetGadgetHotkeyIDs(hotkeys);

		std::vector<SItemTemp> *items = m_levelStart.GetItems();
		bool found;

		for (int i=0;i<5;++i)
		{
			found = false;
			std::vector<SItemTemp>::iterator sit = items->begin();
			for(; sit != items->end(); ++sit)
				if((*sit).id == m_gadgetHotkeyIDs[i])
				{
					found = true;
					break;
				}

			if (!found)
				m_gadgetHotkeyIDs[i] = hotkeys[i];
		}

		char buffer[MAX_MISSION_OBJECTIVE_LENGTH+1];
		for(int i=0;i<MAX_MISSION_OBJECTIVES;++i)
		{
			m_levelStart.GetMissionObjective(i, buffer);
			m_missionObjectives[i] = buffer;
		}
	}
}

void CCampaign::DisplayEndingText()
{
	CScreenMessages::SScreenMessage msg;
	msg.FillDefaults();
	int centerX = (int)((float)GetOTE()->GetWidth() * 0.5f);
	int centerY = (int)((float)GetOTE()->GetHeight() * 0.5f);
	msg.area = rect<int>(centerX-200, centerX+200, centerY-200, centerY+200);
	msg.message = m_endMessage;
	msg.message += "\n\n(Press ";
	CGameSettings &s = *GetOTE()->GetSettings();
	msg.message += s.GetControlSymbolOrCode(s.GetGeneralKeyboardControlSet().menuBack);
	msg.message += " to open menu)";
	msg.flags = DT_CENTER | DT_VCENTER | DT_WORDBREAK;
	msg.time = 3000.f;
	msg.fadeTime = 7.f;
	msg.ID = 25252;
	GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);
}

bool CCampaign::StartLevel()
{
	if(m_currentLevelIndex<0 || m_currentLevelIndex>=(int)m_blocks.size())
		return FALSE;

	m_bLevelCompleted = FALSE;

	if(SUCCEEDED(GetOTE()->StartLevel(m_blocks[m_currentLevelIndex].levelFilename, m_shipID)))
	{
		// Equip gadgets
		m_levelStart.ApplyGadgetsAndItems();

		/*CScreenMessages::SScreenMessage msg;
		msg.FillDefaults();
		msg.message = GetOTE()->GetCurrentLevel()->GetName();
		msg.area = rect<int>(200, 600, 280, 320);
		GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);*/
		std::string str("Entering level ");
		str += GetOTE()->GetCurrentLevel()->GetName();
		GetOTE()->AddScreenMessage(str);

		m_levelStartTime = GetOTE()->GetTimer()->GetTime();

		if(m_blocks[m_currentLevelIndex].bShowIntroduction)
		{
			CDialogue::SComment c;
			strcpy_s(c.comment, 1023, m_blocks[m_currentLevelIndex].introductionText);
			strcpy_s(c.speaker, 31, "Mission objectives");
			m_missionBriefing.ClearComments();
			m_missionBriefing.AddComment(c);
			m_missionBriefing.SetIsAlreadyShown(FALSE);	// always show mission objectives
			m_missionBriefing.OpenDialogue();	// The dialogue will be initiated immediately after the fade in
		}

		return TRUE;
	}

	return FALSE;
}

bool CCampaign::StartDefaultCampaign(long shipID)
{
	if(m_bCampaignLoaded == FALSE)
		return FALSE;

	int i;
	m_currentBlockID = -1;
	for(i=0; i<(int)m_blocks.size(); ++i)
		if(m_blocks[i].blockID == m_firstBlockID)
		{
			m_currentBlockID = m_firstBlockID;
			m_currentLevelIndex = i;
			break;
		}

	if(m_currentBlockID<0)
		return FALSE;

	if(GetOTE()->IsGameRunning())
		GetOTE()->CloseLevel();

	GetOTE()->GetSettings()->SetSetting(GO_SHOW_DIALOGUES, TRUE);

	m_playerStartposID = m_nextStartposID = -1;

	m_shipID = shipID;
	if(m_bForceStartShip)
		m_shipID = m_startShipID;

	for(i=0;i<5;++i)
		m_gadgetHotkeyIDs[i] = -1;

	int lowestAvailableHotkey = 0;
	// Set default hotkeys for the start gadgets
	for(i=0;i<(int)m_startGadgetIDs.size();++i)
	{
		std::pair<SGadgetTemplate, SCustomGadgetTemplate*> *t = GetOTE()->GetGadgetDatabase()->GetGadgetTemplate(m_startGadgetIDs[i]);
		if(t != NULL)
			if(!t->first.bActivateable)
			{
				if(lowestAvailableHotkey < 5)
				{
					m_gadgetHotkeyIDs[lowestAvailableHotkey] = m_startGadgetIDs[i];
					++lowestAvailableHotkey;
				}
			}
	}

	// Fill game situation info manually 'cause can't use autofill here
	m_levelStart.GetItems()->clear();
	m_levelStart.CopyGadgets(&m_startGadgetIDs);
	m_levelStart.SetGadgetHotkeyIDs(m_gadgetHotkeyIDs);
	m_levelStart.SetEquippedGadget(m_equippedGadgets[0], 0);
	m_levelStart.SetEquippedGadget(m_equippedGadgets[1], 1);
	m_levelStart.SetCurrentBlockID(m_firstBlockID);
	m_levelStart.SetShipID(m_shipID);
	m_levelStart.SetPlayerStartPosID(-1); // Use level's default player start pos
	m_levelStart.SetSaveName("Level 1");
	m_levelStart.SignalSituationStored();
	m_levelStart.GetGlobalVariables().clear();

	for(int i=0;i<MAX_MISSION_OBJECTIVES;++i)
		m_missionObjectives[i] = "No mission objective.";

	/*CPlayer *p1 = GetOTE()->GetPlayer(0);
	if(p1 != NULL)
	{
		p1->ResetGadgets();
		p1->ClearItemsAndGadgets();
	}*/

	//if(ApplyStartEquipment() == FALSE)
	//	CPrinter::log("Failed applying starting equipment");

	m_campaignStartTime = GetOTE()->GetTimer()->GetTime();

	return StartLevel();
}

bool CCampaign::StartCampaign(char *filename, long shipID)
{
	if(LoadCampaign(filename) == FALSE)
		return FALSE;

	StartDefaultCampaign(shipID);

	return FALSE;
}

bool CCampaign::ReadCampaignName(std::string &outName, const char *filename)
{
	if(filename == NULL)
		return FALSE;

	fstream f;
	f.open(filename, ios::in | ios::binary);
	if(!f.is_open())
	{
		CPrinter::log("ReadCampaignName: Failed opening campaign file. File is being used by another program or doesn't exist.", LOG_ERROR);
		return FALSE;
	}

	// Read the identifier
	DWORD identifier;
	f.read((char*)&identifier, sizeof(DWORD));

	if(identifier != CAMPAIGN_FILE_BEGIN_IDENTIFIER)
	{
		CPrinter::log("ReadCampaignName: Campaign file begin identifier mismatch, file may be corrupted", LOG_ERROR);
		f.close();
		return FALSE;
	}

	/***
	 * Read the campaign name
	 ***/
	char buffer[64];
	f.read((char*)&buffer, sizeof(char)*64);
	outName = buffer;

	f.close();

	return true;
}

bool CCampaign::LoadCampaign(const char *filename)
{
	if(filename == NULL)
		return FALSE;

	fstream f;
	f.open(filename, ios::in | ios::binary);
	if(!f.is_open())
	{
		CPrinter::log("Failed opening campaign file. File is being used by another program or doesn't exist.", LOG_ERROR);
		::MessageBox(NULL, "Failed opening campaign file. File is being used by another program or doesn't exist.", "OtE Editor error!", MB_OK);
		return FALSE;
	}

	// Read the identifier
	DWORD identifier;
	f.read((char*)&identifier, sizeof(DWORD));

	if(identifier != CAMPAIGN_FILE_BEGIN_IDENTIFIER)
	{
		CPrinter::log("Campaign file begin identifier mismatch, file may be corrupted", LOG_ERROR);
		f.close();
		return FALSE;
	}

	/***
	 * Read the campaign data
	 ***/
	f.read((char*)&m_name, sizeof(char)*64);
	f.read((char*)&m_endMessage, sizeof(char)*2048);

	f.read((char*)&m_bForceStartShip, sizeof(bool));
	f.read((char*)&m_firstBlockID, sizeof(int));

	f.read((char*)&m_equippedGadgets[0], sizeof(int));
	f.read((char*)&m_equippedGadgets[1], sizeof(int));

	f.read((char*)&m_startShipID, sizeof(long));

	m_playerTerminal.ReadTerminalDataFromStream(f);

	// Read the vectors
	int size, i;
	f.read((char*)&size, sizeof(int));
	if(size < 0 || size > 1000)
	{
		CPrinter::log("Block amount beyond boundaries, file may be corrupted", LOG_ERROR);
		f.close();
		return FALSE;
	}

	SCampaignBlock b;
	m_blocks.clear();
	for(i=0; i<size; ++i)
	{
		f.read((char*)&b, sizeof(SCampaignBlock));
		m_blocks.push_back(b);
	}

	f.read((char*)&size, sizeof(int));
	if(size < 0 || size > 100)
	{
		CPrinter::log("Gadget amount beyond boundaries, file may be corrupted", LOG_ERROR);
		f.close();
		return FALSE;
	}

	m_startGadgetIDs.clear();
	long lTemp;
	for(i=0; i<size; ++i)
	{
		f.read((char*)&lTemp, sizeof(long));
		m_startGadgetIDs.push_back(lTemp);
	}

	f.close();

	m_campaignFilename = filename;

	m_bCampaignLoaded = TRUE;

	return TRUE;
}

bool CCampaign::SaveCampaign(const char *filename, bool bNewCampaign)
{
	if(filename == NULL)
	{
		CPrinter::log("Internal error: no save game filename provided", LOG_ERROR);
		return FALSE;
	}

	if(bNewCampaign)
		m_bCampaignLoaded = true;

	if(!m_bCampaignLoaded)
	{
		CPrinter::log("Failed saving game: campaign not loaded", LOG_ERROR);
		return FALSE;
	}

	fstream f;
	f.open(filename, ios::out | ios::binary);
	if(!f.is_open())
	{
		CPrinter::log("Failed opening campaign file for writing", LOG_ERROR);
		::MessageBox(NULL, "Failed opening campaign file for writing", "OtE Editor error!", MB_OK);
		return FALSE;
	}

	DWORD identifier = CAMPAIGN_FILE_BEGIN_IDENTIFIER;

	// Write the file begin identifier
	f.write((const char*)&identifier, sizeof(DWORD));

	/***
	 * Write campaign data
	 ***/
	f.write((const char*)&m_name, sizeof(char)*64);
	f.write((const char*)&m_endMessage, sizeof(char)*2048);

	f.write((const char*)&m_bForceStartShip, sizeof(bool));
	f.write((const char*)&m_firstBlockID, sizeof(int));

	f.write((const char*)&m_equippedGadgets[0], sizeof(int));
	f.write((const char*)&m_equippedGadgets[1], sizeof(int));

	f.write((const char*)&m_startShipID, sizeof(long));

	m_playerTerminal.WriteTerminalDataIntoStream(f);

	// Write the vectors
	int size = m_blocks.size();
	f.write((const char*)&size, sizeof(int));

	SCampaignBlock b;
	vector<SCampaignBlock>::iterator it = m_blocks.begin();
	for(; it != m_blocks.end(); ++it)
	{
		b = (*it);
		f.write((const char*)&b, sizeof(SCampaignBlock));
	}

	size = m_startGadgetIDs.size();
	f.write((const char*)&size, sizeof(int));

	long value;
	vector<long>::iterator it2 = m_startGadgetIDs.begin();
	for(; it2 != m_startGadgetIDs.end(); ++it2)
	{
		value = (*it2);
		f.write((const char*)&value, sizeof(long));
	}

	m_campaignFilename = filename;

	f.close();

	return TRUE;
}

bool CCampaign::SaveGame(const char *filename)
{
	m_levelStart.SetSaveName(GetOTE()->GetCurrentLevel()->GetName().c_str());
	m_levelStart.SetCurrentBlockID(m_currentBlockID);
	m_levelStart.SetCampaignFilename(m_campaignFilename.c_str());

	if(m_levelStart.SaveStoredSituation(filename) == FALSE)
	{
		CPrinter::log("Failed saving game situation", LOG_ERROR);

		GetOTE()->AddScreenMessage("Failed saving game situation");

		return FALSE;
	}

	//msg.message = "Game saved succesfully";
	//GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);

	return TRUE;
}

bool CCampaign::LoadGame(const char *filename)
{
	if(!m_bCampaignLoaded)
	{
		CPrinter::log("Failed loading game: no campaign loaded", LOG_ERROR);
		return FALSE;
	}

	CGameSituation s;
	CScreenMessages::SScreenMessage msg;
	msg.FillDefaults();
	msg.area = rect<int>(100, 700, 280, 320);

	m_campaignStartTime = -1;

	if(s.LoadSituation(filename) == FALSE)
	{
		CPrinter::log("Failed loading saved game", LOG_ERROR);
		msg.message = "Failed loading saved game from file";
		GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);
		return FALSE;
	}

	if(!GetOTE()->LoadCampaign(s.GetCampaignFilename()))
	{
		CPrinter::logError("Failed loading saved game: failed loading the campaign the saved game uses");
		return FALSE;
	}

	int i, id = s.GetCurrentBlockID();
	bool bFound = FALSE;
	for(i=0; i<(int)m_blocks.size(); ++i)
		if(m_blocks[i].blockID == id)
		{
			m_currentLevelIndex = i;
			bFound = TRUE;
			break;
		}
	if(!bFound)
	{
		CPrinter::log("Failed initializing campaign from saved game: couldn't find block ID", LOG_ERROR);
		msg.message = "Failed initializing campaign from saved game: couldn't find block ID";
		GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);
		return FALSE;
	}

	GetOTE()->GetSettings()->SetSetting(GO_SHOW_DIALOGUES, TRUE);
	GetOTE()->GetGUIEnvironment()->SetCursorVisibility(FALSE);

	if(GetOTE()->IsGameRunning())
		GetOTE()->CloseLevel();
	// Transfer the situation to the primary game situation variable only after
	// it's certain that the saved game data is valid
	m_levelStart = s;

	m_currentBlockID = id;
	m_shipID = m_levelStart.GetShipID();
	m_nextStartposID = m_playerStartposID = m_levelStart.GetPlayerStartPosID();

	m_globalVariables.clear();
	m_globalVariables = s.GetGlobalVariables();

	s.GetGadgetHotkeyIDs(m_gadgetHotkeyIDs);

	char buffer[MAX_MISSION_OBJECTIVE_LENGTH+1];
	for(i=0;i<MAX_MISSION_OBJECTIVES;++i)
	{
		s.GetMissionObjective(i, buffer);
		m_missionObjectives[i] = buffer;
	}

	return StartLevel();
}

const char* CCampaign::GetLeveFilename(int ID)
{
	vector<SCampaignBlock>::iterator it = m_blocks.begin();
	for(; it != m_blocks.end(); ++it)
		if(it->blockID == ID)
			return it->levelFilename;
	return NULL;
}

int CCampaign::GetNextBlockID()
{
	return m_blocks[m_currentLevelIndex].nextBlockID;
}

std::string CCampaign::GetMissionObjective(int index)
{
	if(index<0 || index >= MAX_MISSION_OBJECTIVES)
		return "";
	return m_missionObjectives[index];
}

void CCampaign::SetMissionObjective(std::string objective, int index)
{
	if(index<0 || index >= MAX_MISSION_OBJECTIVES)
		return;
	m_missionObjectives[index] = objective;
}

float CCampaign::GetGlobalVariable(const char *name)
{
	if(name == NULL)
		return -1.f;

	std::vector<SGlobalVariable>::iterator it = m_globalVariables.begin();
	for(; it != m_globalVariables.end(); ++it)
		if(!strcmp(it->name, name))
			return it->num;

	return -1.f;
}

bool CCampaign::SetGlobalVariable(const char *name, float value)
{
	if(name == NULL)
		return false;

	std::vector<SGlobalVariable>::iterator it = m_globalVariables.begin();
	for(; it != m_globalVariables.end(); ++it)
		if(!strcmp(it->name, name))
		{
			it->num = value;
			return true;
		}

	SGlobalVariable var;
	var.num = value;
	strcpy_s(var.name, 15, name);
	m_globalVariables.push_back(var);

	return false;
}

bool CCampaign::RemoveGlobalVariable(const char *name)
{
	if(name == NULL)
		return false;

	std::vector<SGlobalVariable>::iterator it = m_globalVariables.begin();
	for(; it != m_globalVariables.end(); ++it)
		if(!strcmp(it->name, name))
		{
			m_globalVariables.erase(it);
			return true;
		}

	return false;
}

bool CCampaign::DoesGlobalVariableExist(const char *name)
{
	if(name == NULL)
		return false;

	std::vector<SGlobalVariable>::iterator it = m_globalVariables.begin();
	for(; it != m_globalVariables.end(); ++it)
		if(!strcmp(it->name, name))
			return true;

	return false;
}

void CCampaign::ResetGlobalVariables()
{
	m_globalVariables.clear();
}

bool CCampaign::EnumerateCampaigns(std::vector<std::pair<std::string,std::string> > &campaigns, const char *folder)
{
	if(folder == NULL)
		return false;

	campaigns.clear();

	HANDLE h;
	WIN32_FIND_DATA filedata;
	char filemask[256];

	sprintf_s(filemask, 255, "%s*.cpg", folder);

	h = FindFirstFile((char*)filemask, &filedata);	//save handle to first file in the directory

	if(h == INVALID_HANDLE_VALUE)
	{
		CPrinter::log("Couldn't find any campaigns");
		return false;
	}

	string filename, name;
	filename = folder;
	filename += (char*)filedata.cFileName;
	if(ReadCampaignName(name, filename.c_str()))
	{
		campaigns.push_back(std::pair<std::string,std::string>(filename,name));
	}

	while(FindNextFile(h, &filedata))	//Get handle to next file as long as there are .tst files
	{
		filename = folder;
		filename += (char*)filedata.cFileName;
		if(ReadCampaignName(name, filename.c_str()))
		{
			campaigns.push_back(std::pair<std::string,std::string>(filename,name));
		}
	}
	FindClose(h);

	return true;
}

void CCampaign::GetGadgetHotkeyIDs(long *outKeys)
{
	if(outKeys == NULL)
		return;
	for(int i=0;i<5;++i)
		outKeys[i] = m_gadgetHotkeyIDs[i];
}

void CCampaign::SetGadgetHotkeyIDs(long *keys)
{
	if(keys == NULL)
		return;
	for(int i=0;i<5;++i)
		m_gadgetHotkeyIDs[i] = keys[i];
}

long CCampaign::GetGadgetHotkeyID(int index)
{
	if(index < 0 || index >= 5)
		return -1;
	return m_gadgetHotkeyIDs[index];
}

void CCampaign::SetGadgetHotkeyID(int index, long ID)
{
	if(index < 0 || index >= 5)
		return;
	// Disallow multiple bindings for a single gadget
	for(int i=0;i<5;++i)
		if(m_gadgetHotkeyIDs[i] == ID)
			m_gadgetHotkeyIDs[i] = -1;
	m_gadgetHotkeyIDs[index] = ID;
}

void CCampaign::ReportGadgetUpgrade(long oldID, long newID)
{
	// TODO:
	for(int i=0;i<5;++i)
		if(m_gadgetHotkeyIDs[i] == oldID)
		{
			m_gadgetHotkeyIDs[i] = newID;
			return;
		}
}

void CCampaign::DisplayLevelTime()
{
	std::stringstream ss;
	long seconds = (long)(GetOTE()->GetTimer()->GetTime() - m_levelStartTime);
	long mins = (long)((double)seconds / 60);
	seconds = seconds % 60;
	ss << mins;
	string str("Level time: ");
	str += ss.str();
	str += " minutes ";
	std::stringstream ss2;
	ss2 << seconds;
	str += ss2.str();
	str += " seconds";
	GetOTE()->AddScreenMessage(str, 6.f);
}

void CCampaign::DisplayCampaignTime()
{
	if(m_campaignStartTime < 0)
	{
		GetOTE()->AddScreenMessage("(No campaign time, started from savegame)");
		return;
	}
	std::stringstream ss;
	long seconds = (long)(GetOTE()->GetTimer()->GetTime() - m_campaignStartTime);
	long mins = (long)((double)seconds / 60);
	seconds = seconds % 60;
	ss << mins;
	string str("Campaign time: ");
	str += ss.str();
	str += " minutes ";
	std::stringstream ss2;
	ss2 << seconds;
	str += ss2.str();
	str += " seconds";
	GetOTE()->AddScreenMessage(str, 16.f);
}