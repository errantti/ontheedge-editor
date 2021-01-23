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

#include "Sector.h"
#include "ObjectManagerSceneNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSector::CSector()
: m_manager(NULL)
{
}

CSector::~CSector()
{
}

void CSector::SetObjectManager(CObjectManagerSceneNode *manager)
{
	m_manager = manager;
}

void CSector::Create(rect<long> area, float miny, float maxy, bool bContainer)
{
	m_bContainer = bContainer;
	area.repair();
	m_area = area;
	m_bbox = aabbox3d(D3DXVECTOR3(area.left, miny, area.top), D3DXVECTOR3(area.right, maxy, area.bottom));
}

void CSector::AddObject(IGameObject *object)
{
	m_objects.push_back(object);
}

void CSector::AddObjectsToVector(vector<IGameObject*> &vec)
{
	list<IGameObject*>::iterator it = m_objects.begin();
	for(;it != m_objects.end();it++)
		vec.push_back(*it);
}

void CSector::OnPreRender(ISceneManager *m, bool isShadowPass)
{
	// 
	//CCamera *cam = ISceneManager::GetActiveCamera();
	list<IGameObject*>::iterator it = m_objects.begin();
	for(; it != m_objects.end(); ++it)
	{
		//m->RegisterNodeForRendering(m_objects[i], SNG_DEFAULT);
		(*it)->OnPreRender(isShadowPass);
	}
}

bool CSector::RemoveObject(IGameObject *object)
{
	list<IGameObject*>::iterator it = m_objects.begin();
	for(;it != m_objects.end();it++)
		if((*it) == object)
		{
			m_objects.erase(it);
			return true;
		}
	return false;
}


void CSector::UpdateObjectPositions()
{
	// Loop through all dynamic objects
	// list<IGameObject*>::iterator it = m_objects.begin();
	IGameObject *temp = NULL;
	/*int i;
	for(i=0; i < m_objects.size(); i++)
		if(m_objects[i]->IsStatic() == FALSE)
		{
			// See if the object has moved out of the sector
			if(!(m_bbox.IntersectsWithPoint(m_objects[i]->GetTranslation())))
			{
				// special case: if this is a container sector
				// then don't let the objects leave it
				if(m_bContainer)
				{
					m_objects[i]->SetTranslation(m_objects[i]->GetLastPosition());
					m_objects[i]->SetSpeed(D3DXVECTOR3(0.f,0.f,0.f));
					continue;
				}

				temp = m_objects[i];
				RemoveObject(m_objects[i]);
				m_manager->AddObjectToInsertionList(temp);
				//--i;
			}
		}/**/

	list<IGameObject*>::iterator it = m_objects.begin();
	for(; it != m_objects.end();)
	{
#ifndef _EDITOR
		if((*it)->IsStatic() == FALSE)
#endif
		{
			// See if the object has moved out of the sector
			if(!(m_bbox.IntersectsWithPoint((*it)->GetTranslation())))
			{
				// special case: if this is a container sector
				// then don't let the objects leave it
				if(m_bContainer)
				{
					(*it)->SetTranslation((*it)->GetLastPosition());
					(*it)->SetSpeed(D3DXVECTOR3(0.f,0.f,0.f));
					++it;
					continue;
				}

				temp = (*it);
				it = m_objects.erase(it);
				m_manager->AddObjectToInsertionList(temp);
				continue;
			}
		}/**/
		++it;
	}
}

void CSector::CheckCollisions(CSector *other, vector<SCollision> &collisions)
{
	//unsigned int i,j;
	list<IGameObject*>::iterator i, j;
	if(other)
	{
		list<IGameObject*> *objects = other->GetObjects();

		// check collisions between objects of this and the other sector
		//for(i = 0; i < m_objects.size(); i++)
		for(i = m_objects.begin(); i != m_objects.end(); ++i)
		{
			//if(m_objects[i]->CollidesWithObjects() == FALSE)
			//	continue;
			//for(j = 0; j < objects->size(); j++)
			for(j = objects->begin(); j != objects->end(); ++j)
			{
				if((*j)->CollidesWithObjects() == FALSE)
					continue;
				(*i)->CheckForCollision(*j, collisions);
			}
		}
	}
	else
	{
		// check the collision between the internal objects
		//for(i = 0; i < m_objects.size(); i++)
		for(i = m_objects.begin(); i != m_objects.end(); ++i)
		{
			//if(m_objects[i]->CollidesWithObjects() == FALSE)
			//	continue;
			j=i;
			for(++j; j != m_objects.end(); ++j)
			{
				if((*j)->CollidesWithObjects() == FALSE)
					continue;
				(*i)->CheckForCollision(*j, collisions);
			}
		}
	}
}