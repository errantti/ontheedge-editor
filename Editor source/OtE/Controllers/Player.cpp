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

#include "..\\OnTheEdge.h"
#include "ControllerManager.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayer::CPlayer() : m_ship(NULL), m_bPossessingByGadget(FALSE), m_bPossessing(false), 
	m_bPossessedAggro(false)
{
	m_controls.FillDefaulKeyboard1();
	m_controllerID = CONTROLLER_ID_PLAYER;

	for(int i=0; i<2; ++i)
	{
		m_gadgetChangeCounter[i] = 0.f;
		m_newGadgetID[i] = -1;
		m_bChangingGadget[i] = false;
	}
}

CPlayer::~CPlayer()
{
	RemoveGadgets();
	RemoveItems();
}

void CPlayer::RemoveGadgets()
{
	vector<IGadget*>::iterator it = m_gadgets.begin();
	for(; it!=m_gadgets.end(); ++it)
		if((*it) != NULL)
			delete (*it);
	m_gadgets.clear();
}

void CPlayer::RemoveItems()
{
	std::vector<IInventoryItem*>::iterator it = m_items.begin();
	for(; it != m_items.end(); ++it)
		if((*it) != NULL)
			delete (*it);
	m_items.clear();
}

void CPlayer::SetShip(CShip *ship)
{
	m_ship = ship;
	if(m_ship != NULL)
		m_bPossessing = false;
	m_mouseMovementBuffer = 0.f;
	m_timeFromMouseChange = 0.f;
}

