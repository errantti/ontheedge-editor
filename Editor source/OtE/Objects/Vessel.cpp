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
#include "VesselShadow.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVessel::CVessel(long id)
: CDynamicObject(NULL, id), m_thrust(15000.f), m_turnSpeed(2.f),
	m_levitationForce(100.f), m_levitationCoefficient(-2.f), m_zeroDistance(7.f),
	m_stunCounter(0.f), 
	m_alteredDamageType(DAMAGE_TYPE_NONE), m_alteredDamageTypeCoefficient(1.f),

	m_peAccelerate(NULL), m_peBrake(NULL), m_peTurn(NULL),
	m_soundAccelerate(NULL), m_soundBreak(NULL), m_soundTurn(NULL),

	m_energy(100.f), 
	m_maxEnergy(100.f),
	m_energyRegeneration(10.f),
	m_stealthCoefficient(1.f),

	m_shadow(NULL)
{
	for(int i=0; i<NUM_GADGETS; ++i)
		m_gadgets[i] = NULL;
}

CVessel::~CVessel()
{
	CAmmo::ReportSourceRemoval(this);
}

void CVessel::Turn(float coefficient)
{
	if(m_stunCounter > 0.f)
		return;

	m_relativeRotation.y += coefficient * m_turnSpeed;

	/*if(m_peTurn)
		m_peTurn->Emit();
	if(m_soundTurn)
		m_soundTurn->Play();/**/
}

void CVessel::TurnManual(float amount)
{
	if(m_stunCounter > 0.f)
		return;
	m_relativeRotation.y += amount;
}

void CVessel::Thrust()
{
	if(m_stunCounter > 0.f)
		return;

	D3DXVECTOR3 force;
	force.x = sinf(m_relativeRotation.y) * m_thrust;
	force.z = cosf(m_relativeRotation.y) * m_thrust;
	force.y = 0.f;
	AddForce(force);

	if(m_peAccelerate)
		m_peAccelerate->Emit();
	if(m_soundAccelerate)
		m_soundAccelerate->Play();
}

void CVessel::Thrust(float coefficient)
{
	if(m_stunCounter > 0.f)
		return;

	D3DXVECTOR3 force;
	float thrust = m_thrust * coefficient;
	force.x = sinf(m_relativeRotation.y) * thrust;
	force.z = cosf(m_relativeRotation.y) * thrust;
	force.y = 0.f;
	AddForce(force);

	if(m_peAccelerate)
		m_peAccelerate->Emit();
	if(m_soundAccelerate)
		m_soundAccelerate->Play();
}

void CVessel::BackThrust()
{
	if(m_stunCounter > 0.f)
		return;

	D3DXVECTOR3 force;
	//float temp = m_thrust * BACK_THRUST_COEFFICIENT;
	float temp = m_thrust * -0.6f;
	force.x = sinf(m_relativeRotation.y) * temp;
	force.z = cosf(m_relativeRotation.y) * temp;
	force.y = 0.f;
	AddForce(force);

	/*if(m_peBrake)
		m_peBrake->Emit();
	if(m_soundBreak)
		m_soundBreak->Play();/**/
}

void CVessel::SideThrust(bool left)
{
	if(m_stunCounter > 0.f)
		return;

	D3DXVECTOR3 force;
	float temp = m_thrust * 0.4f;
	if(left)
		temp = -temp;
	force.z = -sinf(m_relativeRotation.y) * temp;
	force.x = cosf(m_relativeRotation.y) * temp;
	force.y = 0.f;
	AddForce(force);
}

void CVessel::Levitate(float groundDistance)
{
	D3DXVECTOR3 force(0.f,0.f,0.f);
	if(groundDistance < 0.f)
		groundDistance = 0.f;
	force.y = m_levitationCoefficient*(groundDistance - m_zeroDistance) + m_levitationForce;
	if(force.y < 0.f)
		force.y = 0.f;
	AddForce(force);
}

void CVessel::Update(float frameTime)
{
	float terrainHeight = ISceneManager::GetManager()->GetTiledTerrain()->GetTerrainHeight(m_relativeTranslation.x, m_relativeTranslation.z);

	float d = m_relativeTranslation.y - terrainHeight;

	if(m_shadow != NULL)
	{
		// Don't display the shadow against skybox
		if(d > 0.f && terrainHeight > -10.f)
		{
			// Update shadow position
			//D3DXVECTOR3 shadowPos(m_relativeTranslation.x, d + 0.01f, m_relativeTranslation.z);
			D3DXVECTOR3 shadowPos(0.f, -d + 1.f, 0.f);
			m_shadow->SetTranslation(shadowPos);
			m_shadow->SetVisible(true);
		}
		else
			m_shadow->SetVisible(false);
	}

	//d-= m_bs.radius;
	Levitate(d);

	IGameObject::Update(frameTime);

	if(m_stunCounter > 0.f)
		m_stunCounter -= frameTime;

	m_energy += m_energyRegeneration * frameTime;
	if(m_energy > m_maxEnergy)
		m_energy = m_maxEnergy;
}

void CVessel::OnPreRender(bool isShadowPass)
{
	if(m_bVisible && m_mesh)
	{
		if(m_stealthCoefficient > 0.9f || (isShadowPass && m_stealthCoefficient > 0.2f))
			m_manager->RegisterNodeForRendering(this, SNG_DEFAULT);
		else
			m_manager->RegisterNodeForRendering(this, SNG_ALPHA_BLENDED);
	}
	ISceneNode::OnPreRender(isShadowPass);
}

