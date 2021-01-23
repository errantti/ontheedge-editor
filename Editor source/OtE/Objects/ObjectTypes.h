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

#ifndef __OBJECTTYPES_H__
#define __OBJECTTYPES_H__

#include "..\\Edgewalker\\DirectX9\\DirectX9.h"

#define ENEMY_OBJECT_ID		100
#define DYNAMIC_OBJECT_ID	101
#define STATIC_OBJECT_ID	102
#define TRIGGER_OBJECT_ID	103
#define SHIP_OBJECT_ID		104
#define AMMO_OBJECT_ID		105

enum DAMAGE_TYPE {
	DAMAGE_TYPE_ENERGY = 0,	// Energy and laser weapons
	DAMAGE_TYPE_PLASMA,		// Plasma weapons
	DAMAGE_TYPE_PROJECTILE,	// Projectile and explosive weapons
	DAMAGE_TYPE_STUN,		// Stun
	DAMAGE_TYPE_KINETIC,	// Not causing actual damage, only applying force in certain area
	DAMAGE_TYPE_NONE = 0xFFFFFFF
};

enum SCRIPT_OVERRIDE_TYPE {
	SCRIPT_OVERRIDE_ONDEATH=0,
	SCRIPT_OVERRIDE_ONAGGRO,
	SCRIPT_OVERRIDE_ONDAMAGE,
	SCRIPT_OVERRIDE_ONCOLLISION,
	SCRIPT_OVERRIDE_CUSTOM,
	SCRIPT_OVERRIDE_NONE = 0xFFFFFFFF
};

/**
 * Defines an explosion in a struct that can be passed
 * to object manager to handle
 */
struct SExplosion
{
	float 
		radius, 
		damage, 
		force;

	void Init()
	{
		radius = damage = force = 0.f;
	}
};


struct SObjectInstanceInfo
{
	/// Object orientation
	D3DXVECTOR3 pos, rot, effectPos;

	/// Object type (defines in which database the template is searched from)
	int type;

	/// A string by which the object can be referenced, unique within level
	char refName[32];

	SCRIPT_OVERRIDE_TYPE scriptOverrideType;

	union {
		/// Used to override one template-specific script with instance specific one
		char scriptOverride[32];

		/// RefName of the target, used for level teleports to define target level name
		char targetRef[32];
	};

	long controllerID;
	float controllerParams[2];

	long 
		templateID, // type of the object
		id,			// an unique (within level) identifier of the object
		targetID,	// routeID for enemies, etc

		particleEffectID, // ID for particle effect that is attached to object (-1 if none)
		lightEffectID // ID of a light that is attached to the object
	;

	bool
		bRequiresConstantUpdate,
		bVisibleInGame
	;

	void FillDefaults()
	{
		effectPos = rot = pos = D3DXVECTOR3(0.f, 0.f, 0.f);
		type = STATIC_OBJECT_ID;
		refName[0] = '\0';
		scriptOverride[0] = '\0';
		scriptOverrideType = SCRIPT_OVERRIDE_NONE;
		controllerID = -1;
		controllerParams[0] = controllerParams[1] = 0.f;
		templateID = -1;
		id = -1;
		targetID = -1;
		particleEffectID = -1;
		lightEffectID = -1;
		bRequiresConstantUpdate = false;
		bVisibleInGame = true;
	}
};


#endif