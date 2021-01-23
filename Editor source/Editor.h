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

#ifndef __CEDITOR_H__
#define __CEDITOR_H__

#include "OtE\\OnTheEdge.h"
#include "resource.h"

#include "Databases\\Handlers\\DynamicObjectDatabase.h"
#include "Databases\\Handlers\\StaticObjectDatabase.h"
#include "Databases\\Handlers\\ShipDatabase.h"
#include "Databases\\Handlers\\EnemyDatabase.h"
#include "Databases\\Handlers\\TileDatabase.h"
#include "Databases\\Handlers\\ParticleEffectDatabase.h"
#include "Databases\\Handlers\\GadgetDatabaseHandler.h"
#include "Databases\\Handlers\\AmmoDatabaseHandler.h"
#include "Databases\\Handlers\\GlobalScriptsHandler.h"
#include "Databases\\Handlers\\SoundDatabaseHandler.h"
#include "Databases\\Handlers\\LightEffectDatabaseHandler.h"
#include "Databases\\Handlers\\ComponentDatabaseHandler.h"
#include "Databases\\Handlers\\QuestItemDatabaseHandler.h"
#include "Databases\\Handlers\\ShipPatchDatabaseHandler.h"
#include "Tools\\DialogueDatabase.h"
#include "Tools\\AssistantMsgDatabase.h"
#include "Tools\\TerminalDatabase.h"
#include "Tools\\WaypointSetDatabase.h"
#include "Tools\\CampaignEditor.h"

#include "SelectionMeshSceneNode.h"

#define MIN_LEVEL_DIMENSION		8
#define MAX_LEVEL_DIMENSION		512

#define AskLevelSaveContinue() ::MessageBox(GetWindow(), "The modifications to current level will be lost if you haven't saved it. Are you sure you want to continue?", "OtE editor", MB_YESNO)
#define ErrorMsgBox(msg) ::MessageBox(GetWindow(), msg, "OtE Editor error!", MB_OK)

#define GetEditorApp() ((CEditor*)CEditor::GetApp())

enum ACTION_STATE {
	ACTION_STATE_NONE,
	ACTION_STATE_DRAG,
	ACTION_STATE_ADD_OBJECT,
	ACTION_STATE_ADD_TRIGGER,
	ACTION_STATE_RESIZE_TRIGGER,
	ACTION_STATE_ADD_PLAYER,
	ACTION_STATE_ADD_WAYPOINT
};

class CEditor : public COnTheEdge
{
public:
	CEditor();
	virtual ~CEditor();

	inline bool IsLeveLoaded() const {return m_bLevelLoaded;}

	virtual HRESULT OnCreate();
	virtual void OnRelease();
	virtual void OnFlip();

	virtual void OnKeyDown(DWORD dwKey);

	bool NewLevel(int size, int height, const char *tilesetFilename);

	// We don't wanna enemies moving anywhere in editor...
	virtual CEnemyAI *GetEnemyAI() {return NULL;}

	virtual CDynamicObjectDatabase *GetDynamicObjectDatabase() {return &m_dynamicObjectDatabase;}

	void Deselect();

protected:
	HRESULT InitializePrimaryObjects();
	HRESULT InitializeEditor();

	/**
	 * Opens file load dialog for user to select campaign to edit
	 * Failure to choose will cause editor to close. If the user
	 * select file that doesn't exist he is promted whether he
	 * wants to create a new campaign with the given name.
	 */
	bool SelectCampaign(std::string &outFilename, bool &bOutNewCampaign);

	void RenderEditorStateInfo();

	void UpdateView();
	void RenderEditor();

	void UpdateTileEditMode();
	void UpdateObjectEditMode();

	void GetViewRay(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir);
	D3DXVECTOR3 GetNormalizedViewRay();
	D3DXVECTOR3 GetXZPlaneRayCollision(D3DXVECTOR3 &rayPos, D3DXVECTOR3 &rayDir);
	D3DXVECTOR3 GetXZPlaneViewRayCollision();

