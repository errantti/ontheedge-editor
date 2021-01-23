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

#include "MultiplayerMode.h"
#include "OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiplayerMode::CMultiplayerMode() : m_bMatchInitialized(FALSE), m_mode(MULTIPLAYER_MODE_QUICKMATCH),
	m_defaultDistance(150.f), m_defaultCameraVector(D3DXVECTOR3(-50.f, 250.f, -50.f)),
	m_camera(NULL), m_cameraTarget(NULL), m_bCountingToLevelEnd(FALSE), m_levelEndCounter(0.f),
	m_minDistance(170.f), m_maxDistance(400.f)
{
	FindMultiplayerMapFilenames();
}

CMultiplayerMode::~CMultiplayerMode()
{
}

void CMultiplayerMode::FindMultiplayerMapFilenames()
{
	m_levelFilenames.clear();

	HANDLE h;
	WIN32_FIND_DATA filedata;
	char filemask[128];

	strcpy(filemask, "levels\\MP_*.oel");

	h = FindFirstFile((char*)filemask, &filedata);	//save handle to first file in the directory

	if(h == INVALID_HANDLE_VALUE)
	{
		CPrinter::log("Couldn't find any multiplayer maps");
		return;
	}

	string filename;
	filename = (char*)filedata.cFileName;
	m_levelFilenames.push_back(filename);

	while(FindNextFile(h, &filedata))	//Get handle to next file as long as there are .tst files
	{
		filename = (char*)filedata.cFileName;
		m_levelFilenames.push_back(filename);
	}
	FindClose(h);
}

bool CMultiplayerMode::InitializeQuickMatch()
{
	CScreenMessages::SScreenMessage msg;
	msg.FillDefaults();
	msg.time = 2.f;

	/***
	 * Select level, ships and gadgets
	 ***/
	m_bMatchInitialized = FALSE;
	
	// Select level from levels\multiplayer directory
	if(m_levelFilenames.size() <= 0)
	{
		msg.message = "There are no multiplayer maps available";
		GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);
		return FALSE;
	}

	int levelIndex = GetOTE()->RandInt(0, m_levelFilenames.size()-1);
	m_nextLevel = m_levelFilenames[levelIndex];
	
	// Select ships for players (randomply from all the ship templates)
	vector<SShipTemplate> *ships = GetOTE()->GetObjectManager()->GetShipTemplates();
	if(ships == NULL)
	{
		CPrinter::log("Internal error: erraneous ship template vector pointer", LOG_ERROR);
		return FALSE;
	}

	if(ships->size() <= 0)
	{
		msg.message = "There are no ship templates available for multiplayer";
		GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);
		return FALSE;
	}

	m_shipID[0] = GetOTE()->RandInt(0, ships->size()-1);
	m_shipID[0] = (*ships)[m_shipID[0]].vt.dt.templateID;

	m_shipID[1] = GetOTE()->RandInt(0, ships->size()-1);
	m_shipID[1] = (*ships)[m_shipID[1]].vt.dt.templateID;

	// Select gadgets for players' ships.
	// The primary gadget is always a weapon, the secondary gadget is selected
	// randomply from all the other gadgets which have bMultiplayer = true
	//int gadgets[2][2];	// gadgets[playerIndex][gadgetID]
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> > mpGadgets = GetOTE()->GetGadgetDatabase()->GetMultiplayerGadgets();
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> > mpWeapons, mpTemp;

	if(mpGadgets.size() <= 0)
	{
		msg.message = "There are no gadgets available for multiplayer";
		GetOTE()->GetScreenMessageInterface()->AddDetailedMessage(msg);
		return FALSE;
	}

	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = mpGadgets.begin();
	// Dump weapon gadgets into another vector
	for(;it!=mpGadgets.end();)
		if(it->first.gadgetType==GADGET_TYPE_WEAPON)
		{
			mpWeapons.push_back(*it);
			it=mpGadgets.erase(it);
		}
		else
			++it;

	int i;
	GADGET_TYPE gadgetType;
	int weaponIndex;
	for(i=0; i<2; ++i)
	{
		m_gadgets[i][0] = m_gadgets[i][1] = -1;
		// Select gadget for player
		// First select a weapon
		if(mpWeapons.size() > 0)
		{
			weaponIndex = GetOTE()->RandInt(0, mpWeapons.size()-1);
			m_gadgets[i][0] = mpWeapons[weaponIndex].first.ID;
			if(mpGadgets.size() > 0)
			{
				weaponIndex = GetOTE()->RandInt(0, mpGadgets.size()-1);
				m_gadgets[i][1] = mpGadgets[weaponIndex].first.ID;
			}
		}
		else
		{
			// If there are no weapons available, select two other gadgets
			weaponIndex = GetOTE()->RandInt(0, mpGadgets.size()-1);
			m_gadgets[i][0] = mpGadgets[weaponIndex].first.ID;
			if(mpGadgets.size() > 1)
			{
				gadgetType = mpGadgets[weaponIndex].first.gadgetType;
				mpTemp.clear();
				// Select the second gadget (of a different type)
				// Copy gadgets of different type to mpTemp
				for(it=mpGadgets.begin(); it!=mpGadgets.end(); ++it)
					if(gadgetType != it->first.gadgetType)
						mpTemp.push_back(*it);
				if(mpTemp.size() <= 0)
					continue;
				weaponIndex = GetOTE()->RandInt(0, mpTemp.size()-1);
				m_gadgets[i][1] = mpTemp[weaponIndex].first.ID;
			}
		}
	}
	m_bMatchInitialized = TRUE;
	m_mode = MULTIPLAYER_MODE_QUICKMATCH;

	return TRUE;
}