void CPlayer::Update(IGameObject *object, float frameTime)
{
	if(m_ship == NULL)
	{
		if(m_bPossessing && m_possessed)
		{
			m_possessed->SetController(NULL);
			m_bPossessing = false;
			m_possessed = NULL;
		}
		return;
	}

	// Player can control either an enemy or ship
	if(object->GetType() != SHIP_OBJECT_ID && object->GetType() != ENEMY_OBJECT_ID)
		return;

	CVessel *vessel = (CVessel*)object;
	//CShip *ship = (CShip*)object;
	CGameSettings &settings = *CGameSettings::GetSettingsObject();

	bool d1=FALSE,d2=FALSE;
	
	if(m_controls.keyboard)
	{
		CInputKeyboard *k = GetEdgewalkerApp()->GetKeyboard();
		CInputMouse *m = GetEdgewalkerApp()->GetMouse();

		if(k->GetButtonDown(m_controls.left))
		{
			vessel->Turn(-frameTime);
		}
		else if(k->GetButtonDown(m_controls.right))
		{
			vessel->Turn(frameTime);
		}
		else if(settings.GetSetting(GO_MOUSE_ENABLED))
		{
			// Check whether mouse wheel has been rolled and change weapon if it has
			int relZ = m->GetRelativeZ();
			if(relZ < 0)
				PrevWeapon();
			else if(relZ > 0)
				NextWeapon();

			// If none of the keyboard turning keys was pressed, check mouse

			int relX = m->GetRelativeX();
			float coef = 0.f;
			float maxTurnSpeed = vessel->GetTurnSpeed();
			float mouseSensitivity = settings.GetSetting(GO_MOUSE_SENSITIVITY);

			m_mouseMovementBuffer += (float)relX*mouseSensitivity;
			if((relX < 0 && m_mouseMovementBuffer > 0.f) || (relX > 0 && m_mouseMovementBuffer < 0.f))
				m_mouseMovementBuffer = -m_mouseMovementBuffer;

			if(relX == 0)
				m_timeFromMouseChange += frameTime;
			else
				m_timeFromMouseChange = 0.f;

			if(m_timeFromMouseChange>0.04f)
			{
				m_mouseMovementBuffer = 0.f;
			}
			else
			{
				coef = m_mouseMovementBuffer;
			}

			if(coef != 0.f)
			{
				if(coef > maxTurnSpeed)
					coef = maxTurnSpeed;
				else if(coef < -maxTurnSpeed)
					coef = -maxTurnSpeed;

				float old = m_mouseMovementBuffer;
				m_mouseMovementBuffer -= coef;
				if(old*m_mouseMovementBuffer < 0.f)	// Check whether the negativity/positivity has changed
					m_mouseMovementBuffer = 0.f;

				coef *= frameTime;

				vessel->TurnManual(coef);
			}/**/
/*void getPickRay(D3DXVECTOR3& vPickRayOrig, D3DXVECTOR3&vPickRayDir, int x, int y)
{
	LPDIRECT3DDEVICE9 m_pd3dDevice = GetEdgewalkerApp()->GetDevice();
	D3DXMATRIX matProj, matView, m;
	int mX,mY;
	D3DXVECTOR3 v;//, aposvec;
//	D3DVIEWPORT9 vp;
//	LPDIRECT3DSURFACE9 backbuf;
//	D3DSURFACE_DESC bfdesc;



	m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
//	m_pd3dDevice->GetViewport(&vp);
//	m_pd3dDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&backbuf);
//	backbuf->GetDesc(&bfdesc);



	
	mX = x;  // horizontal position of cursor 
	mY = y;  // vertical position of cursor 
 

	// Compute the vector of the pick ray in screen space
	v.x =  ( ( ( 2.0f * mX ) / GetEdgewalkerApp()->GetWidth() ) - 1) / matProj._11;
	v.y = -( ( ( 2.0f * mY ) / GetEdgewalkerApp()->GetHeight() ) - 1) / matProj._22;
	v.z =  1.0f;

	// Get the inverse view matrix
	m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
//	matView = kamera.getViewMatrix();
	D3DXMatrixInverse( &m, NULL, &matView );

	// Transform the screen space pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;

	// Normalize the dir to be sure
	D3DXVec3Normalize(&vPickRayDir, &vPickRayDir);
}

void CPlayer::Update(IGameObject *object, float frameTime)
{
	if(m_ship == NULL)
	{
		if(m_bPossessing && m_possessed)
		{
			m_possessed->SetController(NULL);
			m_bPossessing = false;
			m_possessed = NULL;
		}
		return;
	}

	// Player can control either an enemy or ship
	if(object->GetType() != SHIP_OBJECT_ID && object->GetType() != ENEMY_OBJECT_ID)
		return;

	CVessel *vessel = (CVessel*)object;
	//CShip *ship = (CShip*)object;
	CGameSettings &settings = *CGameSettings::GetSettingsObject();

	bool d1=FALSE,d2=FALSE;
	
	if(m_controls.keyboard)
	{
		CInputKeyboard *k = GetEdgewalkerApp()->GetKeyboard();
		CInputMouse *m = GetEdgewalkerApp()->GetMouse();

		if(k->GetButtonDown(m_controls.left))
		{
			vessel->Turn(-frameTime);
		}
		else if(k->GetButtonDown(m_controls.right))
		{
			vessel->Turn(frameTime);
		}
		else if(settings.GetSetting(GO_MOUSE_ENABLED))
		{
			// If none of the keyboard turning keys was pressed, check mouse
			GetOTE()->GetGUIEnvironment()->SetCursorVisibility(true);
			D3DXVECTOR3 pickRayDir;
			D3DXVECTOR3 pickRayOrig;

  			int absX = m->GetX();
			int absY = m->GetY();

			getPickRay(pickRayOrig, pickRayDir, absX, absY);

			int relX = m->GetRelativeX();
			float coef = 0.f;
			float maxTurnSpeed = vessel->GetTurnSpeed();
			float mouseSensitivity = settings.GetSetting(GO_MOUSE_SENSITIVITY);

			float t = (vessel->GetAbsolutePosition().y - pickRayOrig.y) / pickRayDir.y;

			D3DXVECTOR3 cursorpos3d = pickRayOrig + t * pickRayDir;

			D3DXVECTOR3 vesseldirection;
			D3DXVECTOR3 targetdirection; 
			D3DXVECTOR3 vesselpos = vessel->GetAbsolutePosition();

			D3DXVec3Normalize(&targetdirection, &(cursorpos3d - vessel->GetAbsolutePosition()));
			
			vesseldirection.x = sinf(vessel->GetRotation().y);
			vesseldirection.y = 0;
			vesseldirection.z = cosf(vessel->GetRotation().y);

			float dotp = D3DXVec3Dot(&vesseldirection, &targetdirection);

			if(-vesseldirection.z*targetdirection.x + vesseldirection.x * targetdirection.z < 0) 
			{
				//turn right
				if ( ((1 - dotp)/2) < frameTime * maxTurnSpeed )
				{
					vessel->Turn(((1 - dotp)/2));
				}
				else
				{
					vessel->Turn(frameTime * maxTurnSpeed );
				}
			}
			else
			{
				//turn left
				if ( ((1 - dotp)/2) < frameTime * maxTurnSpeed )
				{
					vessel->Turn(-((1 - dotp)/2));
				}
				else
				{
					vessel->Turn(-frameTime * maxTurnSpeed);
				}
			}/**/
		}

		if(k->GetButtonDown(m_controls.boost))
		{
			if(object->GetType() == SHIP_OBJECT_ID)
			{
				if(((CShip*)vessel)->BoostedThrust(frameTime) == FALSE)
					vessel->Thrust();
			}
			else if(m_bPossessing)
			{
				UnPosses();
			}
			else
				vessel->Thrust();
		}
		else if(k->GetButtonDown(m_controls.forward))
		{
			vessel->Thrust();
		}
		else if(k->GetButtonDown(m_controls.back))
			vessel->BackThrust();

		if(k->GetButtonDown(m_controls.thrustLeft))
			vessel->SideThrust(true);
		else if(k->GetButtonDown(m_controls.thrustRight))
			vessel->SideThrust(false);

		if(k->GetButtonDown(m_controls.gadget1))
			d1 = TRUE;
		if(k->GetButtonDown(m_controls.gadget2))
			d2 = TRUE;

		// TODO: TESTTEST
		if(m->GetMouseLeftDown())
			d1 = TRUE;
		if(m->GetMouseRightDown())
			d2 = TRUE;
	}
	else
	{
		CInputJoystick *j = GetEdgewalkerApp()->GetJoystick();

		if(j->GetState()->lX > 5)
			vessel->Turn(frameTime);
		else if(j->GetState()->lX < -5)
			vessel->Turn(-frameTime);

		if(j->GetButtonDown(m_controls.boost))
		{
			if(object->GetType() == SHIP_OBJECT_ID)
			{
				if(((CShip*)vessel)->BoostedThrust(frameTime) == FALSE)
					vessel->Thrust();
			}
			else if(m_bPossessing)
			{
				UnPosses();
			}
			else
				vessel->Thrust();
		}
		else if(j->GetState()->lY < -5 || j->GetButtonDown(m_controls.forward))
		{
			vessel->Thrust();
		}
		else if(j->GetState()->lY > 5)
			vessel->BackThrust();

		if(j->GetButtonDown(m_controls.gadget1))
			d1 = TRUE;
		if(j->GetButtonDown(m_controls.gadget2))
			d2 = TRUE;
	}

	// TODO: test, remove
	/*static IGadget *g11 = 0;
	static bool meh = false;
	if(g11 == 0)
	{
		g11 = GetOTE()->GetGadgetDatabase()->CreateGadget(1);
		m_gadgets.push_back(g11);
		g11->Equip(m_ship, m_ship->GetEnergyReference());
	}
	else
	{
		if(k->GetButtonDown(DIK_H) && !meh)
		{
			meh = true;
			g11->Activate();
		}
		if(!k->GetButtonDown(DIK_H))
			meh = false;
	}*/

	// Update gadget change
	for(int i=0; i<2; ++i)
	{
		if(m_bChangingGadget[i])
		{
			m_gadgetChangeCounter[i] -= frameTime;
			if(m_gadgetChangeCounter[i] < 0.f)
			{
				// Assume that the operation which initiated gadget equipment checked the required load
				InstantEquipGadget(m_newGadgetID[i], TRUE, FALSE);
				m_bChangingGadget[i] = FALSE;
			}
		}
	}

	if(object->GetType() == SHIP_OBJECT_ID)
		m_ship->UpdateGadgets(d1, d2);
	else if(object->GetType() == ENEMY_OBJECT_ID)
	{
		m_ship->UpdateGadgets(false, false);
		if(d1 && ((CEnemy*)object)->GetGadget(0))
			((CEnemy*)object)->GetGadget(0)->Activate();
		if(d2 && ((CEnemy*)object)->GetGadget(1))
			((CEnemy*)object)->GetGadget(1)->Activate();
	}
}

