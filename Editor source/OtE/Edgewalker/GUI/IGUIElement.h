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

#ifndef __IGUIELEMENT_H__
#define __IGUIELEMENT_H__

#include <string>
#include <windows.h>
#include <list>
#include <d3dx9.h>
#include "..\\rect.h"
#include "IEventReceiver.h"

using namespace std;

class IGUIEnvironment;

class IGUIElement : public IEventReceiver 
{
public:
	IGUIElement(IGUIEnvironment *env, IGUIElement *parent, rect<int> position, long id) : 
	  m_rect(position), m_ID(id), m_environment(env), m_parent(parent),
		m_bHovering(FALSE), m_modColor(0xFFFFFFFF), m_bVisible(TRUE), 
		m_bEnabled(TRUE), m_externalEventHandler(NULL), m_bTabStop(FALSE), 
		m_tabOrder(0), m_bFocused(FALSE)
	{
		m_rect.repair();
		if(parent != NULL)
		{
			parent->AddChild(this);
			UpdateAbsoluteRect();
		}
	}
	virtual ~IGUIElement() 
	{
		RemoveChildren();
	}

	void SetElementRect(rect<int> r) 
	{
		m_rect = r;
		UpdateAbsoluteRect();
	}
	rect<int> GetElementRect() const {return m_rect;}
	rect<int> GetAbsoluteRect() const {return m_absoluteRect;}

	void SetEnabled(bool value) 
	{
		m_bEnabled = value;
		if(!value)
		{
			m_bFocused = false;
			m_bHovering = false;
		}
	}
	void SetVisible(bool value) {m_bVisible = value;}

	bool GetEnabled() const {return m_bEnabled;}
	bool GetVisible() const {return m_bVisible;}

	void SetCaption(const string text) {m_caption = text;}
	const string GetCaption() {return m_caption;}

	void SetID(long value) {m_ID = value;}
	long GetID() const {return m_ID;}

	virtual D3DCOLOR GetModColor() {return m_modColor;}
	virtual void SetModColor(D3DCOLOR color) {m_modColor = color;}

	inline bool IsPointInsideElement(POINT &pos)
	{
		return m_absoluteRect.IsPointInside(pos.x, pos.y);
	}

	virtual IGUIElement *GetActiveElementAt(POINT &pos)
	{
		if(!m_bEnabled)
			return NULL;

		IGUIElement *element = NULL;

		std::list<IGUIElement*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			if((*it)->IsPointInsideElement(pos))
			{
				element = (*it)->GetActiveElementAt(pos);
				if(element != NULL)
					return element;
			}
		return this;
	}

	virtual IGUIElement *GetElementAt(POINT &pos)
	{
		std::list<IGUIElement*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			if((*it)->IsPointInsideElement(pos))
				return (*it)->GetElementAt(pos);
		return this;
	}

	/**
	 * Leave event handling (and distributing it to parent
	 * and children all up to spesific element types
	 **/
	virtual bool OnEvent(SEvent event) = 0;

	virtual void Draw()
	{
		if(!GetVisible())
			return;

		RenderElement();

		std::list<IGUIElement*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			(*it)->Draw();
	}

	IGUIElement *GetChild(long id)
	{
		if(m_ID == id)
			return this;
		std::list<IGUIElement*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			if((*it)->GetID() == id)
				return *it;
		return NULL;
	}

	inline IGUIElement *GetParent() {return m_parent;}
	inline std::list<IGUIElement*> &GetChildren() {return m_children;}

	virtual bool RemoveChild(IGUIElement *child)
	{
		std::list<IGUIElement*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			if((*it) == child)
			{
				delete (*it);
				m_children.erase(it);
				return true;
			}
		return false;
	}

	bool DetachChild(IGUIElement *child)
	{
		std::list<IGUIElement*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			if((*it) == child)
			{
				m_children.erase(it);
				return true;
			}
		return false;
	}

	void RemoveChildren()
	{
		list<IGUIElement*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			if((*it) != NULL)
				delete (*it);
		m_children.clear();
	}

	void Remove()
	{
		if(m_parent)
			m_parent->RemoveChild(this);
	}

	void Detach()
	{
		if(m_parent)
			m_parent->DetachChild(this);
	}

	virtual void AddChild(IGUIElement *child)
	{
		m_children.push_back(child);
	}

	virtual void SetParent(IGUIElement *pNewParent)
	{
		if(!pNewParent)
			return;
		Detach();
		pNewParent->AddChild(this);
	}

	void UpdateAbsoluteRect()
	{
		if(m_parent != NULL)
		{
			rect<int> r = m_parent->GetAbsoluteRect();
			m_absoluteRect = rect<int>(m_rect.left + r.left, 
				m_rect.right + r.left, m_rect.top + r.top, m_rect.bottom + r.top);
		}

		list<IGUIElement*>::iterator it = m_children.begin();
		for(; it != m_children.end(); ++it)
			(*it)->UpdateAbsoluteRect();
	}

	void SetExternalEventHandler(IEventReceiver *handler)
	{
		m_externalEventHandler = handler;
	}

	bool IsTabStop() {return m_bTabStop;}
	void SetIsTabStop(bool value) {m_bTabStop = value;}

	int GetTabOrder() {return m_tabOrder;}
	void SetTabOrder(int value) {m_tabOrder = value;}

	void ForceSetHasFocus(bool bFocus) {m_bFocused = bFocus;}

protected:
	std::list<IGUIElement*> m_children;
	IGUIElement *m_parent;

	IEventReceiver *m_externalEventHandler;

	rect<int> m_rect;			// Upper left corner position relative to parent position
	rect<int> m_absoluteRect;	// Absolute rect of the object
	bool m_bVisible, m_bEnabled;

	string m_caption;

	IGUIEnvironment *m_environment;

	bool m_bHovering, m_bFocused;
	bool m_bTabStop;		// True if this element is selectable by keyboard
	int m_tabOrder;			// Determines the order in which elements are selected by keyboard

	long m_ID;
	D3DCOLOR m_modColor;

	virtual void RenderElement() = 0;
};

#endif // #ifndef __IGUIELEMENT_H__
