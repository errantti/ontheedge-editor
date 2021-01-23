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

#include "MeshSceneNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeshSceneNode::CMeshSceneNode(CMesh *mesh, ISceneNode *parent, ISceneManager *manager, DWORD id,
		D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 scale)
	: IGeometricObject(parent, manager, id, position, rotation, scale), m_mesh(mesh), m_scalingCoefficient(1.f)
{
	UpdateBoundingVolumes();
}

CMeshSceneNode::~CMeshSceneNode()
{
}

void CMeshSceneNode::OnPreRender(bool isShadowPass)
{
	if(m_bVisible && m_mesh)
	{
		m_manager->RegisterNodeForRendering(this, SNG_DEFAULT);
	}
	ISceneNode::OnPreRender(isShadowPass);
}

void CMeshSceneNode::Render(bool isShadowPass)
{
	if(m_mesh == NULL)
		return;

	// Render the mesh
	CDirectX9 *dx = GetDXApp();
	if(dx == NULL)
		return;

	dx->SetTransform(D3DTS_WORLD, &m_absoluteTransformation);
	if (isShadowPass)
		m_mesh->RenderWithoutTexture();
	else
		m_mesh->Render();
}

void CMeshSceneNode::Update(float frameTime)
{
	ISceneNode::Update(frameTime);
}

void CMeshSceneNode::SetMesh(CMesh *mesh)
{
	if(!mesh)
		return;

	m_mesh = mesh;

	UpdateBoundingVolumes();
}

void CMeshSceneNode::UpdateBoundingVolumes()
{
	if(m_mesh == NULL)
		return;

	m_bbox = m_mesh->GetBoundingBox();
	m_bs = m_mesh->GetBoudingSphere();

	float finalCoefficient = m_bvCoefficient * m_scalingCoefficient;

	m_bbox.minCorner = finalCoefficient * m_bbox.minCorner;
	m_bbox.maxCorner = finalCoefficient * m_bbox.maxCorner;
	m_bs.radius = finalCoefficient * m_bs.radius;
}

void CMeshSceneNode::CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions)
{
	if(object == NULL || object == this)
		return;

	if(m_bStatic && object->IsStatic())
		return;

	SCollision collision;

	if(m_collisionType == OBJECT_COLLISION_BOX || object->GetCollisionType() == OBJECT_COLLISION_BOX)
	{
		aabbox3d mybox(GetTransformedBox());
		aabbox3d obbox(object->GetTransformedBox());

		if(!mybox.IntersectsWithBox(obbox))
			return;

		if(object->CollidesWithObjects() == TRUE && CollidesWithObjects() == TRUE)
		{
			GetSimpleBoxCollisionVector(object, collision.v, mybox, obbox);
			collision.o1 = this;
			collision.o2 = object;
			collisions.push_back(collision);
		}

		// Add collision direction and depth to the vector
		RegisterCollision(object);
		object->RegisterCollision(this);
	}
	else
	{
		// Check collision using simple bounding sphere test
		collision.v = object->GetAbsolutePosition() - GetTranslation();
		float r = m_bs.radius + object->GetBoundingSphere().radius;
		float vp2 = collision.v.x*collision.v.x + collision.v.y*collision.v.y + collision.v.z*collision.v.z;
		if(vp2<r*r)
		{
			// Collision
			//coefficient = sqrtfast(r/vp2) -1.f; // = (sqrt(r) - sqrt(vp2)) / sqrt(vp2)
			//collisionvector = coefficient * (c2-c1); // c = center
			if(object->CollidesWithObjects() == TRUE && CollidesWithObjects() == TRUE)
			{
				collision.v *= r*sqrtfast(1.f/vp2) -1.f;
				collision.o1 = this;
				collision.o2 = object;
				collisions.push_back(collision);
			}

			// Add collision direction and depth to the vector
			RegisterCollision(object);
			object->RegisterCollision(this);
		}
	}
}

void CMeshSceneNode::GetSimpleBoxCollisionVector(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &myBox, aabbox3d &obBox)
{
	// Calculate the area that the aabboxes overlap and deduce the
	// direction of collision by finding minimum overlap axis
	float l,temp;
	int axis = 0;
	D3DXVECTOR3 &omi = obBox.minCorner, &oma = obBox.maxCorner, &mi = myBox.minCorner, &ma = myBox.maxCorner;
	collisionVec = D3DXVECTOR3(0.f,0.f,0.f);
	
	if(oma.x > ma.x) {
		if(omi.x < mi.x)
			l = ma.x - mi.x;
		else
			l = ma.x - omi.x;
	} else {
		if(omi.x < mi.x)
			l = oma.x - mi.x;
		else
			l = oma.x - omi.x;
	}

	if(oma.y > ma.y) {
		if(omi.y < mi.y)
			temp = ma.y - mi.y;
		else
			temp = ma.y - omi.y;
	} else {
		if(omi.y < mi.y)
			temp = oma.y - mi.y;
		else
			temp = oma.y - omi.y;
	}
	if(temp < l)
	{
		l = temp;
		axis = 1;
	}

	if(oma.z > ma.z) {
		if(omi.z < mi.z)
			temp = ma.z - mi.z;
		else
			temp = ma.z - omi.z;
	} else {
		if(omi.z < mi.z)
			temp = oma.z - mi.z;
		else
			temp = oma.z - omi.z;
	}
	if(temp < l)
	{
		l = temp;
		axis = 2;
	}

	// Now that we know the collision depth and axis on which it occurred
	// get the collision direction, check whether that component was
	// previously occupied and if not, update the collision
	if(axis == 0)
	{
		if(omi.x + oma.x < mi.x + ma.x)
			collisionVec.x = -l;
		else
			collisionVec.x = l;
		return;
	}
	else if(axis == 1)
	{
		if(omi.y + oma.y < mi.y + ma.y)
			collisionVec.y = -l;
		else
			collisionVec.y = l;
		return;
	}
	else if(axis == 2)
	{
		if(omi.z + oma.z < mi.z + ma.z)
			collisionVec.z = -l;
		else
			collisionVec.z = l;
		return;
	}
}

void CMeshSceneNode::SetScalingCoefficient(float coef)
{
	if(m_scalingCoefficient == coef)
		return;

	if (m_scalingCoefficient != 1.f)
	{
		// If the scaling coefficient had been previously set to some other value
		// rever that change before applying the new coefficient
		float inverse = 1.f / coef;

		// This also maintains the separate m_bvCoefficient which only affects
		// the bounding volumes
		m_bbox.minCorner *= inverse;
		m_bbox.maxCorner *= inverse;
		m_bs.radius *= inverse;
	}

	m_bbox.minCorner *= coef;
	m_bbox.maxCorner *= coef;
	m_bs.radius *= coef;
	m_scalingCoefficient = coef;

	m_relativeScale.x = coef;
	m_relativeScale.y = coef;
	m_relativeScale.z = coef;
}