void CPlayer::RegisterCollision(IGameObject *object, IGeometricObject *other)
{
	// CPlayer class is meant to control only CShip-type of objects
	/*if(object != m_ship)
	{
		return;
	}*/

	bool bShip = false;
	if(object->GetType() == SHIP_OBJECT_ID)
		bShip = true;
	else if(object->GetType() == ENEMY_OBJECT_ID)
		bShip = false;
	else
		return;

	char *targetRef;
	long targetID;

	CVessel *vessel = (CVessel*)object;
	//CShip *ship = (CShip*)object;
	CObjectManagerSceneNode *omsn = GetOTE()->GetObjectManagerSceneNode();
	//IGameObject *target;

	CStaticObject *s = NULL;
	switch(other->GetType())
	{
	case STATIC_OBJECT_ID:
		s = (CStaticObject*)other;
		switch(s->GetStaticObjectType())
		{
		/*** 
		 * ADD ITEMS
		 ***/

		// Gadget ****
		case STATIC_OBJECT_GADGET:
			if(GetOTE()->IsMultiplayer() == FALSE && bShip)
			{
				AddGadget(s->GetTargetID(), TRUE, FALSE);
				((CStaticObject*)other)->Destroy();
			}
			break;

		// Add gadget component ****
		case STATIC_OBJECT_COMPONENT:
			if(GetOTE()->IsMultiplayer() == FALSE && bShip)
			{
				AddComponent(s->GetTargetID(), TRUE);
				((CStaticObject*)other)->Destroy();
			}
			break;

		// Add quest item ****
		case STATIC_OBJECT_QUESTITEM:
			if(GetOTE()->IsMultiplayer() == FALSE && bShip)
			{
				AddQuestItem(s->GetTargetID(), TRUE);
				((CStaticObject*)other)->Destroy();
			}
			break;

		// Add ship patch ****
		case STATIC_OBJECT_PATCH:
			if(GetOTE()->IsMultiplayer() == FALSE && bShip)
			{
				AddPatch(s->GetTargetID(), TRUE);
				((CStaticObject*)other)->Destroy();
			}
			break;

		case STATIC_OBJECT_REPAIR_POD:
			if(GetOTE()->IsMultiplayer() == FALSE)
			{
				//vessel->Repair(GetOTE()->GetCurrentLevel()->GetRepairPodRegeneration() * GetOTE()->GetFrameTime());
				//s->ReportActivation(NULL);
				s->RestoreHealth(vessel, GetOTE()->GetFrameTime());
			}
			break;
		case STATIC_OBJECT_ENERGY_POD:
			if(GetOTE()->IsMultiplayer() == FALSE)
			{
				//vessel->AddEnergy(GetOTE()->GetCurrentLevel()->GetEnergyPodRegeneration() * GetOTE()->GetFrameTime());
				//s->ReportActivation(NULL);
				s->RestoreEnergy(vessel, GetOTE()->GetFrameTime());
			}
			break;
		case STATIC_OBJECT_LEVEL_TELEPORT:
			// End level if entering the teleport is the end condition in the level
			if(GetOTE()->IsMultiplayer() == FALSE && bShip)
			{
				targetID = ((CStaticObject*)other)->GetTargetID();
				if((targetRef = ((CStaticObject*)other)->GetTargetRef()) != NULL)// && (targetID = ) != -1)
					GetOTE()->GetCampaign()->ForceEndLevel(targetRef, targetID);
			}
			break;
		/*case STATIC_OBJECT_NORMAL_TELEPORT:
			// First get the target object
			if(s->GetTargetID() != -1 && bShip)
			{
				target = omsn->GetObject(s->GetTargetID());
				if(target != NULL)
				{
					D3DXVECTOR3 targetPos = target->GetTranslation();
					targetPos.y += 10.f;
					m_ship->SetTranslation(targetPos);
					s->ReportActivation(m_ship);
				}
			}
			break;*/
		}
		break;
	}
}

