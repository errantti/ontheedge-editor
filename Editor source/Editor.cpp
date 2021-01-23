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

#include "Editor.h"
#include "EditorUtilities.h"
#include "Tools\\GetStringWnd.h"
#include "Tools\\NewLevelWnd.h"
#include "Tools\\LevelPropertiesWnd.h"
#include "Tools\\TriggerWnd.h"
#include "Tools\\ObjectPropertiesWnd.h"
#include "Databases\\SelectionWnd.h"
#include "Databases\\DatabaseSelectionWnd.h"
#include "Databases\\TreeSelectionWnd.h"
#include "Databases\\TreeDatabaseSelectionWnd.h"
#include "Databases\\DatabaseUtilities.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEditor::CEditor() : m_cam2(NULL), m_cameraFollowObject(NULL), m_bFreeFlyMode(FALSE),
	m_bTileEditMode(TRUE), m_selectedObject(NULL), m_scrollSpeed(350.f), m_mouseScroll(0.9f),
	m_actionState(ACTION_STATE_NONE), m_bLevelLoaded(FALSE), m_selectedID(0), m_selectedType(-1),
	m_bShowTriggers(TRUE), m_selectionMesh(NULL), m_bTileTrimEnabled(true), m_bRandomizeAutotrim(false),
	m_cooldownTimer(0.f), m_cooldownLength(0.5f), m_tileHotselection(NULL), m_bRapidTileElevation(false),
	m_waypointSet(NULL), m_isometricCameraAngleMultiplier(0), m_lastTileID(-1), m_lastTileID2(-1)
{
	m_menuResource = IDR_EDITOR_MENU;
}

CEditor::~CEditor()
{
}


HRESULT CEditor::OnCreate()
{
	CResourceManager::GetManager().SetDefaultColorKey(D3DCOLOR_RGBA(255, 0, 255, 255));
	CMesh::SetTextureFolder("textures\\");

	LoadEffect("Shaders\\shadowmap.fx");

	if(FAILED(InitializePrimaryObjects()))
	{
		CPrinter::log("Initializing primary engine objects failed", LOG_ERROR);
		return E_FAIL;
	}

	// Disable ambient lighting
	SetAmbientLight(D3DCOLOR_RGBA(0,0,0,255));

	m_om.SetShowTriggers(m_bShowTriggers);

	/**
	 * Set a tiled terrain scene node
	 **/
	CTiledTerrain *terrain = m_manager->AddTiledTerrain(NULL);
	m_omsn->AddExternalCollider(terrain);
	m_om.SetTerrain(terrain);
	CTileManager::GetManager()->SetShowDummyTiles(true);

	/**
	 * Create level object
	 **/
	m_level = new CLevel(terrain, &m_om);

	ShowCursor(TRUE);
	std::string campaignFilename;
	bool bNewCampaign = false;
	if(!SelectCampaign(campaignFilename, bNewCampaign))
	{
		CPrinter::logError("User chose not to load campaign: closing editor");
		ErrorMsgBox("Campaign must be chosen to use the editor");
		return E_FAIL;
	}

	if(!bNewCampaign)
	{
		if(!LoadCampaign(campaignFilename))
		{
			ErrorMsgBox("Failed loading selected campaign. Closing editor...");
			return E_FAIL;
		}
	}

	if(FAILED(InitializeEditor()))
		return E_FAIL;

	ChangeRapidTileElevation();

	// temp
	//m_level->LoadDefaultLevel();
	//m_bLevelLoaded = TRUE;

	return S_OK;
}

HRESULT CEditor::InitializePrimaryObjects()
{
	m_manager = GetSceneManager();
	m_environment = GetGUIEnvironment();
	CResourceManager::GetManager().SetDevice(GetDevice());

	if(m_manager == NULL)
		return E_FAIL;

	//m_menuManager.SetEnvironment(m_environment);
	//m_menuManager.InitializeMenus();

	/**
	 * Add an object manager scene node to the scene
	 **/
	m_omsn = m_manager->AddObjectManager(40, 800, 800, 100);
	m_omsn->EnablePhysics(FALSE);

	m_om.SetObjectManagerSceneNode(m_omsn);

	m_scriptManager.InitializeScriptEngine();

	if(m_soundManager.InitializeSoundSystem() == FALSE)
	{
		CPrinter::log("Failed initializing sound system");
	}
	CSoundSettings::InitializeSettings();

	m_controllerManager.Initialize();

	m_bEngineObjectInitialized = TRUE;
	
	return S_OK;
}

bool CEditor::SelectCampaign(std::string &outFilename, bool &bOutNewCampaign)
{
	OPENFILENAME ofn;
	char filename[256] = "";
	char tempFile[256] = "";

	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = tempFile;
	ofn.lpstrFileTitle = filename;
	ofn.lpstrFilter = "On the Edge campaign (*.cpg)\0*.CPG\0\0";
	ofn.lpstrDefExt = "cpg";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = 256;
	ofn.nMaxFileTitle = 64;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
	//ofn.Flags = OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = "campaigns";

	if(GetOpenFileName(&ofn) == FALSE)
		return false;

	std::string str;
	str = "campaigns\\";
	str += ofn.lpstrFileTitle;
	if((int)str.find(".cpg") < 0)
		str = str.append(".cpg");

	// Check whether the given file exists (user might have changed directory or created new campaign)
	FILE *f = fopen(str.c_str(), "r");
	if(f == NULL)
	{
		if(MessageBox(m_hWnd, 
			"Campaign file you specified doesn't exist. Do you want to create a new campaign with this filename?",
			"OtE Editor", MB_YESNO) == IDNO)
			return false;
		else
		{
			if(!m_campaign.SaveCampaign(str.c_str(), true))
			{
				CPrinter::logError("Failed saving new campaign. Exiting...");
				ErrorMsgBox("Couldn't create new campaign. Exiting...");
				return false;
			}
			else
			{
				CPrinter::log("New campaign created succesfully");
				MessageBox(m_hWnd, "New campaign created succesfully! Note that you have to manually \
\ncreate a folder in campaigns that has the same name as the new campaign file (without \
\nthe identifier .cpg). Under that folder you have to also create two subfolders named \
\n'levels' and 'data'.", "OtE Editor", MB_OK);
			}
		}
		bOutNewCampaign = true;
		//CreateDirectory
	}
	else
	{
		fclose(f);
		bOutNewCampaign = false;
	}

	outFilename = str;

	return true;
}

HRESULT CEditor::InitializeEditor()
{
	/**
	 * Add cameras to the scene set following an invisible 'camera' mesh object
	 **/
	// Set a default cameras to the scene
	m_cameraFollowObject = m_manager->AddMeshSceneNode(NULL, NULL, -1, D3DXVECTOR3(100.f, 160.f, 100.f));
	m_cam = m_manager->AddCameraFPSSceneNode(NULL, -1, D3DXVECTOR3(100.f, 250.f, 100.f), 250.f, 0.015f);
	m_cam->SetRotation(D3DXVECTOR3(0.3f, 0.2f, 0.f));
	((CCameraFPSSceneNode*)m_cam)->SetRotationRequiresMouseRight(true);
	m_cam->SetFarValue(800.f);
	m_cam->CalculateProjectionMatrix();

	m_cam2 = m_manager->AddCameraFollowSceneNode(m_cameraFollowObject, 30, D3DXVECTOR3(0.f, 200.f, -70.f));
	RestoreFlyCamToDefaultPosition();
	m_cam2->SetFarValue(1000.f);
	m_cam2->CalculateProjectionMatrix();

	if(m_bFreeFlyMode)
		m_manager->SetActiveCamera(m_cam);
	else
		m_manager->SetActiveCamera(m_cam2);

	/**
	 * Editor settings
	 **/
	m_dynamicObjectDatabase.SetManager(&m_om);
	m_staticObjectDatabase.SetManager(&m_om);
	m_shipDatabase.SetManager(&m_om);
	m_enemyDatabase.SetManager(&m_om);
	m_tileDatabase.SetManager(CTileManager::GetManager());
	m_dialogueDatabase.SetManager(m_level);
	m_assistantMsgHandler.SetManager(m_level);
	m_terminalDatabase.SetManager(m_level);
	m_waypointSetDatabase.SetManager(m_level);
	m_gadgetDatabaseHandler.SetManager(&m_gadgets);
	m_particleEffectDatabase.SetManager(&m_particleEffects);
	m_ammoDatabaseHandler.SetManager(&m_ammoTemplates);
	m_globalScriptsHandler.SetManager(&m_scriptManager);
	m_soundDatabaseHandler.SetManager(&m_soundManager);
	m_lightEffectHandler.SetManager(&m_lightEffects);
	m_componentHandler.SetManager(&CItemManager::GetManager());
	m_questItemHandler.SetManager(&CItemManager::GetManager());
	m_shipPatchHandler.SetManager(&CItemManager::GetManager());

	//CMesh *selection = new CMesh;
	//if(selection->LoadMesh("meshes\\selection.x", GetDevice()) == TRUE)
	CMesh *selection = CResourceManager::GetManager().GetMesh("meshes\\selection.x");
	if(selection)
	{
		m_selectionMesh = new CSelectionMeshSceneNode(m_sceneManager, selection);
		//if(m_selectionMesh)
		//	m_selectionMesh->SetVisible(FALSE);
	}
	//else
	//	delete selection;

	return S_OK;
}

