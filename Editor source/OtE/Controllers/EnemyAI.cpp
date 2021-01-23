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

#include "EnemyAI.h"
#include "..\\OnTheEdge.h"
#include "ControllerManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnemyAI::CEnemyAI()
{
	m_controllerID = CONTROLLER_ID_ENEMY;
}

CEnemyAI::~CEnemyAI()
{

}

void CEnemyAI::Update(IGameObject *object, float frameTime)
{
	if(object->GetType() != ENEMY_OBJECT_ID)
	{
		// This controller has been inserted to wrong type of object
		return;
	}

	CVessel *target = NULL;

	CEnemy *e = (CEnemy*)object;
	SEnemyAI &AI = e->m_AI;

	// Stun affects by making enemy go 'limp' for the duration
	if(e->m_stunCounter > 0.f)
		return;

	// Handle the enemy behavior according to the AI flags
	CPlayer *player = GetOTE()->GetPlayer(0);

	if(e->m_aggroTarget[0] != NULL)
	{
		// If the enemy is already aggroed, target the one currently on top
		// of the hate list
		target = e->m_aggroTarget[0];
	}
	else
	{
		// Otherwise check whether the primary focus target is within 
		// reaction radius
		target = player->GetPrimaryAggroTarget();

		// TODO: check whether secondary aggro target exists and is closer by
	}

	//CShip *pl1 = GetOTE()->GetPlayerShip(0);
	D3DXVECTOR3 targetpos = D3DXVECTOR3(0.f, 0.f, 0.f);
	D3DXVECTOR3 aimAt;
	
	if(target == NULL)
	{
		e->SetEmote("\\^_^/", 8.f, 0xFFFFAB7F);
		return;
	}

	targetpos = target->GetTranslation();
	D3DXVECTOR3 enpos = e->GetTranslation();

	float stealthMod = target->GetStealthCoefficient();
	// Because all the distances are squared, the stealth mod should be as well
	stealthMod *= stealthMod;

	float dx,dy,dz;
	dx = enpos.x-targetpos.x;
	dy = enpos.y-targetpos.y;
	dz = enpos.z-targetpos.z;
	float distSq = dx*dx + dy*dy + dz*dz;

	aimAt = targetpos;
	// Check if the enemy is supposed to try to anticipate target movement and
	// change the aim position correspondingly
	if(AI.customAIFlags & CUSTOM_AI_FLAG_ANTICIPATE_MOVEMENT)
	{
		D3DXVECTOR3 targetSpeed = target->GetSpeed();
		// Don't let vertical speed affect the aiming
		targetSpeed.y = 0.f;

		// Now add the speed to the aim target modified by how far the target is
		// relative to the desired shooting distance
		aimAt += targetSpeed * (e->m_onePerDesiredShootingDistance * sqrtfast(distSq));
	}

	float aggroDistSq = AI.reactionDistance;
	aggroDistSq *= aggroDistSq;

	// Take stealthing into consideration
	aggroDistSq *= stealthMod;

	float shootDistSq = AI.shootingDistance;
	shootDistSq *= shootDistSq;
	float minDistSq = AI.minimumDistance;
	minDistSq *= minDistSq;
	float aggroMaintainDistSq = AI.aggroMaintainDistance;
	aggroMaintainDistSq *= aggroMaintainDistSq;

	aggroMaintainDistSq *= stealthMod;

	bool bMinDist = FALSE;
	bool bAttacks = FALSE;
	bool bThrust = TRUE;

	bool immobileAttacking = FALSE;

	/*
	 * Determine what the AI object is going to do
	 **/

	if(AI.bMobile == FALSE)
	{
		bThrust = false;
		if(distSq < shootDistSq)
		{
			// Start / keep shooting and closing in
			e->m_state = ENEMY_STATE_SHOOT;
			e->ReportThreat(target, 0.f);
			immobileAttacking = TRUE;
			e->m_targetLocation = targetpos;
		}
		else
			e->m_state = ENEMY_STATE_STILL;
	}
	else
	{
		// When player crosses aggro range limit, mark the enemy aggroed
		if(distSq < aggroDistSq)
			e->ReportThreat(target, 0.f);

		if(AI.aggroResponse != ENEMY_AGGRO_RESPONSE_ATTACK)
		{
			if(e->IsAggroed())
			{
				if(AI.aggroResponse == ENEMY_AGGRO_RESPONSE_ESCAPE)
					e->m_state = ENEMY_STATE_ESCAPING;
				else if(AI.aggroResponse == ENEMY_AGGRO_RESPONSE_FOLLOWROUTE)
				{
					if(e->m_state2 != ENEMY_2NDSTATE_ROUTE)
						e->StartFollowingRoute(true, false);
				}

				if(distSq < shootDistSq)
				{
					if(!AI.bWaitsForGoodAngle)
						bAttacks = TRUE;
				}
				else if(distSq > aggroMaintainDistSq)
				{
					if(e->m_bWasFollowingRoute)
						e->m_state2 = ENEMY_2NDSTATE_ROUTE;
					else
						e->m_state = ENEMY_STATE_STILL;
					//e->m_bIsAggroed = false;
					e->RemoveTarget(target);
				}

				if(e->m_aggroTarget[0] != NULL && AI.aggroResponse == ENEMY_AGGRO_RESPONSE_ESCAPE)
				{
					e->m_targetLocation = enpos + enpos - targetpos;
					e->m_bTryingToReachTarget = TRUE;
				}
			}
		}
		else
		{
			if(distSq < minDistSq)
			{
				// If player hasn't shot enemy or hasn't crossed aggro range 
				// (which can be smaller than minimum or attack range and thus
				// this restriction) the enemy is not supposed react to it
				if(e->m_aggroTarget[0] != NULL)
				{
					// Keep shooting
					/*e->m_state = ENEMY_STATE_COMBAT;
					e->m_bTryingToReachTarget = TRUE;
					e->m_targetLocation = targetpos;*/
					InitiateCombat(e, AI, target, true, aimAt);
					bMinDist = true;
					if(e->m_state == ENEMY_STATE_SHOOT)
						bThrust = false;
				}
			}
			else if(distSq < shootDistSq)
			{
				// If player hasn't shot enemy or hasn't crossed aggro range 
				// (which can be smaller than minimum or attack range and thus
				// this restriction) the enemy is not supposed react to it
				if(e->m_aggroTarget[0] != NULL)
				{
					// Start / keep shooting and closing in
					/*e->m_state = ENEMY_STATE_COMBAT;
					e->m_bTryingToReachTarget = TRUE;
					e->m_targetLocation = targetpos;*/
					InitiateCombat(e, AI, target, true, aimAt);
					bThrust = true;
				}
			}
			else if(distSq < aggroDistSq)
			{
				// Try to reach the player
				/*e->m_state = ENEMY_STATE_FOLLOWING;
				e->m_bTryingToReachTarget = TRUE;
				e->m_targetLocation = targetpos;*/
				InitiateCombat(e, AI, target, false, aimAt);
				bThrust = true;
			}
			else if(distSq < aggroMaintainDistSq)
			{
				if(e->m_aggroTarget[0] != NULL)
				{
					InitiateCombat(e, AI, target, false, aimAt);
					bThrust = true;
				}
			}
			else
			{
				//if(e->m_state == ENEMY_STATE_FOLLOWING || e->m_state == ENEMY_STATE_COMBAT ||
				//	e->m_state == ENEMY_STATE_ESCAPING)
				if(e->m_aggroTarget[0] != NULL)
				{
					if(e->m_bWasFollowingRoute)
					{
						if(AI.customAIFlags & CUSTOM_AI_FLAG_CONTINUE_ROUTE)
							e->StartFollowingRoute(false, true);
						else
							e->StartFollowingRoute(false, false);
					}
					else
						e->m_state = ENEMY_STATE_STILL;
					//e->m_bIsAggroed = false;
					e->RemoveTarget(target);
				}
				/*{
					// If enemy is aggroed but isn't too far from target then set it following
					e->m_state = ENEMY_STATE_FOLLOWING;
					e->m_bTryingToReachTarget = TRUE;
					e->m_targetLocation = targetpos;
				}*/
			}
		}

		if(e->m_state2 == ENEMY_2NDSTATE_ROUTE)
		{
			bThrust = true;
			if(e->m_route == NULL)
				e->m_state = ENEMY_STATE_STILL;
			else
			{
				if(e->m_bReachedMajorTarget)
				{
					// Set target location to the next or closest waypoint in the set
					e->StartTowardsNextWaypoint();
					/*if(AI.customAIFlags & CUSTOM_AI_FLAG_CONTINUE_ROUTE)
						e->StartFollowingRoute(false, true);
					else
						e->StartFollowingRoute(false, false);*/
				}
			}
		}
	}

	if(e->m_state == ENEMY_STATE_STILL_AND_SHOOT)
		bThrust = false;

	/**
	 * Update the object movement etc according to the decision made earlier
	 **/

	// If the object has a target location, move it (or just turn if immobile)
	if(e->m_bTryingToReachTarget || bMinDist || immobileAttacking)
	{
		if(e->m_AI.bSuicide && bMinDist)
		{
			e->Kill();
			return;
		}

		D3DXVECTOR3 vEnemyToTarget(e->m_targetLocation-enpos);
		float angle = 0.f;

		bool useThrust = ((!bMinDist && absf(vEnemyToTarget.y) < 20.f) || e->m_AI.bSuicide) && !e->m_state == ENEMY_STATE_STILL;
		float thrustCoefficient = 1.f;
		//if(e->m_state != ENEMY_STATE_SHOOT && e->m_state != ENEMY_STATE_STILL_AND_SHOOT)
		if(!e->IsAggroed())
			thrustCoefficient = 0.5f;

		if(!e->UpdateMovement(frameTime, bThrust, thrustCoefficient, angle))
		{
			// If the movement method didn't update the angle, it has to be done here manually
			float yRot = e->GetRotation().y;

			// Construct enemy direction vector from the y-rotation angle
			// This is basically done in 2d coordinate system
			D3DXVECTOR3 vEnemyDirection(sinf(yRot), 0.f, cosf(yRot));
			D3DXVECTOR3 temp = vEnemyToTarget /sqrtfast(vEnemyToTarget.x*vEnemyToTarget.x + 
				vEnemyToTarget.y*vEnemyToTarget.y + vEnemyToTarget.z*vEnemyToTarget.z);

			angle = acosf(temp.x*vEnemyDirection.x + temp.z * vEnemyDirection.z);
		}

		/**
		 * 
		 **/
		if((e->m_state == ENEMY_STATE_SHOOT || e->m_state == ENEMY_STATE_STILL_AND_SHOOT || bAttacks) && e->m_AI.bShoots)
		{
			// Use the gadget if the enemy is assigned one and is facing about the right direction
			if(AI.bWaitsForGoodAngle)
			{
				if(angle < 0.83f && absf(vEnemyToTarget.y) < 25.f)
				{
					Shoot(e, AI);
				}
			}
			else
				Shoot(e, AI);
		}
	}
}