bool CMultiplayerMode::StartMatch()
{
	if(m_bMatchInitialized == FALSE)
	{
		CPrinter::log("Trying to start multiplayer match without initializing", LOG_ERROR);
		return FALSE;
	}

	if(FAILED(GetOTE()->StartLevel(m_nextLevel.c_str(), m_shipID[0], m_shipID[1], TRUE, &m_settings)))
	{
		CPrinter::log("Failed starting multiplayer level", LOG_ERROR);
		return FALSE;
	}

	// Equip gadgets
	CPlayer *p;
	for(int i=0;i<2;++i)
	{
		p = GetOTE()->GetPlayer(i);
		p->ResetGadgets();
		p->AddGadget(m_gadgets[i][0]);
		p->AddGadget(m_gadgets[i][1]);
		p->InstantEquipGadget(m_gadgets[i][0], 0);
		p->InstantEquipGadget(m_gadgets[i][1], 1);
	}

	if(InitCamera() == FALSE)
	{
		CPrinter::log("Failed initializing multiplayer camera", LOG_ERROR);
		return FALSE;
	}

	m_bCountingToLevelEnd = FALSE;
	UpdateCamera();
	ISceneManager::GetManager()->SetActiveCamera(m_camera);

	return TRUE;
}

bool CMultiplayerMode::StartQuickMatch()
{
	if(InitializeQuickMatch() == FALSE)
	{
		CPrinter::log("Failed initializing quick match", LOG_ERROR);
		return FALSE;
	}

	/***
	 * Start the game
	 ***/

	return StartMatch();
}

/*** 
 * EndMatch is called when a multiplayer level is closed by the game
 * to decide what to do next
 ***/
void CMultiplayerMode::EndMatch()
{
	m_bMatchInitialized = FALSE;
	RemoveCamera();
	GetOTE()->GetMenuManager()->OpenMenu(MENU_ID_MAIN);
}