void CEditor::OnRelease()
{
	if(m_level)
	{
		delete m_level;
		m_level = NULL;
	}
}

void CEditor::OnFlip()
{
	CSceneManager *manager = GetSceneManager();
	CGUIEnvironment *environment = GetGUIEnvironment();

	float frameTime = GetFrameTime();
	if(frameTime > 0.3f)
		frameTime = 0.3f;

	if(BeginScene(TRUE, TRUE, D3DCOLOR_RGBA(50, 50, 50, 255)) == FALSE)
		return;
	manager->RenderAll();
	RenderEditor();
	manager->UpdateAll(frameTime);

	BeginSprite();
	//environment->DrawAll();
	
	RenderEditorStateInfo();

	EndSprite();
	EndScene();

	UpdateView();

	//environment->Update();

	Sleep(10);
}

// GetViewRay
// Returns normalized ray vector pointing from the camera object
// to the direction the cursor is pointing at
D3DXVECTOR3 CEditor::GetNormalizedViewRay()
{
	D3DXVECTOR3 ray((float)GetWinMouseX()-400.f, 0.f, 300.f-(float)GetWinMouseY());
	ray.y = -sqrtfast(618254.34f-ray.x*ray.x - ray.z*ray.z);

	float l2 = ray.x*ray.x + ray.y*ray.y + ray.z*ray.z;
	ray /= sqrtfast(l2);

	return ray;
}

// GetViewRay
// Returns normalized ray vector pointing from the camera object
// to the direction the cursor is pointing at
void CEditor::GetViewRay(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir)
{
	/*D3DXVECTOR3 ray((float)GetWinMouseX()-400.f, 0.f, 300.f-(float)GetWinMouseY());
	ray.y = -sqrtfast(608254.34f-ray.x*ray.x - ray.z*ray.z);*/

	D3DXMATRIX matProj;
    GetDevice()->GetTransform( D3DTS_PROJECTION, &matProj );

    POINT ptCursor;
    GetCursorPos( &ptCursor );
    ScreenToClient( GetWindow(), &ptCursor );

    // Compute the vector of the pick ray in screen space
    D3DXVECTOR3 v;
	if(m_bWindowed)
	{
		v.x =  ( ( ( 2.0f * ptCursor.x ) / (float)(m_rcScreen.right - m_rcScreen.left)  ) - 1 ) / matProj._11;
		v.y = -( ( ( 2.0f * ptCursor.y ) / (float)(m_rcScreen.bottom - m_rcScreen.top) ) - 1 ) / matProj._22;
	}
	else
	{
		v.x =  ( ( ( 2.0f * ptCursor.x ) / (float)GetWidth()  ) - 1 ) / matProj._11;
		v.y = -( ( ( 2.0f * ptCursor.y ) / (float)GetHeight() ) - 1 ) / matProj._22;
	}
    v.z =  1.0f;

    // Get the inverse view matrix
    D3DXMATRIX matView, m;
    GetDevice()->GetTransform( D3DTS_VIEW, &matView );
    D3DXMatrixInverse( &m, NULL, &matView );

    // Transform the screen space pick ray into 3D space
    dir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
    dir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
    dir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
    pos.x = m._41;
    pos.y = m._42;
    pos.z = m._43;
}

D3DXVECTOR3 CEditor::GetXZPlaneRayCollision(D3DXVECTOR3 &rayPos, D3DXVECTOR3 &rayDir)
{
	D3DXVECTOR3 pos = rayPos;
	float c = -rayPos.y / rayDir.y;
	pos.x += rayDir.x*c;
	pos.z += rayDir.z*c;
	pos.y = 0.f;
	return pos;
}

D3DXVECTOR3 CEditor::GetXZPlaneViewRayCollision()
{
	//D3DXVECTOR3 pos, dir;
	//GetViewRay(pos, dir);
	return GetXZPlaneRayCollision(m_viewRayPos, m_viewRayDir);
}

void CEditor::RenderEditor()
{
	if(m_selectionMesh != NULL)
	{
		if(!m_bTileEditMode)
		{
			// Render a red circle around the selected object
			m_selectionMesh->UpdateSelection((IGeometricObject*)m_selectedObject);
		}
		else
		{
			// In tile mode don't render the mesh at all
			m_selectionMesh->UpdateSelection(NULL);
		}
	}

	if(m_selectedObject != NULL && m_bTileEditMode)
	{
		D3DXCOLOR c(100.f, 100.f, 100.f, 100.f);
		RenderWireframeAABB(((CTile*)m_selectedObject)->GetTransformedBox(), 
			true, 1.f, false, c);
	}

	if(m_tileHotselection != NULL && m_bTileEditMode)
	{
		D3DXCOLOR c(5.5f, 0.5f, 0.5f, 5.5f);
		RenderWireframeAABB(m_tileHotselection->GetTransformedBox(), 
			true, 1.f, false, c);
	}

	if(m_actionState == ACTION_STATE_ADD_WAYPOINT)
	{
		// Render currently active waypoint set
		if(m_waypointSet != NULL)
			m_waypointSet->Render(true);
	}
}

