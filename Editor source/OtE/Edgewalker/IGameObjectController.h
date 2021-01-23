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

#ifndef __IGAMEOBJECTCONTROLLER_H__
#define __IGAMEOBJECTCONTROLLER_H__

#include "IGeometricObject.h"
class IGameObject;

class IGameObjectController
{
public:
	IGameObjectController() : m_bActive(true) {}

	// Update
	/**
	 * This function is called when the object is updated (before moving and collision detection)
	 * /param object - the object which this controller is attached to
	 */
	virtual void Update(IGameObject *object, float frameTime) = 0;

	// RegisterCollision
	/**
	 * This function is called when the object to which this controller
	 * is attached collides with another object
	 * /param object - the object which this controller is attached to
	 * /param other - the object against which the controlled object collided
	 */
	virtual void RegisterCollision(IGameObject *object, IGeometricObject *other) = 0;

	/**
	 * Callback method which is called when object is destroyed
	 */
	virtual void OnDeath(IGameObject *object) {}

	/**
	 * Initializes the variables of the given object so it's ready to
	 * be used with this controller
	 */
	virtual void InitObject(IGameObject *object) {}

	bool IsActive() {return m_bActive;}
	void SetIsActive(bool active) {m_bActive = active;}

	/**
	 * Determines whether the controller needs to be instantiated for
	 * every object it controls. If false then the controller can be
	 * used to control any number of game objects
	 */
	virtual bool RequiresInstantiation() = 0;

	virtual std::string GetControllerName() {return "";}

	inline long GetControllerID() {return m_controllerID;}
	inline void SetControllerID(long id) {m_controllerID = id;}

	virtual IGameObjectController *Clone() = 0;

protected:
	long m_controllerID;
	bool m_bActive;
};

#endif // #ifndef __IGAMEOBJECTCONTROLLER_H__