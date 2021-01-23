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
#include "menus\\MenuFileParser.h"
#include "Edgewalker\\SimpleStaticProfiler.h"
#include <sys\stat.h>

// TODO
#include "Edgewalker\\Scene\\SmoothFollowCamera.h"

bool COnTheEdge::m_bParseMenuDataOnStartup = false;
float COnTheEdge::m_menuDelayTimer = 0.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COnTheEdge::COnTheEdge() : m_cam(NULL), m_secondaryCamera(NULL), m_level(NULL), m_bGamePaused(FALSE),
	m_currentDialogue(NULL), m_bEngineObjectInitialized(FALSE), m_bMultiplayer(FALSE), m_bGameRunning(FALSE),
	m_bFading(FALSE), m_bLockHackEnabled(false), m_bAutoLockHackEnabled(true), m_bGameOver(false)
{
	for(int i=0;i<MAX_PLAYERS;i++)
		m_playerShips[i] = NULL;
}

COnTheEdge::~COnTheEdge()
{
	if(m_level)
	{
		delete m_level;
		m_level = NULL;
	}
}

HRESULT COnTheEdge::OnCreate()
{
	CResourceManager::GetManager().SetDefaultColorKey(D3DCOLOR_RGBA(255, 0, 255, 255));
	CMesh::SetTextureFolder("textures\\");
	LoadEffect("Shaders\\shadowmap.fx");

	if(FAILED(InitializePrimaryObjects()))
	{
		CPrinter::log("Initializing primary engine objects failed", LOG_ERROR);
		return E_FAIL;
	}

	// Load the floating text font and enable it
	CResourceManager::GetManager().SetTextureColorkey(0xFF000000);
	CTexture *floatingTextFont = CResourceManager::GetManager().GetTexture("textures\\asfont.tga");
	CResourceManager::GetManager().RestoreDefaultColorkey();
	CFloatingText::SetTexture(floatingTextFont);
	char charmap[] = {
		'_', '^', ':', ')', '(',
		'>', '<', '.', 'o', 'O',
		'?', '=', '-', '/', '\\',
		'|'
	};
	CFloatingText::SetCharmap(charmap, 16);
	CFloatingText::SetFontSize(D3DXVECTOR2(0.171875f, 0.25f));

	/**
	 * Set a tiled terrain scene node
	 **/
	CTiledTerrain *terrain = m_manager->AddTiledTerrain(NULL);
	if(terrain == NULL)
		return E_FAIL;
	m_omsn->AddExternalCollider(terrain);
	m_om.SetTerrain(terrain);

	m_level = new CLevel(terrain, &m_om);

	// Disable ambient lighting
	SetAmbientLight(D3DCOLOR_RGBA(0,0,0,255));

	// Preload interface stuff
	CHealthBar::LoadImages("imgs\\healthbar\\enemy_stats.png", "imgs\\healthbar\\enemy_shields.png");
	if(m_status.LoadDefaultGraphics() == FALSE)
		CPrinter::log("Failed loading player status display graphics", LOG_ERROR);

	// Set default player controls
	//SPlayerControlSet controlSet;
	//controlSet.FillDefaulKeyboard1();
	m_players[0].SetControlSet(m_settings.GetControlSet(0));
	//controlSet.FillDefaultKeyboard2();
	m_players[1].SetControlSet(m_settings.GetControlSet(1));


	/**
	 * If the user gave the order to parse menu data from
	 * a text file, do it and save the result into a 
	 * binary database
	 **/
	if(m_bParseMenuDataOnStartup)
	{
		CMenuFileParser parser;
		if(!parser.ParseFile("menus.txt", m_menuDatabase.GetMenuData()))
			CPrinter::logError("Failed parsing menu data");
		else
		{
			CPrinter::log("Menu data file parsed succesfully");
			if(!m_menuDatabase.SaveDatabase("data\\menus.dtb"))
				CPrinter::logError("Failed saving menu database");
			else
				CPrinter::log("Menu database saved succesfully");
		}
	}

	// Load menu database
	if(!m_menuDatabase.LoadDatabase("data\\menus.dtb"))
		CPrinter::logError("Failed loading menu database");
	else
		CPrinter::log("Menu database loaded succesfully");

	m_environment->SetCursorVisibility(TRUE);
	m_menuManager.OpenMenu(MENU_ID_MAIN);/**/

	if(!SelectAndLoadCampaign())
	{
		CPrinter::logError("There aren't any campaigns to play");
		return E_FAIL;
	}

	m_settings.SetKeyboardObject(m_keyboard);
	m_settings.SetJoystickObject(m_joystick);

	return S_OK;
}