void CEditor::UpdateView()
{
	float frametime = GetFrameTime();
	if(frametime > 0.3f)
		frametime = 0.3f;

	if(m_cooldownTimer > 0.f)
		m_cooldownTimer -= frametime;

	CInputMouse *m = GetMouse();
	STileInfo *tileInfo = NULL;

	if(!m_bLevelLoaded)
		return;

	if(m_bActive)
	{
		CInputKeyboard *k = GetKeyboard();

		/**
		* Hot key N - change tile mode
		**/
		static bool ndown = false;
		if(k->GetButtonDown(DIK_N) && !ndown)
		{
			ndown = true;
			ChangeTileMode();
		}
		if(!k->GetButtonDown(DIK_N))
			ndown = false;

		/**
		* Hot key Q - enable/disable autotrim
		**/
		static bool qdown = false;
		if(k->GetButtonDown(DIK_Q) && !qdown)
		{
			qdown = true;
			ChangeAutoTrim();
		}
		if(!k->GetButtonDown(DIK_Q))
			qdown = false;

		/**
		* Hot key G - open static object selection window
		**/
		static bool gdown = false;
		if(k->GetButtonDown(DIK_G) && !gdown)
		{
			gdown = true;
			OpenInsertStaticObject();
		}
		if(!k->GetButtonDown(DIK_G))
			gdown = false;

		/**
		* Hot keys 1,2,3,4 - select space tile / platform tile / last tile / tile before last one
		**/
		static bool b3down = false, b4down = false;
		if(k->GetButtonDown(DIK_1))
		{
			tileInfo = CTileManager::GetManager()->GetTileTemplate(TILE_TYPE_EMPTY, 0, 0);
			if(tileInfo != NULL)
			{
				if(!m_bTileEditMode)
					ChangeTileMode();
				m_selectedID = (long)tileInfo->id;
				m_actionState = ACTION_STATE_ADD_OBJECT;
			}
		}
		else if(k->GetButtonDown(DIK_2))
		{
			tileInfo = CTileManager::GetManager()->GetTileTemplate(TILE_TYPE_PLATFORM, 0, 0);
			if(tileInfo != NULL)
			{
				if(!m_bTileEditMode)
					ChangeTileMode();
				m_selectedID = (long)tileInfo->id;
				m_actionState = ACTION_STATE_ADD_OBJECT;
			}
		}
		else if(k->GetButtonDown(DIK_3) && !b3down)
		{
			if(m_lastTileID != -1)
				SelectTile(m_lastTileID);
			b3down = true;
		}
		else if(k->GetButtonDown(DIK_4) && !b4down)
		{
			if(m_lastTileID2 != -1)
				SelectTile(m_lastTileID2);
			b4down = true;
		}

		if(!k->GetButtonDown(DIK_4))
			b4down = false;
		if(!k->GetButtonDown(DIK_3))
			b3down = false;

		if(!m_bFreeFlyMode)
		{
			D3DXVECTOR3 &pos = m_cameraFollowObject->GetTranslation();

			if(k->GetButtonDown(DIK_D))
			{
				pos.x += frametime * m_scrollSpeed;
			}
			else if(k->GetButtonDown(DIK_A))
			{
				pos.x -= frametime * m_scrollSpeed;
			}
			if(k->GetButtonDown(DIK_W))
			{
				pos.z += frametime * m_scrollSpeed;
			}
			else if(k->GetButtonDown(DIK_S))
			{
				pos.z -= frametime * m_scrollSpeed;
			}

			if(m->GetMouseRightDown())
			{
				float rx = (float)m->GetRelativeX();
				float ry = (float)m->GetRelativeY();

				if(m_isometricCameraAngleMultiplier == 0)
				{
					pos.x -= rx * m_mouseScroll;
					pos.z += ry * m_mouseScroll;
				}
				else if(m_isometricCameraAngleMultiplier == 1)
				{
					pos.z += rx * m_mouseScroll;
					pos.x += ry * m_mouseScroll;
				}
				else if(m_isometricCameraAngleMultiplier == 2)
				{
					pos.x += rx * m_mouseScroll;
					pos.z -= ry * m_mouseScroll;
				}
				else if(m_isometricCameraAngleMultiplier == 3)
				{
					pos.z -= rx * m_mouseScroll;
					pos.x -= ry * m_mouseScroll;
				}
			}

			// Möh
			static bool hdown = false;
			if(!hdown && k->GetButtonDown(DIK_H))
			{
				m_isometricCameraAngleMultiplier++;
				if(m_isometricCameraAngleMultiplier > 3)
					m_isometricCameraAngleMultiplier = 0;

				if(m_isometricCameraAngleMultiplier == 0)
					m_cam2->SetTranslation(D3DXVECTOR3(0.f, 200.f, -70.f));
				else if(m_isometricCameraAngleMultiplier == 1)
					m_cam2->SetTranslation(D3DXVECTOR3(-70.f, 200.f, 0.f));
				else if(m_isometricCameraAngleMultiplier == 2)
					m_cam2->SetTranslation(D3DXVECTOR3(0.f, 200.f, 70.f));
				else if(m_isometricCameraAngleMultiplier == 3)
					m_cam2->SetTranslation(D3DXVECTOR3(70.f, 200.f, 0.f));

				hdown = true;
			}
			if(!k->GetButtonDown(DIK_H))
				hdown = false;

			int zMove = 0;
			// Camera zoom
			if((zMove = m->GetRelativeZ()) != 0)
			{
				pos.y -= (float)zMove * 0.3f;
				if(pos.y < 160.f)
					pos.y = 160.f;
				if(pos.y > 450.f)
					pos.y = 450.f;
			}
			m_cameraFollowObject->SetTranslation(pos);
		}
		else
		{
			if(k->GetButtonDown(DIK_F))
			{
				// Return camera back to default position
				RestoreFlyCamToDefaultPosition();
			}
		}

		//if(m_bFreeFlyMode)
		//	return;

		// Handle
		if(m_bTileEditMode)
			UpdateTileEditMode();
		else
			UpdateObjectEditMode();
	}
}

void CEditor::RestoreFlyCamToDefaultPosition()
{
	m_cam->SetTranslation(D3DXVECTOR3(200.f, 300.f, 200.f));
	m_cam->SetRotation(D3DXVECTOR3(0.7f, 0.2f, 0.f));
}

void CEditor::UpdateTileEditMode()
{
	CInputMouse *m = GetMouse();
	CInputKeyboard *k = GetKeyboard();
	D3DXVECTOR3 pos, dir;
	CTile *tile = NULL;

	GetViewRay(m_viewRayPos, m_viewRayDir);
	m_tileHotselection = (CTile*)(GetSceneManager()->GetTiledTerrain()->GetTileByRay(m_viewRayPos, m_viewRayDir));

	static CTile *lastTile = NULL;

	switch(m_actionState)
	{
	case ACTION_STATE_NONE:
		if(m->GetMouseLeftState() == MOUSE_BUTTON_PRESSED && !k->GetButtonDown(DIK_LCONTROL) && !k->GetButtonDown(DIK_LSHIFT))
		{
			// Select tile
			m_selectedObject = m_tileHotselection;
		}
		else if(k->GetButtonDown(DIK_LCONTROL))
		{
			if((m->GetMouseLeftState() == MOUSE_BUTTON_PRESSED && !m_bRapidTileElevation) ||
				(m->GetMouseLeftDown() && m_bRapidTileElevation))
			{
				// Lower the tile
				tile = m_tileHotselection;
				if((tile != lastTile || !m_bRapidTileElevation) && tile != NULL)
				{
					lastTile = tile;
					tile->Lower();

					if(m_bTileTrimEnabled)
						GetSceneManager()->GetTiledTerrain()->TrimTiles(tile, m_bRandomizeAutotrim);

					GetSceneManager()->GetTiledTerrain()->UpdateBoundingBox();
					m_selectedObject = (ISceneNode*)tile;
				}
			}

			if(!m->GetMouseLeftDown())
				lastTile = NULL;
		}
		else if(k->GetButtonDown(DIK_LSHIFT))
		{
			if((m->GetMouseLeftState() == MOUSE_BUTTON_PRESSED && !m_bRapidTileElevation) ||
				(m->GetMouseLeftDown() && m_bRapidTileElevation))
			{
				// Elevate the tile
				tile = m_tileHotselection;
				if((tile != lastTile || !m_bRapidTileElevation) && tile != NULL)
				{
					lastTile = tile;
					tile->Elevate();

					if(m_bTileTrimEnabled)
						GetSceneManager()->GetTiledTerrain()->TrimTiles(tile, m_bRandomizeAutotrim);
					GetSceneManager()->GetTiledTerrain()->UpdateBoundingBox();
					m_selectedObject = (ISceneNode*)tile;
				}
			}

			if(!m->GetMouseLeftDown())
				lastTile = NULL;
		}
		else
			lastTile = NULL;

		break;

	case ACTION_STATE_ADD_OBJECT:
		if(m_cooldownTimer > 0.f)
			break;
		if(::GetAsyncKeyState(VK_SPACE))
		{
			m_actionState = ACTION_STATE_NONE;
			lastTile = NULL;
			break;
		}

		if(m->GetMouseLeftDown())
		{
			if(m_tileHotselection != NULL)
			{
				if(m_tileHotselection != lastTile)
				{
					lastTile = m_tileHotselection;
					lastTile->SetTileInfo(m_selectedID);
					GetSceneManager()->GetTiledTerrain()->UpdateBoundingBox();
					m_selectedObject = (ISceneNode*)lastTile;
				}
			}
			else
			{
				pos = GetXZPlaneViewRayCollision();

				// Check whether a border tile was targeted. Border tiles
				// are always left empty to make it unlikely for objects to
				// fly out of the object manager grid
				if(pos.x < 50.f || pos.x > GetSceneManager()->GetTiledTerrain()->GetWidth() - 50.f ||
					pos.z < 50.f || pos.z > GetSceneManager()->GetTiledTerrain()->GetDepth() - 50.f)
					return;

				tile = GetSceneManager()->GetTiledTerrain()->GetTile(pos.x, pos.z);
				if(tile != lastTile && tile != NULL)
				{
					lastTile = tile;
					tile->SetTileInfo(m_selectedID);
					GetSceneManager()->GetTiledTerrain()->UpdateBoundingBox();
					m_selectedObject = (ISceneNode*)tile;
				}
			}
		}
		else
			lastTile = NULL;
		break;
	}
}

