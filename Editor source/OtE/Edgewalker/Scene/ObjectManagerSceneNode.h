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

#ifndef __COBJECTMANAGERSCENENODE_H__
#define __COBJECTMANAGERSCENENODE_H__

#include "Sector.h"
#include "Frustum.h"
#include <vector>

class CObjectManagerSceneNode : public ISceneNode  
{
protected:
	class SSectorQuadTreeNode
	{
	public:
		aabbox3d bbox;
		SSectorQuadTreeNode *nodes[4];
		CSector *leaf;
		int level;

		SSectorQuadTreeNode() : leaf(NULL), level(0)
		{
			for(int i=0;i<4;++i)
				nodes[i] = NULL;
		}

		~SSectorQuadTreeNode()
		{
			RemoveChildren();
		}

		void RemoveChildren()
		{
			for(int i=0;i<4;++i)
				if(nodes[i] != NULL)
				{
					delete nodes[i];
					nodes[i] = NULL;
				}
		}

		void OnPreRender(ISceneManager *m, CFrustum &f, bool preCulled, bool isShadowPass);
	};

public:
	CObjectManagerSceneNode(ISceneNode *parent, ISceneManager *manager, long id, DWORD width, DWORD height, int sectorSize);
	virtual ~CObjectManagerSceneNode();

	virtual void Create(DWORD width, DWORD height, int sectorSize);

	virtual void OnPreRender(bool isShadowPass);
	virtual void Render(bool isShadowPass);
	virtual void Update(float frameTime);
	virtual aabbox3d &GetBoundingBox() {return m_bbox;}

	virtual void SetYLimits(float min, float max) {m_yMinLimit = min; m_yMaxLimit = max;}

	// Adds a new game object to the manager
	virtual IGameObject *AddNewObject(IGameObject *object);
	// Replaces an existing game object - called for example when a dynamic
	// object moves from a sector to another
	virtual IGameObject *AddObject(IGameObject *object);
	// Does the same as the AddObject but waits until the sectors have been updated before adding the object
	virtual void AddObjectToInsertionList(IGameObject *object);
	virtual void AddMarkedObjects();

	virtual bool RemoveObject(IGameObject *object);
	virtual bool RemoveObject(long id);
	virtual void RemoveAllObjects();

	virtual void AddObjectToRemoveList(IGameObject *object);
	virtual void RemoveMarkedObjects();

	virtual bool RemoveObjectFromSector(IGameObject *object);
	virtual bool ObliterateObjectFromSectors(IGameObject *object);

	virtual IGameObject* GetObject(long id);

	virtual void AddExternalCollider(IGeometricObject *collider);
	virtual void RemoveExternalColliders() {m_colliders.clear();}

	virtual void RegisterVisibleObjects(rect<int> area, bool isShadowPass);

	virtual void BuildSectorTree(rect<int> area, SSectorQuadTreeNode *parent);

	virtual IGameObject *GetObjectFromRaySlow(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir);

	virtual void Reset();

	virtual void GetObjectsInArea(D3DXVECTOR3 center, float radius, vector<IGameObject*> &objs);
	virtual void GetObjectsFromSectorsInArea(rect<float> area, vector<IGameObject*> &objs);

	inline bool IsPhysicsEnabled() {return m_bPhysicsEnabled;}
	inline void EnablePhysics(bool value) {m_bPhysicsEnabled = value;}

	// For special situations only, should never be called without a good reason
	// Always after call manually setting updating you should also disable it
	inline void SetIsUpdating(bool value) {m_bUpdating = value;}

	void SetOnlyUpdateOnArea(bool value);
	void SetFocusPoint(D3DXVECTOR3 point) {m_focusPoint = point;}
	void SetFocusRadius(float radius) {m_focusRadius = radius;}

protected:
	virtual void DoCollisionDetection();
	virtual void UpdateSectors();

	vector<CSector> m_sectors;
	SSectorQuadTreeNode m_rootSectorNode;

	aabbox3d m_bbox;

	DWORD m_width, m_height, m_sectorSize;
	int m_sectorsX, m_sectorsY;

	float m_yMinLimit, m_yMaxLimit, m_onePerSectorSize;

	bool m_bUpdating, m_bPhysicsEnabled;

	bool m_bOnlyUpdateOnArea;
	D3DXVECTOR3 m_focusPoint;
	float m_focusRadius;

	vector<SCollision> m_collisions;

	// Vector that stores pointers to the scene node objects that can collide with
	// dynamic objects handled by this manager
	vector<IGeometricObject*> m_colliders;

	vector<IGameObject*> m_constantUpdateObjects;

	// List of objects to be removed at the end of update cycle
	vector<IGameObject*> m_removeList;
	vector<IGameObject*> m_addList;
};

#endif // #ifndef __COBJECTMANAGERSCENENODE_H__
