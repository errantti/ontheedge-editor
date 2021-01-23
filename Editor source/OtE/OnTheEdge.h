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

#ifndef __CONTHEEDGE_H__
#define __CONTHEEDGE_H__

#include "Edgewalker\\Edgewalker.h"
#include "OtEScript\\ScriptManager.h"
#include "menus\\MenuManager.h"
#include "menus\\MenuDatabase.h"
#include "Controllers\\Player.h"
#include "Controllers\\EnemyAI.h"
#include "Controllers\\ControllerManager.h"
#include "Level.h"
#include "Objects\\ObjectManager.h"
#include "ParticleEffects.h"
#include "AmmoDatabase.h"
#include "Gadgets\\GadgetDatabase.h"
#include "Campaign.h"
#include "Screen\\StatusDisplay.h"
#include "Screen\\ScreenMessages.h"
#include "Screen\\HealthBar.h"
#include "Screen\\AssistantMessages.h"
#include "MultiplayerMode.h"
#include "GameSettings.h"
#include "Sound\\SoundManager.h"
#include "LightEffectManager.h"
#include "Items\\ItemManager.h"
#include "SaveGameManager.h"
#include "ScreenshotManager.h"

#define GetOTE() ((COnTheEdge*)COnTheEdge::GetApp())
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

#define MAX_PLAYERS		2

#define MAXIMUM_FRAME_TIME		0.05f

class COnTheEdge : public CEdgewalker
{
public:
	COnTheEdge();
	virtual ~COnTheEdge();

	HRESULT OnCreate();
	void OnRelease();
	void OnFlip();

	inline CLevel *GetCurrentLevel() {return m_level;}
	inline CEnemyAI *GetEnemyAIController() {return &m_AI;}
	inline CPlayer *GetPlayer(unsigned int index) {return &m_players[index];}

	CMenuManager *GetMenuManager() {return &m_menuManager;}
	CMenuDatabase *GetMenuDatabase() {return &m_menuDatabase;}
	CScriptManager *GetScriptManager() {return &m_scriptManager;}
	CObjectManager *GetObjectManager() {return &m_om;}
	CObjectManagerSceneNode *GetObjectManagerSceneNode() {return m_omsn;}
	CParticleEffects *GetParticleEffects() {return &m_particleEffects;}
	CLightEffectManager *GetLightEffectManager() {return &m_lightEffects;}
	CAmmoDatabase *GetAmmoDatabase() {return &m_ammoTemplates;}
	CGadgetDatabase *GetGadgetDatabase() {return &m_gadgets;}
	CCampaign *GetCampaign() {return &m_campaign;}
	CScreenMessages *GetScreenMessageInterface() {return &m_screenMsgs;}
	CMultiplayerMode *GetMultiplayerHandler() {return &m_multiplayer;}
	CGameSettings *GetSettings() {return &m_settings;}
	CItemManager *GetItemManager() {return &m_itemManager;}
	CControllerManager *GetControllerManager() {return &m_controllerManager;}
	CStatusDisplay *GetStatusDisplay() {return &m_status;}
	CSaveGameManager *GetSaveGameManager() {return &m_saveManager;}
	CScreenshotManager *GetScreenshotManager() {return &m_screenshotManager;}
	CSoundManager *GetSoundManager() {return &m_soundManager;}
	CAssistantMessages *GetAssistant() {return &m_assistantMessages;}

	inline void AddScreenMessage(std::string msg, float time = 4.f) {m_screenMsgs.AddMessage(msg, time);}
	inline void AddScreenMessage(std::string msg, float time, D3DCOLOR color) {m_screenMsgs.AddMessage(msg, time, color);}

	inline CShip *GetPlayerShip(unsigned int index) {return m_level->GetPlayerObject(index);}

	virtual CEnemyAI *GetEnemyAI() {return &m_AI;}

	inline CCameraSceneNode *GetSecondaryCamera() {return m_secondaryCamera;}

	inline bool IsPaused() {return m_bGamePaused;}
	inline void PauseGame() {m_bGamePaused = TRUE;}
	inline void UnpauseGame();

	HRESULT StartLevel(const char *filename, long ship1ID, long ship2ID = -1, bool bMultiplayer = FALSE, 
		CMultiplayerMode::SMultiplayerSettings *settings = NULL);
	void EndLevel(bool bFade, float fadetime);