HRESULT COnTheEdge::InitializePrimaryObjects()
{
	m_manager = GetSceneManager();
	m_environment = GetGUIEnvironment();
	CResourceManager::GetManager().SetDevice(GetDevice());

	if(m_manager == NULL)
		return E_FAIL;

	//m_menuManager.SetEnvironment(m_environment);
	m_menuManager.InitializeMenus();

	/**
	 * Add an object manager scene node to the scene
	 **/
	m_omsn = m_manager->AddObjectManager(40, 800, 800, 100);

	m_om.SetObjectManagerSceneNode(m_omsn);

	m_scriptManager.InitializeScriptEngine();

	if(m_soundManager.InitializeSoundSystem() == FALSE)
	{
		CPrinter::log("Failed initializing sound system");
	}
	CSoundSettings::InitializeSettings();

	m_soundManager.SetEffectVolume(m_settings.GetSetting(GO_SOUND_VOLUME));
	m_soundManager.SetMusicVolume(m_settings.GetSetting(GO_MUSIC_VOLUME));

	m_controllerManager.Initialize();

	m_bEngineObjectInitialized = TRUE;

	return S_OK;
}

HRESULT COnTheEdge::LoadCampaignData(std::string folder)
{
	std::string filename = folder + "peffects.dtb";
	if(m_particleEffects.LoadDatabase(filename.c_str()) == FALSE)
	{
		CPrinter::log("Failed loading particle effects", LOG_ERROR);
	}
	else
	{
		CPrinter::log("Particle effects loaded succesfully");
	}

	filename = folder + "lighteffects.dtb";
	if(m_lightEffects.LoadDatabase(filename.c_str()) == FALSE)
	{
		CPrinter::log("Failed loading light effects", LOG_ERROR);
	}
	else
	{
		CPrinter::log("Light effects loaded succesfully");
	}

	filename = folder + "sounds.dtb";
	if(m_soundManager.LoadDatabase(filename) == FALSE)
	{
		CPrinter::log("Failed loading sound database");
	}
	else
	{
		CPrinter::log("Sound database loaded succesfully");
	}

	filename = folder + "globalScripts.dtb";
	if(m_scriptManager.LoadScriptDatabase(filename, false))
	{
		CPrinter::log("Global scripts succesfully loaded");
	}
	else
	{
		CPrinter::logError("Failed loading global scripts");
	}/**/
	// Update player terminal scripts now that the global scripts have been loaded
	m_campaign.GetPlayerTerminal()->UpdateCommands();

	filename = folder + "ammos.dtb";
	if(m_ammoTemplates.LoadDatabase(filename.c_str()) == FALSE)
	{
		CPrinter::log("Failed loading ammo templates", LOG_ERROR);
	}
	else
	{
		CPrinter::log("Ammo data loaded succesfully");
	}
	m_ammoTemplates.ConstructReferences();

	filename = folder + "gadgets.dtb";
	if(m_gadgets.LoadDatabase(filename.c_str()) == FALSE)
	{
		CPrinter::log("Failed loading gadget templates", LOG_ERROR);
	}
	else
	{
		CPrinter::log("Gadget data loaded succesfully");
	}

	/**
	 * Load object templates
	 **/
	m_om.LoadDefaultDatabases(folder);

	filename = folder + "items.dtb";
	if(m_itemManager.LoadDatabase(filename.c_str()))
	{
		CPrinter::log("Item database loaded succesfully");
	}
	else
	{
		CPrinter::logError("Failed loading items database");
	}

	return S_OK;
}