void CEditor::UpdateObjectEditMode()
{
	CInputMouse *m = GetMouse();
	CInputKeyboard *k = GetKeyboard();
	D3DXVECTOR3 pos, rot, pos2;
	STriggerInfo t;

	if(::GetAsyncKeyState(VK_SPACE))
	{
		//if(m_actionState == ACTION_STATE_ADD_WAYPOINT)
		//	m_level->AddWaypointSet(new CWayPointSet(m_waypointSet));
		m_actionState = ACTION_STATE_NONE;
	}

	/**
	 * Hot key R - Open object properties
	 **/
	static bool rdown = false;
	if(k->GetButtonDown(DIK_R) && !rdown)
	{
		rdown = true;
		if(m_selectedObject != NULL)
		{
			OpenObjectProperties();
		}
	}
	if(!k->GetButtonDown(DIK_R))
		rdown = false;

	if(m_selectedObject != NULL && k->GetButtonDown(DIK_DELETE))
	{
		CEditor::DeleteObject();
	}

	if(m_actionState == ACTION_STATE_NONE)
	{
		if(m->GetMouseLeftState() == MOUSE_BUTTON_PRESSED && !k->GetButtonDown(DIK_LCONTROL) && 
			!k->GetButtonDown(DIK_LSHIFT) && !k->GetButtonDown(DIK_RSHIFT))
		{
			GetViewRay(pos, rot);
			// See if the mouse points any object and if it does, select it
			IGameObject *go = GetObjectManagerSceneNode()->GetObjectFromRaySlow(pos,rot);
			m_selectedObject = go;
		}
		// Change y-coordinate of static object
		else if(k->GetButtonDown(DIK_RSHIFT) && m->GetMouseLeftDown() && m_selectedObject != NULL)
		{
			if(((IGameObject*)m_selectedObject)->GetType() == STATIC_OBJECT_ID)
			{
				pos = m_selectedObject->GetTranslation();
				pos.y -= (float)m->GetRelativeY() * 0.5f;
				m_selectedObject->SetTranslation(pos);
			}
		}
		// Rotate object
		else if(k->GetButtonDown(DIK_LCONTROL) && m->GetMouseLeftDown() && m_selectedObject != NULL)
		{
			// Rotate the selected object
			rot = m_selectedObject->GetRotation();
			rot.y += (float)m->GetRelativeX() * 0.02f;
			m_selectedObject->SetRotation(rot);
		}
		else if(k->GetButtonDown(DIK_LSHIFT) && m->GetMouseLeftDown() && m_selectedObject != NULL)
		{
			// Move the selected object
			GetViewRay(pos, rot);
			if(GetSceneManager()->GetTiledTerrain()->GetRayIntersectLocation(pos, rot, pos2))
			{
				//pos = GetXZPlaneViewRayCollision();
				//pos.y = GetSceneManager()->GetTiledTerrain()->GetTerrainHeight(pos.x, pos.z)+7.f;
				if(((IGameObject*)m_selectedObject)->GetType() != TRIGGER_OBJECT_ID)
					pos2.y += 7.f;
				m_selectedObject->SetTranslation(pos2);
			}
		}
	}
	else if(m_actionState == ACTION_STATE_ADD_OBJECT && m_cooldownTimer<=0.f)
	{
		if(m->GetMouseLeftState() == MOUSE_BUTTON_PRESSED)
		{
			// Add the selected object
			GetViewRay(pos, rot);
			if(GetSceneManager()->GetTiledTerrain()->GetRayIntersectLocation(pos, rot, pos2))
			{
				//pos = GetXZPlaneViewRayCollision();
				//pos.y = GetSceneManager()->GetTiledTerrain()->GetTerrainHeight(pos.x, pos.z)+7.f;
				//pos2.y += 7.f;
				//m_selectedObject->SetTranslation(pos2);
				AddObject(pos2.x, pos2.z);
			}
		}
	}
	else if(m_actionState == ACTION_STATE_ADD_PLAYER)
	{
		if(m->GetMouseLeftDown())
		{
			GetViewRay(pos, rot);
			if(GetSceneManager()->GetTiledTerrain()->GetRayIntersectLocation(pos, rot, pos2))
			{
				// Move the player location and exit player mode
				pos2.y += 7.f;
				m_level->SetPlayerPosition(m_selectedID, pos2);
				m_actionState = ACTION_STATE_NONE;
			}
		}
	}
	else if(m_actionState == ACTION_STATE_ADD_TRIGGER)
	{
		if(m->GetMouseLeftState() == MOUSE_BUTTON_PRESSED && m_cooldownTimer<=0.f)
		{
			GetViewRay(pos, rot);
			if(GetSceneManager()->GetTiledTerrain()->GetRayIntersectLocation(pos, rot, pos2))
			{
				t.FillDefaults();
				//pos = GetXZPlaneViewRayCollision();
				//pos.y = GetSceneManager()->GetTiledTerrain()->GetTerrainHeight(pos.x, pos.z)+0.5f;
				pos2.y += 0.5f;
				t.bbox.minCorner = pos2;
				t.bbox.maxCorner = pos2 + D3DXVECTOR3(50.f, 50.f, 50.f);
				m_selectedObject = m_om.PlaceTrigger(t);
				m_actionState = ACTION_STATE_RESIZE_TRIGGER;
			}
		}
	}
	else if(m_actionState == ACTION_STATE_RESIZE_TRIGGER)
	{
		if(m_selectedObject == NULL)
		{
			m_actionState = ACTION_STATE_NONE;
			return;
		}

		GetViewRay(pos, rot);
		if(GetSceneManager()->GetTiledTerrain()->GetRayIntersectLocation(pos, rot, pos2))
		{
			//pos = GetXZPlaneViewRayCollision();
			t = ((CTrigger*)m_selectedObject)->GetTriggerInfo();
			rot = t.bbox.minCorner;
			if(pos2.x > rot.x)
				rot.x = pos2.x;
			if(pos2.z > rot.z)
				rot.z = pos2.z;
			rot.y+=50.f;
			t.bbox.maxCorner = rot;
			((CTrigger*)m_selectedObject)->ApplyTriggerInfo(t);
			if(m->GetMouseLeftState() == MOUSE_BUTTON_PRESSED)
			{
				m_actionState = ACTION_STATE_NONE;
				return;
			}
		}
	}
	else if(m_actionState == ACTION_STATE_ADD_WAYPOINT)
	{
		if(m_waypointSet != NULL)
		{
			static bool bbackspacedown = false;
			if(m->GetMouseLeftState() == MOUSE_BUTTON_PRESSED && m_cooldownTimer <= 0.f)
			{
				GetViewRay(pos, rot);
				if(GetSceneManager()->GetTiledTerrain()->GetRayIntersectLocation(pos, rot, pos2))
				{
					//pos = GetXZPlaneViewRayCollision();
					//pos.y = GetSceneManager()->GetTiledTerrain()->GetTerrainHeight(pos.x, pos.z)+15.f;
					pos2.y += 15.f;
					m_waypointSet->AddWaypoint(pos2);
					if(m_waypointSet->GetWaypoints()->size() > 40)
					{
						m_actionState = ACTION_STATE_NONE;
						//m_level->AddWaypointSet(new CWayPointSet(m_waypointSet));
					}
				}
			}
			else if(!bbackspacedown && k->GetButtonDown(DIK_BACK))
			{
				m_waypointSet->RemoveLastWaypoint();
				bbackspacedown = true;
			}

			if(!k->GetButtonDown(DIK_BACK))
				bbackspacedown = false;
		}
	}
}

void CEditor::ChangeTileMode()
{
	HMENU item = GetSubMenu(GetMenu(GetWindow()), 2);
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	info.hbmpChecked = NULL;

	m_selectedObject = NULL;
	m_tileHotselection = NULL;
	m_actionState = ACTION_STATE_NONE;

	if(m_bTileEditMode)
	{
		m_bTileEditMode = FALSE;
	}
	else
	{
		m_bTileEditMode = TRUE;
	}

	if(item == NULL)
		return;

	if(GetMenuItemInfo(item, ID_EDIT_TILEEDITMODE, FALSE, &info) == FALSE)
		return;

	if(m_bTileEditMode)
	{
		info.fState |= MFS_CHECKED;
		info.fState ^= MFS_UNCHECKED;
	}
	else
	{
		info.fState ^= MFS_CHECKED;
		info.fState |= MFS_UNCHECKED;
	}

	SetMenuItemInfo(item, ID_EDIT_TILEEDITMODE, FALSE, &info);
}

void CEditor::ChangeViewMode()
{
	// Change view mode
	if(m_bFreeFlyMode)
	{
		m_manager->SetActiveCamera(m_cam2);
		((CCameraFPSSceneNode*)m_cam)->ActivateInput(false);
		m_bFreeFlyMode = FALSE;
	}
	else
	{
		m_manager->SetActiveCamera(m_cam);
		((CCameraFPSSceneNode*)m_cam)->ActivateInput(true);
		m_bFreeFlyMode = TRUE;
	}
	//m_actionState = ACTION_STATE_NONE;
}

