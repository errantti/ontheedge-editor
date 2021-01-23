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

#include "GUIEnvironment.h"
//#include "GUIWindow.h"

CGUIWindow::CGUIWindow(IGUIEnvironment *env, IGUIElement *parent, rect<int> position, long id, IEventReceiver *handler) :
	IGUIElement(env, parent, position, id), m_animation(NULL), m_bEnableControlsAfterAnimation(true), 
	m_externalRendering(NULL)
{
	m_externalEventHandler = handler;

	SEvent event;
	event.type = EW_GUI_EVENT;
	event.guiEvent.type = EWGUI_ELEMENT_CREATED;
	event.guiEvent.element = this;

	if(handler != NULL)
		handler->OnEvent(event);
}

CGUIWindow::~CGUIWindow()
{
	if(m_animation != NULL)
	{
		delete m_animation;
		m_animation = NULL;
	}
}

void CGUIWindow::SetChildrenEnabled(bool value)
{
	list<IGUIElement*>::iterator it = m_children.begin();
	CGUIEnvironment *env = (CGUIEnvironment*)m_environment;
	for(; it != m_children.end(); ++it)
	{
		(*it)->SetEnabled(value);
		if(env->GetFocusElement() == *it)
			env->ForceFocusElement(NULL);
		(*it)->ForceSetHasFocus(false);
	}
}

bool CGUIWindow::OnEvent(SEvent event)
{
	if(event.type == EW_GUI_EVENT)
	{
		switch(event.guiEvent.type)
		{
		case EWGUI_ELEMENT_DESTROYED:
			if(m_externalEventHandler)
				m_externalEventHandler->OnEvent(event);
			Remove();
			return true;

		case EWGUI_WINDOW_ANIMATION_SEQUENCE_FINISHED:
			if(m_bEnableControlsAfterAnimation)
				SetChildrenEnabled(true);

			// Send the message to the handler as well
			if(m_externalEventHandler)
				m_externalEventHandler->OnEvent(event);
			return true;
		}
	}
	if(m_externalEventHandler)
		if(m_externalEventHandler->OnEvent(event))
			return true;
	return m_parent->OnEvent(event);
}

void CGUIWindow::Update()
{
	if(m_animation != NULL)
		m_animation->Update(GetDXApp()->GetFrameTime());
	else
	{
		if(m_externalEventHandler)
			m_externalEventHandler->Update();
	}
}

void CGUIWindow::RenderElement()
{
	GetDXApp()->Draw2DImage(m_background, D3DXVECTOR2((float)m_absoluteRect.left, (float)m_absoluteRect.top));

	if(m_externalRendering)
		m_externalRendering->Render();
}

void CGUIWindow::SetBackgroundImage(string filename)
{
	m_background = CResourceManager::GetManager().GetTexture(filename);
}

void CGUIWindow::SetBackgroundImage(CTexture *tex)
{
	m_background = tex;
}

void CGUIWindow::AttachAnimationSequence(IGUIWindowAnimationSequence *anim)
{
	m_animation = anim;

	// Disable child elements by default for the duration of the animation
	if(anim != NULL)
		SetChildrenEnabled(false);
}

void CGUIWindow::ReportAnimationFinished()
{
	if(m_animation != NULL)
	{
		m_bEnableControlsAfterAnimation = m_animation->EnableControlsAfterAnimation();

		delete m_animation;
		m_animation = NULL;

		SEvent s;
		s.type = EW_GUI_EVENT;
		s.guiEvent.type = EWGUI_WINDOW_ANIMATION_SEQUENCE_FINISHED;
		s.guiEvent.element = (void*)this;

		OnEvent(s);
	}
}

void CGUIWindow::TabSelect(bool forward)
{
	CGUIEnvironment *env = (CGUIEnvironment*)m_environment;
	IGUIElement *focus = env->GetFocusElement(), *pMinOrder = NULL, *pNextOrder = NULL;

	int currentOrder = -1, order, minOrder=10000, nextOrder=10000;
	if(!forward)
	{
		nextOrder = -10000;
		minOrder = -10000;
	}
	list<IGUIElement*>::iterator it = m_children.begin();
	if(focus != NULL)
		if(focus->IsTabStop())
			currentOrder = focus->GetTabOrder();
	for(; it != m_children.end(); ++it)
	{
		if(!(*it)->IsTabStop() || !(*it)->GetVisible() || !(*it)->GetEnabled())
			continue;

		order = (*it)->GetTabOrder();

		if(forward)
		{
			if(order < minOrder)
			{
				pMinOrder = (*it);
				minOrder = order;
			}
			if(order > currentOrder && order < nextOrder)
			{
				pNextOrder = (*it);
				nextOrder = order;
			}
		}
		else
		{
			if(order > minOrder)
			{
				pMinOrder = (*it);
				minOrder = order;
			}
			if(order < currentOrder && order > nextOrder)
			{
				pNextOrder = (*it);
				nextOrder = order;
			}
		}
	}

	if(focus != NULL)
	{
		if(pNextOrder)
		{
			if(pNextOrder != NULL)
				env->SetFocusElement(pNextOrder);
		}
		else
		{
			if(pMinOrder != NULL)
				env->SetFocusElement(pMinOrder);
		}
	}
	else
	{
		if(pMinOrder != NULL)
			env->SetFocusElement(pMinOrder);
	}
}

void CGUIWindow::Draw()
{
	if(!GetVisible())
		return;

	if(m_animation != NULL)
	{
		if(m_animation->OverridesRender())
		{
			m_animation->Render();
			return;
		}
	}

	RenderElement();

	std::list<IGUIElement*>::iterator it = m_children.begin();
	for(; it != m_children.end(); ++it)
		(*it)->Draw();
}