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

#include "WeaponGadget.h"
#include "..\\OnTheEdge.h"

CWeaponGadget::CWeaponGadget(SWeaponGadgetTemplate &t, SGadgetTemplate &gt) : 
	m_ammoAngleUnit(0.1f), 
	m_numBullets(1), 
	m_nextAmmo(0.f),
	m_activationTimer(0.f),
	m_bOneUse(false)
{
	m_bActivateable = true;

	ApplyTemplate(t, gt);
}

CWeaponGadget::~CWeaponGadget() {}

void CWeaponGadget::ApplyTemplate(SWeaponGadgetTemplate &t, SGadgetTemplate &gt)
{
	IGadget::ApplyTemplate(gt);

	m_template = t;

	m_ammoInstData.exp = m_template.explosion;
	m_ammoInstData.bExplodeOnTimerEnd = m_template.bExplodeOnTimerEnd;
	m_ammoInstData.bExplosive = m_template.bExplosive;
	m_ammoInstData.damage = m_template.damage;
	m_ammoInstData.lifetime = m_template.lifetime;

	m_bOneUse = gt.bOneUse;

	m_ammoInstData.hitEffect = GetOTE()->GetParticleEffects()->GetEffectData(m_template.hitEffect);
	m_ammoInstData.hitSound = CSoundManager::GetManager().GetSound(m_template.hitSound);
	m_ammoInstData.onHitScript = CScriptManager::GetManager().GetScript(m_template.hitScript);

	if(m_template.ammoTemplateID > 0)
	{
		m_ammoTemplate = GetOTE()->GetAmmoDatabase()->GetAmmoTemplate(m_template.ammoTemplateID);
		if(m_ammoTemplate == NULL)
		{
			string str = "Couldn't find ammo template id ";
			str += m_template.ammoTemplateID;
			CPrinter::logError(str);
		}
	}
}

bool CWeaponGadget::Equip(CVessel *vessel)
{
	if(!IGadget::Equip(vessel) || m_ammoTemplate == NULL)
		return false;

	if(m_template.mode == WEAPON_MODE_FORWARD || m_template.mode == WEAPON_MODE_FORWARD_ALTERNATING && m_template.numBullets > 1)
		m_ammoAngleUnit = m_template.angle*2.f/(float)(m_template.numBullets-1);
	if(m_template.mode == WEAPON_MODE_NOVA || m_template.mode == WEAPON_MODE_NOVA_ALTERNATING)
		m_ammoAngleUnit = _2PI/(float)m_template.numBullets;
	m_numBullets = (float)m_template.numBullets;

	m_ammoInstData.bEnemy = vessel->IsEnemy();
	m_ammoInstData.sourceID = vessel->GetID();
	m_ammoInstData.source = vessel;

	return true;
}

void CWeaponGadget::Update(float frameTime)
{
	if(m_activationTimer > 0.f)
		m_activationTimer -= frameTime;
}

bool CWeaponGadget::Activate()
{
	if(m_vessel == NULL)
		return false;
	if(m_template.mode != WEAPON_MODE_THROWER && m_ammoTemplate == NULL)
		return false;

	if(m_activationTimer <= 0.f && *m_energy - m_template.energyConsumption > 0.f)
	{
		if(m_activationSound)
			m_activationSound->Play();

		m_activationTimer = m_template.firingPace;
		*m_energy -= m_template.energyConsumption;

		if(m_activationScript != NULL)
			GetOTE()->GetScriptManager()->Execute(m_activationScript, SCRIPT_CALLER_OBJECT, m_vessel->GetType(), m_vessel);

		if(m_template.mode == WEAPON_MODE_THROWER)
		{
			if(m_ammoInstData.hitEffect)
				GetOTE()->GetParticleEffects()->StartEffect(m_vessel->GetTranslation(), 
					m_vessel->GetRotation(), *m_ammoInstData.hitEffect);

			GetOTE()->GetObjectManager()->AddConeAreaDamage(m_vessel->GetTranslation(), 
				m_vessel->GetRotation(), m_template.force, m_ammoInstData.damage, m_template.angle, 
				DAMAGE_TYPE_PLASMA, true, m_vessel->IsEnemy(), m_vessel);
		}
		else if(m_template.numBullets <= 1)
		{
			D3DXVECTOR3 originSpeed(m_vessel->GetSpeed());
			if(m_template.mode == WEAPON_MODE_BACKWARD)
				originSpeed = D3DXVECTOR3(0.f,0.f,0.f);
			//GetOTE()->GetObjectManager()->AddBullet(v.GetTranslation(), v.GetRotation(), v.GetBoundingSphere().radius,
			//	m_template.lifetime, m_template.ammoTemplateID, 0.f, m_template.force, m_template.e, FALSE, v.GetID());
			//CAmmo *ammo = GetOTE()->GetObjectManager()->AddAmmo(m_vessel->GetTranslation(), 
			CAmmo *ammo = GetOTE()->GetObjectManager()->AddAmmo(m_vessel->GetAmmoSpawnPosition(), 
				m_vessel->GetRotation(), originSpeed, m_vessel->GetBoundingSphere().radius, 
				0.f, m_template.force, *m_ammoTemplate, m_ammoInstData, FALSE);
			if(ammo)
				ammo->SetSourceID(m_vessel->GetID());
		}
		else
		{
			if(m_template.mode == WEAPON_MODE_FORWARD)
				LaunchForwardWeapon(FALSE);
			else if(m_template.mode == WEAPON_MODE_FORWARD_ALTERNATING)
				LaunchForwardWeapon(TRUE);
			else if(m_template.mode == WEAPON_MODE_NOVA)
				LaunchNovaWeapon(FALSE);
			else if(m_template.mode == WEAPON_MODE_NOVA_ALTERNATING)
				LaunchNovaWeapon(TRUE);
		}
		return true;
	}
	return false;
}

