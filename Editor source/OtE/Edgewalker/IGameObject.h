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

#ifndef __IGAMEOBJECT_H__
#define __IGAMEOBJECT_H__

#include "Scene\\MeshSceneNode.h"
#include <vector>
#include "IGameObjectController.h"

#define GENERAL_GAME_OBJECT_ID		1

class CObjectManagerSceneNode;

class IGameObject : public CMeshSceneNode  
{
public:
	IGameObject(ISceneNode *parent, long id);
	virtual ~IGameObject();

	virtual void Update(float frameTime);
	//virtual void OnPostRender(float frameTime);

	inline virtual float GetMass() {return m_mass;}
	inline virtual float GetOnePerMass() {return m_onePerMass;}
	virtual void SetMass(float mass) {m_mass = mass; m_onePerMass = 1.f/mass;}

	virtual void SetDragCoefficient(float c) {m_dragCoefficient = c;}
	inline virtual float GetDragCoefficient() {return m_dragCoefficient;}

	virtual void SetController(IGameObjectController *controller) {m_controller = controller;}
	inline virtual IGameObjectController *GetController() {return m_controller;}

	inline virtual D3DXVECTOR3 &GetLastPosition() {return m_lastPosition;}
	inline virtual void SetLastPosition(D3DXVECTOR3 &lpos) {m_lastPosition = lpos;}

	virtual void RegisterCollision(IGeometricObject *object);

	inline virtual bool IsAffectedByGravity() {return m_bGravity;}
	inline virtual bool IsAffectedByDrag() {return m_bDrag;}

	virtual void SetIsAffectedByGravity(bool v) {m_bGravity = v;}
	virtual void SetIsAffectedByDrag(bool v) {m_bDrag = v;}

	inline virtual D3DXVECTOR3 GetDeltaMovement() {return m_lastPosition-m_relativeTranslation;}

	inline virtual D3DXVECTOR3 &GetSpeed() {return m_speed;}
	inline virtual void SetSpeed(D3DXVECTOR3 &v) {m_speed = v;}
	inline virtual void SetSpeedRel(D3DXVECTOR3 &vr) {m_speed += vr;}

	virtual vector<D3DXVECTOR3> &GetForces() {return m_forces;}
	virtual vector<D3DXVECTOR3> &GetImpulses() {return m_impulses;}

	virtual void AddForce(D3DXVECTOR3 force) {m_forces.push_back(force);}
	virtual void AddImpulse(D3DXVECTOR3 impulse) {m_impulses.push_back(impulse);}

	virtual void UpdateBoundingVolumes();
	virtual aabbox3d GetTransformedBox();

	inline void SetRequiresConstantUpdate(bool value) {m_bRequiresConstantUpdate = value;}
	inline bool GetRequiresConstantUpdate() {return m_bRequiresConstantUpdate;}

	inline virtual bool CollidesWithObjects() {return m_bCollidesWithObjects;}
	inline virtual bool CollidesWithExternalGeometric() {return m_bCollidesWithExternalObjects;}
	inline virtual bool RequiresCollisionDetermination() {return m_bRequiresCollisionDetermination;}

	virtual void SetCollidesWithObjects(bool value) {m_bCollidesWithObjects = value;}
	virtual void SetCollidesWithExternalGeometric(bool value) {m_bCollidesWithExternalObjects = value;}
	virtual void SetRequiresCollisionDetermination(bool value) {m_bRequiresCollisionDetermination = value;}

	// Sets the position of the object and reduces speed to zero
	inline virtual void SetBeginningPosition(D3DXVECTOR3 &pos)
	{
		m_relativeTranslation = pos;
		m_lastPosition = pos;
	}

protected:
	// m_onePerMass - optimization, stores 1 / m_mass to avoid unnecessary
	// float divisions
	float m_mass, m_onePerMass, m_dragCoefficient;
	//D3DXVECTOR3 m_speed;
	D3DXVECTOR3 m_lastPosition, m_speed;

	// Forces that affect the object at the current frame
	// NOTE! Gravity and drag are added automatically
	vector<D3DXVECTOR3> m_forces;

	// Impulses are like forces but they affect the object only
	// for a very brief time and are thus handled differently
	vector<D3DXVECTOR3> m_impulses;

	bool m_bGravity, m_bDrag;	// Is the object affected by gravity or drag?
	bool m_bCollidesWithObjects, m_bCollidesWithExternalObjects, m_bRequiresCollisionDetermination;
	bool m_bRequiresConstantUpdate;

	IGameObjectController *m_controller;
};

#endif // #ifndef __IGAMEOBJECT_H__