void CEnemyAI::RegisterCollision(IGameObject *object, IGeometricObject *other)
{
	if(object->GetType() != ENEMY_OBJECT_ID)
	{
		// This controller has been inserted to wrong type of object
		return;
	}

	CEnemy *e = (CEnemy*)object;

	if(other->GetType() == STATIC_GEOMETRIC_OBJECT_ID)
		return;

	if(e->m_AI.bExplodesOnCollision)
	{
		// Kill the enemy object and add an explosion
		return;
	}
}

void CEnemyAI::InitiateCombat(CEnemy *e, SEnemyAI &AI, CVessel *target, bool bInsideShootingDist, D3DXVECTOR3 targetPos)
{
	if(e->m_aggroTarget[0] == NULL)
		e->ReportThreat(target, 0.f);

	CVessel *aggroTarget = e->m_aggroTarget[0];
	CTiledTerrain *terrain = GetOTE()->GetCurrentLevel()->GetTerrainRef();

	D3DXVECTOR3 lastGoodTile;
	bool bPit = false;

	ENEMY_STATE &state = e->m_state;
	e->m_state2 = ENEMY_2NDSTATE_DYNAMIC;

	if(AI.aggroResponse == ENEMY_AGGRO_RESPONSE_ATTACK && AI.bMobile)
	{
		if(AI.bBolted)
		{
			e->m_bTryingToReachTarget = true;
			e->m_targetLocation = targetPos;
			if(bInsideShootingDist)
				state = ENEMY_STATE_STILL_AND_SHOOT;
			else
				state = ENEMY_STATE_STILL;
			return;
		}

		if(AI.customAIFlags & CUSTOM_AI_FLAG_DISABLE_PATHFINDING)
		{
			e->SetEmote("?O_o?", -1000.f, 0xFFAFAFFF);
			e->m_majorTarget = e->m_targetLocation = target->GetTranslation();
			e->m_bTryingToReachTarget = true;
			state = ENEMY_STATE_SHOOT;
			return;
		}

		if(e->m_wayClearCounter <= 0.f)
		{
			if(terrain->IsWayClear(e->GetTranslation(), targetPos, bPit, lastGoodTile))
			{
				e->SetEmote("\\>_</", 6.f, 0xFFAFAFFF);
				state = ENEMY_STATE_SHOOT;
				e->m_majorTarget = e->m_targetLocation = targetPos;
				e->m_bTryingToReachTarget = true;
			}
			else
			{
				e->SetEmote("-.-", 5.f, 0xFFAFAFAF);
				if(state != ENEMY_STATE_FOLLOWING_PATH)
				{
					if(bPit && bInsideShootingDist)
					{
						state = ENEMY_STATE_STILL_AND_SHOOT;
						e->m_majorTarget = e->m_targetLocation = targetPos;
						e->m_bTryingToReachTarget = true;
					}
					else
						state = ENEMY_STATE_STILL;
				}
			}
			e->m_wayClearCounter = 1.5f;
		}

		if(state == ENEMY_STATE_STILL)
		{
			if(e->m_pathSearchCounter <= 0.f && (!bInsideShootingDist || state == ENEMY_STATE_STILL))
			{
				terrain->FindPath(e->GetTranslation(), aggroTarget->GetTranslation(), !bInsideShootingDist, e->m_currentPath);
				if(!e->m_currentPath.empty())
				{
					e->SetEmote("??O.o??", -1000.f, 0xFF6F7FFF);
					state = ENEMY_STATE_FOLLOWING_PATH;
					e->m_majorTarget = target->GetTranslation();
					e->m_targetLocation = e->m_currentPath[e->m_currentPath.size()-1];
					e->m_currentPath.pop_back();
					e->m_bTryingToReachTarget = true;
				}
				e->m_pathSearchCounter = 3.5f;
			}
		}
		else if(state == ENEMY_STATE_SHOOT)
		{
			e->m_bTryingToReachTarget = true;
			e->m_targetLocation = target->GetTranslation();
		}
	}
}

