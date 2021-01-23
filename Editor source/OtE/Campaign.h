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

#ifndef __CCAMPAIGN_H__
#define __CCAMPAIGN_H__

#include "Edgewalker\\DirectX9\\Texture.h"
#include "Screen\\Dialogue.h"
#include "Screen\\Terminal.h"
#include "GameSituation.h"
#include <vector>

#define CAMPAIGN_FILE_BEGIN_IDENTIFIER	0x193B5AC0

enum LEVEL_END_TYPE
{
	LEVEL_END_ENEMIES_KILLED=1,
	LEVEL_END_TARGET_DESTROYED,
	LEVEL_END_SCRIPT,
	LEVEL_END_NONE,
	LEVEL_END_FORCEDWORD=0xFFFFFFFF
};

class CCampaign  
{
public:
	struct SCampaignBlock
	{
		char levelFilename[128];
		char scriptName[32];
		char introductionText[512];		// Comment in the dialogue box that is opened when the level starts
		int blockID, nextBlockID;
		LEVEL_END_TYPE endType;
		long targetID;
		bool bShowIntroduction;

		void FillDefaults()
		{
			levelFilename[0] = '\0';
			scriptName[0] = '\0';
			introductionText[0] = '\0';
			blockID = nextBlockID = -1;
			bShowIntroduction = FALSE;
			endType = LEVEL_END_NONE;
			targetID = -1;
		}
	};

	CCampaign();
	~CCampaign();

	//inline int GetCurrentLevel() {return m_currentLevel;}

	// Check if the given condition ends the level
	bool EndLevel(LEVEL_END_TYPE endType, long targetID = -1);
	void ForceEndLevel(std::string levelFilename, long startposID);
	void RestartLevel();

	void NextLevel();

	void InitDefaultCampaign();

	bool StartDefaultCampaign(long shipID);
	bool StartCampaign(char *filename, long shipID);

	bool LoadCampaign(const char *filename);
	bool SaveCampaign(const char *filename, bool bNewCampaign = false);

	static bool ReadCampaignName(std::string &outName, const char *filename);

	void AddBlock(SCampaignBlock b) {m_blocks.push_back(b);}

	bool SaveGame(const char *filename);
	bool LoadGame(const char *filename);

	inline int GetCurrentBlockID() {return m_currentBlockID;}
	int GetNextBlockID();
	inline std::string GetCampaignFilename() {return m_campaignFilename;}

	void DisplayEndingText();

	/**
	 * campaign data access
	 **/
	inline std::vector<SCampaignBlock> *GetBlocks() {return &m_blocks;}
	inline std::vector<long> *GetStartGadgetIDs() {return &m_startGadgetIDs;}
	inline long GetEquippedGadget(bool slot) {return m_equippedGadgets[(int)slot];}
	inline bool IsShipForced() {return m_bForceStartShip;}
	inline long GetStartShipID() {return m_startShipID;}
	inline int GetFirstBlockID() {return m_firstBlockID;}
	inline char *GetEndMessage() {return m_endMessage;}
	inline char *GetName() {return m_name;}

	inline void SetIsShipForced(bool value) {m_bForceStartShip = value;}
	inline void SetFirstBlockID(int value) {m_firstBlockID = value;}
	inline void SetStartShipID(int value) {m_startShipID = value;}
	inline void SetEquippedGadget(bool slot, int ID) {m_equippedGadgets[(int)slot] = ID;}

	std::string GetMissionObjective(int index);
	int GetMissionState(int index);
	void SetMissionObjective(std::string objective, int index);
	void SetMissionState(int state, int index);
	void ResetMission(int index);

	float GetGlobalVariable(const char *name);
	bool SetGlobalVariable(const char *name, float value);
	bool RemoveGlobalVariable(const char *name);
	bool DoesGlobalVariableExist(const char *name);
	void ResetGlobalVariables();
	std::vector<SGlobalVariable> &GetGlobalVariableList() {return m_globalVariables;}

	void GetGadgetHotkeyIDs(long *outKeys);
	void SetGadgetHotkeyIDs(long *keys);
	long GetGadgetHotkeyID(int index);
	void SetGadgetHotkeyID(int index, long ID);
	void ReportGadgetUpgrade(long oldID, long newID);

	void DisplayLevelTime();
	void DisplayCampaignTime();

	long GetPlayerStartposID() {return m_playerStartposID;}

	inline CTerminal *GetPlayerTerminal() {return &m_playerTerminal;}

	/**
	 * Enumerates through all the campaign files in the given folder,
	 * opens the files and checks their names. Filenames and names of 
	 * all valid campaigns in the folder are returned in a vector
	 * containing pairs of strings with first being the filename
	 * and second in-game name of the campaign in question
	 */
	static bool EnumerateCampaigns(std::vector<std::pair<std::string,std::string> > &campaigns, const char *folder);

protected:

	const char* GetLeveFilename(int ID);
	bool StartLevel();

	/**
	 * Campaign data
	 **/
	std::vector<SCampaignBlock> m_blocks;
	std::vector<long> m_startGadgetIDs;	// Gadgets that the player has at the beginning
	long m_equippedGadgets[2];			// Gadgets that the ship has equipped at the beginning
	long m_startShipID;					// The ship that the player has in the beginning
	int m_firstBlockID;

	bool m_bForceStartShip;				// If false then it's possible for the player to choose his own ship
										// (if the game enables it)
	char m_endMessage[2048];
	char m_name[64];

	std::string m_campaignFilename;

	CTerminal m_playerTerminal;

	/**
	 * In-game variables
	 **/
	bool m_bLevelCompleted;				// If this is true when NextLevel() is called next level is loaded, otherwise
										// the current level restarts
	bool m_bCampaignLoaded;

	std::string m_missionObjectives[MAX_MISSION_OBJECTIVES];
	int m_questStates[MAX_MISSION_OBJECTIVES];
	std::vector<SGlobalVariable> m_globalVariables;

	long m_nextStartposID;

	char m_introductionScreenBackground[128];
	int m_currentBlockID, m_currentLevelIndex;
	int m_nextLevelIndex;
	long m_playerStartposID;
	bool m_bStartLevelDefault;

	// Campaign and single level run timing
	double m_levelStartTime, m_campaignStartTime;

	// NUM_GADGET_HOTKEYS - 
	long m_gadgetHotkeyIDs[5];

	long m_shipID;

	CDialogue m_missionBriefing;		// Dialogue used to show mission objectives
	CGameSituation m_levelStart;		// The situation at the beginning of the level
};

#endif // #ifndef __CCAMPAIGN_H__