	virtual BOOL OnMessage(UINT iMessage, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	void ChangeTileMode();
	void ChangeViewMode();
	void ChangeTriggerRenderMode();

	void OpenInsertPlayer();
	void OpenInsertEnemy();
	void OpenInsertDynamicObject();
	void OpenInsertStaticObject();
	void OpenInsertTrigger();
	void OpenInsertWaypointSet();
	void OpenSelectTile();
	void ChangeAutoTrim();
	void ChangeRandomizeAutotrim();

	void OpenLevelProperties();
	void OpenLevelScripts();
	void OpenDialoguesWindow();
	void OpenAssistantMsgsWindow();
	void OpenTerminalsWindow();
	void OpenObjectProperties();
	void DeleteObject();
	void DeleteAllObjects();
	void ChangeRapidTileElevation();
	void OpenWaypointSetsWindow();
	void OpenCampaignEditor();

	// Open database calls
	void OpenShipDatabase();
	void OpenEnemyDatabase();
	void OpenDynamicObjectDatabase();
	void OpenStaticObjectDatabase();
	void OpenEffectsDatabase();
	void OpenAmmoDatabase();
	void OpenGadgetDatabase();
	void OpenGlobalScriptsWindow();
	void OpenSoundsWindow();
	void OpenLightEffectDatabaseWindow();
	void OpenComponentWindow();
	void OpenQuestItemWindow();
	void OpenShipPatchWindow();

	void EditTileset();
	void CreateTileset();

	void OpenNewLevelWindow();
	void SaveLevel();
	void SaveLevelAs();
	void OpenLevel();
	void CloseLevel();

	void OpenAboutWindow();

	void OpenTriggerProperties();
	void OpenGameObjectProperties();

	void SelectTile(DWORD id);

	void AddObject(float x, float z);

	void RestoreFlyCamToDefaultPosition();

	CCameraSceneNode *m_cam2;
	CMeshSceneNode *m_cameraFollowObject;
	CSelectionMeshSceneNode *m_selectionMesh;

	int m_isometricCameraAngleMultiplier;

	D3DXVECTOR3 m_viewRayPos, m_viewRayDir;
	CTile *m_tileHotselection;

	ISceneNode *m_selectedObject;
	long m_selectedID;
	int m_selectedType;

	DWORD m_lastTileID, m_lastTileID2;

	ACTION_STATE m_actionState;

	bool m_bFreeFlyMode;
	bool m_bTileEditMode;
	bool m_bShowTriggers;
	/// If true then elevating / lowering tiles doesn't require seperate clicks
	bool m_bRapidTileElevation;

	bool m_bLevelLoaded;

	bool m_bTileTrimEnabled, m_bRandomizeAutotrim;

	float m_scrollSpeed, m_mouseScroll;
	float m_cooldownTimer, m_cooldownLength;

	CDynamicObjectDatabase m_dynamicObjectDatabase;
	CStaticObjectDatabase m_staticObjectDatabase;
	CShipDatabase m_shipDatabase;
	CEnemyDatabase m_enemyDatabase;
	CTileDatabase m_tileDatabase;
	CDialogueDatabase m_dialogueDatabase;
	CAssistantMsgHandler m_assistantMsgHandler;
	CTerminalDatabase m_terminalDatabase;
	CWaypointSetDatabase m_waypointSetDatabase;
	CParticleEffectDatabase m_particleEffectDatabase;
	CGadgetDatabaseHandler m_gadgetDatabaseHandler;
	CAmmoDatabaseHandler m_ammoDatabaseHandler;
	CGlobalScriptsHandler m_globalScriptsHandler;
	CSoundDatabaseHandler m_soundDatabaseHandler;
	CLightEffectDatabaseHandler m_lightEffectHandler;
	CComponentDatabaseHandler m_componentHandler;
	CQuestItemDatabaseHandler m_questItemHandler;
	CShipPatchDatabaseHandler m_shipPatchHandler;

	CWayPointSet *m_waypointSet;
};

#endif // #ifndef __CEDITOR_H__