void CEnemyAI::Shoot(CEnemy *e, SEnemyAI &AI)
{
	if(e->m_bGadgetEnabled[0] && e->m_gadgets[0])
		e->m_gadgets[0]->Activate();

	if(e->m_gadgets[1] != NULL)
	{
		if(e->m_bGadgetEnabled[1])
			e->m_gadgets[1]->Activate();
		else
		{
			bool bChanged = false;
			// See if gadget 2 should be enabled
			if(AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_AGGROTIMELIMIT)
			{
				if(e->m_aggroedTime >= AI.gadget2TriggerParams[0])
				{
					bChanged = true;
				}
			}
			else if(AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_HEALTHLIMIT)
			{
				if(e->m_hitPoints <= AI.gadget2TriggerParams[0])
				{
					bChanged = true;
				}
			}
			else if(AI.gadget2Trigger == ENEMY_GADGET2_TRIGGERTYPE_SCRIPT)
			{
				// Run a script to see whether to activate gadget 2
				if(e->m_getGadget2Trigger != NULL && e->m_gadgetCheckScriptCounter <= 0.f)
				{
					CScriptManager::GetManager().Execute(e->m_getGadget2Trigger, SCRIPT_CALLER_OBJECT, ENEMY_OBJECT_ID, e, &bChanged);
					e->m_gadgetCheckScriptCounter = 1.f;
				}
			}

			if(bChanged)
			{
				e->m_bGadgetEnabled[1] = true;
				if(AI.gadget2DisableGadget1)
				{
					e->m_bGadgetEnabled[0] = false;
				}
			}
		}
	}
}