void COnTheEdge::ResetCampaignData()
{
	m_particleEffects.ClearDatabase();
	m_lightEffects.ClearDatabase();
	m_soundManager.ClearDatabase();
	m_scriptManager.ClearDatabase();
	m_ammoTemplates.ClearDatabase();
	m_gadgets.UnloadTemplates();
	m_om.ClearAllTemplateDatabases();
	m_itemManager.ClearDatabase();

	CPrinter::log("Old campaign data unloaded");
}

bool COnTheEdge::LoadCampaign(std::string filename)
{
	int index = (int)filename.find(".cpg");
	int len = (int)filename.size();
	if(index < len - 4)
	{
		CPrinter::logError("LoadCampaign: failed loading campaign: incorrect campaign file identifier");
		return false;
	}

	std::string campaignFolder(filename);
	campaignFolder.replace(len-4, len-1, "\\");

	if(m_campaign.GetCampaignFilename() == filename)
	{
		// If the campaign is already loaded, don't do anything
		return true;
	}
	else if(m_campaign.GetCampaignFilename() != "")
	{
		// If there's already another campaign loaded then its data has to
		// be cleaned before loading the new campaign
		ResetCampaignData();
	}

	m_level->SetLevelDirectory(campaignFolder + "levels\\");

	if(m_campaign.LoadCampaign(filename.c_str()) == FALSE)
	{
		CPrinter::log("Failed loading campaign", LOG_ERROR);
		return false;
	}

	if(FAILED(LoadCampaignData(campaignFolder + "data\\")))
	{
		CPrinter::log("Initializing game databases failed", LOG_ERROR);
		return false;
	}

	int corePathEnd = (int)campaignFolder.find("\\");
	std::string savesFolder(campaignFolder);
	savesFolder.replace(0, corePathEnd, "saves");
	m_saveManager.SetSaveFolder(savesFolder);

	// Create the campaign specific saves folder if it doesn't exist
	CreateDirectory(savesFolder.c_str(), NULL);

	// and create the autosaves subfolder
	std::string autosavesFolder = savesFolder + "autosaves\\";
	CreateDirectory(autosavesFolder.c_str(), NULL);

	std::string msg = "Campaign ";
	msg += filename;
	msg += " succesfully loaded.";
	CPrinter::log(msg.c_str());

	m_currentCampaignFolder = campaignFolder;

	return true;
}

bool COnTheEdge::SelectAndLoadCampaign()
{
	// Let's first attempt to load the default campaign specified in settings.ini
	std::string name;
	if(CCampaign::ReadCampaignName(name, m_settings.GetDefaultCampaign().c_str()))
	{
		if(!LoadCampaign(m_settings.GetDefaultCampaign()))
		{
			return false;
		}
		return true;
	}/**/

	// If that didn't work then enumerate through all the available campaigns and
	// load the first of those
	std::vector<std::pair<std::string,std::string> > campaigns;
	CCampaign::EnumerateCampaigns(campaigns, "campaigns\\");

	if(campaigns.empty())
	{
		CPrinter::logError("There are no campaigns to play!");
		return false;
	}

	if(!LoadCampaign(campaigns[0].first))
	{
		return false;
	}

	return true;
}

void COnTheEdge::OnRelease()
{
	if(m_level)
	{
		delete m_level;
		m_level = NULL;
	}
}

