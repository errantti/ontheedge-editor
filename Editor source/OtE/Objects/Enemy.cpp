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

unsigned int CEnemy::m_enemyCount = 0;
std::list<CEnemy*> CEnemy::m_allEnemies;
bool CEnemy::m_bDeathCall = false;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnemy::CEnemy() : m_state(ENEMY_STATE_STILL), m_aggroedTime(0.f),
	m_waypointIndex(0), m_bTryingToReachTarget(FALSE), m_waypointSetID(-1),
	m_onAggro(NULL), m_AIControllerBackup(NULL), m_bCanBePossessed(false), m_bDisplayHPBar(false),
	m_bPossessed(false), m_route(NULL),
	m_timeFromHelpCall(10.f), m_bPotentialTarget(false), m_pathSearchCounter(0.f),
	m_wayClearCounter(0.f), m_gadgetCheckScriptCounter(0.f), m_state2(ENEMY_2NDSTATE_DYNAMIC),
	m_bReachedMajorTarget(false), m_bReducedSpeed(false), m_onePerDesiredShootingDistance(0.007f),
	m_floatingText(NULL)
{
	m_type = ENEMY_OBJECT_ID;
	++m_enemyCount;
	m_bIsEnemy = true;
	m_bVisiblyEnemy = true;

	m_bGadgetEnabled[0] = false;
	m_bGadgetEnabled[1] = false;

	ResetTargets();

	m_allEnemies.push_back(this);
}

CEnemy::~CEnemy()
{
	RemoveGadgets();
	std::list<CEnemy*>::iterator it = m_allEnemies.begin();
	for(; it != m_allEnemies.end(); ++it)
		if((*it) == this)
		{
			m_allEnemies.erase(it);
			break;
		}
	if(m_bPotentialTarget || m_bDeathCall)
		ReportPotentialTargetDestruction(this);

	--m_enemyCount;
}

void CEnemy::ReportPotentialTargetDestruction(CVessel *target)
{
	if(target == NULL)
		return;
	std::list<CEnemy*>::iterator it = m_allEnemies.begin();
	for(; it != m_allEnemies.end(); ++it)
		(*it)->RemoveTarget(target);
}

void CEnemy::RemoveGadgets()
{
	for(int i=0;i<NUM_GADGETS;++i)
		if(m_gadgets[i])
		{
			m_gadgets[i]->UnEquip();
			delete m_gadgets[i];
			m_gadgets[i] = NULL;
		}
	m_bGadgetEnabled[0] = m_bGadgetEnabled[1] = false;
}

void CEnemy::ApplyTemplate(SEnemyTemplate t, bool bInitiateEffects, bool bRemoveOldEffects)
{
	ApplyVesselTemplate(t.vt, bInitiateEffects, bRemoveOldEffects);

	if(CGameSettings::GetSettingsObject()->GetSetting(GO_ANIME_SMILEYS_ENABLED))
		CreateFloatingText();

	m_AI = t.AI;
	if(m_AI.bBolted)
		m_bStatic = TRUE;
	if(!t.AI.bIsEnemy)
	{
		m_bIsEnemy = m_bVisiblyEnemy = false;
	}

	m_bDisplayHPBar = t.bDisplayHealthBar;
	m_bCanBePossessed = t.bCanBePossessed;

	// Only apply if the value is in a 'sane range' (this property was added to where previously
	// was randomized block so the value won't be set for most enemies)
	if(t.AI.desiredShootingDistance > 5.f && t.AI.desiredShootingDistance < 1000.f)
		m_onePerDesiredShootingDistance = 1.f / t.AI.desiredShootingDistance;

	// Force a sensible frequency
	if(m_AI.helpCallFrequency < 1.5f)
		m_AI.helpCallFrequency = 1.5f;

	// Gadgets
	RemoveGadgets();
	for(int i=0;i<NUM_GADGETS;++i)
	{
		m_gadgets[i] = GetOTE()->GetGadgetDatabase()->CreateGadget(t.gadgetID[i]);
		if(m_gadgets[i])
			m_gadgets[i]->Equip(this);
	}

	m_bGadgetEnabled[0] = m_bGadgetEnabled[1] = false;
	m_getGadget2Trigger = CScriptManager::GetManager().GetScript(t.AI.gadget2TriggerScript);
	if(m_gadgets[0])
		m_bGadgetEnabled[0] = true;
	if(m_gadgets[1] && m_AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_ACTIVE)
		m_bGadgetEnabled[1] = true;
}