void CEditor::ChangeTriggerRenderMode()
{
	HMENU item = GetSubMenu(GetMenu(GetWindow()), 2);
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	info.hbmpChecked = NULL;

	m_bShowTriggers = !m_bShowTriggers;

	if(GetMenuItemInfo(item, ID_EDIT_SHOWTRIGGERS, FALSE, &info) == FALSE)
		return;

	if(m_bShowTriggers)
	{
		info.fState |= MFS_CHECKED;
		info.fState ^= MFS_UNCHECKED;
	}
	else
	{
		info.fState ^= MFS_CHECKED;
		info.fState |= MFS_UNCHECKED;
	}

	SetMenuItemInfo(item, ID_EDIT_SHOWTRIGGERS, FALSE, &info);

	m_om.SetShowTriggers(m_bShowTriggers);
}

void CEditor::RenderEditorStateInfo()
{
	CFont *font = GetEdgewalkerApp()->GetGUIEnvironment()->GetDefaultFont();

	rect<int> mode(20, 250, 10, 30);
	rect<int> selectedTileType(20, 350, 30, 50);
	rect<int> modeParams(20, 250, 50, 110);
	rect<int> selected(20, 350, 110, 130);
	rect<int> selected2(20, 350, 130, 150);

	if(m_bTileEditMode)
	{
		STileInfo *info;
		if(m_actionState == ACTION_STATE_ADD_OBJECT)
		{
			info = CTileManager::GetManager()->GetTileTemplate((DWORD)m_selectedID);
			if(info == NULL)
			{
				m_actionState = ACTION_STATE_NONE;
			}
			else
			{
				font->Print("Adding tile (space to stop)", selected);
				font->Print(info->name, selected2);
			}
		}
		font->Print("Tile mode", mode);
		if(m_tileHotselection != NULL)
		{
			D3DXVECTOR3 pos = m_tileHotselection->GetTranslation();
			char tileModeMsg[256];
			sprintf(tileModeMsg, "X: %i\nY: %i\nZ: %i", (int)pos.x, (int)pos.y, (int)pos.z);
			font->Print(tileModeMsg, modeParams);

			info = CTileManager::GetManager()->GetTileTemplate((DWORD)m_tileHotselection->GetTileID());
			if(info != NULL)
			{
				sprintf(tileModeMsg, "Type: <%s>", info->name);
				font->Print(tileModeMsg, selectedTileType);
			}
		}
	}
	else
	{
		font->Print("Object mode", mode);
		if(m_actionState == ACTION_STATE_ADD_OBJECT)
			font->Print("Adding object (space to stop)", selected);
		else if(m_actionState == ACTION_STATE_ADD_TRIGGER)
			font->Print("Adding trigger (space to stop)", selected);
		else if(m_actionState == ACTION_STATE_ADD_WAYPOINT)
			font->Print("Editing waypoint set (space to stop)", selected);
	}

	if(m_selectedObject == NULL)
		return;
}

void CEditor::AddObject(float x, float z)
{
	D3DXVECTOR3 pos(x, 0.f, z);

	CObjectManager *m = GetObjectManager();

	switch(m_selectedType)
	{
	case ENEMY_OBJECT_ID:
		m_selectedObject = m->DropEnemy(m_selectedID, pos, -1, 0, true);
		break;
	case DYNAMIC_OBJECT_ID:
		m_selectedObject = m->DropDynamicObject(m_selectedID, pos, true);
		break;
	case STATIC_OBJECT_ID:
		m_selectedObject = m->DropStaticObject(m_selectedID, pos, -1, true, true);
		break;
	}
}

bool CEditor::NewLevel(int size, int height, const char *tilesetFilename)
{
	if(size < MIN_LEVEL_DIMENSION || size > MAX_LEVEL_DIMENSION)
		return false;

	if(height < 1 || height > MAX_HEIGHT_LEVEL)
		return false;

	if(GetCurrentLevel()->NewLevel((UINT)size, (UINT)size, (UINT)height, tilesetFilename, true) == FALSE)
	{
		CPrinter::log("Error making new level", LOG_ERROR);
		return false;
	}
	m_selectedObject = NULL;
	m_bLevelLoaded = TRUE;

	return true;
}

void CEditor::OpenNewLevelWindow()
{
	CNewLevelWnd n;

	if(m_bLevelLoaded)
		if(AskLevelSaveContinue() == IDNO)
			return;

	n.Open(GetWindow());

	m_selectedObject = NULL;
}

void CEditor::SaveLevel()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor", MB_OK);
		return;
	}

	if(m_level->GetFilename() == "")
	{
		SaveLevelAs();
		return;
	}

	if(m_level->SaveLevel() == TRUE)
		::MessageBox(GetWindow(), "Level saved succesfully", "OtE Editor", MB_OK);
	else
		::MessageBox(GetWindow(), "Failed saving level", "OtE Editor error!", MB_OK);
}

void CEditor::SaveLevelAs()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor", MB_OK);
		return;
	}

	OPENFILENAME ofn;
	std::string strDefault;
	if(m_level->GetFilename() == "")
	{
		strDefault= m_level->GetName();
		strDefault += ".oel";
	}
	else
		strDefault = m_level->GetFilename();

	char filename[128];
	strcpy(filename, strDefault.c_str());

	char folder[256];
	strncpy(folder, m_level->GetLevelDirectory().c_str(), 255);

	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.hwndOwner = GetWindow();
	ofn.lpstrFile = NULL;
	ofn.lpstrFileTitle = filename;
	ofn.lpstrFilter = "On the Edge level (*.oel)\0*.OEL\0\0";
	ofn.lpstrDefExt = "oel";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = 127;
	ofn.nMaxFileTitle = 63;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_SHAREAWARE | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.lpstrInitialDir = folder;

	if(!GetSaveFileName(&ofn))
		return;

	strDefault = filename;
	if((int)strDefault.find(".oel") == -1)
	{
		int ind = (int)strDefault.find('.');
		if(ind != -1)
			strDefault.erase(ind);
		strDefault += ".oel";
	}
	/*int i=0;
	while(filename[i] != '.' && i<strlen(filename)) ++i;
	if(i>strlen(filename)-4)
	{
		if(filename[i+1] != 'o' || filename[i+2] != 'e' || filename[i+3] != 'l')
			strDefault += ".oel";
	}*/

	if(m_level->SaveLevel(strDefault.c_str()) == TRUE)
		::MessageBox(GetWindow(), "Level saved succesfully", "OtE Editor", MB_OK);
	else
		::MessageBox(GetWindow(), "Failed saving level", "OtE Editor error!", MB_OK);
}

void CEditor::OpenLevel()
{
	if(m_bLevelLoaded)
		if(AskLevelSaveContinue() == IDNO)
			return;

	OPENFILENAME ofn;
	char filename[256];
	char temp[256];

	char folder[256];
	strncpy(folder, m_level->GetLevelDirectory().c_str(), 255);

	ZeroMemory(&ofn,sizeof(OPENFILENAME));
	ofn.hwndOwner = GetWindow();
	ofn.lpstrFile = temp;
	ofn.lpstrFileTitle = filename;
	ofn.lpstrFilter = "On the Edge level (*.oel)\0*.OEL\0\0";
	ofn.lpstrDefExt = "oel";
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = 127;
	ofn.nMaxFileTitle = 63;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_CREATEPROMPT | OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = folder;

	if(!GetOpenFileName(&ofn))
		return;

	m_selectedObject = NULL;
	m_tileHotselection = NULL;
	m_actionState = ACTION_STATE_NONE;
	if(m_level->LoadLevel(filename, false, true, true, true, true, true))
	{
		m_bLevelLoaded = TRUE;
		m_level->SetPlayerMeshes();
	}
}

void CEditor::CloseLevel()
{
	if(m_bLevelLoaded)
	{
		if(AskLevelSaveContinue() == IDNO)
			return;
	}
	else
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor", MB_OK);
		return;
	}

	m_level->CloseLevel();

	m_bLevelLoaded = FALSE;
	m_selectedObject = NULL;
	m_actionState = ACTION_STATE_NONE;
	m_tileHotselection = NULL;
}

void CEditor::SelectTile(DWORD id)
{
	if(m_lastTileID != id)
		m_lastTileID2 = m_lastTileID;

	if(!m_bTileEditMode)
		ChangeTileMode();
	m_lastTileID = id;
	m_selectedID = (long)id;
	m_actionState = ACTION_STATE_ADD_OBJECT;
}

/////////////////////////

