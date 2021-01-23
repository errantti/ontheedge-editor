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

#include "Trigger.h"
#include "..\\OnTheEdge.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrigger::CTrigger()
: IGameObject(NULL, -1), m_actionID(-1), m_triggeringObjectTypes(TRIGGER_OBJECT_PLAYER), 
	m_bTriggered(FALSE), m_bMultitriggering(FALSE), m_triggerType(TRIGGER_TYPE_DIALOGUE),
	m_multiTriggerTimer(0.f), m_multitriggeringDelay(MULTITRIGGER_DELAY), m_damage(100.f),
	m_damageType(DAMAGE_TYPE_ENERGY), m_bActive(TRUE), m_bAEAtObject(TRUE)
{
	m_bRequiresCollisionDetermination = FALSE;
	m_bCollidesWithExternalObjects = FALSE;
	m_bStatic = TRUE;

	m_type = TRIGGER_OBJECT_ID;

	m_bVisible = FALSE;
}

CTrigger::~CTrigger()
{
}

void CTrigger::OnPreRender(bool isShadowPass)
{
	if(GetOTE()->GetObjectManager()->GetShowTriggers() && !isShadowPass)
		m_manager->RegisterNodeForRendering(this, SNG_DEFAULT);
}

void CTrigger::Render(bool isShadowPass)
{
	CDirectX9 *dx = GetDXApp();
	dx->RenderWireframeAABB(GetTransformedBox(), false, 0.f, false, D3DCOLOR_RGBA(255, 255, 255, 255));

	/*D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	dx->GetDevice()->SetTransform(D3DTS_WORLD, &m);

	D3DXVECTOR3 points[8];
	aabbox3d box = GetTransformedBox();
	m_transformedBox.GetCorners(points);

	WORD indeces[24] = {0,1,1,3,3,2,2,0,
						4,5,5,7,7,6,6,4,
						0,4,1,5,2,6,3,7};

	dx->EnableLighting(FALSE);
	dx->GetDevice()->SetFVF(D3DFVF_XYZ);

	dx->GetDevice()->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 0, 24, 12, 
		indeces, D3DFMT_INDEX16, points, sizeof(D3DXVECTOR3));

	dx->EnableLighting(TRUE);*/
}

void CTrigger::RegisterCollision(IGeometricObject *object)
{
	if(!m_bActive)
		return;

	// Find out if the object is of proper type
	//if(!m_bMultitriggering && m_bTriggered)
	//	return;
	if(object->GetType() == AMMO_OBJECT_ID)
		return;

	IGameObject *o = (IGameObject*)object;
	if(m_triggeringObjectTypes & TRIGGER_OBJECT_SPECIFIC_ID)
	{
		if(o->GetID() == m_triggeringObjectID)
		{
			Trigger((CDynamicObject*)object);
			return;
		}
	}

	if(o->GetType() == ENEMY_OBJECT_ID)
	{
		if(m_triggeringObjectTypes & TRIGGER_OBJECT_ENEMY)
			Trigger((CDynamicObject*)object);
		else if(m_triggeringObjectTypes & TRIGGER_OBJECT_POSSESSED_ENEMY)
		{
			if(((CEnemy*)o)->IsPossessed())
				Trigger((CDynamicObject*)object);
		}
	}
	else if(o->GetType() == DYNAMIC_OBJECT_ID)
	{
		if(m_triggeringObjectTypes & TRIGGER_OBJECT_OBJECT)
			Trigger((CDynamicObject*)object);
	}
	else if(o->GetType() == SHIP_OBJECT_ID)
	{
		if(m_triggeringObjectTypes & TRIGGER_OBJECT_PLAYER)
		{
			Trigger((CDynamicObject*)object, GetOTE()->GetPlayer(0)->IsPossessing());
		}
	}
}

