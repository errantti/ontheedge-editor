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

#include "ObjectManagerSceneNode.h"
#include "..\\PhysicsEngine.h"
#include <d3dx9math.h>
#include "CameraSceneNode.h"


/*** 
 * 
 ***/

void CObjectManagerSceneNode::SSectorQuadTreeNode::OnPreRender(ISceneManager *m, CFrustum &f, bool preCulled, bool isShadowPass)
{
	if(leaf != NULL)
		leaf->OnPreRender(m, isShadowPass);
	else
	{
		if(preCulled)
		{
			for(int i=0;i<4;++i)
				if(nodes[i] != NULL)
					nodes[i]->OnPreRender(m, f, true, isShadowPass);
		}
		else
		{
			int result;
			for(int i=0;i<4;++i)
				if(nodes[i] != NULL)
				{
					result = f.DoesIntersectWithAABB(nodes[i]->bbox);
					if(result == INSIDE)
						nodes[i]->OnPreRender(m, f, true, isShadowPass);
					else if(result == INTERSECTING)
						nodes[i]->OnPreRender(m, f, false, isShadowPass);
				}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectManagerSceneNode::CObjectManagerSceneNode(ISceneNode *parent, ISceneManager *manager,
		long id, DWORD width, DWORD height, int sectorSize)
		:	ISceneNode(parent, manager, id), m_width(width), m_height(height),
			m_sectorSize(sectorSize), m_yMaxLimit(400.f), m_yMinLimit(-50.f),
			m_bUpdating(FALSE), m_bPhysicsEnabled(TRUE)
{
	Create(width, height, sectorSize);
}

void CObjectManagerSceneNode::Reset()
{
	RemoveAll();
	m_sectors.clear();
	m_rootSectorNode.RemoveChildren();
}

void CObjectManagerSceneNode::RemoveAllObjects()
{
	RemoveAll();
	vector<CSector>::iterator it = m_sectors.begin();
	for(; it != m_sectors.end(); ++it)
		it->RemoveAllObjects();
}

void CObjectManagerSceneNode::Create(DWORD width, DWORD height, int sectorSize)
{
	RemoveAll();
	m_sectors.clear();

	// Emulate quadtree: squarical map
	if(width > height)
		height = width;
	if(height > width)
		width = height;

	m_width = width;
	m_height = height;

	if(m_sectorSize < 1)
		m_sectorSize = 10;
	if(m_width == 0)
		m_width = m_sectorSize;
	if(m_height == 0)
		m_height = m_sectorSize;

	m_onePerSectorSize = 1.f / (float)m_sectorSize;

	m_sectorsX = (int)(m_width / m_sectorSize);
	if(m_width % m_sectorSize != 0)
	{
		// Make sure m_sectorSize is factor of m_width
		m_width += m_sectorSize - m_width % m_sectorSize;
		++m_sectorsX;
	}

	m_sectorsY = (int)(m_height / m_sectorSize);
	if(m_height % m_sectorSize != 0)
	{
		// Make sure m_sectorSize is factor of m_height
		m_height += m_sectorSize - m_height % m_sectorSize;
		++m_sectorsY;
	}

	// Create the sectors
	// The +1 is there for the 'outer sector' (outside the object manager
	// grid)
	m_sectors.resize(m_sectorsX * m_sectorsY + 1);

	int x,y,offset,temp;
	rect<long> r;

	for(y=0;y<m_sectorsY;y++)
	{
		offset=y*m_sectorsX;
		r.top = y*m_sectorSize;
		r.bottom = (y+1)*m_sectorSize;

		for(x=0;x<m_sectorsX;x++)
		{
			temp = offset+x;
			r.left = x*m_sectorSize;
			r.right = (x+1)*m_sectorSize;

			m_sectors[temp].SetObjectManager(this);
			m_sectors[temp].Create(r, m_yMinLimit, m_yMaxLimit);
		}
	}
	r.left = -200;
	r.right = m_width + 200;
	r.top = -200;
	r.bottom = m_height + 200;
	m_sectors[m_sectors.size()-1].Create(r, m_yMinLimit - 100.f, m_yMaxLimit + 100.f, true);
	m_sectors[m_sectors.size()-1].SetObjectManager(this);

	// Build static sector tree for quick hierarchial OnPreRender culling
	m_rootSectorNode.RemoveChildren();
	m_rootSectorNode.level = 0;
	BuildSectorTree(rect<int>(0, m_sectorsX, 0, m_sectorsY), &m_rootSectorNode);
}

CObjectManagerSceneNode::~CObjectManagerSceneNode()
{
}

// This should never be called
void CObjectManagerSceneNode::Render(bool isShadowPass)
{
}

/**
 * Determine the visible objects using quadtree partitioning
 * and let them register themselves to the scene manager
 * VERY BIG TODO! Really, what the hell was I thinking? O_o
 **/
void CObjectManagerSceneNode::OnPreRender(bool isShadowPass)
{
	if(m_sectors.size() == 0)
		return;

	//RegisterVisibleObjects(rect<int>(0, m_sectorsX, 0, m_sectorsY));
	CFrustum &f = m_manager->GetActiveCamera()->GetFrustum();
	m_rootSectorNode.OnPreRender(m_manager, f, false, isShadowPass);
	m_sectors[m_sectors.size()-1].OnPreRender(m_manager, isShadowPass);
}

void CObjectManagerSceneNode::RegisterVisibleObjects(rect<int> area, bool isShadowPass)
{
	/*int width = area.right - area.left, height = area.bottom - area.top;
	if(width < 2 || height < 2)
	{
		int offset;
		for(int z=area.top; z<area.bottom; ++z)
		{
			offset = z*m_sectorsX;
			for(int x=area.left; x<area.right; ++x)
				m_sectors[offset+x].OnPreRender(m_manager);
		}
		return;
	}

	
	aabbox3d box(D3DXVECTOR3(0.f, m_yMinLimit, 0.f), D3DXVECTOR3(100.f, m_yMaxLimit, 100.f));

	float hwidth = (float)width * 0.5f, hheight = (float)height * 0.5f;

	float minx = (float)area.left, minz = (float)area.top, maxx = (float)area.right, maxz = (float)area.bottom;
	float sectorSize = (float)m_sectorSize;

	//int offset;
	for(int z=0; z<2; ++z)
	{
		//offset = z*m_sectorsX;
		for(int x=0; x<2; ++x)
		{
		}
	}

	for(float z=minz; z<=maxz; z+=hheight)
	{
		int zi = (int)z;
		int zi2 = (int)(z + hheight);
		box.minCorner.z = (float)zi * sectorSize;
		box.maxCorner.z = (float)zi2 * sectorSize;
		for(float x=minx; x<=maxx; x+=hwidth)
		{
			int xi = (int)x;
			int xi2 = (int)(x + hwidth);
			box.minCorner.x = (float)xi * sectorSize;
			box.maxCorner.x = (float)xi2 * sectorSize;
		}
	}

	unsigned int i;
	for(i=0;i<m_sectors.size();i++)
		m_sectors[i].OnPreRender(m_manager);*/
	m_sectors[m_sectors.size()-1].OnPreRender(m_manager, isShadowPass);
}

void CObjectManagerSceneNode::BuildSectorTree(rect<int> area, SSectorQuadTreeNode *parent)
{
	float minx = (float)area.left, minz = (float)area.top, maxx = (float)area.right, maxz = (float)area.bottom;
	float sectorSize = (float)m_sectorSize;

	parent->bbox.minCorner = D3DXVECTOR3(minx*sectorSize, m_yMinLimit, minz*sectorSize);
	parent->bbox.maxCorner = D3DXVECTOR3(maxx*sectorSize, m_yMaxLimit, maxz*sectorSize);

	int width = area.right - area.left, height = area.bottom - area.top;
	if(width < 2 && height < 2)
	{
		parent->leaf = &m_sectors[area.top * m_sectorsX + area.left];
		return;
	}

	float hwidth = (float)width * 0.5f, hheight = (float)height * 0.5f;
	int i=0;

	for(float z=minz; z<maxz; z+=hheight)
	{
		int zi = (int)z;
		int zi2 = (int)(z + hheight);
		//box.minCorner.z = (float)zi * sectorSize;
		//box.maxCorner.z = (float)zi2 * sectorSize;
		for(float x=minx; x<maxx; x+=hwidth)
		{
			int xi = (int)x;
			int xi2 = (int)(x + hwidth);
			//box.minCorner.x = (float)xi * sectorSize;
			//box.maxCorner.x = (float)xi2 * sectorSize;
			parent->nodes[i] = new SSectorQuadTreeNode();
			parent->nodes[i]->level = parent->level + 1;
			BuildSectorTree(rect<int>(xi, xi2, zi, zi2), parent->nodes[i]);
			++i;
		}
	}
}

void CObjectManagerSceneNode::Update(float frameTime)
{
	m_bUpdating = TRUE;

	// Let objects update themselves
	// The dynamic objects have to register themselves to
	// the physics engine
	ISceneNode::Update(frameTime);

	//INT64 f = GetDXApp()->GetTimer()->GetTicks();

	CPhysicsEngine *pse = CPhysicsEngine::GetEngine();

	if(m_bPhysicsEnabled)
	{
		// Then accumulate forces affecting the objects
		pse->AccumulateForces();
		//f = GetDXApp()->GetTimer()->GetTicks()-f;

		// ...and move them...
		pse->Movement(frameTime);

		// ...do collision detection...
		DoCollisionDetection();

		// ...and determine collision response...
		pse->HandleCollisions(frameTime);
	}

	// ...and clear the objects from physics engine
	// Execute this function whether physics engine is enabled becouse collision data
	// will be added to physics engine vector anyway
	pse->EndFrame();

	RemoveMarkedObjects();

	// Update the sectors
	UpdateSectors();

	AddMarkedObjects();

	m_bUpdating = FALSE;
}

IGameObject *CObjectManagerSceneNode::AddObject(IGameObject *object)
{
	if(object == NULL)
		return NULL;

	static D3DXVECTOR3 pos;
	pos = object->GetTranslation();
	D3DXVECTOR3 m=pos;
	int x,y;

	if(pos.y<m_yMinLimit)
	{
		object->SetSpeed(D3DXVECTOR3(0.f,0.f,0.f));
		object->SetTranslation(D3DXVECTOR3(pos.x, m_yMinLimit+1.f, pos.z));
	}
	if(pos.y>m_yMaxLimit)
	{
		object->SetSpeed(D3DXVECTOR3(0.f,0.f,0.f));
		object->SetTranslation(D3DXVECTOR3(pos.x, m_yMaxLimit-1.f, pos.z));
	}

	if(pos.x<0.f || pos.z < 0.f || pos.x > m_width || pos.z > m_height)
	{
		// add the object to the sector that contains areas outside
		// the actual sector grid
		m_sectors[m_sectors.size()-1].AddObject(object);
	}
	else
	{
		x=(int)((float)pos.x*m_onePerSectorSize);
		y=(int)((float)pos.z*m_onePerSectorSize);

		m_sectors[y*m_sectorsX+x].AddObject(object);
	}

	return object;
}

void CObjectManagerSceneNode::AddObjectToInsertionList(IGameObject *object)
{
	vector<IGameObject*>::iterator it = m_addList.begin();
	for(;it!=m_addList.end();++it)
		if((*it) == object)
			return;
	m_addList.push_back(object);
}

void CObjectManagerSceneNode::AddMarkedObjects()
{
	vector<IGameObject*>::iterator it = m_addList.begin();
	for(;it!=m_addList.end();++it)
		AddObject(*it);
	m_addList.clear();
}

IGameObject *CObjectManagerSceneNode::AddNewObject(IGameObject *object)
{
	AddChild(object);

	return AddObject(object);
}

bool CObjectManagerSceneNode::RemoveObject(IGameObject *object)
{
	if(object == NULL)
		return false;

	if(!RemoveChild(object))
		return false;

	//if(!RemoveObjectFromSector(object))
		ObliterateObjectFromSectors(object);

	return true;
}

bool CObjectManagerSceneNode::RemoveObject(long id)
{
	list<ISceneNode*>::iterator it = m_children.begin();
	for(;it != m_children.end();it++)
		if((*it)->GetID() == id)
			return RemoveObject((IGameObject*)(*it));

	return false;
}

void CObjectManagerSceneNode::AddObjectToRemoveList(IGameObject *object)
{
	if(m_bUpdating)
		m_removeList.push_back(object);
	else
	{
		RemoveObject(object);
	}
}

void CObjectManagerSceneNode::RemoveMarkedObjects()
{
	vector<IGameObject*>::iterator it = m_removeList.begin();
	for(;it != m_removeList.end();it++)
	{
		if((*it) == NULL)
			continue;
		RemoveObject((*it));
	}
	m_removeList.clear();
}

bool CObjectManagerSceneNode::RemoveObjectFromSector(IGameObject *object)
{
	if(object == NULL)
		return false;

	static D3DXVECTOR3 pos;
	pos = object->GetTranslation();
	int x,y;

	if(pos.x<0 || pos.z < 0 || pos.x > m_width || pos.z > m_height)
	{
		// add the object to the sector that contains areas outside
		// the actual sector grid
		return m_sectors[m_sectors.size()-1].RemoveObject(object);
	}
	else
	{
		x=(int)(pos.x*m_onePerSectorSize);
		y=(int)(pos.z*m_onePerSectorSize);

		return m_sectors[y*m_sectorsX+x].RemoveObject(object);
	}
}

// If RemoveObjectFromSector fails and it is known that the object pointer
// resides in a sector due to a bug, this function will kill the pointer
bool CObjectManagerSceneNode::ObliterateObjectFromSectors(IGameObject *object)
{
	vector<CSector>::iterator it = m_sectors.begin();
	for(;it != m_sectors.end();it++)
		if(it->RemoveObject(object))
			return TRUE;

	return FALSE;
}

IGameObject *CObjectManagerSceneNode::GetObject(long id)
{
	if(id < 0)
		return NULL;
	list<ISceneNode*>::iterator it = m_children.begin();
	for(;it != m_children.end();it++)
		if((*it)->GetID() == id)
			return (IGameObject*)(*it);

	return NULL;
}

IGameObject *CObjectManagerSceneNode::GetObjectFromRaySlow(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir)
{
	list<ISceneNode*>::iterator it = m_children.begin();
	IGameObject *go = NULL;
	bsphere3d bs;

	//D3DXVECTOR3 ndir = 
	//float l = 1.f / sqrtfast(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
	//D3DXVECTOR3 ndir(dir.x * l, dir.y * l, dir.z * l);
	D3DXVec3Normalize(&dir, &dir);

	for(; it != m_children.end(); it++)
	{
		go = (IGameObject*)(*it);
		bs = go->GetBoundingSphere();
		D3DXVECTOR3 f = bs.center+go->GetTranslation();
		if(CMath::DoesRayIntersectSphere(pos, dir, f, bs.radius * 1.4f))
			return (IGameObject*)(*it);
	}

	return NULL;
}

void CObjectManagerSceneNode::DoCollisionDetection()
{
	unsigned int i;
	CPhysicsEngine *pse = CPhysicsEngine::GetEngine();

	m_collisions.clear();

	// Do collision detection with external colliders (such as terrain)
	// using geometric object interface
	list<ISceneNode*>::iterator it;
	for(i=0;i<m_colliders.size();i++)
	{
		it = m_children.begin();
		for(;it!=m_children.end();it++)
		{
			if(((IGeometricObject*)(*it))->CollidesWithExternalGeometric() == FALSE)
				continue;
			m_colliders[i]->CheckForCollision((IGeometricObject*)(*it), m_collisions);
		}
	}

	// Do collision detection among the managed objects using quadtree
	// partitioning

	unsigned int size = m_sectors.size();
	if(size == 0)
		return;

	// First loop through all sectors and do collision detection inside each sector
	for(i=0;i<size;i++)
	{
		m_sectors[i].CheckCollisions(NULL, m_collisions);
	}

	--size;

	// then loop through all the borders and check the collisions against the 'other areas'
	// sector
	// x-aligned borders
	for(i=0;i<m_sectorsX;i++)
		m_sectors[i].CheckCollisions(&m_sectors[size], m_collisions);
	for(i=m_sectorsX*(m_sectorsY-1); i<size; i++)
		m_sectors[i].CheckCollisions(&m_sectors[size], m_collisions);

	// and z-aligned borders
	for(i=m_sectorsX; i<size; i+=m_sectorsX)
		m_sectors[i].CheckCollisions(&m_sectors[size], m_collisions);
	int temp = size-m_sectorsX-1, temp2;
	for(i=m_sectorsX+m_sectorsX-1; i<temp; i+=m_sectorsX)
		m_sectors[i].CheckCollisions(&m_sectors[size], m_collisions);

	// and finally loop through each object and check collision against
	// objects in right, lower and rightlower neightbor sectors
	temp = size-m_sectorsX;
	for(i=0; i<temp; i++)
	{
		temp2 = i+m_sectorsX;
		if((i+1)%m_sectorsX != 0)
		{
			m_sectors[i].CheckCollisions(&m_sectors[i+1], m_collisions);
			m_sectors[i].CheckCollisions(&m_sectors[temp2+1], m_collisions);
		}
		m_sectors[i].CheckCollisions(&m_sectors[temp2], m_collisions);
	}

	// Send the collision information to the physisc engine
	pse->RegisterCollisions(m_collisions);
}

void CObjectManagerSceneNode::GetObjectsInArea(D3DXVECTOR3 center, float radius, vector<IGameObject*> &objs)
{
	float x,y,z;
	D3DXVECTOR3 trans;
	float radius2 = radius*radius;
	
	list<ISceneNode*>::iterator it = m_children.begin();
	IGameObject *go = NULL;
	for(; it != m_children.end(); it++)
	{
		go = (IGameObject*)(*it);
		trans = go->GetTranslation();
		x = center.x-trans.x;
		y = center.y-trans.y;
		z = center.z-trans.z;
		if(x*x+y*y+z*z<radius2)
			objs.push_back(go);
	}
}

void CObjectManagerSceneNode::GetObjectsFromSectorsInArea(rect<float> area, vector<IGameObject*> &objs)
{
	int x1,x2,y1,y2,x,y,yTemp;
	x1=(int)(area.left*m_onePerSectorSize);
	y1=(int)(area.top*m_onePerSectorSize);
	if(x1<0) x1 = 0;
	if(y1<0) y1 = 0;

	x2=(int)(area.right*m_onePerSectorSize);
	y2=(int)(area.bottom*m_onePerSectorSize);
	if(x2>=m_sectorsX)
		x2=m_sectorsX-1;
	if(y2>=m_sectorsY)
		y2=m_sectorsY-1;

	for(y=y1; y<=y2; ++y)
	{
		yTemp = y*m_sectorsX;
		for(x=x1; x<=x2; ++x)
			m_sectors[yTemp+x].AddObjectsToVector(objs);
	}
}

void CObjectManagerSceneNode::AddExternalCollider(IGeometricObject *collider)
{
	if(collider == NULL)
		return;

	for(unsigned int i=0;i<m_colliders.size();i++)
		if(collider == m_colliders[i])
			return;

	m_colliders.push_back(collider);
}

void CObjectManagerSceneNode::UpdateSectors()
{
	for(unsigned int i=0;i<m_sectors.size();i++)
		m_sectors[i].UpdateObjectPositions();
}