void COnTheEdge::OnFlip()
{
	//static CSimpleStaticProfiler profiler("renderscene.txt"), profiler2("update.txt");

	//int d[]={1,0};int *i=&d[0];++*(&(++*(&(++*++i)+1))+1);--*--i;cout<<d[0]<<d[1];

	float fFrameTime = GetFrameTime();
	double time = m_Timer.GetTime();

	// Prevent too long delays from screwing the engine
	if(fFrameTime > MAXIMUM_FRAME_TIME)
		fFrameTime = MAXIMUM_FRAME_TIME;

	if(BeginScene(TRUE, TRUE, D3DCOLOR_RGBA(50, 50, 50, 255)) == FALSE)
		return;
	
	//profiler.StartTiming();
	m_manager->RenderAll();
	//profiler.StopTiming();

	BeginSprite();
	if(!m_bFading)
	{
		if(m_level->IsLeveLoaded())
			m_status.Render();
		//if(m_currentDialogue)
		//	m_currentDialogue->Render();
		m_hpBar.Render();
	}
	m_environment->DrawAll();

	// temp code
	//static bool end = FALSE;
	rect<int> r(0, GetWidth(), 0, GetHeight());
	if(m_level->GetPlayerObject(0) == NULL  && !m_bFading && !m_menuManager.IsMenuOpen() && !m_bMultiplayer && m_bGameRunning)
	{
		//end = TRUE;
		static CTexture *backg = NULL;
		if(backg == NULL)
			backg = CResourceManager::GetManager().GetTexture("imgs\\restartBackg.png");
		Draw2DImage(backg, D3DXVECTOR2((r.right >> 1) - 256, (r.bottom >> 1) - 32), 0xC7FFFFFF);
		m_bGameOver = true;
		std::string str("Press ");
		str += m_settings.GetControlSymbolOrCode(m_settings.GetGeneralKeyboardControlSet().generalOk);
		str += " to restart the level";
		m_environment->GetDefaultFont()->Print(str.c_str(), r, D3DCOLOR_RGBA(255, 255, 255, 255), DT_CENTER | DT_VCENTER);
	}
	/*else if(CEnemy::GetEnemyCount() == 0 && !m_bMultiplayer && m_bGameRunning)
	{
		//end = TRUE;
		m_bGameOver = true;
		//m_environment->GetDefaultFont()->Print("Level completed! Press Enter to restart the level", r, D3DCOLOR_RGBA(255, 255, 255, 255), DT_CENTER | DT_VCENTER);
	}*/
	//

	RenderFade();
	m_screenMsgs.Render();

	EndSprite();

	//profiler2.StartTiming();
	//if(!m_bGamePaused && !m_bFading)
	if(!m_bGamePaused)
	{
		m_level->RestrictPlayersToGameArea();
		m_manager->UpdateAll(fFrameTime);
		m_scriptManager.UpdateTimers(fFrameTime);
		m_status.Update(fFrameTime);
	}
	else
		UpdateInputDevices();
	m_screenMsgs.Update(fFrameTime);
	m_assistantMessages.Update(fFrameTime);

	m_environment->Update();
	m_soundManager.Update(fFrameTime);
	//profiler2.StopTiming();

	//if(m_currentDialogue && !m_bFading)
	//	m_currentDialogue->Update(fFrameTime);

	if(!m_bGamePaused && m_bGameRunning)
	{
		// Set updating on (the objects aren't removed instantly but when RemoveMarkedObjects is called)
		// to prevent objects from being deleted when they are still being updated
		m_omsn->SetIsUpdating(TRUE);
		m_om.KillObjectsBelowSurface();
		m_omsn->SetIsUpdating(FALSE);
		m_omsn->RemoveMarkedObjects();

		m_hpBar.Update(fFrameTime);

		CheckGadgetHotkeys();

		if(m_bMultiplayer)
			m_multiplayer.Update(fFrameTime);

		if(m_keyboard->GetButtonDown(m_settings.GetGeneralKeyboardControlSet().quickLoad))
			m_saveManager.LoadAutosave();

		//Sleep(1);
	}
	else
		Sleep(10);
	//Sleep(10);

	UpdateFade();

	m_menuDelayTimer -= fFrameTime;
	if (m_menuDelayTimer < 0.f)
		m_menuDelayTimer = 0.f;

	EndScene();

	if(m_bLockHackEnabled)
		SynchronizeCPUGPUHack();
	else
	{
		if(m_bAutoLockHackEnabled)
		{
			// Check whether there have been long enough spikes in frame duration
			if(fFrameTime > 0.06f && m_bGameRunning && !m_bGamePaused)
			{
				//CPrinter::log("FPS problems: CPU-GPU synchronization hack enabled");
				//m_bLockHackEnabled = true;
			}
		}
	}/**/

	/**
	 * TEMPORARY
	 **/

	static bool bd = false;
	if(m_keyboard->GetButtonDown(m_settings.GetGeneralKeyboardControlSet().takeScreenshot) && !bd)
	{
		bd = TRUE;
		m_screenshotManager.TakeScreenshot();
	}
	if(!m_keyboard->GetButtonDown(m_settings.GetGeneralKeyboardControlSet().takeScreenshot))
		bd = false;

	static bool bEscDown = false, bJustBackToGame = false;

	if(!m_menuManager.IsMenuOpen() && !m_bFading)
	{
		static bool hd = FALSE;
		if(KEY_DOWN('H') && !hd)
		{
			hd = true;
			CSimpleStaticProfiler::ProfilingEnabled = !CSimpleStaticProfiler::ProfilingEnabled;
		}
		if(!KEY_DOWN('H'))
			hd = false;
		static bool enter=FALSE, be=FALSE;
		if(m_settings.IsOpenInventoryDown() && !be && m_currentDialogue == NULL && 
			!m_bMultiplayer && m_players[0].GetShip())
		{
			//CScript *s = m_scriptManager.GetScript("TestScript");
			//if(s) CScriptManager::Execute(s, SCRIPT_CALLER_NONE, 0, NULL);
			if(m_menuManager.IsMenuOpen())
			{
				if(m_menuManager.GetCurrentMenuID() == MENU_ID_INVENTORY)
				{
					m_menuManager.CloseMenu();
					UnpauseGame();
				}
			}
			else
			{
				if (CanOpenMenu())
				{
					PauseGame();
					m_menuManager.OpenMenu(MENU_ID_INVENTORY, TRUE);
					ReportMenuOpened();
				}
			}
			be = TRUE;
		}
		if(!m_settings.IsOpenInventoryDown())
			be = FALSE;
		if(m_settings.IsGeneralOkDown() && !enter && !m_menuManager.IsMenuOpen())
		{
			if(m_bGameOver)
			{
				m_bGameOver = FALSE;
				// The level will be restarted becouse completion criteria will not have been met
				EndLevel(TRUE, 1.5f);
			}
			//m_bGamePaused = !m_bGamePaused;
			enter = TRUE;
		}
		if(!m_settings.IsGeneralOkDown())
			enter = FALSE;
		if(m_settings.IsOpenPlayerTerminalDown())
		{
			m_campaign.GetPlayerTerminal()->OpenTerminal();
		}
		if(m_settings.IsMenuBackDown() && !bEscDown && !bJustBackToGame && CanOpenMenu())
		{
			bEscDown = TRUE;
			m_menuManager.OpenMenu(MENU_ID_MAIN);
			ReportMenuOpened();
			m_bGamePaused = TRUE;
			// Remove the ending text if it's open so it won't overlap the menu
			m_screenMsgs.RemoveDetailedMessage(25252);
		}
		bJustBackToGame = false;
	}
	else
		bJustBackToGame = true;

	if(!m_settings.IsMenuBackDown())
		bEscDown = false;
	else
		bEscDown = true;
	/***/

	if(m_settings.GetSetting(GO_FPS_LIMIT_ENABLED))
	{
		double dFrameTime = m_Timer.GetTime() - time;
		while(dFrameTime < 0.01)
		{
			Sleep(1);
			dFrameTime = m_Timer.GetTime() - time;
		}/**/
	}
}