/*************************************
 *
 * Member functions for opening other windows
 *
 *************************************/

/**
 * Level edit windows
 **/

void CEditor::OpenSelectTile()
{
	if(!m_bTileEditMode)
		ChangeTileMode();
	//if(m_bFreeFlyMode)
	//	ChangeViewMode();

	CTileManager *m = CTileManager::GetManager();
	std::vector<STileInfo> &temps = m->GetTemplateVector();

	CTreeSelectionWnd s;
	long selected;
	for(int i=0;i<temps.size();i++)
	{
		s.AddEntry(SSelectionEntry(temps[i].name, temps[i].id, temps[i].subcategory));
	}

	if(s.OpenWindow(GetWindow(), selected, "tile"))
	{
		//m_selectedID = selected;
		//m_actionState = ACTION_STATE_ADD_OBJECT;
		SelectTile((DWORD)selected);
		m_cooldownTimer = m_cooldownLength;
	}
}

void CEditor::ChangeAutoTrim()
{
	HMENU item = GetSubMenu(GetMenu(GetWindow()), 2);
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	info.hbmpChecked = NULL;

	m_selectedObject = NULL;
	m_actionState = ACTION_STATE_NONE;

	if(m_bTileTrimEnabled)
	{
		m_bTileTrimEnabled = FALSE;
	}
	else
	{
		m_bTileTrimEnabled = TRUE;
	}

	if(item == NULL)
		return;

	if(GetMenuItemInfo(item, ID_EDIT_AUTOTRIM, FALSE, &info) == FALSE)
		return;

	if(m_bTileTrimEnabled)
	{
		info.fState |= MFS_CHECKED;
		info.fState ^= MFS_UNCHECKED;
	}
	else
	{
		info.fState ^= MFS_CHECKED;
		info.fState |= MFS_UNCHECKED;
	}

	SetMenuItemInfo(item, ID_EDIT_AUTOTRIM, FALSE, &info);
}

void CEditor::ChangeRandomizeAutotrim()
{
	HMENU item = GetSubMenu(GetMenu(GetWindow()), 2);
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	info.hbmpChecked = NULL;

	m_selectedObject = NULL;
	m_actionState = ACTION_STATE_NONE;

	if(m_bRandomizeAutotrim)
	{
		m_bRandomizeAutotrim = FALSE;
	}
	else
	{
		m_bRandomizeAutotrim = TRUE;
	}

	if(item == NULL)
		return;

	if(GetMenuItemInfo(item, ID_EDIT_RANDOMIZEAUTOTRIM, FALSE, &info) == FALSE)
		return;

	if(m_bRandomizeAutotrim)
	{
		info.fState |= MFS_CHECKED;
		info.fState ^= MFS_UNCHECKED;
	}
	else
	{
		info.fState ^= MFS_CHECKED;
		info.fState |= MFS_UNCHECKED;
	}

	SetMenuItemInfo(item, ID_EDIT_RANDOMIZEAUTOTRIM, FALSE, &info);
}

void CEditor::OpenLevelProperties()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	CLevel::SLevelFileHeader s = m_level->GetLevelFileHeader();
	CLevelPropertiesWnd lp;
	if(lp.Open(GetWindow(), s) == TRUE)
		m_level->ApplyHeader(s);
}

void CEditor::OpenLevelScripts()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	CDatabaseSelectionWnd s;
	m_globalScriptsHandler.UseLocalScripts(true);
	s.OpenWindow(GetWindow(), "Level scripts", &m_globalScriptsHandler, FALSE, FALSE);
}

void CEditor::OpenDialoguesWindow()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Dialogues", &m_dialogueDatabase, FALSE, FALSE);
}

void CEditor::OpenAssistantMsgsWindow()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Assistant msgs", &m_assistantMsgHandler, FALSE, FALSE);
}

void CEditor::OpenTerminalsWindow()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Terminals", &m_terminalDatabase, FALSE, FALSE);
}

void CEditor::OpenObjectProperties()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor", MB_OK);
		return;
	}
	if(m_selectedObject == NULL)
	{
		::MessageBox(GetWindow(), "No object selected!", "OtE Editor", MB_OK);
		return;
	}
	if(m_bTileEditMode)
	{
		::MessageBox(GetWindow(), "Selected object doesn't have any adjustable properties", "OtE Editor", MB_OK);
		return;
	}

	IGameObject *o = (IGameObject*)m_selectedObject;
	switch(o->GetType())
	{
	case TRIGGER_OBJECT_ID:
		// Open trigger properties dialog
		OpenTriggerProperties();
		break;
	case STATIC_OBJECT_ID:
	case ENEMY_OBJECT_ID:
	case SHIP_OBJECT_ID:
	case DYNAMIC_OBJECT_ID:
		OpenGameObjectProperties();
		break;
	default:
		::MessageBox(GetWindow(), "Selected object doesn't have any adjustable properties", "OtE Editor", MB_OK);
		break;
	}
}

void CEditor::DeleteObject()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor", MB_OK);
		return;
	}

	if(m_selectedObject == NULL)
	{
		::MessageBox(GetWindow(), "No object selected!", "OtE Editor", MB_OK);
		return;
	}

	if(m_omsn->RemoveObject((IGameObject*)m_selectedObject))
		m_selectedObject = NULL;
}

void CEditor::DeleteAllObjects()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor", MB_OK);
		return;
	}
	if(MessageBox(m_hWnd, "Are you sure you want to delete all objects from the level?", "Delete all objects", MB_OKCANCEL) == IDOK)
		m_omsn->RemoveAllObjects();
}

void CEditor::ChangeRapidTileElevation()
{
	HMENU item = GetSubMenu(GetMenu(GetWindow()), 2);
	MENUITEMINFO info;
	info.cbSize = sizeof(MENUITEMINFO);
	info.fMask = MIIM_STATE;
	info.hbmpChecked = NULL;

	m_selectedObject = NULL;
	m_actionState = ACTION_STATE_NONE;

	m_bRapidTileElevation = !m_bRapidTileElevation;

	if(item == NULL)
		return;

	if(GetMenuItemInfo(item, ID_EDIT_RAPIDTILEELEVATION, FALSE, &info) == FALSE)
		return;

	if(m_bRapidTileElevation)
	{
		info.fState |= MFS_CHECKED;
		info.fState ^= MFS_UNCHECKED;
	}
	else
	{
		info.fState ^= MFS_CHECKED;
		info.fState |= MFS_UNCHECKED;
	}

	SetMenuItemInfo(item, ID_EDIT_RAPIDTILEELEVATION, FALSE, &info);
}

void CEditor::OpenWaypointSetsWindow()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor", MB_OK);
		return;
	}
	CSelectionWnd s;

	vector<CWayPointSet*> *w = m_level->GetWaypointSets();

	vector<CWayPointSet*>::iterator it = w->begin();
	for(; it != w->end(); ++it)
		s.AddEntry(SSelectionEntry((*it)->GetName(), (*it)->GetID()));

	long selected;
	if(s.OpenWindow(GetWindow(), selected, "Select waypointset to edit"))
	{
		m_cooldownTimer = m_cooldownLength;

		if(m_bTileEditMode)
			ChangeTileMode();

		m_waypointSet = m_level->GetWayPointSet((int)selected);
		if(m_waypointSet != NULL)
		{
			m_actionState = ACTION_STATE_ADD_WAYPOINT;
		}
	}
}

void CEditor::OpenCampaignEditor()
{
	CCampaignEditor c;
	c.Open(m_hWnd, "OnTheEdge.cpg");
}

/**
 * Insert object windows
 **/

void CEditor::OpenInsertPlayer()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	if(m_bTileEditMode)
		ChangeTileMode();
	//if(m_bFreeFlyMode)
	//	ChangeViewMode();

	CSelectionWnd s;
	long selected;
	s.AddEntry(SSelectionEntry("Player 1 (single player)", 0));
	s.AddEntry(SSelectionEntry("Player 2", 1));
	if(s.OpenWindow(GetWindow(), selected, "player"))
	{
		m_actionState = ACTION_STATE_ADD_PLAYER;
		m_selectedID = selected;
	}
}