void CPlayer::OnDeath(IGameObject *object)
{
	if(!m_bPossessing)
		return;
	if(object == m_possessed)
	{
		UnPosses();
		GetOTE()->AddScreenMessage("Possessed enemy destroyed");
	}
}

D3DXVECTOR3 CPlayer::GetFocusPoint()
{
	if(m_bPossessing)
		return m_possessed->GetTranslation();
	else
		return m_ship->GetTranslation();
}

void CPlayer::PossessEnemy(CEnemy *target)
{
	if(target == NULL)
		return;
	m_ship->SetController(NULL);
	target->Possess(this);
	GetOTE()->ReportNewFocusTarget(target);
	m_bPossessing = true;
	m_possessed = target;
	m_bPossessedAggro = false;
}

void CPlayer::UnPosses()
{
	if(!m_bPossessing || m_possessed == NULL || m_ship == NULL)
	{
		m_bPossessing = false;
		m_possessed = NULL;
		return;
	}

	m_ship->SetController(this);
	((CEnemy*)m_possessed)->UnPossess();
	((CEnemy*)m_possessed)->ReportThreat(m_ship, 2000.f);

	GetOTE()->ReportNewFocusTarget(m_ship);

	m_bPossessing = false;
	m_possessed = NULL;
}

CVessel *CPlayer::GetPossessedEnemy()
{
	if(m_bPossessing)
		return m_possessed;
	return NULL;
}

CVessel *CPlayer::GetPrimaryAggroTarget()
{
	if(m_bPossessing && m_bPossessedAggro)
		return m_possessed;
	return m_ship;
}

CVessel *CPlayer::GetSecondaryAggroTarget()
{
	if(m_bPossessing && m_bPossessedAggro)
		return m_ship;
	return NULL;
}

void CPlayer::ReportPossessedAggro()
{
	m_bPossessedAggro = true;
}










/*********************
 * Items and gadgets stuff
 *********************/

bool CPlayer::AddGadget(long ID, bool bNotify, bool bSkipExtraChecks)
{
	IGadget *oldGadget = GetGadgetInInventory(ID);
	if(oldGadget != NULL)
	{
		if(bNotify)
			GetOTE()->AddScreenMessage("You have that already");
		return TRUE;
	}

	// If there's a gadget component in the inventory that can upgrade
	// this gadget then immediately do the upgrade
	bool autoUpgraded = false;
	CGadgetComponent *component = GetGadgetComponentInInventory(ID);
	if(component != NULL)
	{
		ID = component->GetUpgradedGadgetID();
		autoUpgraded = true;
		RemoveItem(component);

		oldGadget = GetGadgetInInventory(ID);
		if(oldGadget != NULL)
		{
			if(bNotify)
				GetOTE()->AddScreenMessage("You have that already");
			return TRUE;
		}
	}

	// The player doesn't have the gadget so add it
	IGadget *gadget = GetOTE()->GetGadgetDatabase()->CreateGadget(ID);
	if(gadget)
	{
		//m_gadgets.push_back(gadget);
		m_gadgets.insert(m_gadgets.begin(), gadget);

		if(bNotify)
		{
			std::string msg;
			if(autoUpgraded)
				msg = "New gadget: ";
			else
				msg = "New gadget: ";
			msg += gadget->GetName();
			if(autoUpgraded)
				msg += " (upgraded)";
			GetOTE()->GetScreenMessageInterface()->AddMessage(msg);
		}

		if(!bSkipExtraChecks)
		{
			if(!gadget->IsActivateable())
			{
				// Auto-equip first secondary gadget
				if(CountSecondaryGadgets() == 1)
					EquipGadget(gadget->GetID(), true);
			}
			else if(!autoUpgraded)
			{
				// Assign a key shortcut for each new weapon gadget (that isn't an upgrade)
				AssignKeyShortcut(gadget, bNotify);
			}
		}

		return TRUE;
	}
	return FALSE;
}

