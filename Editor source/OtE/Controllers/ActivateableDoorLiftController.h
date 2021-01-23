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

#ifndef __CACTIVATEABLEDOORLIFTCONTROLLER_H__
#define __CACTIVATEABLEDOORLIFTCONTROLLER_H__

#include "..\\Edgewalker\\IGameObject.h"
#include "..\\Objects\\ISimpleObjectControlInterface.h"

#define ACTIVATEABLE_DOOR_LIFT_CONTROLLER_ID		1007

/**
 * Controller that makes a static object act like an activateable
 * lift or door. Inactive by default, once set active will perform
 * the animation once and then turn itself inactive again
 * Parameters:
 *  - runtime 1: 1.f or -1.f, coefficient by which the speed is multiplied,
 *		also states whether the door/lift is up or down (and thus going down or up)
 *  - runtime 2: none
 * 
 *  - input 1: time it takes to open/close or rise/lower
 *  - input 2: speed and y-direction (plus the initial state)
 */
class CActivateableDoorLiftController : public IGameObjectController
{
public:
	CActivateableDoorLiftController();

	virtual void Update(IGameObject *object, float frameTime);
	virtual void InitObject(IGameObject *object);
	virtual void RegisterCollision(IGameObject *object, IGeometricObject *other) {}
	virtual void OnDeath(IGameObject *object) {}

	virtual bool RequiresInstantiation() {return true;}

	virtual std::string GetControllerName() {return "Activateable door / lift";}

	virtual IGameObjectController *Clone() 
	{
		CActivateableDoorLiftController *c = new CActivateableDoorLiftController(*this);
		return (IGameObjectController*)c;
	}
private:
	bool m_bActiveLastFrame;
	IGameObject *m_doorLift;
	float m_counter;
};

#endif // __CACTIVATEABLEDOORLIFTCONTROLLER_H__