void CEnemy::ApplyObjectInfo(SObjectInstanceInfo &info, bool bRemoveOldEffects)
{
	m_waypointSetID = info.targetID;
	m_bWasFollowingRoute = true;
	m_route = GetOTE()->GetCurrentLevel()->GetWayPointSet(m_waypointSetID);

	if(m_AI.customAIFlags & CUSTOM_AI_FLAG_DONT_FOLLOW_ROUTE)
	{
		m_bWasFollowingRoute = false;
		m_state = ENEMY_STATE_STILL;
	}
	else
		StartFollowingRoute(true, false);


	// Remove old particle and light effects
	// TODO: actually check that this only removes the particle and light effects
	if(bRemoveOldEffects)
	{
		RemoveAll();

		// Lil fix here. RemoveAll removes the shadow as well so it has to be
		// reset and recreated
		m_shadow = NULL;
		CreateShadow(false);
	}

	// Create new particle and light effects
	CLightEffectManager::GetManager().CreateLightEffect(this, info.lightEffectID, info.effectPos);
	CParticleEffects::GetManager().StartEffect(this, info.particleEffectID, info.effectPos);

	/**
	 * NOTE: Things that must be updated outside of
	 * this method:
	 * position - has to be correctly placed into object manager grid
	 * refname - has to be updated into refname storage
	 * (id - if it will need to be mapped in the future)
	 */
	m_ID = info.id;
	m_relativeRotation = info.rot;
	m_bRequiresConstantUpdate = info.bRequiresConstantUpdate;

	if(info.scriptOverrideType != SCRIPT_OVERRIDE_NONE)
		SetOverrideScript(info.scriptOverrideType, info.scriptOverride);
	else if(strlen(info.targetRef) > 0)
	{
		if(m_refTarget == NULL)
			m_refTarget = new char[32];
		strcpy(m_refTarget, info.targetRef);
	}

	// Add controller 
	IGameObjectController *overrideController = CControllerManager::GetManager().GetController(info.controllerID);

	if(overrideController != NULL)
	{
		if(m_controller != NULL)
			if(m_controller->RequiresInstantiation())
				CControllerManager::GetManager().RemoveInstancedController(m_controller);

		m_controller = overrideController;

		for(int i=0;i<2;++i)
			m_controlParams[i] = info.controllerParams[i];

		if(m_controller != NULL)
			m_controller->InitObject(this);
	}
}

void CEnemy::UpdateObjectInfo()
{
	CDynamicObject::UpdateObjectInfo();
	m_instInfo->targetID = m_waypointSetID;
}

void CEnemy::RegisterCollision(IGeometricObject *object)
{
	if(m_AI.bExplodesOnCollision)
	{
		int type = ((IGameObject*)object)->GetType();
		if(type == DYNAMIC_OBJECT_ID || type == ENEMY_OBJECT_ID || type == SHIP_OBJECT_ID)
		{
			Kill();
			return;
		}
	}

	m_onCollision.Trigger(this);

	if(m_controller)
		m_controller->RegisterCollision(this, object);
}

