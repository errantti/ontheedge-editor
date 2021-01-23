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

#ifndef __CPHYSICSENGINE_H__
#define __CPHYSICSENGINE_H__

#include "IGameObject.h"

#define DEFAULT_GRAVITY		23.f
#define DRAG_THRESHOLD		2.f
#define OBJECT_KINETIC_COEFFICIENT		0.04f

class CPhysicsEngine
{
public:
	CPhysicsEngine();
	virtual ~CPhysicsEngine();

	void RegisterDynamicObject(IGameObject *object);
	void RegisterCollision(SCollision c);
	void RegisterCollisions(vector<SCollision> &collisions) {m_collisions = collisions;}

	void AccumulateForces();
	void VerletMovement(float frameTime);
	void Movement(float frameTime);

	void HandleCollisions(float frameTime);

	void EndFrame();

	void SetGravity(float gravity) {m_gravity = gravity;}
	void SetDensity(float density) {m_density = density;}
	void SetFriction(float friction) {m_friction = friction;}

	static CPhysicsEngine *GetEngine() {return m_engineObject;}

	inline void Reset() {m_objects.clear(); m_collisions.clear();}

protected:
	static CPhysicsEngine *m_engineObject;

	vector<IGameObject*> m_objects;
	vector<SCollision> m_collisions;

	// NOTE! m_gravity = gravity acceleration
	// m_density = environmental density (fluid/air density)
	float m_gravity, m_density, m_friction;
};

#endif // #ifndef __CPHYSICSENGINE_H__
