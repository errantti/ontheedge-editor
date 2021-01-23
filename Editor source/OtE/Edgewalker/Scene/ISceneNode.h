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

#ifndef __ISCENENODE_H__
#define __ISCENENODE_H__

#include <list>
#include "ISceneManager.h"
#include "..\\Math.h"

class ISceneNode
{
public:
	ISceneNode(ISceneNode *parent, ISceneManager *manager, long id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f))
		:	m_relativeScale(scale), m_relativeTranslation(position),
			m_relativeRotation(rotation), m_bVisible(TRUE),
			m_manager(manager), m_ID(id), m_parent(parent),
			m_bAutomaticCulling(TRUE)
	{
		if(parent)
			parent->AddChild(this);

		UpdateAbsolutePosition();
	}

	virtual ~ISceneNode()
	{
		list<ISceneNode*>::iterator it = m_children.begin();
		for(; it != m_children.end(); it++)
		{
			if((*it) != NULL)
			{
				delete (*it);
				(*it) = NULL;
			}
		}
		m_children.clear();
	}

	virtual void OnPreRender(bool isShadowPass)
	{
		if(GetVisible())
		{
			list<ISceneNode*>::iterator it = m_children.begin();
			for(; it != m_children.end(); it++)
				(*it)->OnPreRender(isShadowPass);
		}
	}

	virtual void Update(float frameTime)
	{
		list<ISceneNode*>::iterator it = m_children.begin();
		for(; it != m_children.end(); it++)
			(*it)->Update(frameTime);
	}
	
	// OnPostRender
	// Called after update each frame, this function updates
	// the absolute position and maybe does some other stuff
	virtual void OnPostRender(float frameTime)
	{
		UpdateAbsolutePosition();

		list<ISceneNode*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			(*it)->OnPostRender(frameTime);
	}

	virtual void AddChild(ISceneNode *child)
	{
		if(child)
		{
			child->Detach();
			m_children.push_back(child);
			child->m_parent = this;
		}
	}

	virtual void Remove()
	{
		if(m_parent)
			m_parent->RemoveChild(this);
	}

	virtual void Detach()
	{
		if(m_parent)
			m_parent->DetachChild(this);
	}

	/**
	 * Detaches a child node from the tree and deletes it
	 **/
	virtual bool RemoveChild(ISceneNode *child)
	{
		list<ISceneNode*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
		{
			if((*it) == child)
			{
				delete (*it);
				m_children.erase(it);
				return TRUE;
			}
		}
		return FALSE;
	}

	virtual void RemoveAll()
	{
		list<ISceneNode*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			delete (*it);
		m_children.clear();
	}

	/**
	 * Detaches a child node from the tree
	 **/
	virtual bool DetachChild(ISceneNode *child)
	{
		list<ISceneNode*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
		{
			if((*it) == child)
			{
				m_children.erase(it);
				return TRUE;
			}
		}
		return FALSE;
	}

	/**
	 * Pure virtual functions
	 **/
	virtual void Render(bool isShadowPass) = 0;
	virtual aabbox3d &GetBoundingBox() = 0;

	// Functions to optimize rendering by minimizing the amount
	// of material / texture changes
	virtual void RenderSubset(bool isShadowPass) {}
	virtual void SetSubset() {}
	virtual int GetSubsetCount() {return 0;}
	virtual CMesh *GetMesh() {return NULL;}

	virtual aabbox3d GetTransformedBox()
	{
		aabbox3d box = GetBoundingBox();
		return box;
	}

	void SetVisible(bool value) {m_bVisible = value;}
	bool GetVisible() const {return m_bVisible;}

	void SetAutomaticCulling(bool value) {m_bAutomaticCulling = value;}
	bool GetAutomaticCulling() {return m_bAutomaticCulling;}

	void SetID(long id) {m_ID = id;}
	long GetID() {return m_ID;}

	ISceneNode *GetParent() {return m_parent;}
	list<ISceneNode*>& GetChildren() {return m_children;}

	const D3DXMATRIX &GetAbsoluteTransformation() const {return m_absoluteTransformation;}
	virtual D3DXVECTOR3 GetAbsolutePosition() 
	{
		D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
		return CMath::MatrixVectorMultiply(m_absoluteTransformation, pos);
	}

	virtual D3DXVECTOR3 GetTranslation() const {return m_relativeTranslation;}
	virtual D3DXVECTOR3 GetRotation() const {return m_relativeRotation;}
	virtual D3DXVECTOR3 GetScale() const {return m_relativeScale;}

	virtual void SetTranslation(D3DXVECTOR3 &vec) {m_relativeTranslation = vec;}
	virtual void SetRotation(D3DXVECTOR3 &vec) {m_relativeRotation = vec;}
	virtual void SetScale(D3DXVECTOR3 &vec) {m_relativeScale = vec;}

	virtual void SetTranslationRel(D3DXVECTOR3 &vec) {m_relativeTranslation += vec;}
	virtual void SetRotationRel(D3DXVECTOR3 &vec) {m_relativeRotation += vec;}

	virtual D3DXMATRIX GetRelativeTransformation()
	{
		return CMath::ConstructTransformationMatrix(m_relativeTranslation, m_relativeRotation, m_relativeScale);
	}

	virtual void UpdateAbsolutePosition()
	{
		if(m_parent)
		{
			//D3DXMatrixMultiply(&m_absoluteTransformation, 
			//	(D3DXMATRIX*)m_parent->GetAbsoluteTransformation(), 
			//	(D3DXMATRIX*)GetRelativeTransformation());

			//m_absoluteTransformation = m_parent->GetAbsoluteTransformation() * GetRelativeTransformation();
			m_absoluteTransformation = GetRelativeTransformation() * m_parent->GetAbsoluteTransformation();
		}
		else
			m_absoluteTransformation = GetRelativeTransformation();
	}

	virtual void SetParent(ISceneNode *pNewParent)
	{
		if(!pNewParent)
			return;
		Detach();
		pNewParent->AddChild(this);
	}

protected:
	list<ISceneNode*> m_children;
	ISceneNode *m_parent;

	D3DXMATRIX m_absoluteTransformation;
	D3DXVECTOR3 m_relativeTranslation;
	D3DXVECTOR3 m_relativeRotation;
	D3DXVECTOR3 m_relativeScale;

	long m_ID;

	ISceneManager *m_manager;

	bool m_bVisible;
	bool m_bAutomaticCulling;
};

#endif // #ifndef __ISCENENODE_H__
