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
#include "Ammo.h"

std::list<CAmmo*> CAmmo::m_allAmmos;

CAmmo::CAmmo(long id) : IGameObject(NULL, id), m_killTimer(2.f), m_hitEffect(NULL), m_hitSound(NULL),
m_source(NULL), m_hitScript(NULL)
{
	m_type = AMMO_OBJECT_ID;
	m_bRequiresCollisionDetermination = false;
	m_bCollidesWithObjects = true;
	m_bCollidesWithStaticObjects = true;
	m_bDrag = false;
	m_bStatic = false;

	m_allAmmos.push_back(this);
}

CAmmo::~CAmmo()
{
	std::list<CAmmo*>::iterator it = m_allAmmos.begin();
	for(; it != m_allAmmos.end(); ++it)
		if((*it) == this)
		{
			m_allAmmos.erase(it);
			break;
		}
}

void CAmmo::ReportSourceRemoval(CVessel *source)
{
	if(source == NULL)
		return;
	std::list<CAmmo*>::iterator it = m_allAmmos.begin();
	for(; it != m_allAmmos.end(); ++it)
		(*it)->RemoveSpecifiedSource(source);
}

void CAmmo::RemoveSpecifiedSource(CVessel *source)
{
	if(source == m_source)
		m_source = NULL;
}

void CAmmo::ApplyTemplate(SAmmoTemplate &t, bool bResourcesPreloaded)
{
	m_bGravity = t.bAffectedByGravity;
	m_bDrag = t.bAffectedByGravity;
	m_bCollidesWithExternalObjects = t.bCollidesWithStaticObjects;
	m_bCollidesWithStaticObjects = t.bCollidesWithStaticObjects;

	// TODO: attach controller

	m_mass = t.mass;
	m_damageType = t.damageType;
	
	if(!bResourcesPreloaded)
	{
		// Load mesh and particle effect
		// TODO:
	}
}

void CAmmo::ApplyInstanceData(SAmmoInstanceData &data)
{
	m_damage = data.damage;
	m_explosion = data.exp;

	m_hitEffect = data.hitEffect;
	m_hitSound = data.hitSound;
	m_hitScript = data.onHitScript;

	m_killTimer = data.lifetime;

	m_bEnemy = data.bEnemy;
	m_bExplodeOnTimerEnd = data.bExplodeOnTimerEnd;
	m_bExplosive = data.bExplosive;

	m_sourceID = data.sourceID;
	m_source = data.source;
}

void CAmmo::RegisterCollision(IGeometricObject *object)
{
	// Handle damage from the collision
	int type = object->GetType();
	if(type == DYNAMIC_OBJECT_ID || type == ENEMY_OBJECT_ID || type == SHIP_OBJECT_ID)
	{
		CDynamicObject *o = (CDynamicObject*)object;

		if(o->GetID() != m_sourceID)
		{
			// Kill the object
			if(m_bExplosive)
				Explode();
			else
			{
				// Kinetic ammos should always be explosive
				if(m_damageType != DAMAGE_TYPE_KINETIC)
				{
					// Inflict the damage to the target unless it belongs to the same team
					//if(o->GetTeamID() != m_teamID)
					o->InflictDamage(m_damage, m_bEnemy, m_damageType, m_source);

					if(fabs(m_explosion.radius) > 0.01f)
						AddAmmoFeedback(o);
					
					if(m_hitEffect != NULL)
						GetOTE()->GetParticleEffects()->StartEffect(GetTranslation(), GetRotation(), *m_hitEffect);
					if(m_hitSound != NULL)
						m_hitSound->Play();
					if(m_hitScript != NULL)
						GetOTE()->GetScriptManager()->Execute(m_hitScript, SCRIPT_CALLER_OBJECT, type, object);
				}
			}
			Kill();
		}/**/
	}
	else if(type != TRIGGER_OBJECT_ID && m_bCollidesWithStaticObjects)
	{
		// Kill the object regardless of whether the object it collided with is dynamic or static
		if(m_bExplosive)
			Explode();
		else
		{
			if(m_hitEffect != NULL)
				GetOTE()->GetParticleEffects()->StartEffect(GetTranslation(), GetRotation(), *m_hitEffect);
			if(m_hitSound != NULL)
				m_hitSound->Play();
			if(m_hitScript != NULL && m_damage == 0)
				GetOTE()->GetScriptManager()->Execute(m_hitScript, SCRIPT_CALLER_OBJECT, type, this);
		}
		Kill();
	}

	if(m_controller)
		m_controller->RegisterCollision(this, object);
}

void CAmmo::AddAmmoFeedback(CDynamicObject *ob)
{
	if(!ob->RequiresCollisionDetermination())
		return;
	float co = sqrtfast(m_speed.x*m_speed.x+m_speed.y*m_speed.y+m_speed.z*m_speed.z);
	ob->AddImpulse(m_speed / co * m_explosion.radius);
}

void CAmmo::Kill()
{
	CObjectManagerSceneNode *n = GetEdgewalkerApp()->GetSceneManager()->GetObjectManager();

	n->AddObjectToRemoveList(this);
}

void CAmmo::Explode()
{
	if(!m_bExplosive)
		return;
	if(m_hitEffect != NULL)
		GetOTE()->GetParticleEffects()->StartEffect(GetTranslation(), GetRotation(), *m_hitEffect);
	if(m_hitSound != NULL)
		m_hitSound->Play();

	if(m_damageType == DAMAGE_TYPE_KINETIC)
		GetOTE()->GetObjectManager()->AddImpulseForce(GetTranslation(), D3DXVECTOR3(1.f, 0.f, 0.f), m_explosion.radius, m_explosion.force, 6.28f);
	else
		GetOTE()->GetObjectManager()->AddExplosionForce(GetTranslation(), m_explosion, m_bEnemy, m_source);

	Kill();
}

void CAmmo::Update(float frameTime)
{
	IGameObject::Update(frameTime);

	m_killTimer -= frameTime;
	if(m_killTimer < 0.f)
		Kill();
}