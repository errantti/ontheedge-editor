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

#ifndef __CENEMY_H__
#define __CENEMY_H__

#include "Vessel.h"
#include "..\\Gadgets\\GadgetDatabase.h"
#include "..\\WayPointSet.h"
#include "FloatingText.h"


enum ENEMY_STATE {
	ENEMY_STATE_STILL=0,
	ENEMY_STATE_FOLLOWING_PATH,
	ENEMY_STATE_STILL_AND_SHOOT,
	ENEMY_STATE_SHOOT,
	ENEMY_STATE_ESCAPING
};

enum ENEMY_2NDSTATE {
	ENEMY_2NDSTATE_DYNAMIC=0,
	ENEMY_2NDSTATE_ROUTE
};

enum ENEMY_GADGET2_TRIGGERTYPE {
	ENEMY_GADGET2_TRIGGERTYPE_ACTIVE=0,
	ENEMY_GADGET2_TRIGGERTYPE_HEALTHLIMIT,
	ENEMY_GADGET2_TRIGGERTYPE_DAMAGERESPONSE,
	ENEMY_GADGET2_TRIGGERTYPE_AGGROTIMELIMIT,
	ENEMY_GADGET2_TRIGGERTYPE_SCRIPT,
	ENEMY_GADGET2_TRIGGERTYPE_FORCEDWORD=0xFFFFFFFF
};

enum ENEMY_AGGRO_RESPONSE {
	ENEMY_AGGRO_RESPONSE_ATTACK=0,
	ENEMY_AGGRO_RESPONSE_ESCAPE,
	ENEMY_AGGRO_RESPONSE_FOLLOWROUTE,
	ENEMY_AGGRO_RESPONSE_FORCEDWORD = 0xFFFFFFFF
};

#define CUSTOM_AI_FLAG_DONT_FOLLOW_ROUTE	1
#define CUSTOM_AI_FLAG_CONTINUE_ROUTE		2
#define CUSTOM_AI_FLAG_DISABLE_PATHFINDING	4
#define CUSTOM_AI_FLAG_USE_CUSTOM_ROUTE		8
#define CUSTOM_AI_FLAG_ANTICIPATE_MOVEMENT	16

#define ENEMY_GADGET2_TRIGGER_PARAMS	2

#define NUM_AGGRO_TARGETS		2

struct SEnemyAI
{
	bool 
		bMobile, // moves at least in some way (for example rotates)
		bSuicide, // explodes when gets close enough to the target, not on random collision
		bShoots, // activates given gadget automatically at shoot distance
		bExplodesOnCollision, // explodes when collides with any dynamic object
		bBolted, // isn't affected by physics and doesn't move (can still rotate)
		bCallsForHelp, // if true, aggroes all enemies within certain radius when aggroed
		bWaitsForGoodAngle, // when shooting, only shoots when target is in front of the enemy
		bIsEnemy // If true, the enemy is actual enemy, if false, it targets player's enemies
	;

	ENEMY_AGGRO_RESPONSE aggroResponse;

	DWORD customAIFlags;

	float 
		minimumDistance, 
		shootingDistance, 
		reactionDistance,
		aggroMaintainDistance,
		helpCallRadius,
		helpCallFrequency
	;

	// Second gadget
	ENEMY_GADGET2_TRIGGERTYPE gadget2Trigger;
	float gadget2TriggerParams[ENEMY_GADGET2_TRIGGER_PARAMS];
	//float gadget2TriggerParam1;
	char gadget2TriggerScript[32];
	bool gadget2DisableGadget1;
	float desiredShootingDistance;

	char unused[12];	// Space reserved for future use to provide compatibility with additions

	void FillDefaults()
	{
		bMobile = TRUE;
		bSuicide = FALSE;
		bShoots = TRUE;
		bExplodesOnCollision = FALSE;
		bBolted = FALSE;
		bCallsForHelp = FALSE;
		bWaitsForGoodAngle = TRUE;
		bIsEnemy = TRUE;

		aggroResponse = ENEMY_AGGRO_RESPONSE_ATTACK;
		customAIFlags = 0;

		minimumDistance = 50.f;
		shootingDistance = 140.f;
		reactionDistance = 200.f;
		aggroMaintainDistance = 400.f;
		helpCallRadius = 60.f;
		helpCallFrequency = 2.f;

		gadget2Trigger = ENEMY_GADGET2_TRIGGERTYPE_HEALTHLIMIT;
		gadget2TriggerParams[0] = gadget2TriggerParams[1] = 0.f;
		gadget2TriggerScript[0] = '\0';
		gadget2DisableGadget1 = false;

		for(int i=0;i<16;++i)
			unused[i] = 0;
	}
};

struct SEnemyTemplate
{
	SEnemyAI AI;
	SVesselTemplate vt;
	bool 
		bDisplayHealthBar,
		bCanBePossessed;
	long gadgetID[2];

	char onAggroScript[32];

	DAMAGE_TYPE alteredDamageType;
	float alteredDamageTypeCoefficient;

	float unused[16];	// Space reserved for future use to provide compatibility with additions

	void FillDefaults()
	{
		AI.FillDefaults();
		vt.FillDefaults();

		bDisplayHealthBar = false;
		bCanBePossessed = false;

		for(int i=0;i<NUM_GADGETS;++i)
			gadgetID[i] = -1;

		onAggroScript[0] = '\0';
		alteredDamageType = DAMAGE_TYPE_NONE;
		alteredDamageTypeCoefficient = 1.f;

		for(int i=0;i<16;++i)
			unused[i] = 0.f;
	}
};

/**
 * CEnemy handles a single enemy object
 */
class CEnemy : public CVessel
{
	friend class CEnemyAI;
public:
	CEnemy();
	virtual ~CEnemy();