void CEnemy::Update(float frameTime)
{
	if(m_bDead)	// Don't bother updating dead enemy
		return;
	CVessel::Update(frameTime);
	CEnemy *e = this;

	for(int i=0;i<NUM_GADGETS;++i)
	{
		if(m_gadgets[i])
			m_gadgets[i]->Update(frameTime);
	}

	if(m_bPossessed)
		return;

	/// Update counters and timers
	m_timeFromHelpCall += frameTime;
	if(m_aggroTarget[0] != NULL)
	{
		CallForHelp();
		m_aggroedTime += frameTime;
	}

	if(m_gadgetCheckScriptCounter > 0.f)
		m_gadgetCheckScriptCounter -= frameTime;
	if(m_pathSearchCounter > 0.f)
		m_pathSearchCounter -= frameTime;
	if(m_wayClearCounter > 0.f)
		m_wayClearCounter -= frameTime;

	if(m_state == ENEMY_STATE_FOLLOWING_PATH && !(m_AI.customAIFlags & CUSTOM_AI_FLAG_DISABLE_PATHFINDING))
	{
		m_blockedCounter += frameTime;
		if(m_blockedCounter > 1.f)
		{
			float distSq = m_movedDistance.x*m_movedDistance.x + m_movedDistance.z*m_movedDistance.z + m_movedDistance.y*m_movedDistance.y;
			if(distSq < 50.f)
			{
				// Enemy is blocked so find a new path to target
				FindPath(m_majorTarget, true, false, 200);
			}
			m_blockedCounter = 0.f;
			m_movedDistance = D3DXVECTOR3(0.f, 0.f, 0.f);
		}
		m_movedDistance += m_relativeTranslation - m_lastFrameLocation;
		m_lastFrameLocation = m_relativeTranslation;
	}

	static float distsq = 25.f * 25.f, slowDistSq = 50.f * 50.f;
	bool pathingDone = false;
	m_bReducedSpeed = false;
	if(m_bTryingToReachTarget)
	{
		D3DXVECTOR3 e = m_relativeTranslation - m_targetLocation;
		float enemyToTargetDistSq = e.x*e.x + e.z*e.z;
		if(enemyToTargetDistSq < distsq)
		{
			m_bTryingToReachTarget = false;
			SlowDown();

			if(m_state2 == ENEMY_2NDSTATE_ROUTE)
			{
				if(m_currentPath.size() < 2)
				{
					e = m_relativeTranslation - m_majorTarget;
					if(e.x*e.x + e.z*e.z < distsq)
					{
						m_bReachedMajorTarget = true;
						pathingDone = true;
					}
				}
			}
			if(m_state == ENEMY_STATE_FOLLOWING_PATH && !pathingDone)
			{
				// Continue following the path
				if(!m_currentPath.empty())
				{
					m_bTryingToReachTarget = true;
					m_targetLocation = m_currentPath[m_currentPath.size()-1];
					m_currentPath.pop_back();
				}
				else
				{
					// If the path is done and no more instructions have been given (major target etc)
					m_state = ENEMY_STATE_STILL;
					m_bTryingToReachTarget = false;
				}
			}
		}
		else if(enemyToTargetDistSq < slowDistSq)
		{
			if(m_state == ENEMY_STATE_FOLLOWING_PATH)
				m_bReducedSpeed = true;
		}
	}

	// Update enemy life bar if aggroed and the enemy type is supposed to display life
	if(m_aggroTarget[0] != NULL && m_bDisplayHPBar)
	{
		GetOTE()->SetHPBarHP(m_hitPoints, m_maxHitPoints);
	}
}

bool CEnemy::UpdateMovement(float frameTime, bool bThrust, float thrustCoefficient, float &outAngle)
{
	if(!m_AI.bMobile || m_stunCounter > 0.f)
		return false;

	float yRot = GetRotation().y;

	// Construct enemy direction vector from the y-rotation angle
	// This is basically done in 2d coordinate system
	D3DXVECTOR3 vEnemyToTarget(m_targetLocation - m_relativeTranslation);
	D3DXVECTOR3 vEnemyDirection(sinf(yRot), 0.f, cosf(yRot));
	D3DXVECTOR3 normalizedETT = vEnemyToTarget / sqrtfast(vEnemyToTarget.x*vEnemyToTarget.x + 
		vEnemyToTarget.y*vEnemyToTarget.y + vEnemyToTarget.z*vEnemyToTarget.z);

	float angle = acosf(normalizedETT.x*vEnemyDirection.x + normalizedETT.z * vEnemyDirection.z);
	float absAngle = absf(angle);

	if(m_bSlowingDown)
	{
		if(sqrtfast(m_speed.x*m_speed.x + m_speed.z*m_speed.z) < 10.f || vEnemyDirection.x * vEnemyToTarget.x + vEnemyDirection.z * vEnemyToTarget.z > 0.f)
			m_bSlowingDown = false;
		else
			BackThrust();
	}

	// Turn the object
	if(absAngle > 0.1f)
	{
		if(-vEnemyDirection.z*vEnemyToTarget.x + vEnemyDirection.x*vEnemyToTarget.z < 0.f)
		{
			Turn(frameTime);
		}
		else
		{
			Turn(-frameTime);
		}
	}

	outAngle = angle;
	if(bThrust && !m_AI.bBolted)
	{
		// Accelerate the object if necessary
		// (if the angle between the direction of enemy and target is less than 90 degrees)
		//if(vEnemyDirection.x * vEnemyToTarget.x + vEnemyDirection.z * vEnemyToTarget.z > 0.f)
		if(absAngle < 0.78f)
		{
			if(m_bReducedSpeed)
			{
				float speedSq = m_speed.x * m_speed.x + m_speed.z * m_speed.z;

				if(speedSq > 2200.f)
					BackThrust();
				//else if(speedSq < 200.f)
				//	Thrust(thrustCoefficient);
				else if(thrustCoefficient > 0.3f)
					Thrust(0.3f);
				else
					Thrust(thrustCoefficient);
			}
			else
				Thrust(thrustCoefficient);
		}
	}

	return true;
}