bool CPlayer::AddComponent(long ID, bool bNotify)
{
	// See if the player already has it
	CGadgetComponent *component = (CGadgetComponent*)GetItemInInventory(INVENTORY_ITEM_COMPONENT, ID);
	if(component != NULL)
	{
		if(bNotify)
			GetOTE()->AddScreenMessage("You already have that");
		return TRUE;
	}

	component = CItemManager::GetManager().CreateGadgetComponent(ID);
	//component = CItemManager::GetManager().CreateGadgetComponent(1);
	if(component == NULL)
	{
		CPrinter::logError("Failed creating gadget component: id not found");
		return FALSE;
	}

	if(ReplaceGadget(component->GetTargetGadgetID(), component->GetUpgradedGadgetID()))
	{
		if(bNotify)
		{
			IGadget *g = GetGadgetInInventory(component->GetUpgradedGadgetID());

			if(g != NULL)
			{
				std::string str("Upgraded to ");
				str += g->GetName();
				GetOTE()->AddScreenMessage(str);
			}
			else
				GetOTE()->AddScreenMessage("Gadget upgraded!");
		}
		delete component;
		return TRUE;
	}

	m_items.push_back(component);

	if(bNotify)
	{
		std::string str = "New gadget component";
		//str += component->GetName();
		GetOTE()->AddScreenMessage(str);
	}

	return TRUE;
}

IInventoryItem *CPlayer::AddQuestItem(long ID, bool bNotify)
{
	CQuestItem *item = (CQuestItem*)GetItemInInventory(INVENTORY_ITEM_QUESTITEM, ID);
	if(item != NULL)
	{
		if(bNotify)
			GetOTE()->AddScreenMessage("You already have that item");
		return item;
	}

	item = CItemManager::GetManager().CreateQuestItem(ID);
	if(item == NULL)
	{
		CPrinter::logError("Failed creating quest item: id not found");
		return NULL;
	}

	m_items.push_back(item);

	if(bNotify)
	{
		std::string str = "New item: ";
		str += item->GetName();
		GetOTE()->AddScreenMessage(str);
	}

	return item;
}

bool CPlayer::AddPatch(long ID, bool bNotify)
{
	vector<CShipPatch> &patches = m_ship->m_patches;
	vector<CShipPatch>::iterator it = patches.begin();
	for(; it != patches.end(); ++it)
		if(it->GetID() == ID)
		{
			if(bNotify)
				GetOTE()->AddScreenMessage("You already have that patch!");
			return TRUE;
		}

	CShipPatch *patch = CItemManager::GetManager().CreateShipPatch(ID);
	if(patch == NULL)
	{
		CPrinter::logError("Failed creating ship patch: id not found");
		return FALSE;
	}

	patch->Attach(*m_ship);

	patches.push_back(*patch);

	if(bNotify)
	{
		string str = "Patch ";
		str += patch->GetName();
		str += " installed";
		GetOTE()->AddScreenMessage(str);
	}

	delete patch;

	return TRUE;
}

void CPlayer::AssignKeyShortcut(IGadget *gadget, bool bNotify)
{
	long hotkeys[5];
	CCampaign *c = GetOTE()->GetCampaign();
	c->GetGadgetHotkeyIDs(hotkeys);
	for(int i=0; i<5; ++i)
		if(hotkeys[i] == -1)
		{
			c->SetGadgetHotkeyID(i, gadget->GetID());
			if(bNotify)
			{
				CGameSettings &s = *CGameSettings::GetSettingsObject();
				BYTE *keys = s.GetGadgetShortcuts();
				GetOTE()->AddScreenMessage("Hotkey: " + s.GetKeyString(keys[i]));
			}
			return;
		}
}

int CPlayer::CountSecondaryGadgets()
{
	int count = 0;
	vector<IGadget*>::iterator it = m_gadgets.begin();
	for(; it!=m_gadgets.end(); ++it)
		if(!(*it)->IsActivateable())
			++count;
	return count;
}

bool CPlayer::ReplaceGadget(long sourceID, long upgradedID)
{
	IGadget *oldGadget = GetGadgetInInventory(sourceID);
	if(oldGadget == NULL)
		return FALSE;

	// See if the player already has the upgraded gadget
	IGadget *gadget = GetGadgetInInventory(upgradedID);
	if(gadget != NULL)
		return TRUE;

	gadget = GetOTE()->GetGadgetDatabase()->CreateGadget(upgradedID);
	if(gadget == NULL)
	{
		CPrinter::logError("Failed creating gadget: id not found");
		return FALSE;
	}

	// Now remove the gadget from the ship
	int oldGadgetShipIndex = -1;
	if(m_ship->GetGadget(0) == oldGadget)
		oldGadgetShipIndex = 0;
	else if(m_ship->GetGadget(1) == oldGadget)
		oldGadgetShipIndex = 1;
	else if(m_bChangingGadget[0] && m_newGadgetID[0] == oldGadget->GetID())
		oldGadgetShipIndex = 0;
	else if(m_bChangingGadget[1] && m_newGadgetID[1] == oldGadget->GetID())
		oldGadgetShipIndex = 1;

	// Report gadget change to update gadget hotkeys if necessary
	GetOTE()->GetCampaign()->ReportGadgetUpgrade(sourceID, upgradedID);

	if(oldGadgetShipIndex != -1)
		if(oldGadget->GetGadgetType() == GADGET_TYPE_POSSESS)
			if(m_bPossessingByGadget && m_bPossessing)
				UnPosses();
	oldGadget->UnEquip();
	ForceRemoveGadget(oldGadget);

	m_gadgets.insert(m_gadgets.begin(), gadget);

	m_ship->SetGadget(oldGadgetShipIndex, gadget);
	gadget->Equip(m_ship);

	return TRUE;
}