	// Dialogue class calls these when a dialogue is initiated / ends.
	void DialogueInitiated(CDialogue *dialogue);
	void DialogueEnded();

	void ReportNewFocusTarget(ISceneNode *target);

	inline bool AreEngineObjectInitialized() {return m_bEngineObjectInitialized;}

	inline bool IsMultiplayer() {return m_bMultiplayer;}

	bool StartCampaign(long shipID);
	bool StartMultiplayer();

	inline bool IsGameRunning() {return m_bGameRunning;}
	void SetIsGameRunning(bool value) {m_bGameRunning = value;}

	bool IsGameOver() {return m_bGameOver;}
	void SetGameOver(bool value) {m_bGameOver = value;}

	void CampaignCompleted();

	bool LoadGame();
	bool SaveGame();

	bool LoadCampaign(std::string filename);
	void ResetCampaignData();

	virtual void UpdateInputDevices();

	std::string GetCurrentCampaignFolder() {return m_currentCampaignFolder;}

	bool SelectAndLoadCampaign();

	inline void SetHPBarHP(float hp, float max) {m_hpBar.SetHealth(hp, max);}

	void CloseLevel();

	// Called when the game is going on and menu has been closed and game is supposed to continue
	void ContinueGame();

	void EnableCPUGPUSynchHack(bool value) {m_bLockHackEnabled = value;}
	void EnableAutoSynchHack(bool value);

	static bool m_bParseMenuDataOnStartup;

	static float GetMenuDelayTimer() {return m_menuDelayTimer;}
	static bool CanOpenMenu() {return (m_menuDelayTimer==0)?true:false;}
	static void ReportMenuOpened();

protected:
	virtual HRESULT InitializePrimaryObjects();
	virtual HRESULT LoadCampaignData(std::string folder);

	void RenderFade();
	void UpdateFade();
	void StartFade(float fadetime, bool bIn);
	void CheckGadgetHotkeys();

	void RenderShadow();

	/**
	 * A little hack to get rid of a major synching problem
	 * between CPU and GPU resulting in unplayable game due
	 * to massive 'lagging'. Basically what this does is
	 * force the CPU to wait for the graphics card to finish
	 * processing the scene before continuing to the next
	 * frame
	 */
	void SynchronizeCPUGPUHack();

	/**
	 * Game engine objects
	 **/
	CSceneManager *m_manager;
	CGUIEnvironment *m_environment;

	CCameraSceneNode *m_cam, *m_secondaryCamera;
	CMeshSceneNode *m_ship;

	CObjectManagerSceneNode *m_omsn;
	CObjectManager m_om;

	CSoundManager m_soundManager;
	CScriptManager m_scriptManager;

	CParticleEffects m_particleEffects;
	CLightEffectManager m_lightEffects;
	CAmmoDatabase m_ammoTemplates;
	CGadgetDatabase m_gadgets;
	CItemManager m_itemManager;

	CStatusDisplay m_status;

	CMenuManager m_menuManager;
	CMenuDatabase m_menuDatabase;

	CControllerManager m_controllerManager;

	CShip *m_playerShips[MAX_PLAYERS];
	CPlayer m_players[MAX_PLAYERS];
	CEnemyAI m_AI;

	CDialogue *m_currentDialogue;

	CLevel *m_level;
	CCampaign m_campaign;
	CMultiplayerMode m_multiplayer;

	CScreenMessages m_screenMsgs;
	CGameSettings m_settings;
	CAssistantMessages m_assistantMessages;

	CHealthBar m_hpBar;

	CSaveGameManager m_saveManager;
	CScreenshotManager m_screenshotManager;

	/**
	 * Various variables
	 **/
	bool m_bGamePaused, m_bEngineObjectInitialized, m_bFading, m_bFadingIn;
	bool m_bMultiplayer, m_bGameRunning;
	bool m_bGameOver;
	bool m_bHotkey1Down;

	static float m_menuDelayTimer;

	bool m_bLockHackEnabled, m_bAutoLockHackEnabled;
	float m_fadeCounter, m_fadeTime;

	std::string m_currentCampaignFolder;
};

#endif // #ifndef __CONTHEEDGE_H__