void CEditor::OpenInsertEnemy()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	if(m_bTileEditMode)
		ChangeTileMode();
	//if(m_bFreeFlyMode)
	//	ChangeViewMode();

	CSelectionWnd s;
	long selected;
	
	std::vector<SEnemyTemplate> *enemies = GetObjectManager()->GetEnemyTemplates();
	for(DWORD i=0;i<enemies->size();i++)
		s.AddEntry(SSelectionEntry((*enemies)[i].vt.dt.name, (*enemies)[i].vt.dt.templateID));

	if(s.OpenWindow(GetWindow(), selected, "enemy"))
	{
		m_selectedID = selected;
		m_actionState = ACTION_STATE_ADD_OBJECT;
		m_selectedType = ENEMY_OBJECT_ID;

		m_cooldownTimer = m_cooldownLength;
	}
}

void CEditor::OpenInsertDynamicObject()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	if(m_bTileEditMode)
		ChangeTileMode();
	//if(m_bFreeFlyMode)
	//	ChangeViewMode();

	CSelectionWnd s;
	long selected;
	
	std::vector<SDynamicObjectTemplate> *objects = GetObjectManager()->GetDynamicObjectTemplates();
	for(DWORD i=0;i<objects->size();i++)
		s.AddEntry(SSelectionEntry((*objects)[i].name, (*objects)[i].templateID));

	if(s.OpenWindow(GetWindow(), selected, "dynamic object"))
	{
		m_selectedID = selected;
		m_actionState = ACTION_STATE_ADD_OBJECT;
		m_selectedType = DYNAMIC_OBJECT_ID;

		m_cooldownTimer = m_cooldownLength;
	}
}

void CEditor::OpenInsertStaticObject()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	if(m_bTileEditMode)
		ChangeTileMode();
	//if(m_bFreeFlyMode)
	//	ChangeViewMode();

	CTreeSelectionWnd s;
	long selected;
	
	std::vector<SStaticObjectTemplate> *objects = GetObjectManager()->GetStaticObjectTemplates();
	for(DWORD i=0;i<objects->size();i++)
		s.AddEntry(SSelectionEntry((*objects)[i].name, (*objects)[i].templateID, (*objects)[i].subCategory));

	if(s.OpenWindow(GetWindow(), selected, "static object"))
	{
		m_selectedID = selected;
		m_actionState = ACTION_STATE_ADD_OBJECT;
		m_selectedType = STATIC_OBJECT_ID;

		m_cooldownTimer = m_cooldownLength;
	}
}

void CEditor::OpenInsertWaypointSet()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	CGetStringWnd w;
	std::string s;// = m_waypointSet.GetName();
	if(w.OpenWindow(GetWindow(), s, "Name waypoint set") == FALSE)
		return;
	if(s.size() < 3)
	{
		::MessageBox(GetWindow(), "Too short name", "Error", MB_OK);
		return;
	}
	if(m_bTileEditMode)
		ChangeTileMode();

	//m_waypointSet.GetWaypoints()->clear();
	//m_waypointSet.SetName(s);
	//m_waypointSet.SetID(m_level->GetNewWaypointSetID());
	m_waypointSet = m_level->CreateNewWaypointSet(s);

	m_selectedObject = NULL;
	//if(m_bFreeFlyMode)
	//	ChangeViewMode();
	if(m_waypointSet != NULL)
		m_actionState = ACTION_STATE_ADD_WAYPOINT;
}

void CEditor::OpenInsertTrigger()
{
	if(!m_bLevelLoaded)
	{
		::MessageBox(GetWindow(), "No level loaded!", "OtE Editor - error!", MB_OK);
		return;
	}

	//if(m_bFreeFlyMode)
	//	ChangeViewMode();
	if(m_bTileEditMode)
		ChangeTileMode();
	m_actionState = ACTION_STATE_ADD_TRIGGER;
	m_selectedType = TRIGGER_OBJECT_ID;
}


/**
 * Database windows
 **/

void CEditor::OpenShipDatabase()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit databases while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Ship database", &m_shipDatabase, TRUE);
}

void CEditor::OpenEnemyDatabase()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit databases while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Enemy database", &m_enemyDatabase, TRUE, TRUE, TRUE, TRUE);
}

void CEditor::OpenDynamicObjectDatabase()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit databases while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Dynamic object database", &m_dynamicObjectDatabase, TRUE, TRUE, TRUE, TRUE);
}

void CEditor::OpenStaticObjectDatabase()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit databases while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	//CDatabaseSelectionWnd s;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Static object database", &m_staticObjectDatabase, TRUE, TRUE, TRUE, TRUE);
}

void CEditor::OpenEffectsDatabase()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit databases while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Particle effect database", &m_particleEffectDatabase, TRUE, TRUE, TRUE, TRUE);
}

void CEditor::OpenLightEffectDatabaseWindow()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit databases while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Light effect database", &m_lightEffectHandler, TRUE, TRUE, TRUE, TRUE);
}

void CEditor::OpenAmmoDatabase()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit databases while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Ammo database", &m_ammoDatabaseHandler, TRUE, TRUE, TRUE, TRUE);
}

void CEditor::OpenGadgetDatabase()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit databases while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Gadget database", &m_gadgetDatabaseHandler, TRUE, TRUE, TRUE, FALSE);
}

void CEditor::OpenGlobalScriptsWindow()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit global scripts while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	m_globalScriptsHandler.UseLocalScripts(false);
	s.OpenWindow(GetWindow(), "Global scripts", &m_globalScriptsHandler);
}

void CEditor::OpenSoundsWindow()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit sounds while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Sound database", &m_soundDatabaseHandler);
}

void CEditor::OpenComponentWindow()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit sounds while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Gadget component database", &m_componentHandler, TRUE, TRUE, TRUE);
}

void CEditor::OpenQuestItemWindow()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit sounds while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Gadget quest item database", &m_questItemHandler, TRUE, TRUE, TRUE);
}

void CEditor::OpenShipPatchWindow()
{
	if(m_bLevelLoaded)
		if(MessageBox(m_hWnd, "It is not recommended to edit sounds while a level is loaded.\nIt might cause the program to crash and in worst case mess up the database.\nDo you want to continue?",
			"Are you sure?", MB_OKCANCEL) == IDCANCEL)
			return;
	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Gadget ship patch database", &m_shipPatchHandler, TRUE, TRUE, TRUE);
}


// Tilesets

void CEditor::EditTileset()
{
	if(m_bLevelLoaded)
	{
		ErrorMsgBox("You cannot edit tilesets while level is loaded");
		return;
	}

	std::string tileset;
	if(!CDatabaseUtilities::GetTilesetFilename(GetWindow(), tileset))
	{
		return;
	}

	//std::string filename = "data\\" + tileset;

	if(CTileManager::GetManager()->LoadTileset(tileset.c_str(), false) == FALSE)
	{
		CPrinter::logError("Failed loading default level: failed loading tileset");
		ErrorMsgBox("Failed loading tileset");
		return;
	}

	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Tileset " + tileset, &m_tileDatabase);
}

void CEditor::CreateTileset()
{
	if(m_bLevelLoaded)
	{
		ErrorMsgBox("You cannot create or edit tilesets while level is loaded");
		return;
	}

	CGetStringWnd wnd;
	std::string str;
	if(!wnd.OpenWindow(GetWindow(), str, "Give the name of the tileset"))
	{
		return;
	}

	if(str.size() < 3)
	{
		ErrorMsgBox("Too short tileset name");
		return;
	}
	if(str.size() > 30)
	{
		ErrorMsgBox("Too long tileset name");
		return;
	}

	int size = (int)str.size();
	for(int i=0; i<size; ++i)
		if(!isdigit(str[i]) && !isalpha(str[i]))
		{
			ErrorMsgBox("Given name contained illegal characters: tileset name may only contain letters and digits");
			return;
		}

	std::string filename = "data\\";
	filename += str;
	filename += ".oet";

	// Check whether a tileset with that name already exists
	fstream f;
	f.open(filename.c_str(), ios::in);
	if(f.is_open())
	{
		ErrorMsgBox("Tileset with the given name already exists");
		return;
	}

	CTileManager::GetManager()->ResetTileset();

	CTileManager::GetManager()->SetDefaultTilesetFilename(filename);

	filename = "textures\\tilesets\\";
	filename += str;
	filename += "\\";
	CTileManager::GetManager()->SetTexturePath(filename);

	CreateDirectory(filename.c_str(), NULL);

	filename = "meshes\\tilesets\\";
	filename += str;
	filename += "\\";
	CTileManager::GetManager()->SetMeshPath(filename);

	CreateDirectory(filename.c_str(), NULL);

	CDatabaseSelectionWnd s;
	s.OpenWindow(GetWindow(), "Tileset " + str, &m_tileDatabase);
}