void CPlayer::UnEquipGadget(int slot)
{
	if(m_ship == NULL)
		return;
	if(m_bPossessing)
	{
		GetOTE()->AddScreenMessage("Can't change gadgets while possessing");
		return;
	}
	if(slot<0||slot>1)
		return;
	if(m_ship->m_gadgets[slot])
	{
		m_bChangingGadget[slot] = false;
		m_ship->m_gadgets[slot]->UnEquip();
		m_ship->m_gadgets[slot] = NULL;
	}
}

IGadget *CPlayer::GetShipGadget(int slot, bool &outIsBeingEquipped)
{
	if(m_ship == NULL)
		return NULL;
	if(slot<0||slot>1)
		return NULL;

	if(m_bChangingGadget[slot])
	{
		outIsBeingEquipped = true;
		return GetGadgetInInventory(m_newGadgetID[slot]);
	}
	else
	{
		outIsBeingEquipped = false;
		return m_ship->m_gadgets[slot];
	}
}

bool CPlayer::EquipGadget(long ID, bool bNotify)
{
	if(m_ship == NULL)
		return FALSE;
	if(m_bPossessing)
	{
		if(bNotify)
			GetOTE()->AddScreenMessage("Can't change equipped items while possessing");
		return false;
	}

	vector<IGadget*>::iterator it = m_gadgets.begin();
	for(; it!=m_gadgets.end(); ++it)
		if((*it)->GetID() == ID)
		{
			// Check if the same kind of gadget is already equipped in the other slot
			/*if(m_ship->m_gadgets[(int)(!slot)])
				if(m_ship->m_gadgets[(int)(!slot)]->GetGadgetType() == (*it)->GetGadgetType())
					return FALSE;*/

			int slot = 1, other = 0;
			if((*it)->IsActivateable())
			{
				slot = 0;
				other = 1;
			}

			bool changing2;
			IGadget *g2 = GetShipGadget(other, changing2);

			// Check whether ship capacity is sufficient to hold both the gadgets
			if(g2 != NULL)
			{
				if(g2->GetCapacityRequirement() + (*it)->GetCapacityRequirement() > m_ship->GetMaxLoad())
				{
					// If not, check if it's at least sufficient to hold the new gadget alone
					if((*it)->GetCapacityRequirement() > m_ship->GetMaxLoad())
					{
						// No, so abort
						if(bNotify)
							GetOTE()->AddScreenMessage("Cannot carry that much");
						return false;
					}

					if(bNotify)
						GetOTE()->AddScreenMessage("Insufficient capacity");

					if(changing2)
						m_bChangingGadget[other] = false;
					else
					{
						m_ship->m_gadgets[other]->UnEquip();
						m_ship->m_gadgets[other] = NULL;
					}
				}
			}
			else if((*it)->GetCapacityRequirement() > m_ship->GetMaxLoad())
			{
				// No, so abort
				if(bNotify)
					GetOTE()->AddScreenMessage("Insufficient capacity");
				return false;
			}

			// Unequip existing gadget in that slot
			if(m_ship->m_gadgets[slot])
			{
				m_ship->m_gadgets[slot]->UnEquip();
				m_ship->m_gadgets[slot] = NULL;
			}

			// It's ok to equip
			m_bChangingGadget[slot] = TRUE;
			//m_gadgetChangeCounter[slot] = GADGET_CHANGE_DELAY;
			m_gadgetChangeCounter[slot] = 0.7f;
			m_newGadgetID[slot] = ID;

			return TRUE;;
		}

	return FALSE;
}

bool CPlayer::InstantEquipGadget(long ID, bool bNotify, bool bCheckLoad)
{
	if(m_ship == NULL)
		return FALSE;
	if(m_bPossessing)
	{
		if(bNotify)
			GetOTE()->AddScreenMessage("Can't change gadgets while possessing");
		return false;
	}

	std::string s;

	vector<IGadget*>::iterator it = m_gadgets.begin();
	for(; it!=m_gadgets.end(); ++it)
		if((*it)->GetID() == ID)
		{
			// Check if the same kind of gadget is already equipped in the other slot
			/*if(m_ship->m_gadgets[(int)(!slot)])
				if(m_ship->m_gadgets[(int)(!slot)]->GetGadgetType() == (*it)->GetGadgetType())
					return FALSE;*/

			int slot = 1, other = 0;
			if((*it)->IsActivateable())
			{
				slot = 0;
				other = 1;
			}

			if(m_ship->m_gadgets[slot])
				m_ship->m_gadgets[slot]->UnEquip();
			if(bCheckLoad)
			{
				if(m_ship->GetLoad()+(*it)->GetCapacityRequirement() > m_ship->GetMaxLoad())
				{
					m_ship->m_gadgets[slot] = NULL;
					s = "Insufficient capacity";
					GetOTE()->GetScreenMessageInterface()->AddMessage(s);
					return FALSE;
				}
			}

			m_ship->m_gadgets[slot] = (*it);
			(*it)->Equip(m_ship);

			if(bNotify)
			{
				s = (*it)->GetName();
				s +=" equipped";
				GetOTE()->GetScreenMessageInterface()->AddMessage(s);
			}
			return TRUE;
		}
	return FALSE;
}