void CMultiplayerMode::Update(float frameTime)
{
	UpdateCamera();

	CShip *pl[2] = {NULL, NULL};
	pl[0] = GetOTE()->GetPlayer(0)->GetShip();
	pl[1] = GetOTE()->GetPlayer(1)->GetShip();

	std::string msg;

	if(m_bCountingToLevelEnd && m_levelEndCounter<0.f)
	{
		GetOTE()->EndLevel(TRUE, 2.f);
		m_bCountingToLevelEnd = FALSE;

		if(pl[0] == NULL && pl[1] == NULL)
			msg = "Game ended in draw";
		else if(pl[0] == NULL)
			msg = "Player 2 has won the fight!";
		else
			msg = "Player 1 has won the fight!";
		GetOTE()->AddScreenMessage(msg);
	}

	if(m_bCountingToLevelEnd)
		m_levelEndCounter -= frameTime;
	else
	{
		// If a player has been destroyed, initiate a counter to count time
		// to the end of the level (this is becouse both players might be falling
		// from the platform but the last to drop would win without the counter)
		// If when the counter is at zero one player is still alive then he/she
		// is declared the winner of the match
		if(pl[0] == NULL || pl[1] == NULL)
		{
			m_bCountingToLevelEnd = TRUE;
			m_levelEndCounter = MULTIPLAYER_LEVEL_END_DELAY;
		}
	}
}

bool CMultiplayerMode::InitCamera()
{
	if(m_camera || m_cameraTarget)
		RemoveCamera();

	ISceneManager *mgr = ISceneManager::GetManager();
	if(mgr == NULL)
		return FALSE;

	m_cameraTarget = mgr->AddMeshSceneNode(NULL, NULL, 267, D3DXVECTOR3(150.f, 50.f, 150.f));
	if(m_cameraTarget == NULL)
	{
		CPrinter::log("Failed creating multiplayer camera follow object", LOG_ERROR);
		return FALSE;
	}

	m_camera = mgr->AddCameraFollowSceneNode(m_cameraTarget, 268, m_defaultCameraVector);
	if(m_camera == NULL)
	{
		CPrinter::log("Failed reating multiplayer camera", LOG_ERROR);
		return FALSE;
	}
	m_camera->SetFarValue(800.f);
	m_camera->CalculateProjectionMatrix();

	return TRUE;
}

void CMultiplayerMode::RemoveCamera()
{
	ISceneManager *mgr = ISceneManager::GetManager();

	if(m_camera)
	{
		if(mgr->GetActiveCamera() == m_camera)
			mgr->SetActiveCamera(NULL);
		m_camera->Remove();
		m_camera = NULL;
	}
	if(m_cameraTarget)
	{
		m_cameraTarget->Remove();
		m_cameraTarget = NULL;
	}
}

void CMultiplayerMode::UpdateCamera()
{
	CShip *pl[2] = {NULL, NULL};
	pl[0] = GetOTE()->GetPlayer(0)->GetShip();
	pl[1] = GetOTE()->GetPlayer(1)->GetShip();

	if(pl[0] == NULL || pl[1] == NULL)
		return;							// Don't update if either player is dead

	if(m_camera == NULL || m_cameraTarget == NULL)
		return;

	// Position the camera target between the ships
	D3DXVECTOR3 Ship1to2 = pl[1]->GetTranslation()-pl[0]->GetTranslation();
	D3DXVECTOR3 targetPos = pl[0]->GetTranslation() + 0.5f * Ship1to2;
	m_cameraTarget->SetTranslation(targetPos);

	// Set camera zoom (relative to the distance of the ships from each other)
	// When the distance of the ships is m_defaultDistance the coefficient of the
	// camera direction vector is 1.f
	float dist = sqrtfast(Ship1to2.x*Ship1to2.x + Ship1to2.y*Ship1to2.y + Ship1to2.z*Ship1to2.z);
	if(dist < m_minDistance)
		dist = m_minDistance;
	if(dist > m_maxDistance)
		dist = m_maxDistance;
	D3DXVECTOR3 cameraDir = dist/m_defaultDistance * m_defaultCameraVector;
	m_camera->SetTranslation(cameraDir);
}