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

#ifndef __CDYNAMICOBJECT_H__
#define __CDYNAMICOBJECT_H__

#include "..\\Edgewalker\\IGameObject.h"
#include "..\\ActionEvent.h"
#include "ObjectTypes.h"
#include "ISimpleObjectControlInterface.h"

/**
 * Includes the information of a single dynamic object
 * template (blueprint for a dynamic object)
 */
struct SDynamicObjectTemplate
{
	bool 
		bIndestructible, 
		bAffectedByGravity,
		bCollidesWithExternalObjects, 
		bAffectedByDrag, 
		bRequiresCollisionDetermination, 
		bCollidesWithObjects,
		bDropWhenLoaded
	;

	DWORD hitPoints;

	char meshFilename[256];	// Mesh for the object
	char name[64];			// Name of the object type (for editor, not supposed to be shown in-game)
	char subCategory[32];	// The custom subcategory into which the object belongs

	float 
		bvCoefficient,		// bounding volume coefficient, bounding sphere radius / box height is 
							// multiplied by this number.
		mass,				// mass of the object
		dragCoefficient;	// determines how much drag affects this object
	
	long 
		templateID,
		defaultParticleEffectID;

	SActionEvent
		onDestruction,
		onDamageTaken,
		onCollision
	;

	D3DXVECTOR3 defaultEffectPos;

	OBJECT_COLLISION_CHECK collisionType;
	float collisionParam1;

	void FillDefaults()
	{
		bIndestructible = FALSE;
		bAffectedByGravity = TRUE;
		bAffectedByDrag = TRUE;
		bCollidesWithExternalObjects = TRUE;
		bCollidesWithObjects = TRUE;
		bRequiresCollisionDetermination = TRUE;
		bDropWhenLoaded = TRUE;
		hitPoints = 100;
		meshFilename[0] = '\0';
		name[0] = '\0';
		subCategory[0] = '\0';
		bvCoefficient = DEFAULT_OBJECT_BOUNDING_VOLUME_COEFFICIENT;
		mass = 100.f;
		templateID = -1;
		collisionParam1 = 0;
		collisionType = OBJECT_COLLISION_SPHERE;
		dragCoefficient = 20.f;
		defaultParticleEffectID = -1;
		defaultEffectPos = D3DXVECTOR3(0.f, 0.f, 0.f);

		onDestruction.FillDefaults();
		onDamageTaken.FillDefaults();
		onCollision.FillDefaults();
	}
};

class CVessel;

/**
 * CDynamicObject handles a single dynamic object (which
 * can be enemy, player, boulder in the level etc...)
 */
class CDynamicObject : public IGameObject, public ISimpleObjectControlInterface
{
public:
	CDynamicObject(ISceneNode *parent, long id);
	CDynamicObject(ISceneNode *parent, long id, SDynamicObjectTemplate temp);
	virtual ~CDynamicObject();

	virtual void ApplyObjectInfo(SObjectInstanceInfo &info, bool bRemoveOldEffects);
	virtual SObjectInstanceInfo *GetObjectInfo(bool autoUpdate = false);
	virtual void UpdateObjectInfo();
	virtual SObjectInstanceInfo ConstructObjectInfo();

	virtual void InflictDamage(float amount, bool bEnemy, DAMAGE_TYPE type = DAMAGE_TYPE_ENERGY, CVessel *source = NULL);
	virtual void Repair(float amount);

	inline void SetIndestructible(bool value) {m_bIndestructible = value;}
	inline bool IsIndestructible() {return m_bIndestructible;}

	void ApplyDynamicObjectTemplate(SDynamicObjectTemplate t);

	virtual void Kill();

	virtual void OnDeath();

	bool IsDead() {return m_bDead;}

	inline void SetTeamID(long id) {m_teamID = id;}
	inline long GetTeamID() {return m_teamID;}

	inline float GetHitpoints() {return m_hitPoints;}
	inline float GetMaxHitPoints() {return m_maxHitPoints;}

	virtual float &GetInputControlParam(int index) {return m_controlParams[index];}
	virtual void SetInputControlParam(int index, float value);

protected:
	virtual void SetOverrideScript(SCRIPT_OVERRIDE_TYPE type, char *name);

	float m_hitPoints, m_maxHitPoints;
	bool m_bIndestructible;
	bool m_bDead;		// True if the object is dead or about to die but hasn't been deleted yet
						// Meant to prevent an recursive bug with death explosions

	long m_templateID, m_teamID;

	SObjectInstanceInfo *m_instInfo;
	char *m_refTarget;

	float m_controlParams[INPUT_CONTROL_PARAMS];

	CActionEvent m_onDestruction, m_onDamageTaken, m_onCollision;
};

#endif // #ifndef __CDYNAMICOBJECT_H__