void CEnemy::InflictDamage(float amount, bool bEnemy, DAMAGE_TYPE type, CVessel *source)
{
	if(m_bDead)
		return;

	SetEmote(":<", 3.f, 0xFFFFFFFF);

	CVessel::InflictDamage(amount, bEnemy, type, source);

	if(m_AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_DAMAGERESPONSE)
	{
		if(m_gadgets[1])
		{
			m_gadgets[1]->Activate();
		}
	}

	ReportThreat(source, amount);

	// Make sure that the enemy 'notes' the damage done and starts attacking back
	// (in case the enemy was too far away from player to start following)
	/*if(!m_bIsAggroed && !bEnemy && !m_bPossessed)
	{
		CShip *pl1 = GetOTE()->GetPlayerShip(0);
		if(pl1)
		{
			// Don't aggro over some stray enemy fire, only if player is close enough
			D3DXVECTOR3 plpos = pl1->GetTranslation();
			D3DXVECTOR3 enpos = GetTranslation();

			float x = plpos.x - enpos.x;
			float y = plpos.y - enpos.y;
			float z = plpos.z - enpos.z;

			if(x*x+y*y+z*z < m_AI.aggroMaintainDistance * m_AI.aggroMaintainDistance)
			{
				AggroEnemy();
			}
		}
	}*/
}

void CEnemy::Kill()
{
	CLevel *lvl = GetOTE()->GetCurrentLevel();
	if(lvl)
		lvl->ReportEnemyRemoval(this);

	if(m_controller)
		m_controller->OnDeath(this);

	CDynamicObject::Kill();
}

void CEnemy::Possess(IGameObjectController *controller)
{
	if(controller == NULL)
		return;
	m_AIControllerBackup = m_controller;
	m_controller = controller;
	m_bPossessed = true;
	m_bPotentialTarget = true;
	m_bSlowingDown = false;
}

void CEnemy::UnPossess()
{
	if(m_AIControllerBackup != NULL)
	{
		m_controller = m_AIControllerBackup;
		m_AIControllerBackup = NULL;
	}
	m_bPossessed = false;
}

void CEnemy::SetOverrideScript(SCRIPT_OVERRIDE_TYPE type, char *name)
{
	if(name == NULL)
		return;

	if(type == SCRIPT_OVERRIDE_ONAGGRO)
		m_onAggro = CScriptManager::GetManager().GetScript(name);
	else
		CDynamicObject::SetOverrideScript(type, name);
}

void CEnemy::StartFollowingRoute(bool fromStart, bool continueLast)
{
	if(m_route == NULL)
		return;
	if(m_route->GetWaypointCount() == 0)
		return;

	m_state2 = ENEMY_2NDSTATE_ROUTE;
	m_state = ENEMY_STATE_FOLLOWING_PATH;
	InitPathFollowState();
	if(fromStart)
	{
		m_waypointIndex = 0;
		m_majorTarget = m_route->GetFirstWaypoint();
	}
	else 
	{
		if(continueLast)
			m_majorTarget = m_route->GetCurrentWaypoint(m_waypointIndex);
		else
			m_majorTarget = m_route->GetClosestWaypoint(m_relativeTranslation, &m_waypointIndex);
	}

	if(m_AI.customAIFlags & CUSTOM_AI_FLAG_USE_CUSTOM_ROUTE)
	{
		m_targetLocation = m_majorTarget;
		m_bTryingToReachTarget = true;
		m_bReachedMajorTarget = false;
		m_currentPath.clear();
	}
	else if(FindPath(m_majorTarget, true, true, 200))
	{
		m_bWasFollowingRoute = true;
	}
}

void CEnemy::StopFollowingRoute()
{
	if(m_state2 == ENEMY_2NDSTATE_ROUTE)
	{
		m_state2 = ENEMY_2NDSTATE_DYNAMIC;
		m_state = ENEMY_STATE_STILL;
		m_bTryingToReachTarget = false;
	}
	m_bWasFollowingRoute = false;
}