void CTrigger::Trigger(CDynamicObject *object, bool bPossessingShip)
{
	if(m_bMultitriggering)
	{
		if(m_multiTriggerTimer > 0.f)
			return;
	}
	else if(m_bTriggered)
		return;

	switch(m_triggerType)
	{
	case TRIGGER_TYPE_DIALOGUE:
		if(!bPossessingShip)
			GetOTE()->GetCurrentLevel()->InitiateDialogue(m_actionID);
		break;
	case TRIGGER_TYPE_ASSISTANTMSG:
		if(!bPossessingShip)
			GetOTE()->GetCurrentLevel()->DisplayAssistentMessage(m_actionID);
		break;
	case TRIGGER_TYPE_SCREEN_MESSAGE:
		if(!bPossessingShip)
			GetOTE()->AddScreenMessage(m_scriptNameMsg, (float)strlen(m_scriptNameMsg) * 0.2f);
		break;
	case TRIGGER_TYPE_TERMINAL:
		if(!bPossessingShip)
			GetOTE()->GetCurrentLevel()->OpenTerminal(m_actionID);
		break;
	case TRIGGER_TYPE_DAMAGES:
		object->InflictDamage(m_damage, true, m_damageType, NULL);
	case TRIGGER_TYPE_EFFECT:
		if(m_bAEAtObject)
			m_actionEvent.Trigger(object);
		else
			m_actionEvent.Trigger(m_relativeTranslation);
		break;
	};

	if(!m_bMultitriggering)
	{
		// Remove the trigger object from scene
		m_bActive = FALSE;
	}
	else
		m_multiTriggerTimer = m_multitriggeringDelay;

	m_bTriggered = TRUE;
}

aabbox3d CTrigger::GetTransformedBox()
{
	//aabbox3d box(D3DXVECTOR3(m_bbox.minCorner + m_relativeTranslation), D3DXVECTOR3(m_bbox.maxCorner + m_relativeTranslation));
	return m_transformedBox;
}

void CTrigger::Update(float frameTime)
{
	if(m_multiTriggerTimer > 0.f)
		m_multiTriggerTimer -= frameTime;
}

STriggerInfo CTrigger::GetTriggerInfo()
{
	STriggerInfo s;
	s.actionID = m_actionID;
	s.bbox = m_transformedBox;
	s.bAEAtObject = m_bAEAtObject;
	s.bActiveByDefault = m_bActive;
	s.bMultitriggering = m_bMultitriggering;
	s.multitriggeringDelay = m_multitriggeringDelay;
	s.id = m_ID;
	s.triggeringObjectTypes = m_triggeringObjectTypes;
	s.type = m_triggerType;
	s.triggeringObjectID = m_triggeringObjectID;
	s.damage = m_damage;
	s.damageType = m_damageType;
	if(m_triggerType == TRIGGER_TYPE_SCREEN_MESSAGE)
		strncpy(s.message, m_scriptNameMsg, TRIGGER_MESSAGE_LENGTH-1);
	else
		s.ae = m_aeTemplate;

	return s;
}

void CTrigger::ApplyTriggerInfo(STriggerInfo s)
{
	m_actionID = s.actionID;
	m_transformedBox = s.bbox;//aabbox3d box(D3DXVECTOR3(m_bbox.minCorner + m_relativeTranslation), D3DXVECTOR3(m_bbox.maxCorner + m_relativeTranslation));
	m_bbox = aabbox3d(D3DXVECTOR3(0.f, 0.f, 0.f), D3DXVECTOR3(s.bbox.maxCorner.x-s.bbox.minCorner.x, 
		s.bbox.maxCorner.y-s.bbox.minCorner.y, s.bbox.maxCorner.z-s.bbox.minCorner.z));
	m_bAEAtObject = s.bAEAtObject;
	m_bActive = s.bActiveByDefault;
	m_bMultitriggering = s.bMultitriggering;
	m_multitriggeringDelay = s.multitriggeringDelay;
	m_ID = s.id;
	m_triggeringObjectTypes = s.triggeringObjectTypes;
	m_triggerType = s.type;
	m_triggeringObjectID = s.triggeringObjectID;
	SetTranslation(s.bbox.minCorner);
	SetRotation(D3DXVECTOR3(0.f, 0.f,0.f));

	m_damage = s.damage;
	m_damageType = s.damageType;

	if(s.type == TRIGGER_TYPE_SCREEN_MESSAGE)
		strncpy(m_scriptNameMsg, s.message, TRIGGER_MESSAGE_LENGTH-1);
	else
	{
		m_aeTemplate = s.ae;
		m_actionEvent.SetData(s.ae);
	}

	// calculate bounding sphere
	m_bs.center = m_bbox.GetCenter()-m_bbox.minCorner;
	float dx = m_bbox.maxCorner.x - m_bbox.minCorner.x, dz = m_bbox.maxCorner.z-m_bbox.minCorner.z;
	m_bs.radius = sqrtfast(dx*dx+50.f*50.f+dz*dz)*0.4f;
}

void CTrigger::SetTranslation(D3DXVECTOR3 &vec)
{
	m_transformedBox.minCorner = vec;
	m_transformedBox.maxCorner = D3DXVECTOR3(vec.x + m_bbox.maxCorner.x, vec.y + m_bbox.maxCorner.y, vec.z + m_bbox.maxCorner.z);
	m_relativeTranslation = vec;
}