void CVessel::Render(bool isShadowPass)
{
	if(m_mesh == NULL)
		return;

	// Render the mesh
	CDirectX9 *dx = GetDXApp();
	if(dx == NULL)
		return;

	dx->SetTransform(D3DTS_WORLD, &m_absoluteTransformation);

	if (isShadowPass)
		m_mesh->RenderWithoutTexture();
	else if(m_stealthCoefficient > 0.9f)
		m_mesh->Render();
	else
	{
		IDirect3DDevice9 *pDevice = GetDXApp()->GetDevice();

		// Blend options
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVBLENDFACTOR);
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

		DWORD temp = (int)(255.f*m_stealthCoefficient);
		if(temp>255)
			temp = 255;
		temp = temp << 24 | temp << 16 | temp << 8 | temp;
		pDevice->SetRenderState(D3DRS_BLENDFACTOR, temp);

		m_mesh->Render();

		pDevice->SetRenderState(D3DRS_BLENDFACTOR, 0xFFFFFFFF);
	}

	/*D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	dx->SetTransform(D3DTS_WORLD, &mat);
	D3DXMatrixTranslation(&mat, m_relativeTranslation.x, m_relativeTranslation.y, m_relativeTranslation.z);
	CGUIEnvironment *env = GetOTE()->GetGUIEnvironment();
	CFont *f = env->GetDefaultFont();
	f->Print("oijslekjrlsij", rect<int>(320, 450, 25, 225), 0xFFFFFFFF, DT_RIGHT);*/
}

void CVessel::ApplyVesselTemplate(SVesselTemplate t, bool bInitiateEffects, bool bRemoveOldEffects)
{
	ApplyDynamicObjectTemplate(t.dt);
	m_levitationCoefficient = t.levitationCoefficient;
	m_levitationForce = t.levitationForce;
	m_thrust = t.thrust;
	m_turnSpeed = t.turnSpeed;
	m_zeroDistance = t.zeroDistance;
	m_explosion = t.explosion;
	m_ammoSpawnAdjustment = t.ammoSpawnAdjustment;

	m_energyRegeneration = t.baseEnergyRegeneration;
	m_maxEnergy = t.baseEnergy;
	m_energy = m_maxEnergy;

	if(bRemoveOldEffects)
		RemoveAll();

	if(bInitiateEffects)
		CreateVesselEffects(t);

	CreateShadow(false);
}

void CVessel::CreateShadow(bool bForceCreate)
{
	if(m_shadow != NULL)
	{
		// Shadow already exists, skip creation
		return;
	}

	if(CGameSettings::GetSettingsObject()->GetSetting(GO_STATIC_SHADOWS_ENABLED) || bForceCreate)
	{
		CMesh *shadowMesh = CResourceManager::GetManager().GetMesh("meshes\\shadow.x");
		if(shadowMesh == NULL)
		{
			// Failed loading shadow mesh
			return;
		}
		CVesselShadow *shadow = new CVesselShadow(shadowMesh, this, m_manager);
		if(shadow != NULL)
		{
			// Set shadow invisible by default, vessel's update will set it visible when required
			shadow->SetVisible(false);
			m_shadow = shadow;
		}
	}
}

void CVessel::CreateVesselEffects(SVesselTemplate &t)
{
	m_peAccelerate = GetOTE()->GetParticleEffects()->StartEffect(this, t.peIDAccelerate);
	m_soundAccelerate = CSoundManager::GetManager().GetSound(t.soundIDAccelerate);

	/*m_peBrake = GetOTE()->GetParticleEffects()->StartEffect(this, t.peIDBrake);
	m_soundBrake = CSoundManager::GetManager().GetSound(t.soundIDBrake);

	m_peTurn = GetOTE()->GetParticleEffects()->StartEffect(this, t.peIDAccelerate);
	m_soundTurn = CSoundManager::GetManager().GetSound(t.soundIDAccelerate);/**/
}

void CVessel::OnDeath()
{
	// Create an explosion
	GetOTE()->GetObjectManager()->AddExplosionForce(GetTranslation(), m_explosion);
	//GetOTE()->GetParticleEffects()->StartEffect(GetTranslation(), GetRotation(), m_explosionTypeID);
	m_onDestruction.Trigger(m_relativeTranslation);
	//GetOTE()->GetParticleEffects()->StartEffect(GetTranslation(), GetRotation(), 2);
}

void CVessel::InflictDamage(float amount, bool bEnemy, DAMAGE_TYPE type, CVessel *source)
{
	if(m_bDead) // Can't kill me if I'm dead already :>
		return;

	if(type == m_alteredDamageType)
		amount *= m_alteredDamageTypeCoefficient;

	if(amount <= 0.f)
		return;

	if(type == DAMAGE_TYPE_STUN)
	{
		m_stunCounter = amount;
		return;
	}

	if(m_bIndestructible)
		return;

	m_onDamageTaken.Trigger(this);

	m_hitPoints -= amount;

	if(m_hitPoints <= 0)
	{
		m_bDead = TRUE;
		// Kill the object
		Kill();
	}
}

void CVessel::AddEnergy(float amount)
{
	m_energy += amount;
	if(m_energy > m_maxEnergy)
		m_energy = m_maxEnergy;
}

bool CVessel::IsFullEnergy()
{
	if(m_energy == m_maxEnergy)
		return true;
	return false;
}

bool CVessel::IsFullHealth()
{
	if(m_hitPoints == m_maxHitPoints)
		return true;
	return false;
}

IGadget *CVessel::GetGadget(int index)
{
	if(index < 0 || index >= NUM_GADGETS)
		return NULL;
	return m_gadgets[index];
}

void CVessel::SetGadget(int index, IGadget *gadget)
{
	if(index < 0 || index >= NUM_GADGETS)
		return;
	m_gadgets[index] = gadget;
}

D3DXVECTOR3 CVessel::GetAmmoSpawnPosition()
{
	return CMath::MatrixVectorMultiply(m_absoluteTransformation, m_ammoSpawnAdjustment);
}