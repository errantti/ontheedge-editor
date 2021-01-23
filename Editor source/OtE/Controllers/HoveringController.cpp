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

#include "HoveringController.h"
#include "..\\OnTheEdge.h"

CHoveringController::CHoveringController()
{
	m_controllerID = HOVERING_CONTROLLER_ID;
}

void CHoveringController::InitObject(IGameObject *object)
{
	if(object == NULL)
		return;

	ISimpleObjectControlInterface *params;
	if(object->GetType() == STATIC_OBJECT_ID)
		params = (ISimpleObjectControlInterface*)((CStaticObject*)object);
	else
		params = (ISimpleObjectControlInterface*)((CDynamicObject*)object);

	float &accelerationSpeed = params->GetInputControlParam(0);
	float &maxSpeed = params->GetInputControlParam(1);

	float &speed = params->GetRuntimeControlParam(0);
	float &acceleration = params->GetRuntimeControlParam(1);

	speed = maxSpeed;
	acceleration = 0.f;
}

void CHoveringController::Update(IGameObject *object, float frameTime)
{
	if(object == NULL)
		return;

	ISimpleObjectControlInterface *params;
	if(object->GetType() == STATIC_OBJECT_ID)
		params = (ISimpleObjectControlInterface*)((CStaticObject*)object);
	else
		params = (ISimpleObjectControlInterface*)((CDynamicObject*)object);

	float &accelerationSpeed = params->GetInputControlParam(0);
	float &maxSpeed = params->GetInputControlParam(1);

	float &speed = params->GetRuntimeControlParam(0);
	float &acceleration = params->GetRuntimeControlParam(1);

	/**
	 * 
	 **/
	if(speed > 0.f)
		acceleration -= accelerationSpeed * frameTime;
	else
		acceleration += accelerationSpeed * frameTime;

	speed += acceleration * frameTime;
	object->SetTranslationRel(D3DXVECTOR3(0.f, speed * frameTime, 0.f));
}