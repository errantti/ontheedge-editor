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

#include "StaticObject.h"
#include "..\\OnTheEdge.h"

#define GADGET_ROTATION_SPEED	1.5f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaticObject::CStaticObject()
: IGameObject(NULL, -1), m_targetID(-1), m_instInfo(NULL), m_refTarget(NULL), m_pEffect(NULL),
	m_restorationCapacity(0.f), m_depleatedTimer(0.f)
{
	m_bStatic = TRUE;
	m_type = STATIC_OBJECT_ID;
}

CStaticObject::~CStaticObject()
{
	if(m_instInfo != NULL)
	{
		delete m_instInfo;
		m_instInfo = NULL;
	}

	if(m_refTarget != NULL)
	{
		delete [] m_refTarget;
		m_refTarget = NULL;
	}

	// Remove controller if it was specifically instanced for this object
	//if(m_controller != NULL)
	//	if(m_controller->RequiresInstantiation())
	//		CControllerManager::GetManager().RemoveInstancedController(m_controller);
}

void CStaticObject::OnPreRender(bool isShadowPass)
{
#ifndef _EDITOR
	if(m_alpha > 0.f)
#endif
		CMeshSceneNode::OnPreRender(isShadowPass);
}

void CStaticObject::RegisterCollision(IGeometricObject *object)
{
	IGameObject *target;

	if(m_soType == STATIC_OBJECT_NORMAL_TELEPORT)
	{
		int type = object->GetType();
		if(type == SHIP_OBJECT_ID || type == ENEMY_OBJECT_ID || type == DYNAMIC_OBJECT_ID)
		{
			if(!((CDynamicObject*)object)->IsDead() && m_targetID != -1)
			{
				target = GetOTE()->GetObjectManagerSceneNode()->GetObject(m_targetID);
				if(target != NULL)
				{
					D3DXVECTOR3 targetPos = target->GetTranslation();
					targetPos.y += 10.f;
					object->SetTranslation(targetPos);
					ReportActivation((IGameObject*)object);
				}
			}
		}
	}
}

void CStaticObject::ApplyStaticObjectTemplate(SStaticObjectTemplate t)
{
	m_bvCoefficient = t.bvCoefficient;
	m_bCollidesWithObjects = t.bCollidesWithObjects;
	m_soType = t.type;
	m_templateID = t.templateID;
	if(t.type == STATIC_OBJECT_GADGET || t.type == STATIC_OBJECT_COMPONENT ||
		t.type == STATIC_OBJECT_QUESTITEM || t.type == STATIC_OBJECT_PATCH)
		m_targetID = t.targetID;
	m_customAction.SetData(t.customAction);
	m_alpha = t.alpha;
	m_collisionType = t.collisionType;
	if(m_soType == STATIC_OBJECT_ENERGY_POD || m_soType == STATIC_OBJECT_REPAIR_POD)
	{
		m_pEffect = GetOTE()->GetParticleEffects()->StartEffect(this, m_customAction.GetData().particleEffectID);
		m_restorationCapacity = 100.f;
	}

	if(t.collisionParam1 > 0.01f)
		SetScalingCoefficient(t.collisionParam1);
}

void CStaticObject::ApplyObjectInfo(SObjectInstanceInfo &info, bool bRemoveOldEffects)
{
	// Remove old particle and light effects
	// TODO: actually check that this only removes the particle and light effects
	if(bRemoveOldEffects)
		RemoveAll();

	// Create new particle and light effects
	CLightEffectManager::GetManager().CreateLightEffect(this, info.lightEffectID, info.effectPos);
	CParticleEffects::GetManager().StartEffect(this, info.particleEffectID, info.effectPos);

	/**
	 * NOTE: Things that must be updated outside of
	 * this method:
	 * position - has to be correctly placed into object manager grid
	 * refname - has to be updated into refname storage
	 * (id - if it will need to be mapped in the future)
	 */
	m_ID = info.id;
	m_relativeRotation = info.rot;
	m_bRequiresConstantUpdate = info.bRequiresConstantUpdate;

	if(info.scriptOverrideType == SCRIPT_OVERRIDE_CUSTOM)
		m_customAction.SetScript(info.scriptOverride);
	else if(strlen(info.targetRef) > 0)
	{
		SetTargetRef(info.targetRef);
	}
	
	// Add controller
	if(m_controller != NULL)
		if(m_controller->RequiresInstantiation())
			CControllerManager::GetManager().RemoveInstancedController(m_controller);

	m_controller = CControllerManager::GetManager().GetController(info.controllerID);
	for(int i=0;i<2;++i)
		m_controlParams[i] = info.controllerParams[i];
	if(m_controller != NULL)
		m_controller->InitObject(this);

	m_targetID = info.targetID;

	if(m_soType == STATIC_OBJECT_ENERGY_POD || m_soType == STATIC_OBJECT_REPAIR_POD)
		m_restorationCapacity = (float)m_targetID;

	// Target ids of pickable items are defined by the templates so don't go and change them
	/*if(m_soType != STATIC_OBJECT_GADGET && m_soType != STATIC_OBJECT_PATCH &&
		m_soType != STATIC_OBJECT_QUESTITEM && m_soType != STATIC_OBJECT_COMPONENT)
	{
		m_targetID = info.targetID;
	}*/
}

