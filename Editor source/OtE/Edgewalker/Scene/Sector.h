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

#ifndef __CSECTOR_H__
#define __CSECTOR_H__

#include "..\\IGameObject.h"

class CSector
{
public:
	CSector();
	virtual ~CSector();

	void SetObjectManager(CObjectManagerSceneNode *manager);

	// Create
	// Initializes the sector object
	// /param area - the x-z area of the sector
	// /param maxy / miny - max and min y values (for the bounding box)
	// /param bContainer - 'container' sector / 'outer' sector, not a real sector
	// but the area that's outside the sector grid. No object can cross borders
	// of this sector
	void Create(rect<long> area, float miny, float maxy, bool bContainer = false);
	void AddObject(IGameObject *object);
	bool RemoveObject(IGameObject *object);
	//bool RemoveObject(unsigned int index);

	inline void RemoveAllObjects() {m_objects.clear();}

	void OnPreRender(ISceneManager *m, bool isShadowPass);

	void CheckCollisions(CSector *other, vector<SCollision> &collisions);

	list<IGameObject*> *GetObjects() {return &m_objects;}
	void AddObjectsToVector(vector<IGameObject*> &vec);

	void UpdateObjectPositions();

protected:
	// Objects currently on this sector
	list<IGameObject*> m_objects;

	// Area of the sector
	rect<long> m_area;
	aabbox3d m_bbox;

	CObjectManagerSceneNode *m_manager;

	bool m_bContainer;
};

#endif // #ifndef __CSECTOR_H__
