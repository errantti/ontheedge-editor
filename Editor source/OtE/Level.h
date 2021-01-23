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

#ifndef __CLEVEL_H__
#define __CLEVEL_H__

#include "Edgewalker\\Edgewalker.h"
#include "Objects\\Trigger.h"
#include "Objects\\Enemy.h"
#include "Objects\\Ship.h"
#include "Objects\\ObjectManager.h"
#include "WayPointSet.h"
#include "Screen\\Dialogue.h"
#include "Screen\\Terminal.h"
#include "Controllers\\Player.h"
#include "Edgewalker\\DirectX9\\IDirectXResource.h"
#include "Sound\\Sound.h"
#include <fstream>

#define LEVEL_FILE_BEGIN_IDENTIFIER		0x273810C4

enum CAMERA_TYPE {
	CAMERA_TYPE_FIXED_FOLLOW = 0,
	CAMERA_TYPE_FREE_FOLLOW,
	CAMERA_TYPE_FOLLOW_BEHIND,
	CAMERA_TYPE_FIRST_PERSON,
	CAMERA_TYPE_FPS = 0xFFFFFFFF
};

/**
 * CLevel handles a single level and stores information
 * about objects, triggers and level settings
 * Derive from IDirectXResource to be able to set 
 * lighting etc correctly after a device reset
 */
class CLevel : public IDirectXResource
{
public:
	struct SLevelFileHeader
	{
		DWORD levelFileBeginIdentifier;
		char name[32];
		char description[1024];
		char skyboxTexture[6][128];
		char tilesetFilename[128];
		float repairPodRegeneration, energyPodRegeneration;
		float gravity, density, friction;
		D3DXVECTOR3 lightDirection;
		D3DCOLOR lightColor, ambientColor;
		SObjectInstanceInfo player[2];
		POINT mapSize;
		long musicID[3];
		bool bSinglePlayerMap, bLoopMusic;
		bool bHasBossMusicTheme;
		CAMERA_TYPE cameraType;
		D3DXVECTOR3 cameraParam;
		bool bForceLevelCamera;
		DWORD numObjects, numTriggers, numWaypointSets, numDialogues, numTerminals;
		DWORD dwMapSize;
		DWORD dwObjectsSize;
		DWORD numAssistantMessages;
		DWORD dwTerminalsSize;
		DWORD dwWaypointSetSize;

		void FillDefaults()
		{
			name[0] = '\0';
			description[0] = '\0';
			tilesetFilename[0] = '\0';
			levelFileBeginIdentifier = LEVEL_FILE_BEGIN_IDENTIFIER;
			for(int i=0;i<6;i++)
				skyboxTexture[i][0] = '\0';
			repairPodRegeneration = 15.f;
			energyPodRegeneration = 100.f;
			gravity = DEFAULT_GRAVITY;
			density = 0.08f;
			friction = 0.01f;
			lightDirection = D3DXVECTOR3(0.5f, -1.f, -0.5f);
			lightColor = D3DCOLOR_RGBA(150,150,150,255);
			ambientColor = D3DCOLOR_RGBA(0, 0, 0, 255);
			::memset((void*)&player[0], 0, sizeof(SObjectInstanceInfo));
			::memset((void*)&player[1], 0, sizeof(SObjectInstanceInfo));
			player[0].pos = D3DXVECTOR3(100.f, 7.f, 100.f);
			player[1].pos = D3DXVECTOR3(50.f, 7.f, 50.f);
			cameraType = CAMERA_TYPE_FIXED_FOLLOW;
			cameraParam = D3DXVECTOR3(100.f, 250.f, 50.f);
			bForceLevelCamera = false;
			mapSize.x = mapSize.y = 16;
			bSinglePlayerMap = TRUE;
			bLoopMusic = TRUE;
			bHasBossMusicTheme = FALSE;
			musicID[0] = musicID[1] = musicID[2] = -1;
			numObjects = numTriggers = 0;
			dwMapSize = dwObjectsSize = dwTerminalsSize = dwWaypointSetSize = 0;
			numAssistantMessages = 0;
		}
	};

	CLevel(CTiledTerrain *terrain, CObjectManager *objectManager);
	virtual ~CLevel();

	void RemoveDialogues();
	void RemoveWaypointSets();
	void RemoveTerminals();

	bool LoadDefaultLevel();	// Temporary

	// NewLevel
	/// Creates an empty map of wanted size with default settings
	bool NewLevel(unsigned int x, unsigned int z, unsigned int height, const char *tilesetFilename, bool msgBoxReport = false);

	bool LoadLevel(const char *filename, bool bAddPlayer = FALSE, bool bAddEnemies = TRUE, bool bAddEnergyPods = TRUE,
		bool bAddRepairPods = TRUE, bool bAddLevelTeleports = TRUE, bool bEditor = FALSE);
	bool SaveLevel(const char *filename = NULL);
	bool ApplyHeader(SLevelFileHeader &h);
	SLevelFileHeader GetLevelFileHeader();

	inline float GetGravity() {return m_gravity;}
	inline float GetDensity() {return m_density;}
	inline float GetFriction() {return m_friction;}

	inline float SetGravity(float value) {m_gravity = value;}
	inline float SetDensity(float value) {m_density = value;}
	inline float SetFriction(float value) {m_friction = value;}