void CEnemy::StartTowardsNextWaypoint()
{
	m_bReachedMajorTarget = false;
	if(m_route == NULL)
		return;

	m_state = ENEMY_STATE_FOLLOWING_PATH;
	m_state2 = ENEMY_2NDSTATE_ROUTE;
	InitPathFollowState();
	m_majorTarget = m_route->GetNextWaypoint(m_waypointIndex);

	if(m_AI.customAIFlags & CUSTOM_AI_FLAG_USE_CUSTOM_ROUTE)
	{
		m_targetLocation = m_majorTarget;
		m_bTryingToReachTarget = true;
		m_bReachedMajorTarget = false;
		m_currentPath.clear();
	}
	else if(FindPath(m_majorTarget, true, true, 300))
		m_bWasFollowingRoute = true;
	else
		m_state = ENEMY_STATE_STILL;
}

void CEnemy::CallForHelp()
{
	if(!m_AI.bCallsForHelp || m_threat[0] == NULL)
		return;
	if(m_timeFromHelpCall < m_AI.helpCallFrequency)
		return;
	m_timeFromHelpCall = 0.f;
	AggroEnemiesInArea(m_relativeTranslation, m_AI.helpCallRadius, m_aggroTarget[0]);
}

void CEnemy::ReportThreat(CVessel *source, float amount)
{
	if(source == NULL)
		return;
	if(source->GetType() == ENEMY_OBJECT_ID)
	{
		if(((CEnemy*)source)->IsPossessed())
		{
			GetOTE()->GetPlayer(0)->ReportPossessedAggro();
		}
		else
		{
			// Don't aggro on friendly fire
			return;
		}
	}

	if(source == m_aggroTarget[0])
	{
		// If the threat was from primary target then nothing needs be changed
		m_threat[0] += amount;
		return;
	}

	if(source == m_aggroTarget[1])
	{
		m_threat[1] += amount;
		if(m_threat[1] > m_threat[0])
			SwitchAggro();
		return;
	}

	int i;
	// Try to add the new target if there are slots left
	for(i=0; i<2; ++i)
		if(m_aggroTarget[i] == NULL)
		{
			m_aggroTarget[i] = source;
			m_threat[i] = amount;

			if(i == 0)
				OnAggro();
			else if(m_threat[1] > m_threat[0])
				SwitchAggro();

			return;
		}

	// ...and if there weren't, at least check whether the new threat amount was greater
	// than the sum of either old
	if(amount >= m_threat[1])
	{
		m_aggroTarget[1] = source;
		m_threat[1] = amount;
		if(m_threat[1] > m_threat[0])
			SwitchAggro();
	}
}

void CEnemy::RemoveTarget(CVessel *target)
{
	for(int i=0; i<2; ++i)
		if(m_aggroTarget[i] == target)
		{
			m_aggroTarget[i] = NULL;
			if(i == 0)
				SwitchAggro();
			return;
		}
}

void CEnemy::SwitchAggro()
{
	CVessel *t = m_aggroTarget[0];
	m_aggroTarget[0] = m_aggroTarget[1];
	m_aggroTarget[1] = t;
	float t2 = m_threat[0];
	m_threat[0] = m_threat[1];
	m_threat[1] = t2;
}

void CEnemy::ForceAggroTarget(CVessel *target)
{
	if(target == NULL)
		return;

	if(target->GetType() == ENEMY_OBJECT_ID)
		((CEnemy*)target)->m_bPotentialTarget = true;

	if(m_aggroTarget[0] == NULL)
	{
		m_aggroTarget[0] = target;
		m_threat[0] = 1000.f;
		OnAggro();
		return;
	}

	m_aggroTarget[1] = target;
	m_threat[1] = m_threat[0] + 1000.f;
	SwitchAggro();
}

void CEnemy::OnAggro()
{
	m_aggroedTime = 0.f;
	CallForHelp();
	if(m_onAggro != NULL)
		CScriptManager::GetManager().Execute(m_onAggro, SCRIPT_CALLER_OBJECT, ENEMY_OBJECT_ID, this);
}

void CEnemy::ResetTargets()
{
	for(int i=0; i < 2; ++i)
	{
		m_aggroTarget[i] = NULL;
		m_threat[i] = 0.f;
	}
}

void CEnemy::AggroEnemiesInArea(D3DXVECTOR3 pos, float radius, CVessel *target)
{
	if(radius < 10.f)
		return;
	rect<float> area(pos.x - radius, pos.x + radius, pos.z - radius, pos.z + radius);
	vector<IGameObject*> objs;
	GetOTE()->GetObjectManagerSceneNode()->GetObjectsFromSectorsInArea(area, objs);

	vector<IGameObject*>::iterator it = objs.begin();
	for(; it != objs.end(); ++it)
		if((*it)->GetType() == ENEMY_OBJECT_ID)
		{
			((CEnemy*)(*it))->ReportThreat(target, 0.f);
		}
}