/**
 * A little hack to get rid of a major synching problem
 * between CPU and GPU resulting in unplayable game due
 * to massive 'lagging'. Basically what this does is
 * force the CPU to wait for the graphics card to finish
 * processing the scene before continuing to the next
 * frame
 * MAJOR DISASTER - completely messed device restoration procedure
 * Disabled but left here for documentation and possible later use
 */
void COnTheEdge::SynchronizeCPUGPUHack()
{
	/*IDirect3DSurface9 *s;
	D3DLOCKED_RECT lr;
	RECT rec;
	rec.bottom = 2; rec.top = 0; rec.left = 0; rec.right = 2;
	m_pD3DDevice->GetRenderTarget(0, &s);
	s->LockRect(&lr, &rec, D3DLOCK_READONLY);
	s->UnlockRect();*/
}

void COnTheEdge::EnableAutoSynchHack(bool value)
{
	m_bAutoLockHackEnabled = value;
}

void COnTheEdge::CheckGadgetHotkeys()
{
	BYTE *hotkeys = m_settings.GetGadgetShortcuts();
	for(int i=0;i<NUM_GADGET_HOTKEYS; ++i)
		if(m_keyboard->GetButtonDown(hotkeys[i]))
		{
			if(m_bHotkey1Down)
				return;
			long id = m_campaign.GetGadgetHotkeyID(i);
			if(id != -1)
			{
				m_players[0].EquipGadget(id, true);
			}
			return;
		}
	m_bHotkey1Down = false;
}

