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

#ifndef __CSTATICOBJECT_H__
#define __CSTATICOBJECT_H__

#include "..\\Edgewalker\\IGameObject.h"
#include "..\\ActionEvent.h"
#include "ObjectTypes.h"
#include "ISimpleObjectControlInterface.h"


enum STATIC_OBJECT_TYPE {
	/// Static terrain objects
	STATIC_OBJECT_DEBRIS,

	/// Energy pod, restores certain amount of energy per second when touched by player
	STATIC_OBJECT_ENERGY_POD,

	/**
	 * Repair pod, restores certain amount of health per second when touched by player
	 * Limited number of uses (or the total time used)
	 */
	STATIC_OBJECT_REPAIR_POD,

	/// A gadget that player can pick up (targetID = gadget type ID, from template)
	STATIC_OBJECT_GADGET,

	/// Teleports the player to another level
	/**
	 * Teleports the player to another level. The level and the start position
	 * in the level are determined by the refname of the object instance
	 */
	STATIC_OBJECT_LEVEL_TELEPORT,

	/// Teleports the player to another location in the current level
	STATIC_OBJECT_NORMAL_TELEPORT,

	/**
	 * Invisible object that can be used as a pointer, holder for visible particle
	 * effects, spawn location or pretty much whatever you can imagine
	 * NOTE! Make sure not to load ghost object's mesh and make it invisible in-game
	 */
	STATIC_OBJECT_GHOST_OBJECT,

	/// A starting position for player
	STATIC_OBJECT_PLAYER_STARTPOS,

	/// An upgrade to the ship (targetID = patch type id, from template)
	STATIC_OBJECT_PATCH,

	/// A gadget upgrade component (targetID = component type id, from template)
	STATIC_OBJECT_COMPONENT,

	/// A quest item (targetID = quest item id, from template)
	STATIC_OBJECT_QUESTITEM,

	/// Static object type count and forcing the enum to be a DWORD
	STATIC_OBJECT_FORCEDWORD=0xFFFFFFFF
};

struct SStaticObjectTemplate
{
	long 
		templateID, // unique template identifier
		targetID,	// used for determining the type of item if this is gadget, patch, component or quest item
		defaultParticleEffectID;

	char meshFilename[256];		// Mesh for the object
	char name[64];				// Name of the object type (for editor, not supposed to be shown in-game)
	char subCategory[32];		// The custom subcategory into which the object belongs

	float bvCoefficient;		// Bounding volume coefficient, see dynamic object template for info

	bool 
		bCollidesWithObjects,
		bDropWhenLoaded;		// If true, when creating the object manager disregards 
								// the loaded y-coordinate and sets the object on the ground

	STATIC_OBJECT_TYPE type;	// Type of the static object

	float alpha;				// Making it possible to make partly visible static objects

	SActionEvent customAction;	// Custom action, to be defined later

	D3DXVECTOR3 defaultEffectPos;

	OBJECT_COLLISION_CHECK collisionType;
	float collisionParam1;

	void FillDefaults()
	{
		meshFilename[0] = '\0';
		name[0] = '\0';
		templateID = -1;
		targetID = -1;
		bvCoefficient = DEFAULT_OBJECT_BOUNDING_VOLUME_COEFFICIENT;
		bCollidesWithObjects = TRUE;
		bDropWhenLoaded = TRUE;
		type = STATIC_OBJECT_DEBRIS;
		collisionParam1 = 0;
		collisionType = OBJECT_COLLISION_SPHERE;
		defaultParticleEffectID = -1;
		alpha = 1.f;
		defaultEffectPos = D3DXVECTOR3(0.f, 0.f, 0.f);

		customAction.FillDefaults();
	}
};

class CVessel;

/**
 * CStaticObject handles a single static object (doesn't move,
 * cannot be destroyed, isn't affected by physics etc...)
 * Static objects can be deleted from the scene and added dynamically
 **/
class CStaticObject : public IGameObject, public ISimpleObjectControlInterface
{
public:
	CStaticObject();
	virtual ~CStaticObject();

	virtual void OnPreRender(bool isShadowPass);
	virtual void RegisterCollision(IGeometricObject *object);

	void ApplyStaticObjectTemplate(SStaticObjectTemplate t);

	void ApplyObjectInfo(SObjectInstanceInfo &info, bool bRemoveOldEffects);
	virtual SObjectInstanceInfo *GetObjectInfo(bool autoUpdate = false);
	void UpdateObjectInfo();
	SObjectInstanceInfo ConstructObjectInfo();

	inline long GetTargetID() {return m_targetID;}
	inline void SetTargetID(long value) {m_targetID = value;}
	inline STATIC_OBJECT_TYPE GetStaticObjectType() {return m_soType;}

	virtual void Update(float frameTime);

	char *GetTargetRef() {return m_refTarget;}
	void SetTargetRef(const char *str);

	virtual float &GetInputControlParam(int index) {return m_controlParams[index];}
	virtual void SetInputControlParam(int index, float value);

	void Destroy();

	void ReportActivation(IGameObject *activator);

	bool RestoreEnergy(CVessel *target, float frameTime);
	bool RestoreHealth(CVessel *target, float frameTime);

protected:
	STATIC_OBJECT_TYPE m_soType;
	long m_templateID, m_targetID;

	SObjectInstanceInfo *m_instInfo;
	char *m_refTarget;

	float m_controlParams[INPUT_CONTROL_PARAMS];
	float m_restorationCapacity, m_depleatedTimer;

	float m_alpha;

	CActionEvent m_customAction;

	IParticleSystemSceneNode *m_pEffect;
};

#endif // #ifndef __CSTATICOBJECT_H__
