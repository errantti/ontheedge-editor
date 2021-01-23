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

#include "IGameObject.h"
#include "PhysicsEngine.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IGameObject::IGameObject(ISceneNode *parent, long id)
: CMeshSceneNode(NULL, parent, ISceneManager::GetManager(), id), m_mass(100.f), m_onePerMass(0.01f),
		  m_bGravity(TRUE), m_dragCoefficient(20.0f), m_controller(NULL), m_bDrag(TRUE),
		  m_lastPosition(D3DXVECTOR3(0.f, 0.f, 0.f)), m_speed(D3DXVECTOR3(0.f, 0.f, 0.f)), 
		  m_bCollidesWithExternalObjects(TRUE), m_bCollidesWithObjects(TRUE), 
		  m_bRequiresCollisionDetermination(TRUE), m_bRequiresConstantUpdate(FALSE)
{
	// Set this object to dynamic by default
	m_bStatic = FALSE;
	m_bvCoefficient = 0.6f;
	UpdateBoundingVolumes();
	m_type = GENERAL_GAME_OBJECT_ID;
}

IGameObject::~IGameObject()
{
}

void IGameObject::Update(float frameTime)
{
	if(m_controller)
		m_controller->Update(this, frameTime);

	CMeshSceneNode::Update(frameTime);

	if(!m_bStatic)
		CPhysicsEngine::GetEngine()->RegisterDynamicObject(this);
}

/*void IGameObject::OnPostRender(float frameTime)
{
	ISceneNode::OnPostRender(frameTime);
	//m_forces.clear();
	//m_impulses.clear();
}*/

void IGameObject::RegisterCollision(IGeometricObject *object)
{
	if(m_controller)
		m_controller->RegisterCollision(this, object);
}

void IGameObject::UpdateBoundingVolumes()
{
	CMeshSceneNode::UpdateBoundingVolumes();
	/*if(m_mesh == NULL)
		return;

	m_bs = m_mesh->GetBoudingSphere();
	m_bs.radius *= m_bvCoefficient;

	m_bbox = aabbox3d(D3DXVECTOR3(m_bs.center.x - m_bs.radius, m_bs.center.y - m_bs.radius, m_bs.center.z - m_bs.radius),
		D3DXVECTOR3(m_bs.center.x + m_bs.radius, m_bs.center.y + m_bs.radius, m_bs.center.z + m_bs.radius));*/
}

aabbox3d IGameObject::GetTransformedBox()
{
	// TODO: Do this with absolute transformation
	return aabbox3d(m_bbox.minCorner+m_relativeTranslation, m_bbox.maxCorner+m_relativeTranslation);
}