void CEnemy::SetWaypointSetID(int ID)
{
	m_waypointSetID = ID;
	m_route = GetOTE()->GetCurrentLevel()->GetWayPointSet(ID);
}

void CEnemy::Thrust()
{
	if(m_bSlowingDown)
		return;
	CVessel::Thrust();
}

void CEnemy::Thrust(float coefficient)
{
	if(m_bSlowingDown)
		return;
	CVessel::Thrust(coefficient);
}

void CEnemy::SlowDown()
{
	m_bSlowingDown = true;
}

void CEnemy::Render(bool isShadowPass)
{
	CVessel::Render(isShadowPass);
	//RenderPath(true);
}

void CEnemy::InitPathFollowState()
{
	m_blockedCounter = 0.f;
	m_movedDistance = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_lastFrameLocation = m_relativeTranslation;
}

bool CEnemy::FindPath(D3DXVECTOR3 target, bool bAcceptIncomplete, bool bSkipTimer, int iterations)
{
	if(m_pathSearchCounter > 0.f && !bSkipTimer)
		return false;
	GetOTE()->GetCurrentLevel()->GetTerrainRef()->FindPath(m_relativeTranslation, target, bAcceptIncomplete, m_currentPath, iterations);
	m_pathSearchCounter = 4.f;
	m_majorTarget = target;
	m_bReachedMajorTarget = false;
	m_bTryingToReachTarget = true;
	if(!m_currentPath.empty())
	{
		m_targetLocation = m_currentPath[m_currentPath.size()-1];
		m_currentPath.pop_back();
		return true;
	}
	return false;
}

void CEnemy::RenderPath(bool renderWithZBuffer)
{
	IDirect3DDevice9 *pd = GetDXApp()->GetDevice();

	if(m_currentPath.size() < 2)
	{
		// Nothing to render
		return;
	}
	
	// Render the waypoints as line leading from each waypoint to 
	// the next waypoint in the set
	//for(int i=0;i<(int)m_currentPath.size();++i)
	//	m_currentPath[i].y = 15.f;

	DWORD fvfBack;
	pd->GetFVF(&fvfBack);
	pd->SetFVF(D3DFVF_XYZ);

	GetDXApp()->EnableLighting(FALSE);
	if(!renderWithZBuffer)
		GetDXApp()->EnableZBuffer(FALSE);
	pd->SetTexture(0, NULL);

	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	pd->SetTransform(D3DTS_WORLD, &m);

	pd->DrawPrimitiveUP(D3DPT_LINESTRIP, (int)m_currentPath.size()-1, 
		m_currentPath[0], sizeof(D3DXVECTOR3));

	pd->SetFVF(fvfBack);
	GetDXApp()->EnableLighting(TRUE);
	if(!renderWithZBuffer)
		GetDXApp()->EnableZBuffer(TRUE);
}

bool CEnemy::IsGadgetEnabled(int index)
{
	if(index < 0 || index > 1)
		return false;
	return m_bGadgetEnabled[index];
}

void CEnemy::SetGadgetEnabled(int index, bool enabled)
{
	if(index < 0 || index > 1)
		return;
	m_bGadgetEnabled[index] = enabled;
}

void CEnemy::CreateFloatingText()
{
	if(m_floatingText != NULL)
		return;

	int r = GetDXApp()->RandInt(0,5);
	std::string defaultEmote;
	if(r == 0)
		defaultEmote = "o.O";
	else if(r == 1)
		defaultEmote = "O.o";
	else if(r == 2)
		defaultEmote = "O_o";
	else if(r == 3)
		defaultEmote = "o_O";
	else if(r == 4)
		defaultEmote = ":|";
	else if(r == 5)
		defaultEmote = "...";
	m_floatingText = new CFloatingText(this, ISceneManager::GetManager(), defaultEmote, 28734874, D3DXVECTOR3(0.f, 35.f, 0.f), D3DXVECTOR3(6.f, 10.f, 15.f));
	m_floatingText->SetColor(0x00AFAFFF);
}

void CEnemy::SetEmote(const std::string text, float time, DWORD color)
{
	if(m_floatingText == NULL)
		return;
	if(text == m_prevEmote)
		return;
	m_prevEmote = text;
	m_floatingText->SetText(text);
	m_floatingText->SetFadeTrigger(time, 1.5f);
	m_floatingText->SetColor(color);
}