void COnTheEdge::RenderShadow()
{
	if(BeginScene(TRUE, TRUE, D3DCOLOR_RGBA(50, 50, 50, 255)) == FALSE)
		return;
	
	//profiler.StartTiming();
	m_manager->RenderShadow();

	EndScene();
}

void COnTheEdge::ReportNewFocusTarget(ISceneNode *target)
{
	if(m_cam == NULL)
		return;
	if(target == NULL)
	{
		// TODO
		return;
	}

	m_cam->Detach();
	target->AddChild(m_cam);
}

HRESULT COnTheEdge::StartLevel(const char *filename, long ship1ID, long ship2ID, bool bMultiplayer, 
							   CMultiplayerMode::SMultiplayerSettings *settings)
{
	/**
	 * Load default level
	 **/

	if(m_bEngineObjectInitialized == FALSE)
	{
		CPrinter::log("Critical internal error: primary engine objects not initialized", LOG_ERROR);
		return E_FAIL;
	}

	if(m_level == NULL || filename == NULL)
	{
		CPrinter::log("Internal error: level object pointer zero or no filename provided", LOG_ERROR);
		return E_FAIL;
	}

	bool bLoadSuccesful;
	if(bMultiplayer)
	{
		if(settings)
			bLoadSuccesful = m_level->LoadLevel(filename, FALSE, settings->bEnemies, settings->bEnergyPods, settings->bRepairPods, FALSE);
		else
			bLoadSuccesful = m_level->LoadLevel(filename, FALSE, FALSE, FALSE, FALSE, FALSE);
	}
	else
	{
		bLoadSuccesful = m_level->LoadLevel(filename);
		//bLoadSuccesful = true;
		// TODO: remove
		//CTileManager::GetManager()->FillDefaultTileData();
		//CTileManager::GetManager()->LoadTileResources();
		//m_level->LoadDefaultLevel();
	}

	m_bGameOver = false;

	if(bLoadSuccesful == FALSE)
	{
		CPrinter::log("Failed loading level", LOG_ERROR);
		return E_FAIL;
	}
	m_bGameRunning = TRUE;

	/**
	 * Add a player ship
	 **/
	CShip *shipObject;
	//m_players[0].Reset();
	shipObject = m_level->CreatePlayer(0, ship1ID, &m_players[0], 201);
	if(shipObject == NULL)
	{
		CPrinter::log("Failed creating player 1 ship object", LOG_ERROR);
		return E_FAIL;
	}
	m_playerShips[0] = shipObject;

	// Set a default camera to the scene
	SPlayerCameraSettings cams(m_settings.GetPlayerCameraSettings());

	if(m_secondaryCamera == NULL)
	{
		m_secondaryCamera = m_manager->AddCameraFPSSceneNode(NULL, -1, D3DXVECTOR3(100.f, 28.f, -100.f), 0.f, 0.0f);
		m_secondaryCamera->SetNearValue(cams.nearClipPlane);
		m_secondaryCamera->SetFarValue(cams.farClipPlane);
		m_secondaryCamera->CalculateProjectionMatrix();
	}

	m_scriptManager.HideVisualTimer();
	m_scriptManager.RemoveTimers();

	m_bMultiplayer = bMultiplayer;
	if(m_bMultiplayer)
	{
		// Create ship for the second player
		shipObject = m_level->CreatePlayer(1, ship2ID, &m_players[1], 202);

		//e = (CParticleSystemSceneNode*)(m_particleEffects.StartEffect(shipObject, 3));
		//shipObject->SetEngineParticleEmitter(e);

		// Create a special camera
		// CMultiplayerMode creates and updates the camera in multiplayer
	}
	else
	{
		/**
		 * Add a camera to the scene set following the player ship
		 **/
		if(m_cam != NULL)
		{
			m_cam->Remove();
			m_cam = NULL;
		}

		//m_cam = m_manager->AddCameraFollowSceneNode(shipObject, -1, D3DXVECTOR3(-50.f, 250.f, 100.f));
		if(cams.bUseFollowCam)
		{
			m_cam = new CSmoothFollowCamera(shipObject, m_sceneManager, -1, cams.distance, cams.angle, 0.f);
			m_cam->SetNearValue(cams.nearClipPlane);
			m_cam->SetFarValue(cams.farClipPlane);
			m_cam->CalculateProjectionMatrix();
			//m_cam = new CSmoothFollowCamera(shipObject, m_sceneManager, -1, cams.distance, 1.1f, 0.f);
			((CSmoothFollowCamera*)m_cam)->SetAngleSmoothingCoefficient(cams.smoothingFactor);
		}
		else
		{
			m_cam = m_manager->AddCameraFollowSceneNode(shipObject, -1, m_level->GetLevelFileHeader().cameraParam);
			m_cam->SetNearValue(cams.nearClipPlane);
			m_cam->SetFarValue(cams.farClipPlane);
			m_cam->CalculateProjectionMatrix();
			if(m_level->GetLevelFileHeader().cameraType == CAMERA_TYPE_FIRST_PERSON)
				((CCameraFollowSceneNode*)m_cam)->SetRotateWithTarget(true);
		}
		m_manager->SetActiveCamera(m_cam);
	}

	if(m_saveManager.Autosave())
		m_screenMsgs.AddMessage("Game autosaved");
	else
		m_screenMsgs.AddMessage("Failed autosaving");

	// Report menu opened so that player cannot open inventory before fade starts
	ReportMenuOpened();

	StartFade(1.5f, TRUE);

	// Turn menu music off
	m_soundManager.ReportGameUnPaused();

	return S_OK;
}

