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

#ifndef __IGEOMETRICOBJECT_H__
#define __IGEOMETRICOBJECT_H__

#include "Scene\\ISceneNode.h"
#include <vector>

#define DEFAULT_OBJECT_BOUNDING_VOLUME_COEFFICIENT	0.7f
#define STATIC_GEOMETRIC_OBJECT_ID		-1

class IGeometricObject;

enum OBJECT_COLLISION_CHECK {
	OBJECT_COLLISION_CYLINDER=0,
	/* Apply a little trick here, as the sphere is the default
		method of detection, it's natuarelly represented as number
		one. When another object demands different kind of collision
		detection the collision types can simply be multiplied and
		collision detection type deduced from the result as follows:
		0 : cylinder
		1 : sphere
		higher : box
		*/
	OBJECT_COLLISION_SPHERE,		
	OBJECT_COLLISION_BOX,
	OBJECT_COLLISION_FORCEDWORD = 0xFFFFFFFF
};

struct SCollision
{
	// Pointers to colliding objects
	// NOTE! Either one has to be dynamic object always
	IGeometricObject *o1, *o2;

	/**
	 * Collision is static if one of the objects colliding is static
	 * This is mostly for physics engine optimizations
	 */
	bool bStatic;

	// Direction and depth of the collision
	// The vector points from first object to the second one
	D3DXVECTOR3 v;
	// How deep the objects collided
	// NOTE! Included in colVec
	//float depth;
};

class IGeometricObject : public ISceneNode
{
public:
	IGeometricObject(ISceneNode *parent, ISceneManager *manager, long id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f))
		: ISceneNode(parent, manager, id, position, rotation, scale),
		m_bvCoefficient(1.f), m_bStatic(TRUE), m_type(STATIC_GEOMETRIC_OBJECT_ID),
		m_bbox(aabbox3d(0.f))
	{
		m_bAutomaticCulling = FALSE;
	}
	virtual ~IGeometricObject() {}

	virtual bsphere3d &GetBoundingSphere() {return m_bs;}
	virtual aabbox3d &GetBoundingBox() {return m_bbox;}
	virtual bsphere3d GetTransformedSphere()
	{
		bsphere3d sphere(m_bs);
		//CMath::MatrixVectorMultiply(m_absoluteTransformation, sphere.center);
		sphere.center = m_relativeTranslation;
		return sphere;
	}

	virtual aabbox3d GetTransformedBox()
	{
		aabbox3d box(m_bbox);
		box.minCorner += m_relativeTranslation;
		box.maxCorner += m_relativeTranslation;
		return box;
	}

	virtual void Move(D3DXVECTOR3 rel) {m_relativeTranslation += rel;}

	inline virtual D3DXVECTOR3 GetDeltaMovement() {return D3DXVECTOR3(0.f, 0.f, 0.f);}

	inline virtual float GetMass() {return 0.f;}
	inline virtual float GetOnePerMass() {return 0.f;}

	virtual D3DXVECTOR3 &GetSpeed() {return *((D3DXVECTOR3*)0);}
	inline virtual void SetSpeed(D3DXVECTOR3 &v) {}
	inline virtual void SetSpeedRel(D3DXVECTOR3 &vr) {}

	virtual bool IsStatic() {return m_bStatic;}
	virtual void SetStatic(bool value) {m_bStatic = value;}

	inline virtual float GetBoundingVolumeCoefficient() {return m_bvCoefficient;}
	virtual void SetBoundingVolumeCoefficient(float c) {m_bvCoefficient = c;}

	inline virtual bool CollidesWithObjects() {return TRUE;}
	inline virtual bool CollidesWithExternalGeometric() {return TRUE;}
	inline virtual bool RequiresCollisionDetermination() {return TRUE;}

	virtual void SetType(int t) {m_type = t;}
	inline virtual int GetType() {return m_type;}

	inline OBJECT_COLLISION_CHECK GetCollisionType() {return m_collisionType;}
	inline void SetCollisionType(OBJECT_COLLISION_CHECK type) {m_collisionType = type;}

	// CheckForCollision
	// Checks if the given object collides with this one. If it does the
	// function adds information about the collision to the given
	// vector array /param collisions
	virtual void CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions) = 0;
	virtual void UpdateBoundingVolumes() = 0;

	virtual void RegisterCollision(IGeometricObject *object) = 0;

protected:
	// bounding volumes
	bsphere3d m_bs;
	aabbox3d m_bbox;

	bool m_bStatic;		// Is the object static? (doesn't move or break)
	float m_bvCoefficient;	// The coefficient by which the mesh bounding volumes are multiplied

	int m_type;

	OBJECT_COLLISION_CHECK m_collisionType;
};

#endif // #ifndef __IGEOMETRICOBJECT_H__