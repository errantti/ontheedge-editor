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

#ifndef __CAMMO_H__
#define __CAMMO_H__

#include "..\\Edgewalker\\IGameObject.h"
#include "ObjectTypes.h"
#include "..\\ParticleEffects.h"
#include "..\\Sound\\Sound.h"

/**
 * Stores the essential information about a single
 * ammo type (kind of blueprint for an ammo). The
 * type of the ammo has nothing to do with damage,
 * explosion etc, the template only defines the
 * general behavior of the ammo while in the air
 * and the visual appearence
 */
struct SAmmoTemplate
{
	char name[64];
	char meshFilename[128];
	long particleEffectID, ID, controllerID;
	float mass;
	DAMAGE_TYPE damageType;

	bool 
		//bHasParticleEffect, 
		bAffectedByGravity,
		bCollidesWithStaticObjects;

	SParticleSystemInfo *pEffect;
	CMesh *mesh;
	IGameObjectController *controller;

	SAmmoTemplate() : mesh(0), pEffect(0), controller(0)
	{
	}

	void FillDefaults()
	{
		name[0] = '\0';
		meshFilename[0] = '\0';
		particleEffectID = -1;
		ID = -1;
		controllerID = -1;
		mass = 2.f;
		bAffectedByGravity = FALSE;
		damageType = DAMAGE_TYPE_ENERGY;
		bCollidesWithStaticObjects = true;
	}
};

class CVessel;
class CScript;
class CDynamicObject;

/**
 * Contains the instance specific properties of
 * an ammo like damage, explosion. hitEffect and
 * hitSound should be preinitialized pointers to
 * respective resource objects so using them
 * will be quick
 */
struct SAmmoInstanceData
{
	SExplosion exp;

	float damage, lifetime;
	long sourceID;

	bool bEnemy, bExplosive, bExplodeOnTimerEnd;

	SParticleSystemInfo *hitEffect;
	CSound *hitSound;

	// The vessel that caused the damage, if the cause was not vessel then this value is NULL
	CVessel *source;
	CScript *onHitScript;
};

/**
 * A single ammo object (not necessarily a projectile)
 */
class CAmmo : public IGameObject
{
public:
	CAmmo(long id);
	virtual ~CAmmo();

	void ApplyTemplate(SAmmoTemplate &t, bool bResourcesPreloaded = true);
	void ApplyInstanceData(SAmmoInstanceData &data);

	void SetDamage(float damage) {m_damage = damage;}
	void SetDamageType(DAMAGE_TYPE type) {m_damageType = type;}
	void SetExplosion(SExplosion exp) {m_explosion = exp; m_bExplosive = true;}

	virtual void RegisterCollision(IGeometricObject *object);

	virtual void Update(float frameTime);

	DAMAGE_TYPE GetDamageType() {return m_damageType;}

	inline void SetKillTimer(float time) {m_killTimer = time;}
	inline void SetSourceID(long id) {m_sourceID = id;}

	CVessel *GetSource() {return m_source;}

	void Kill();
	void Explode();

	void RemoveSpecifiedSource(CVessel *source);

	static void ReportSourceRemoval(CVessel *source);

private:
	void AddAmmoFeedback(CDynamicObject *ob);

	float m_killTimer, m_damage;
	long m_sourceID;

	bool m_bEnemy, m_bExplosive, m_bExplodeOnTimerEnd, m_bCollidesWithStaticObjects;
	SExplosion m_explosion;

	DAMAGE_TYPE m_damageType;

	CVessel *m_source;

	SParticleSystemInfo *m_hitEffect;
	CSound *m_hitSound;
	CScript *m_hitScript;

	static std::list<CAmmo*> m_allAmmos;
};

#endif // #ifndef __CAMMO_H__