/**
 * Immediately closes the current level
 **/
void COnTheEdge::CloseLevel()
{
	m_level->CloseLevel();
	m_physicsEngine->Reset();

	m_cam = NULL;	// The player-follow camera is deleted in the object manager reset
	m_players[0].SetShip(NULL);
	m_players[1].SetShip(NULL);

	m_currentDialogue = NULL;
	m_bGameRunning = FALSE;
	m_assistantMessages.Reset();

	// Reset the scene manager to remove trash objects like particle emitter that haven't been
	// automatically removed
	//m_sceneManager->RemoveNonSpecialObjects();
	/*if(FAILED(InitializePrimaryObjects()))
	{
		CPrinter::log("Failed reinitializing primary engine objects", LOG_ERROR);
		PostQuitMessage(0);
	}*/
}

void COnTheEdge::EndLevel(bool bFade, float fadetime)
{
	if(fadetime < 0.1f || !bFade)
	{
		CloseLevel();
		m_bFading = FALSE;
		m_hpBar.ForceDeactivate();
		if(!m_bMultiplayer)
			m_campaign.NextLevel();
		else
			m_multiplayer.EndMatch();
		return;
	}

	StartFade(fadetime, FALSE);
}

void COnTheEdge::StartFade(float fadetime, bool bIn)
{
	m_fadeCounter = m_fadeTime = fadetime;
	m_bFading = TRUE;
	m_bFadingIn = bIn;
	m_bGamePaused = TRUE;
	if(bIn && m_level->IsLeveLoaded())
		m_sceneManager->OnPostRender(GetFrameTime());
}