	void ApplyPhysicalValues();

	void InitiateDialogue(int dialogueID);
	void AddDialogue(CDialogue *d) {if(d != 0) m_dialogues.push_back(d);}

	void OpenTerminal(long terminalID);
	void AddTerminal(CTerminal *t) {if(t) m_terminals.push_back(t);}
	CTerminal *GetTerminal(long terminalID);

	inline string &GetName() {return m_name;}
	inline string &GetDescription() {return m_description;}
	inline string &GetFilename() {return m_filename;}
	inline string &GetSkyboxTexture(int index) {return m_skyboxTextures[index];}

	inline void SetName(string s) {m_name = s;}
	inline void SetDescription(string s) {m_description = s;}
	inline void SetFilename(string s) {m_filename = s;}
	inline void SetSkyboxTexture(string s, int index) {m_skyboxTextures[index] = s;}

	inline const bool IsLeveLoaded() const {return m_bLevelLoaded;}

	void SetPlayerPosition(int player, D3DXVECTOR3 pos);

	inline vector<CDialogue*> *GetDialogues() {return &m_dialogues;};
	inline vector<CWayPointSet*> *GetWaypointSets() {return &m_waypointSets;}
	inline vector<CTerminal*> *GetTerminals() {return &m_terminals;}
	inline vector<pair<int,string> > *GetAssistentMessages() {return &m_assistentMessages;}

	inline void AddWaypointSet(CWayPointSet *wps) {if(wps) m_waypointSets.push_back(wps);}
	CWayPointSet *CreateNewWaypointSet(std::string name);
	CWayPointSet *GetWayPointSet(int id);

	bool DisplayAssistentMessage(int id);

	CShip *CreatePlayer(int index, long templateID, CPlayer *controller, long id = -1);
	inline CShip *GetPlayerObject(int index) {return m_playerObjects[index];}

	inline bool IsShowingPlayers() {return m_bShowPlayerShips;}
	inline bool IsShowingStartingPositions() {return m_bShowStartingPositions;}

	inline void SetShowPlayers(bool value) {m_bShowPlayerShips = value;}
	inline void SetShowStartingPositions(bool value) {m_bShowStartingPositions = value;}

	inline bool IsSinglePlayer() {return !m_bMultiplayer;}
	void SetIsSinglePlayer(bool value) {m_bMultiplayer = !value;}

	inline float GetEnergyPodRegeneration() {return m_energyPodRegeneration;}
	inline float GetRepairPodRegeneration() {return m_repairPodRegeneration;}

	inline string &GetTilesetFilename() {return m_tilesetFilename;}
	inline void SetTilesetFilename(string filename) {m_tilesetFilename = filename;}

	inline int GetNumEnemies() {return m_enemyCount;}

	void PlayRandomBackgroundMusic();

	void SetLevelDirectory(std::string folder) {m_defaultDirectory = folder;}
	std::string GetLevelDirectory() {return m_defaultDirectory;}

	void RestrictPlayersToGameArea();

	void CloseLevel();

	void OnDeviceLost();
	void OnDeviceReset();

	int GetNewWaypointSetID();

	void SetPlayerMeshes();

	CTiledTerrain *GetTerrainRef() {return m_terrain;}

protected:
	friend class CShip;
	friend class CEnemy;

	void SetLevelLighting();
	void SetLevelSkybox();

	void ReportShipRemoval(CShip *ship);
	void ReportEnemyRemoval(CEnemy *enemy);

	void WriteScriptFilenames(std::fstream &f);
	bool SkipScriptFilenames(std::fstream &f);

	CTiledTerrain *m_terrain;
	CObjectManagerSceneNode *m_scnManager;
	CObjectManager *m_om;

	vector<CDialogue*> m_dialogues;
	vector<CTerminal*> m_terminals;
	vector<CWayPointSet*> m_waypointSets;
	vector<pair<int,string> > m_assistentMessages;

	float m_gravity, m_density, m_friction;
	float m_energyPodRegeneration, m_repairPodRegeneration;

	string m_name, m_filename, m_description, m_skyboxTextures[6];

	SObjectInstanceInfo m_playerPositions[2];

	bool m_bSinglePlayerMap, m_bLevelLoaded, m_bMultiplayer;

	D3DCOLOR m_lightColor, m_ambientColor;
	D3DXVECTOR3 m_lightDirection;

	CAMERA_TYPE m_cameraType;
	D3DXVECTOR3 m_cameraParam;
	bool m_bForceLevelCamera;

	long m_musicID[3];
	bool m_bLoopMusic, m_bHasBossMusic;

	string m_defaultDirectory;
	string m_tilesetFilename;

	int m_enemyCount;

	bool m_bShowPlayerShips, m_bShowStartingPositions;

	CLightSceneNode *m_levelLight;
	CSkyBoxSceneNode *m_levelSkybox;

	CSound *m_music[3];

	/// Pointer to 'flag' meshes, usually these won't be displayed in game
	CMeshSceneNode *m_playerMesh[2];

	/// Pointers to ingame ship objects
	CShip *m_playerObjects[2];
};

#endif // #ifndef __CLEVEL_H__