/*bool CPlayer::InstantEquipExternalGadget(IGadget *gadget, bool bNotify)
{
	if(m_ship == NULL || gadget == NULL)
		return FALSE;

	int slot = 1;
	if(gadget->IsActivateable())
		slot = 0;

	if(m_ship->m_gadgets[slot])
		m_ship->m_gadgets[slot]->UnEquip();
	m_ship->m_gadgets[slot] = gadget;
	gadget->Equip(m_ship);

	if(bNotify)
	{
		std::string s;
		s = gadget->GetName();
		s +=" equipped";
		GetOTE()->GetScreenMessageInterface()->AddMessage(s);
	}

	return TRUE;
}*/

/*void CPlayer::EquipNextGadget(int slot)
{
	if(m_ship == NULL || slot < 0 || slot > 1 || m_gadgets.size() == 0)
		return;

	int ID = m_ship->m_gadgets[slot]->GetID();
	IGadget *g1 = m_ship->m_gadgets[slot];
	IGadget *g2 = m_ship->m_gadgets[(int)(!slot)];

	if(m_gadgets.size() == 1)
	{
		if(g1 || g2) return;
		EquipGadget(m_gadgets[0]->GetID());
		return;
	}

	vector<IGadget*>::iterator it = m_gadgets.begin(), it2;
	if(g1)
	{
		for(; it!=m_gadgets.end(); ++it)
			if((*it)->GetID() == ID)
			{
				it2 = it;
				++it;
				if(it == m_gadgets.end())
					it = m_gadgets.begin();
				break;
			}
		if(it == m_gadgets.end())
			return;
	}

	// it is now pointing to the next possible gadget
	for(; it!=it2; ++it)
	{
		if(g2 == NULL)
		{
			EquipGadget((*it)->GetID());
			return;
		}
		if(g2->GetGadgetType() != (*it)->GetGadgetType())
		{
			// Player can't have two gadgets of the same kind equipped at the same time
			EquipGadget((*it)->GetID());
			return;
		}
		if(it == m_gadgets.end())
			it = m_gadgets.begin();
	}
}*/

IGadget *CPlayer::GetEquippedGadget(int slot)
{
	if(slot < 0 || slot > 1)
		return NULL;
	return m_ship->m_gadgets[slot];
}

void CPlayer::StopChangingGadgets()
{
	for(int i=0; i<2; ++i)
		m_bChangingGadget[i] = false;
}

IGadget *CPlayer::GetFirstGadgetInInventory()
{
	if(m_gadgets.size() == 0)
		return NULL;
	return m_gadgets[0];
}

void CPlayer::Reset()
{
	ResetGadgets();
	m_ship = NULL;
}

void CPlayer::ResetGadgets()
{
	if(m_ship)
		m_ship->RemoveGadgets();
	RemoveGadgets();
	for(int i=0;i<2;++i)
		m_bChangingGadget[i] = FALSE;
}

void CPlayer::ClearItemsAndGadgets()
{
	ResetGadgets();
	RemoveItems();
}

void CPlayer::GetGadgetIDsInInventory(vector<long> &gadgetIDs)
{
	vector<IGadget*>::iterator it = m_gadgets.begin();
	for(; it!=m_gadgets.end(); ++it)
		if(*it != NULL)
			gadgetIDs.push_back((*it)->GetID());
}

void CPlayer::GetEquippedGadgetIDs(long &gadget1, long &gadget2)
{
	gadget1 = gadget2 = -1;
	if(m_ship)
	{
		if(m_ship->m_gadgets[0])
			gadget1 = m_ship->m_gadgets[0]->GetID();
		if(m_ship->m_gadgets[1])
			gadget2 = m_ship->m_gadgets[1]->GetID();
	}
}

CGadgetComponent *CPlayer::GetGadgetComponentInInventory(long sourceGadgetID)
{
	vector<IInventoryItem*>::iterator it = m_items.begin();
	for(; it != m_items.end(); ++it)
		if((*it)->GetItemType() == INVENTORY_ITEM_COMPONENT)
		{
			if(((CGadgetComponent*)(*it))->GetTargetGadgetID() == sourceGadgetID)
				return (CGadgetComponent*)(*it);
		}
	return NULL;
}