void COnTheEdge::RenderFade()
{
	struct fadevertex {float x,y,z,w;DWORD c;};
	fadevertex v[4];
	float width = (float)GetWidth(), height = (float)GetHeight();
	v[0].x = v[0].y = v[0].z = v[0].w = 0.f;
	v[1].x=0.f;v[1].y=height;v[1].z=v[1].w=0.f;
	v[3].x=width;v[3].y=height;v[2].z=v[2].w=1.f;
	v[2].x=width;v[2].y=0.f;v[3].z=v[3].w=0.f;

	v[0].w=v[1].w=v[2].w=v[3].w=10.f;
	v[0].z=v[1].z=v[2].z=v[3].z=10.f;

	// Set alpha value
	float fTemp = m_fadeCounter/m_fadeTime;
	if(!m_bFadingIn)
		fTemp = 1.f-fTemp;
	v[0].c=v[1].c=v[2].c=v[3].c=D3DCOLOR_RGBA(0, 0, 0, (int)(fTemp*255.f));

	if(!m_bFading)
		return;

	EnableZBuffer(FALSE);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// Render an alpha blended black quad over the whole screen
	m_pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &v, sizeof(fadevertex)); 
}

void COnTheEdge::UpdateFade()
{
	if(!m_bFading || m_menuManager.IsMenuOpen())
		return;

	m_fadeCounter -= GetFrameTime();
	if(m_fadeCounter <= 0.f)
	{
		m_bFading = FALSE;
		if(!m_currentDialogue)
			m_bGamePaused = FALSE;

		if(m_bFadingIn)
		{
		}
		else
		{
			EndLevel(FALSE, 0.f);
			return;
		}
	}
}

void COnTheEdge::DialogueInitiated(CDialogue *dialogue)
{
	if(dialogue == NULL)
		return;

	// Pause game and set current dialogue
	m_bGamePaused = TRUE;
	m_currentDialogue = dialogue;
}

void COnTheEdge::DialogueEnded()
{
	m_bGamePaused = FALSE;
	m_currentDialogue = NULL;
}

void COnTheEdge::ContinueGame()
{
	if(!m_bGameRunning)
		return;
	if(!m_bFading && m_currentDialogue == NULL)
		UnpauseGame();
}

void COnTheEdge::UnpauseGame()
{
	m_bGamePaused = FALSE;
	m_soundManager.ReportGameUnPaused();
}

bool COnTheEdge::StartCampaign(long shipID)
{
	m_bMultiplayer = FALSE;
	if(m_campaign.StartDefaultCampaign(shipID) == FALSE)
	{
		CPrinter::log("Failed starting campaign", LOG_ERROR);
		return FALSE;
	}
	m_bGameRunning = TRUE;
	m_environment->SetCursorVisibility(FALSE);
	return TRUE;
}

bool COnTheEdge::StartMultiplayer()
{
	m_currentDialogue = NULL;
	return m_multiplayer.StartQuickMatch();
}

void COnTheEdge::CampaignCompleted()
{
	m_bGameRunning = FALSE;
}

bool COnTheEdge::LoadGame()
{
	if(!m_bGameRunning)
		return FALSE;

	return m_campaign.SaveGame("save001.svg");
}

bool COnTheEdge::SaveGame()
{
	if(m_campaign.LoadGame("save001.svg"))
	{
		m_bGameRunning = TRUE;
		return TRUE;
	}
	return FALSE;
}

void COnTheEdge::UpdateInputDevices()
{
	if(m_settings.GetSetting(GO_MOUSE_ENABLED))
	{
		if(m_mouse)
			m_mouse->Update();
	}
	else
	{
		POINT pos;
		pos.x = 0;
		pos.y = 0;
		m_mouse->SetCursorPosition(pos);
		m_environment->SetCursorVisibility(false);
	}
	if(m_keyboard)
		m_keyboard->Update();
	if(m_joystick)
		m_joystick->Update();

	// Don't let the cursor go outside of the window
#ifndef _EDITOR
	if(IsActive())
	{
		SetCursorPos(m_rcScreen.left + 400, m_rcScreen.top + 300);
	}
#endif
}

void COnTheEdge::ReportMenuOpened()
{
	m_menuDelayTimer = 0.5f;
}