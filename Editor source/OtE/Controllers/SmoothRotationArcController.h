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

#ifndef __CSMOOTHROTATIONARCCONTROLLER_H__
#define __CSMOOTHROTATIONARCCONTROLLER_H__

#include "..\\Edgewalker\\IGameObject.h"
#include "..\\Objects\\ISimpleObjectControlInterface.h"

#define SMOOTH_ARC_ROTATION_CONTROLLER_ID		1005

/**
 * Controller that animates objects hovering up and down.
 * Parameters:
 *  - runtime 1: current y-rotation speed
 *  - runtime 2: current y-rotation acceleration
 * 
 *  - input 1: rotation acceleration per second
 *  - input 2: max rotation speed
 */
class CSmoothRotationArcController : public IGameObjectController
{
public:
	CSmoothRotationArcController();

	virtual void Update(IGameObject *object, float frameTime);
	virtual void InitObject(IGameObject *object);
	virtual void RegisterCollision(IGameObject *object, IGeometricObject *other) {}
	virtual void OnDeath(IGameObject *object) {}

	virtual bool RequiresInstantiation() {return false;}

	virtual std::string GetControllerName() {return "Smooth arc rotation";}

	virtual IGameObjectController *Clone() 
	{
		CSmoothRotationArcController *c = new CSmoothRotationArcController(*this);
		return (IGameObjectController*)c;
	}
};

#endif // __CSMOOTHROTATIONARCCONTROLLER_H__