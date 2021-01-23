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

#include "MenuStackManager.h"
#include "Animators\\LinearInterpolateAnimator.h"
#include "..\\Edgewalker\\Edgewalker.h"

CMenuStackManager::CMenuStackManager() : m_transitionDuration(0.4f), m_windowDistance(5)
{
}

CMenuStackManager &CMenuStackManager::GetManager()
{
	static CMenuStackManager stackManager;
	return stackManager;
}

void CMenuStackManager::InstantClearStack()
{
	m_stack.clear();
}

void CMenuStackManager::InstantCloseAllStackMenus()
{
	std::vector<CGUIWindow*>::iterator it = m_stack.begin();
	for(; it != m_stack.end(); ++it)
		(*it)->Remove();
	m_stack.clear();
}

void CMenuStackManager::RemoveTopmost(bool bAnimateTransition)
{
	if(m_stack.size() == 0)
		return;

	CGUIWindow *win = m_stack[m_stack.size()-1];
	if(win != NULL && bAnimateTransition)
	{
		rect<int> r = win->GetAbsoluteRect();
		POINT p, t;
		p.x = r.left;
		p.y = r.top;
		t.x = p.x;
		t.y = GetDXApp()->GetHeight();
		win->AttachAnimationSequence(new CLinearInterpolateAnimator(win, m_transitionDuration, p, t, false));
	}

	m_stack.pop_back();

	if(m_stack.size() > 0)
	{
		//m_stack[m_stack.size()-1]->SetChildrenEnabled(true); //>SetEnabled(true);
		ExecuteTransition(bAnimateTransition);
	}
}

void CMenuStackManager::RemoveWindow(CGUIWindow *window, bool bAnimateTransition)
{
	bool visibleChanged = false;
	std::vector<CGUIWindow*>::iterator it = m_stack.begin();
	for(int i=0; it != m_stack.end(); ++it, ++i)
		if((*it) == window)
		{
			if(i >= (int)m_stack.size() - 3)
				visibleChanged = true;
			m_stack.erase(it);
			if(visibleChanged)
				ExecuteTransition(bAnimateTransition);
			return;
		}
}

void CMenuStackManager::AddFlowMenu(CGUIWindow *window, bool bAnimateTransition, bool bDisableLast)
{
	if(window == NULL)
		return;
	m_stack.push_back(window);

	// Set new window's initial position
	rect<int> r = window->GetElementRect();
	int height = r.getHeight();
	r.top = GetDXApp()->GetHeight();
	r.bottom = r.top + height;
	window->SetElementRect(r);

	ExecuteTransition(bAnimateTransition);

	//if(bDisableLast && m_stack.size() > 1)
	//	m_stack[m_stack.size()-2]->SetChildrenEnabled(false);
}

CGUIWindow *CMenuStackManager::GetTopmostPointer()
{
	if(m_stack.size() == 0)
		return NULL;
	return m_stack[m_stack.size()-1];
}

void CMenuStackManager::ExecuteTransition(bool bAnimateTransition)
{
	if(m_stack.size() == 0)
		return;

	// Calculate the area of visible windows (the two topmost)
	rect<int> area, w1, w2, w3;
	CGUIWindow *win1 = m_stack[m_stack.size()-1], *win2 = NULL, *win3 = NULL;
	if(m_stack.size() > 1)
		win2 = m_stack[m_stack.size()-2];
	if(m_stack.size() > 2)
		win3 = m_stack[m_stack.size()-3];

	int width = 0, height = 0;
	w1 = win1->GetElementRect();
	width = w1.getWidth();
	height = w1.getHeight();

	if(win2 != NULL)
	{
		w2 = win2->GetElementRect();
		if(w2.getWidth() > width)
			width = w2.getWidth();
		height += m_windowDistance + w2.getHeight();
	}
	if(win3 != NULL)
		w3 = win3->GetElementRect();

	POINT screen;
	screen.x = GetDXApp()->GetWidth();
	screen.y = GetDXApp()->GetHeight();
	POINT halfScreen;
	halfScreen.x = (int)((float)screen.x * 0.5f);
	halfScreen.y = (int)((float)screen.y * 0.5f);

	area.left = halfScreen.x - (int)((float)width * 0.5f);
	area.top = halfScreen.y - (int)((float)height * 0.5f);
	area.right = area.left + width;
	area.bottom = area.top + height;

	POINT w1Source, w2Source, w3Source;
	w1Source.x = w1.left;
	w1Source.y = w1.top;
	w2Source.x = w2.left;
	w2Source.y = w2.top;
	w3Source.x = w3.left;
	w3Source.y = w3.top;

	POINT w1Target, w2Target, w3Target;
	if(win2 == NULL)
	{
		w1Target.x = area.left;
		w1Target.y = area.top;
		if(bAnimateTransition)
			win1->AttachAnimationSequence(new CLinearInterpolateAnimator(win1, m_transitionDuration, w1Source, w1Target, true));
		else
			win1->SetElementRect(area);
	}
	else
	{
		w2Target.x = halfScreen.x - (int)((float)w2.getWidth() * 0.5f);
		w2Target.y = area.top;
		w1Target.x = halfScreen.x - (int)((float)w1.getWidth() * 0.5f);
		w1Target.y = area.top + w2.getHeight() + m_windowDistance;
		if(bAnimateTransition)
		{
			win1->AttachAnimationSequence(new CLinearInterpolateAnimator(win1, m_transitionDuration, w1Source, w1Target, true));
			win2->AttachAnimationSequence(new CLinearInterpolateAnimator(win2, m_transitionDuration, w2Source, w2Target, false));
		}
		else
		{
			area.left = w2Target.x;
			area.right = w2Target.x + w2.getWidth();
			area.top = w2Target.y;
			area.bottom = w2Target.y + w2.getHeight();
			win2->SetElementRect(area);

			area.left = w1Target.x;
			area.right = w1Target.x + w1.getWidth();
			area.top = w1Target.y;
			area.bottom = w1Target.y + w1.getHeight();
			win1->SetElementRect(area);
		}
	}

	if(win3 != NULL)
	{
		w3Target.x = w3.left;
		w3Target.y = -w3.getHeight();
		if(bAnimateTransition)
			win3->AttachAnimationSequence(new CLinearInterpolateAnimator(win3, m_transitionDuration, w3Source, w3Target, false));
		else
		{
			area.left = w3Target.x;
			area.right = w3Target.x + w3.getWidth();
			area.top = w3Target.y;
			area.bottom = w3Target.y + w3.getHeight();
			win3->SetElementRect(area);
		}
	}
}