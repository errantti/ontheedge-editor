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

#include "ActivateableDoorLiftController.h"
#include "..\\OnTheEdge.h"

CActivateableDoorLiftController::CActivateableDoorLiftController() : m_bActiveLastFrame(false),
	m_doorLift(NULL)
{
	m_controllerID = ACTIVATEABLE_DOOR_LIFT_CONTROLLER_ID;
	m_bActive = false;
}

void CActivateableDoorLiftController::InitObject(IGameObject *object)
{
	if(object == NULL)
		return;
	if(object->GetType() != STATIC_OBJECT_ID)
		return;
	m_bActiveLastFrame = false;
	m_doorLift = object;

	ISimpleObjectControlInterface *params;
	//if(object->GetType() == STATIC_OBJECT_ID)
	params = (ISimpleObjectControlInterface*)((CStaticObject*)object);
	//else
	//	params = (ISimpleObjectControlInterface*)((CDynamicObject*)object);

	float &time = params->GetInputControlParam(0);
	float &speed = params->GetInputControlParam(1);

	float &coef = params->GetRuntimeControlParam(0);
	m_counter = time;

	coef = 1.f;
}

void CActivateableDoorLiftController::Update(IGameObject *object, float frameTime)
{
	if(object == NULL || !m_bActive)
	{
		m_bActiveLastFrame = false;
		return;
	}
	if(object != m_doorLift)
		return;

	ISimpleObjectControlInterface *params;
	params = (ISimpleObjectControlInterface*)((CStaticObject*)object);

	if(!m_bActiveLastFrame && m_bActive)
	{
		m_counter = params->GetInputControlParam(0);
		m_bActiveLastFrame = true;
	}

	float &speed = params->GetInputControlParam(1);

	float &coef = params->GetRuntimeControlParam(0);

	m_counter -= frameTime;
	if(m_counter < 0.f)
	{
		m_bActive = false;
		coef = -coef;
		return;
	}

	object->SetTranslationRel(D3DXVECTOR3(0.f, speed * coef * frameTime, 0.f));
}