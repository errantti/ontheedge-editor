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

#ifndef __CWEAPONGADGET_H__
#define __CWEAPONGADGET_H__

#include "IGadget.h"
#include "..\\Objects\\Ammo.h"

enum WEAPON_MODE {
	WEAPON_MODE_FORWARD=0,				// The default mode, weapon shoots all ammos forward at once
	WEAPON_MODE_NOVA,					// The weapon shoots all ammos at once in all directions
	WEAPON_MODE_FORWARD_ALTERNATING,	// The weapon shoots a single ammo at a time forward alternating 
										// the beginning position determined by numBullets and angle
										// (numBullets = 3 <-> three different starting spots - right in front
										// of the ship, a little left of that and a little right (determined by
										// angle))
	WEAPON_MODE_NOVA_ALTERNATING,		// The nova equivalent of WEAPON_MODE_FORWARD_ALTERNATING
	WEAPON_MODE_BACKWARD,
	WEAPON_MODE_THROWER,
	WEAPON_MODE_RAY,
	WEAPON_MODE_COUNT,
	WEAPON_MODE_FORCEDWORD=0xFFFFFFFF
};

/**
 *
 */
struct SWeaponGadgetTemplate : public SCustomGadgetTemplate
{
	/// Ammo weapons = speed, thrower = radius, ray = max length
	float force;
	float energyConsumption, firingPace;
	/**
	 * Ammo weapons = if more than one bullets, the angle on the radius of the
	 * source object. Thrower = angle of the cone of damage
	 */
	float angle;
	float lifetime;

	/**
	 * Ammo weapons: ammo template id
	 * Thrower weapons: particle effect id
	 * Ray weapons: ray type id
	 */
	int ammoTemplateID;

	/**
	 * If weapon mode is non-alternating ammo then this is the number of bullets
	 * launched when the weapon is activated. If it's alternating then this is
	 * the number of bullets that would be shot if the weapon wasn't alternating
	 */
	int numBullets;

	WEAPON_MODE mode;
	SExplosion explosion;
	bool bExplosive, bExplodeOnTimerEnd;
	float damage;

	long hitSound, hitEffect;
	char hitScript[32];

	void FillDefaults()
	{
		force = 150.f;
		energyConsumption = 10.f;
		firingPace = 0.5f;
		angle = 0.f;
		lifetime = 1.5f;
		mode = WEAPON_MODE_FORWARD;
		explosion.damage = 0.f;
		explosion.force = 0.f;
		explosion.radius = 0.f;
		bExplosive = false;
		bExplodeOnTimerEnd = false;
		numBullets = 1;
		damage = 40.f;
		ammoTemplateID = -1;

		hitSound = hitEffect = -1;
		hitScript[0] = '\0';
	}
};

/**
 * Gadget type that shoots damaging or
 * stunning objects
 */
class CWeaponGadget : public IGadget
{
public:
	CWeaponGadget(SWeaponGadgetTemplate &t, SGadgetTemplate &gt);
	~CWeaponGadget();

	void ApplyTemplate(SWeaponGadgetTemplate &t, SGadgetTemplate &gt);

	bool Equip(CVessel *vessel);
	//void UnEquip();
	void Update(float frameTime);

	bool Activate();

	bool IsOneUseGadget() {return m_bOneUse;}

protected:
	void SetDescription();

	float m_nextAmmo, m_numBullets;
	float m_ammoAngleUnit;
	float m_activationTimer;

	bool m_bOneUse;

	SWeaponGadgetTemplate m_template;

	SAmmoTemplate *m_ammoTemplate;
	SAmmoInstanceData m_ammoInstData;

	void LaunchNovaWeapon(bool bAlternating);
	void LaunchForwardWeapon(bool bAlternating);
};

#endif // #ifndef __CWEAPONGADGET_H__