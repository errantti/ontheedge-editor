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

#include "PhysicsEngine.h"

CPhysicsEngine *CPhysicsEngine::m_engineObject = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPhysicsEngine::CPhysicsEngine()
: m_gravity(DEFAULT_GRAVITY), m_friction(0.01f), m_density(0.08f)
{
	m_engineObject = this;
}

CPhysicsEngine::~CPhysicsEngine()
{
}

void CPhysicsEngine::RegisterDynamicObject(IGameObject *object)
{
	if(object == NULL)
		return;

	m_objects.push_back(object);
}

void CPhysicsEngine::EndFrame()
{
	m_objects.clear();
}

/**
 * Move objects using verlet integration step
 **/
void CPhysicsEngine::VerletMovement(float frameTime)
{
	int i;

	D3DXVECTOR3 temp, newPos, speed;
	vector<D3DXVECTOR3> *forces, *impulses;

	float timeStep = frameTime*frameTime, v2;
	D3DXVECTOR3 gravity(0.f, -m_gravity * timeStep, 0.f);

	// Loop through all objects and move them using verlet integration
	// If the object is affected by impulses they are applied here also
	for(i=0;i<m_objects.size();i++)
	{
		newPos = m_objects[i]->GetTranslation();
		D3DXVECTOR3& old = m_objects[i]->GetLastPosition();
		temp = newPos;

		speed = newPos-old;

		forces = &m_objects[i]->GetForces();
		impulses = &m_objects[i]->GetImpulses();

		// Drag equation (approximation):
		// D = C * p * v^2
		// where D is the drag force, C drag coefficient (stored inpidendently
		// for each dynamic object) and v the speed of the object
		if(m_objects[i]->IsAffectedByDrag())
		{
			v2 = speed.x*speed.x + speed.y*speed.y + speed.z*speed.z;
			if(v2>DRAG_THRESHOLD)
			{
				(*forces)[0] -= frameTime * sqrtfast(v2) * m_objects[i]->GetDragCoefficient() * m_density * speed;
			}
		}

		if(impulses->size()>0)
			(*forces)[0] += (*impulses)[0]*frameTime;
		if(forces->size() == 0)
			newPos += speed;
		else
			newPos += speed + (*forces)[0] * timeStep * m_objects[i]->GetOnePerMass();

		if(m_objects[i]->IsAffectedByGravity())
			newPos += gravity;
		old = temp;

		m_objects[i]->SetTranslation(newPos);
		m_objects[i]->SetLastPosition(old);

		forces->clear();
		impulses->clear();
	}
}

/**
 * Movement using basic Newtonian mechanics
 **/
void CPhysicsEngine::Movement(float frameTime)
{
	int i;

	D3DXVECTOR3 temp, newPos, speed;
	vector<D3DXVECTOR3> *forces, *impulses;

	float v2;
	D3DXVECTOR3 gravity(0.f, -m_gravity * frameTime, 0.f);

	// Loop through all objects and move them using verlet integration
	// If the object is affected by impulses they are applied here also
	for(i=0;i<m_objects.size();i++)
	{
		newPos = m_objects[i]->GetTranslation();

		speed = m_objects[i]->GetSpeed();

		forces = &m_objects[i]->GetForces();
		impulses = &m_objects[i]->GetImpulses();

		// Drag equation (approximation):
		// D = C * p * v^2
		// where D is the drag force, C drag coefficient (stored inpidendently
		// for each dynamic object) and v the speed of the object
		if(m_objects[i]->IsAffectedByDrag())
		{
			v2 = speed.x*speed.x + speed.y*speed.y + speed.z*speed.z;
			 //(*forces)[0] -= (frameTime * sqrtfast(v2) * m_objects[i]->GetDragCoefficient() * m_density) * speed;
			(*forces)[0] -= (sqrtfast(v2) * m_objects[i]->GetDragCoefficient() * m_density) * speed;
		}

		if(impulses->size()>0)
			speed += (*impulses)[0]*m_objects[i]->GetOnePerMass();
		if(forces->size() != 0)
			speed += (*forces)[0] * frameTime * m_objects[i]->GetOnePerMass();

		if(m_objects[i]->IsAffectedByGravity())
			speed += gravity;

		newPos += speed*frameTime;

		m_objects[i]->SetSpeed(speed);
		m_objects[i]->SetLastPosition(m_objects[i]->GetTranslation());
		m_objects[i]->SetTranslation(newPos);

		forces->clear();
		impulses->clear();
	}
}

/**
 * This function accumulates all forces affecting the objects at
 * the current frame and stores the result in the forces list
 * cell 0
 **/
void CPhysicsEngine::AccumulateForces()
{
	int i,f;
	vector<D3DXVECTOR3> *forces;
	D3DXVECTOR3 result(0.f,0.f,0.f);

	// Loop through all objects and accumulate the forces affecting them
	for(i=0;i<m_objects.size();i++)
	{
		forces = &m_objects[i]->GetForces();

		// Accumulate forces
		if(forces->size()!=0)
		{
			result = (*forces)[0];
			for(f=1;f<forces->size();f++)
				result+=(*forces)[f];

			(*forces)[0] = result;
		}
		else
			forces->push_back(D3DXVECTOR3(0.f, 0.f, 0.f));

		// Accumulate impulses
		forces = &m_objects[i]->GetImpulses();
		if(forces->size() != 0)
			for(f=1;f<forces->size()-1;f++)
				(*forces)[0]+=(*forces)[f];
	}
}

/**
 * Collisions are handled with simple projection
 * Collision are thought as constraints with only
 * minimum distances
 **/
void CPhysicsEngine::HandleCollisions(float frameTime)
{
	float t = 0.08f / frameTime * OBJECT_KINETIC_COEFFICIENT;

	for(int i=0;i<m_collisions.size();i++)
	{
		if(m_collisions[i].o1->IsStatic())
		{
			if(m_collisions[i].o2->RequiresCollisionDetermination() == FALSE ||
				m_collisions[i].o1->RequiresCollisionDetermination() == FALSE)
				continue;
			// Project only the dynamic object
			m_collisions[i].o2->Move(m_collisions[i].v);

			((IGameObject*)m_collisions[i].o2)->SetSpeedRel(m_collisions[i].v * t);
		}
		else if(m_collisions[i].o2->IsStatic())
		{
			if(m_collisions[i].o2->RequiresCollisionDetermination() == FALSE ||
				m_collisions[i].o1->RequiresCollisionDetermination() == FALSE)
				continue;
			// Project only the dynamic object
			m_collisions[i].o1->Move(-m_collisions[i].v);
			((IGameObject*)m_collisions[i].o1)->SetSpeedRel(-m_collisions[i].v * t);
		}
		else
		{
			if(m_collisions[i].o1->RequiresCollisionDetermination() == FALSE ||
				m_collisions[i].o2->RequiresCollisionDetermination() == FALSE)
				continue;
			// Handle collision of two dynamic objects
			// TODO: update the function to take mass and velocity into account

			// temporary measure: project both objects equally
			m_collisions[i].o1->Move(-0.5f * m_collisions[i].v);
			m_collisions[i].o2->Move(0.5f * m_collisions[i].v);

			// Modify the speeds
			((IGameObject*)m_collisions[i].o1)->SetSpeedRel(-m_collisions[i].v * t);
			((IGameObject*)m_collisions[i].o2)->SetSpeedRel(m_collisions[i].v * t);
		}
	}
}

void CPhysicsEngine::RegisterCollision(SCollision c)
{
	m_collisions.push_back(c);
}