	inline float GetTimeFromInitiatedAction() {return m_aggroedTime;}
	inline void ResetTimeFromAction() {m_aggroedTime=0.f;}
	inline void UpdateTimeFromAction(float t) {m_aggroedTime += t;}

	virtual void Update(float frameTime);
	virtual void Render(bool isShadowPass);

	virtual void InflictDamage(float amount, bool bEnemy, DAMAGE_TYPE type = DAMAGE_TYPE_ENERGY, CVessel *source = NULL);

	virtual void RegisterCollision(IGeometricObject *object);

	void ApplyTemplate(SEnemyTemplate t, bool bInitiateEffects = true, bool bRemoveOldEffects = false);

	virtual void ApplyObjectInfo(SObjectInstanceInfo &info, bool bRemoveOldEffects);
	virtual void UpdateObjectInfo();
	//virtual SObjectInstanceInfo ConstructObjectInfo();

	void SetWaypointSetID(int ID);
	int GetWaypointSetID() {return m_waypointSetID;}

	SEnemyAI &GetAI() {return m_AI;}

	virtual void Kill();

	static unsigned int GetEnemyCount() {return m_enemyCount;}

	inline bool IsMobile() {return m_AI.bMobile;}
	inline bool IsBoltedToGround() {return m_AI.bBolted;}

	bool IsPossessed() {return m_bPossessed;}
	bool IsAggroed() {return ((m_aggroTarget[0] != NULL)?true:false);}

	void Possess(IGameObjectController *controller);
	void UnPossess();

	bool GetCanBePossessed() {return m_bCanBePossessed;}

	void RemoveGadgets();

	void StartFollowingRoute(bool fromStart = false, bool continueLast = true);
	void StartTowardsNextWaypoint();
	void StopFollowingRoute();

	void CallForHelp();

	void ReportThreat(CVessel *source, float amount);

	void ForceAggroTarget(CVessel *target);

	CVessel *GetAggroTarget(int index) {return m_aggroTarget[index];}
	void ResetAggroTargets();

	CVessel *GetPrimaryAggroTarget() {return m_aggroTarget[0];}
	CVessel *GetSecondaryAggroTarget() {return m_aggroTarget[1];}

	float GetThreat(int targetIndex) {return m_threat[targetIndex];}

	void RemoveTarget(CVessel *target);
	void ResetTargets();

	virtual void Thrust();
	virtual void Thrust(float coefficient);

	bool IsGadgetEnabled(int index);
	void SetGadgetEnabled(int index, bool enabled);

	void RenderPath(bool renderWithZBuffer);

	void SetEmote(const std::string text, float time = 7.f, DWORD color = 0xFFFFFFFF);

	/**
	 * Slows the enemy movement down until the speed is under a certain limit
	 * After this limit is reached everything continues normally. During the
	 * slowdown the enemy cannot thrust normally
	 */
	void SlowDown();

	bool UpdateMovement(float frameTime, bool bThrust, float thrustCoefficient, float &outAngle);

	static void ReportPotentialTargetDestruction(CVessel *target);

	// Set true when an enemy has added another enemy to aggro list
	// This causes enemies to emit a 'death call' to clear other enemies'
	// aggro lists for that enemy. This flag stays true until the end
	// of level when set and is set to false when level starts
	static void EnableDeathCall(bool bValue) {m_bDeathCall = bValue;}

private:
	void AggroEnemiesInArea(D3DXVECTOR3 pos, float radius, CVessel *target);
	void SwitchAggro();
	void OnAggro();
	void CreateFloatingText();

	virtual void SetOverrideScript(SCRIPT_OVERRIDE_TYPE type, char *name);
	bool FindPath(D3DXVECTOR3 target, bool bAcceptIncomplete, bool bSkipTimer = false, int iterations = 50);

	void InitPathFollowState();

	SEnemyAI m_AI;
	ENEMY_STATE m_state;
	ENEMY_2NDSTATE m_state2;

	int m_waypointIndex, m_waypointSetID;
	D3DXVECTOR3 m_targetLocation, m_majorTarget;
	bool m_bTryingToReachTarget, m_bReachedMajorTarget;
	//bool m_bIsAggroed;		// If true this enemy is pursuing / attacking a target
	bool m_bDisplayHPBar;

	CWayPointSet *m_route;

	CVessel *m_aggroTarget[NUM_AGGRO_TARGETS];
	float m_threat[NUM_AGGRO_TARGETS];

	CScript *m_onAggro, *m_getGadget2Trigger;

	std::vector<D3DXVECTOR3> m_currentPath;

	/**
	 * Pointer to AI controller object is stored here while
	 * this enemy is possessed by player
	 */
	IGameObjectController *m_AIControllerBackup;

	long m_enemyTypeID;

	// Timers and counters
	float m_aggroedTime, m_timeFromHelpCall;
	float m_gadgetCheckScriptCounter;
	float m_wayClearCounter;
	float m_pathSearchCounter;
	float m_blockedCounter;

	float m_onePerDesiredShootingDistance;

	D3DXVECTOR3 m_lastFrameLocation, m_movedDistance;

	bool m_bPossessed, m_bCanBePossessed, m_bWasFollowingRoute;

	bool m_bSlowingDown, m_bReducedSpeed;

	CFloatingText *m_floatingText;
	std::string m_prevEmote;

	/**
	 * True if this enemy has been possessed or it is / has been player's ally
	 * (that is, if it may have been the aggro target of another enemy)
	 */
	bool m_bPotentialTarget;
	bool m_bGadgetEnabled[2];

	static unsigned int m_enemyCount;
	static bool m_bDeathCall;

	static std::list<CEnemy*> m_allEnemies;
};

#endif // #ifndef __CENEMY_H__