SObjectInstanceInfo *CStaticObject::GetObjectInfo(bool autoUpdate)
{
	if(autoUpdate)
		UpdateObjectInfo();
	return m_instInfo;
}

void CStaticObject::UpdateObjectInfo()
{
	if(m_instInfo == NULL)
	{
		m_instInfo = new SObjectInstanceInfo;
		m_instInfo->FillDefaults();
	}

	SObjectInstanceInfo &i = *m_instInfo;
	i.id = m_ID;
	i.pos = m_relativeTranslation;
	i.rot = m_relativeRotation;
	i.templateID = m_templateID;
	i.type = STATIC_OBJECT_ID;
	i.targetID = m_targetID;
	i.bRequiresConstantUpdate = m_bRequiresConstantUpdate;
}

SObjectInstanceInfo CStaticObject::ConstructObjectInfo()
{
	SObjectInstanceInfo s;
	s.FillDefaults();

	s.bRequiresConstantUpdate = m_bRequiresConstantUpdate;
	if(m_controller)
	{
		s.controllerID = m_controller->GetControllerID();
		s.controllerParams[0] = m_controlParams[0];
		s.controllerParams[1] = m_controlParams[1];
	}

	s.id = m_ID;
	s.pos = m_relativeTranslation;
	s.rot = m_relativeRotation;
	s.templateID = m_templateID;
	s.type = STATIC_OBJECT_ID;
	s.targetID = m_targetID;

	return s;
}

void CStaticObject::SetInputControlParam(int index, float value)
{
	if(index < 0 || index > 1)
		return;
	m_controlParams[index] = value;
}

void CStaticObject::Update(float frameTime)
{
	if(m_depleatedTimer > 0.f)
		m_depleatedTimer -= frameTime;

	if(m_soType == STATIC_OBJECT_GADGET || m_soType == STATIC_OBJECT_PATCH ||
		m_soType == STATIC_OBJECT_QUESTITEM || m_soType == STATIC_OBJECT_COMPONENT)
	{
		// Rotate the gadget around Y-axis
		m_relativeRotation.y += frameTime * GADGET_ROTATION_SPEED;

		// Just a safety measure
		if(m_relativeRotation.y > 2*D3DX_PI)
			m_relativeRotation.y -= 2*D3DX_PI;
	}

	if(m_controller != NULL)
		m_controller->Update(this, frameTime);

	ISceneNode::Update(frameTime);
}

void CStaticObject::Destroy()
{
	if(m_soType == STATIC_OBJECT_GADGET || m_soType == STATIC_OBJECT_PATCH ||
		m_soType == STATIC_OBJECT_QUESTITEM || m_soType == STATIC_OBJECT_COMPONENT)
	{
		m_customAction.Trigger(m_relativeTranslation, m_relativeRotation);
	}
	ISceneManager::GetManager()->GetObjectManager()->AddObjectToRemoveList(this);
}

void CStaticObject::ReportActivation(IGameObject *activator)
{
	if(m_soType == STATIC_OBJECT_NORMAL_TELEPORT && activator != NULL)
	{
		m_customAction.Trigger(activator);
	}
	/*else if(m_soType == STATIC_OBJECT_ENERGY_POD || m_soType == STATIC_OBJECT_REPAIR_POD)
	{
		if(m_pEffect != NULL)
			m_pEffect->Emit();
		m_customAction.PlaySound();
		//m_customAction.Trigger(
	}*/
}

void CStaticObject::SetTargetRef(const char *str)
{
	if(str == NULL)
		return;
	if(m_refTarget == NULL)
		m_refTarget = new char[32];
	strncpy(m_refTarget, str, 31);
}

bool CStaticObject::RestoreEnergy(CVessel *target, float frameTime)
{
	if(target == NULL)
		return false;
	if(m_restorationCapacity <= 0.f)
	{
		if(m_depleatedTimer <= 0.f)
		{
			m_depleatedTimer = 3.f;
			GetOTE()->AddScreenMessage("Energy pod depleated");
		}
		return false;
	}
	if(target->IsFullEnergy())
		return false;

	float amount = GetOTE()->GetCurrentLevel()->GetEnergyPodRegeneration() * frameTime;
	target->AddEnergy(amount);
	m_restorationCapacity -= amount;

	if(m_pEffect != NULL)
		m_pEffect->Emit();

	return true;
}

bool CStaticObject::RestoreHealth(CVessel *target, float frameTime)
{
	if(target == NULL)
		return false;
	if(m_restorationCapacity <= 0.f)
	{
		if(m_depleatedTimer <= 0.f)
		{
			m_depleatedTimer = 3.f;
			GetOTE()->AddScreenMessage("Repair pod depleated");
		}
		return false;
	}
	if(target->IsFullHealth())
		return false;

	float amount = GetOTE()->GetCurrentLevel()->GetRepairPodRegeneration() * frameTime;
	target->Repair(amount);
	m_restorationCapacity -= amount;

	if(m_pEffect != NULL)
		m_pEffect->Emit();

	return true;
}