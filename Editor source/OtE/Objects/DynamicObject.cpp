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

#include "DynamicObject.h"
#include "..\\OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDynamicObject::CDynamicObject(ISceneNode *parent, long id)
: IGameObject(parent, id), m_bIndestructible(FALSE), m_teamID(-1), m_bDead(FALSE),
	m_instInfo(NULL), m_refTarget(NULL)
{
	m_type = DYNAMIC_OBJECT_ID;
}

CDynamicObject::~CDynamicObject()
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
	/*if(m_controller != NULL)
		if(m_controller->RequiresInstantiation())
		{
			//if(CControllerManager::GetManager() != NULL)
			//	CControllerManager::GetManager().RemoveInstancedController(m_controller);
		}*/
}

// ApplyTemplate
// Fills the object with template information
void CDynamicObject::ApplyDynamicObjectTemplate(SDynamicObjectTemplate t)
{
	m_bIndestructible = t.bIndestructible;
	m_bCollidesWithExternalObjects = t.bCollidesWithExternalObjects;
	m_bCollidesWithObjects = t.bCollidesWithObjects;
	m_bGravity = t.bAffectedByGravity;
	m_bDrag = t.bAffectedByDrag;
	m_bRequiresCollisionDetermination = t.bRequiresCollisionDetermination;
	m_bvCoefficient = t.bvCoefficient;
	m_dragCoefficient = t.dragCoefficient;
	m_maxHitPoints = m_hitPoints = (float)t.hitPoints;
	m_mass = t.mass;
	m_templateID = t.templateID;
	m_collisionType = t.collisionType;

	m_onDamageTaken.SetData(t.onDamageTaken);
	m_onCollision.SetData(t.onCollision);
	m_onDestruction.SetData(t.onDestruction);

	if(t.collisionParam1 > 0.01f)
		SetScalingCoefficient(t.collisionParam1);
}

void CDynamicObject::ApplyObjectInfo(SObjectInstanceInfo &info, bool bRemoveOldEffects)
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

	if(info.scriptOverrideType != SCRIPT_OVERRIDE_NONE)
		SetOverrideScript(info.scriptOverrideType, info.scriptOverride);
	else if(strlen(info.targetRef) > 0)
	{
		if(m_refTarget == NULL)
			m_refTarget = new char[32];
		strcpy(m_refTarget, info.targetRef);
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

	//m_targetID = info.targetID;
}

void CDynamicObject::SetInputControlParam(int index, float value)
{
	if(index < 0 || index > 1)
		return;
	m_controlParams[index] = value;
}

SObjectInstanceInfo *CDynamicObject::GetObjectInfo(bool autoUpdate)
{
	if(autoUpdate)
		UpdateObjectInfo();
	return m_instInfo;
}

void CDynamicObject::UpdateObjectInfo()
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
	i.type = m_type;
	//i.targetID = m_targetID;
	i.bRequiresConstantUpdate = m_bRequiresConstantUpdate;
}

SObjectInstanceInfo CDynamicObject::ConstructObjectInfo()
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
	s.type = m_type;
	//s.targetID = m_targetID;

	return s;
}

void CDynamicObject::InflictDamage(float amount, bool bEnemy, DAMAGE_TYPE type, CVessel *source)
{
	if(m_bDead || m_bIndestructible) // Can't kill me if I'm dead already :>
		return;

	m_onDamageTaken.Trigger(this);

	m_hitPoints -= amount;
	if(m_hitPoints <= 0.f)
	{
		// Kill the object
		Kill();
	}
}

void CDynamicObject::Repair(float amount)
{
	m_hitPoints += amount;
	if(m_hitPoints > m_maxHitPoints)
		m_hitPoints = m_maxHitPoints;
}

void CDynamicObject::Kill()
{
	CObjectManagerSceneNode *n = GetEdgewalkerApp()->GetSceneManager()->GetObjectManager();

	OnDeath();

	m_bDead = true;
	n->AddObjectToRemoveList(this);
}

void CDynamicObject::OnDeath()
{
	m_onDestruction.Trigger(m_relativeTranslation);
}

void CDynamicObject::SetOverrideScript(SCRIPT_OVERRIDE_TYPE type, char *name)
{
	if(name == NULL)
		return;

	switch(type)
	{
	case SCRIPT_OVERRIDE_ONDEATH:
		m_onDestruction.SetScript(name);
		break;
	case SCRIPT_OVERRIDE_ONCOLLISION:
		m_onCollision.SetScript(name);
		break;
	case SCRIPT_OVERRIDE_ONDAMAGE:
		m_onDamageTaken.SetScript(name);
		break;
	}
}