void CWeaponGadget::LaunchForwardWeapon(bool bAlternating)
{
	D3DXVECTOR3 originSpeed(m_vessel->GetSpeed());
	// D3DXVECTOR3 translation = m_vessel->GetTranslation();
	D3DXVECTOR3 translation = m_vessel->GetAmmoSpawnPosition();

	// If the weapon hasn't been equipped there will be some problems
	if(bAlternating)
	{
		if(m_nextAmmo >= m_numBullets)
			m_nextAmmo = 0.f;
		//GetOTE()->GetObjectManager()->AddBullet(v.GetTranslation(), v.GetRotation(), v.GetBoundingSphere().radius,
		//	m_t.lifetime, m_t.ammoTemplateID, m_nextAmmo * m_ammoAngleUnit - m_t.angle, m_t.force, m_t.e, TRUE, v.GetID());
		//CAmmo *ammo = GetOTE()->GetObjectManager()->AddAmmo(m_vessel->GetTranslation(), 
		CAmmo *ammo = GetOTE()->GetObjectManager()->AddAmmo(translation, 
				m_vessel->GetRotation(), originSpeed, m_vessel->GetBoundingSphere().radius, 
				m_nextAmmo * m_ammoAngleUnit - m_template.angle, m_template.force, 
				*m_ammoTemplate, m_ammoInstData, TRUE);
		if(ammo)
			ammo->SetSourceID(m_vessel->GetID());
		m_nextAmmo += 1.f;
	}
	else
	{
		for(float i=0.f; i<m_numBullets; i+=1.f)
		{
			//GetOTE()->GetObjectManager()->AddBullet(v.GetTranslation(), v.GetRotation(), v.GetBoundingSphere().radius,
			//m_t.lifetime, m_t.ammoTemplateID, i * m_ammoAngleUnit - m_t.angle, m_t.force, m_t.e, TRUE, v.GetID());
			CAmmo *ammo = GetOTE()->GetObjectManager()->AddAmmo(translation, 
				m_vessel->GetRotation(), originSpeed, m_vessel->GetBoundingSphere().radius, 
				i * m_ammoAngleUnit - m_template.angle, m_template.force, 
				*m_ammoTemplate, m_ammoInstData, TRUE);
			if(ammo)
				ammo->SetSourceID(m_vessel->GetID());
		}
	}
}

void CWeaponGadget::LaunchNovaWeapon(bool bAlternating)
{
	//D3DXVECTOR3 &originSpeed = m_vessel->GetSpeed();

	// D3DXVECTOR3 translation = m_vessel->GetTranslation();
	D3DXVECTOR3 translation = m_vessel->GetAmmoSpawnPosition();

	// If the weapon hasn't been equipped there will be some problems
	if(bAlternating)
	{
		if(m_nextAmmo >= m_numBullets)
			m_nextAmmo = 0.f;
		//GetOTE()->GetObjectManager()->AddBullet(v.GetTranslation(), v.GetRotation(), v.GetBoundingSphere().radius,
		//	m_t.lifetime, m_t.ammoTemplateID, m_nextAmmo * m_ammoAngleUnit, m_t.force, m_t.e, FALSE, v.GetID());

		CAmmo *ammo = GetOTE()->GetObjectManager()->AddAmmo(translation, 
				//m_vessel->GetRotation(), originSpeed, m_vessel->GetBoundingSphere().radius, 
				m_vessel->GetRotation(), D3DXVECTOR3(0.f, 0.f, 0.f), m_vessel->GetBoundingSphere().radius, 
				m_nextAmmo * m_ammoAngleUnit, m_template.force, 
				*m_ammoTemplate, m_ammoInstData, FALSE);
		if(ammo)
			ammo->SetSourceID(m_vessel->GetID());

		m_nextAmmo += 1.f;
	}
	else
	{
		for(float i=0.f; i<m_numBullets; i+=1.f)
		{
			//GetOTE()->GetObjectManager()->AddBullet(v.GetTranslation(), v.GetRotation(), v.GetBoundingSphere().radius,
			//m_t.lifetime, m_t.ammoTemplateID, i * m_ammoAngleUnit, m_t.force , m_t.e, FALSE, v.GetID());
			CAmmo *ammo = GetOTE()->GetObjectManager()->AddAmmo(translation, 
				//m_vessel->GetRotation(), originSpeed, m_vessel->GetBoundingSphere().radius, 
				m_vessel->GetRotation(), D3DXVECTOR3(0.f, 0.f, 0.f), m_vessel->GetBoundingSphere().radius, 
				i * m_ammoAngleUnit, m_template.force, 
				*m_ammoTemplate, m_ammoInstData, FALSE);
			if(ammo)
				ammo->SetSourceID(m_vessel->GetID());
		}
	}
}

void CWeaponGadget::SetDescription()
{
}