//
void CEditor::OpenTriggerProperties()
{
	CTrigger *t = (CTrigger*)m_selectedObject;
	if(t->GetType() != TRIGGER_OBJECT_ID)
		return;

	STriggerInfo info = t->GetTriggerInfo();
	CTriggerWnd w;
	if(w.OpenWindow(GetWindow(), info, FALSE) == FALSE)
		return;
	t->ApplyTriggerInfo(info);
}

void CEditor::OpenGameObjectProperties()
{
	IGameObject *o = (IGameObject*)m_selectedObject;

	if(o == NULL)
		return;

	CObjectPropertiesWnd w;

	int type = o->GetType();
	SObjectInstanceInfo *info = NULL;

	if(type == STATIC_OBJECT_ID)
	{
		info = ((CStaticObject*)o)->GetObjectInfo(true);
	}
	else if(type == DYNAMIC_OBJECT_ID)
	{
		info = ((CDynamicObject*)o)->GetObjectInfo(true);
	}
	else if(type == ENEMY_OBJECT_ID)
	{
		info = ((CEnemy*)o)->GetObjectInfo(true);
	}

	if(info == NULL)
		return;

	if(w.Open(m_hWnd, info, o))
	{
		if(type == STATIC_OBJECT_ID)
			((CStaticObject*)o)->ApplyObjectInfo(*info, true);
		else if(type == DYNAMIC_OBJECT_ID)
			((CDynamicObject*)o)->ApplyObjectInfo(*info, true);
		else if(type == ENEMY_OBJECT_ID)
			((CEnemy*)o)->ApplyObjectInfo(*info, true);
	}
}

BOOL CALLBACK AboutWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_CLOSE:
		EndDialog(hWnd, FALSE);
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, FALSE);
		}
	}
	return FALSE;
}

void CEditor::OpenAboutWindow()
{
	DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), m_hWnd, AboutWndProc);
}

/****
 * OnKeyDown
 ****/

void CEditor::OnKeyDown(DWORD dwKey)
{
	switch(dwKey)
	{
	case 'M':
		ChangeViewMode();
		break;

	case 'T':
		// Open tile selection vindow
		OpenSelectTile();
		break;

	case 'E':
		OpenInsertEnemy();
		break;

	case 'I':
		if(m_bTileEditMode)
		{
			// If in tile edit mode, rotate the selected tile 90 degrees clockwise
			if(m_selectedObject == NULL)
				break;

			((CTile*)m_selectedObject)->RotateClockwise();
		}
		break;

	case 'O':
		if(m_bTileEditMode)
		{
			if(m_selectedObject == NULL)
				break;

			((CTile*)m_selectedObject)->Elevate();
			if(m_bTileTrimEnabled)
				GetSceneManager()->GetTiledTerrain()->TrimTiles((CTile*)m_selectedObject, m_bRandomizeAutotrim);
			GetSceneManager()->GetTiledTerrain()->UpdateBoundingBox();
		}
		break;

	case 'L':
		if(m_bTileEditMode)
		{
			if(m_selectedObject == NULL)
				break;

			((CTile*)m_selectedObject)->Lower();
			if(m_bTileTrimEnabled)
				GetSceneManager()->GetTiledTerrain()->TrimTiles((CTile*)m_selectedObject, m_bRandomizeAutotrim);
			GetSceneManager()->GetTiledTerrain()->UpdateBoundingBox();
		}
		break;

	case VK_ESCAPE:
		if(m_bLevelLoaded)
		{
			if(AskLevelSaveContinue() == IDYES)
				Close();
		}
		else
			Close();
	}
}

/********
 * Main window message handlers
 ********/

BOOL CEditor::OnMessage(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	if(IApplication::OnMessage(iMessage, wParam, lParam))
		return TRUE;

	switch(iMessage)
	{
	case WM_COMMAND:
		return OnCommand(wParam, lParam);
		break;
	}

	return FALSE;
}

BOOL CEditor::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	/* ** File menu ** */

	case ID_FILE_NEW_LEVEL:
		OpenNewLevelWindow();
		break;
	case ID_FILE_OPEN_LEVEL:
		OpenLevel();
		break;
	case ID_FILE_SAVE_LEVEL:
		SaveLevel();
		break;
	case ID_FILE_SAVEAS:
		SaveLevelAs();
		break;
	case ID_FILE_CLOSE_LEVEL:
		CloseLevel();
		break;
	case ID_FILE_QUIT:
		if(m_bLevelLoaded)
			if(AskLevelSaveContinue() == IDNO)
				break;
		Close();
		break;

	/* ** Edit menu ** */
	case ID_EDIT_LEVELPROPERTIES:
		OpenLevelProperties();
		break;
	case ID_EDIT_LEVELSCRIPTS:
		OpenLevelScripts();
		break;
	case ID_EDIT_TILEEDITMODE:
		ChangeTileMode();
		break;
	case ID_EDIT_SELECTTILE:
		OpenSelectTile();
		break;
	case ID_EDIT_AUTOTRIM:
		ChangeAutoTrim();
		break;
	case ID_EDIT_RANDOMIZEAUTOTRIM:
		ChangeRandomizeAutotrim();
		break;
	case ID_EDIT_DIALOGUES:
		OpenDialoguesWindow();
		break;
	case ID_EDIT_ASSISTANTMESSAGES:
		OpenAssistantMsgsWindow();
		break;
	case ID_EDIT_TERMINALS:
		OpenTerminalsWindow();
		break;
	case ID_EDIT_OBJECTPROPERTIES:
		OpenObjectProperties();
		break;
	case ID_EDIT_REPAIR:
		if(m_bLevelLoaded)
			m_om.RepairObjectPositions();
		break;
	case ID_EDIT_DELETEOBJECT:
		CEditor::DeleteObject();
		break;
	case ID_DELETE_ALL_OBJECTS:
		CEditor::DeleteAllObjects();
		break;
	case ID_EDIT_RAPIDTILEELEVATION:
		ChangeRapidTileElevation();
		break;
	case ID_EDIT_RESETCAMERAPOSITION:
		if(m_bFreeFlyMode)
			RestoreFlyCamToDefaultPosition();
		break;
	case ID_EDIT_WAYPOINTS:
		OpenWaypointSetsWindow();
		break;
	case ID_EDIT_SHOWTRIGGERS:
		ChangeTriggerRenderMode();
		break;
	case ID_EDIT_CAMPAIGN_EDITOR:
		OpenCampaignEditor();
		break;

	/* ** Insert menu ** */
	case ID_INSERT_PLAYER1:
		OpenInsertPlayer();
		break;
	case ID_INSERT_ENEMY:
		OpenInsertEnemy();
		break;
	case ID_INSERT_DYNAMICOBJECT:
		OpenInsertDynamicObject();
		break;
	case ID_INSERT_STATICOBJECT:
		OpenInsertStaticObject();
		break;
	case ID_INSERT_TRIGGER:
		OpenInsertTrigger();
		break;
	case ID_INSERT_WAYPOINTSET:
		OpenInsertWaypointSet();
		break;

	/* ** Database menu ** */
	case ID_DATABASE_SHIPS:
		OpenShipDatabase();
		break;
	case ID_DATABASE_ENEMIES:
		OpenEnemyDatabase();
		break;
	case ID_DATABASE_DYNAMICOBJECTS:
		OpenDynamicObjectDatabase();
		break;
	case ID_DATABASE_STATICOBJECTS:
		OpenStaticObjectDatabase();
		break;
	case ID_DATABASE_EFFECTS:
		OpenEffectsDatabase();
		break;
	case ID_DATABASE_LIGHTEFFECTS:
		OpenLightEffectDatabaseWindow();
		break;
	case ID_DATABASE_AMMOS:
		OpenAmmoDatabase();
		break;
	case ID_DATABASE_GADGETS:
		OpenGadgetDatabase();
		break;
	case ID_DATABASE_GLOBALSCRIPTS:
		OpenGlobalScriptsWindow();
		break;
	case ID_DATABASE_SOUNDS:
		OpenSoundsWindow();
		break;
	case ID_DATABASE_TILESET:
		EditTileset();
		break;
	case ID_DATABASE_CREATETILESET:
		CreateTileset();
		break;
	case ID_DATABASE_COMPONENTS:
		OpenComponentWindow();
		break;
	case ID_DATABASE_QUESTITEMS:
		OpenQuestItemWindow();
		break;
	case ID_DATABASE_SHIPPATCHES:
		OpenShipPatchWindow();
		break;

	/* About */
	case ID_ABOUT:
		OpenAboutWindow();
		break;
	}

	return FALSE;
}