IInventoryItem *CPlayer::GetItemInInventory(INVENTORY_ITEM_TYPE type, long id)
{
	vector<IInventoryItem*>::iterator it = m_items.begin();
	for(; it != m_items.end(); ++it)
		if((*it)->GetItemType() == type)
			if((*it)->GetID() == id)
				return (*it);
	return NULL;
}

IGadget *CPlayer::GetGadgetInInventory(long gadgetID)
{
	vector<IGadget*>::iterator it = m_gadgets.begin();
	for(; it!=m_gadgets.end(); ++it)
		if((*it)->GetID() == gadgetID)
			return (*it);
	return NULL;
}

void CPlayer::RemoveItem(IInventoryItem *corpse)
{
	if(corpse == NULL)
		return;
	vector<IInventoryItem*>::iterator it = m_items.begin();
	for(; it != m_items.end(); ++it)
		if(*it == corpse)
		{
			delete (*it);
			m_items.erase(it);
			return;
		}
}

void CPlayer::RemoveGadget(long gadgetID)
{
	int i;
	vector<IGadget*>::iterator it = m_gadgets.begin();
	for(; it!=m_gadgets.end(); ++it)
		if((*it)->GetID() == gadgetID)
		{
			for(i=0;i<2;++i)
				if(m_newGadgetID[i] == gadgetID)
					m_bChangingGadget[i] = false;
			for(i=0;i<2;++i)
				if(m_ship->m_gadgets[i] == (*it))
				{
					if(m_bPossessingByGadget)
						UnPosses();
					m_ship->m_gadgets[i] = NULL;
				}
			delete (*it);
			m_gadgets.erase(it);
			return;
		}
}

void CPlayer::ForceRemoveGadget(IGadget *corpse)
{
	if(corpse == NULL)
		return;
	vector<IGadget*>::iterator it = m_gadgets.begin();
	for(; it!=m_gadgets.end(); ++it)
		if(*it == corpse)
		{
			delete *it;
			m_gadgets.erase(it);
			return;
		}
}

void CPlayer::GetAllItemsInInventory(std::vector<SItemTemp> &items)
{
	std::vector<IGadget*>::iterator it = m_gadgets.begin();
	std::vector<IInventoryItem*>::iterator it2 = m_items.begin();
	for(; it != m_gadgets.end(); ++it)
		items.push_back(SItemTemp(INVENTORY_ITEM_GADGET, (*it)->GetID()));

	if(m_ship != NULL)
	{
		vector<CShipPatch> &patches = m_ship->m_patches;
		vector<CShipPatch>::iterator it3 = patches.begin();
		for(; it3 != patches.end(); ++it3)
			items.push_back(SItemTemp(INVENTORY_ITEM_PATCH, it3->GetID()));
	}

	for(; it2 != m_items.end(); ++it2)
		items.push_back(SItemTemp((*it2)->GetItemType(), (*it2)->GetID()));
}

void CPlayer::AddItems(std::vector<SItemTemp> &items)
{
	std::vector<SItemTemp>::iterator it = items.begin();
	for(; it != items.end(); ++it)
	{
		switch(it->type)
		{
		case INVENTORY_ITEM_COMPONENT:
			AddComponent(it->id, false);
			continue;
		case INVENTORY_ITEM_GADGET:
			AddGadget(it->id, false, true);
			continue;
		case INVENTORY_ITEM_PATCH:
			AddPatch(it->id, false);
			continue;
		case INVENTORY_ITEM_QUESTITEM:
			AddQuestItem(it->id, false);
			continue;
		}
	}
}

void CPlayer::PrevWeapon()
{
	if(m_ship == NULL)
		return;

	IGadget *g = m_ship->m_gadgets[0];
	IGadget *prev = NULL;

	// Find the current gadget and maintain pointer to the previous
	// weapon gadget
	vector<IGadget*>::iterator it = m_gadgets.begin();
	if(g != NULL)
	{
		for(; it != m_gadgets.end(); ++it)
		{
			if(*it == g)
				break;
			if((*it)->IsActivateable())
				prev = *it;
		}
	}

	if(prev != NULL)
		EquipGadget(prev->GetID());
	else
	{
		// Find last weapon gadget
		for(it = m_gadgets.begin(); it != m_gadgets.end(); ++it)
			if((*it)->IsActivateable())
				prev = *it;
		// Equip if found
		if(prev != NULL)
			EquipGadget(prev->GetID());
	}
}

void CPlayer::NextWeapon()
{
	if(m_ship == NULL)
		return;

	IGadget *g = m_ship->m_gadgets[0];

	vector<IGadget*>::iterator it = m_gadgets.begin();

	// Find currently equipped gadget in the list
	if(g != NULL)
	{
		for(; it!=m_gadgets.end(); ++it)
			if(*it == g)
			{
				++it;
				break;
			}
	}

	// And then find the next weapon
	for(; it != m_gadgets.end(); ++it)
		if((*it)->IsActivateable())
		{
			EquipGadget((*it)->GetID());
			return;
		}

	// If one wasn't found, start from the beginning
	for(it = m_gadgets.begin(); it != m_gadgets.end(); ++it)
		if((*it)->IsActivateable())
		{
			EquipGadget((*it)->GetID());
			return;
		}
}