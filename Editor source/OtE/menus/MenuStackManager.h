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

#ifndef __CMENUSTACKMANAGER_H__
#define __CMENUSTACKMANAGER_H__

#include "IMenuHandler.h"
#include "..\\Edgewalker\\GUI\\GUIWindow.h"

/**
 * Manages the menu stack and window flow. If there
 * are several menus on the screen at the same time
 * that need to be automatically positioned and 
 * have the transitions animated then this manager
 * handles the job
 */
class CMenuStackManager
{
public:
	CMenuStackManager();

	void RemoveTopmost(bool bAnimateTransition);
	void RemoveWindow(CGUIWindow *window, bool bAnimateTransition);
	void AddFlowMenu(CGUIWindow *window, bool bAnimateTransition, bool bDisableLast = true);

	void InstantClearStack();
	void InstantCloseAllStackMenus();

	void ExecuteTransition(bool bAnimateTransition);

	void SetTransitionDuration(float duration) {m_transitionDuration = duration;}

	CGUIWindow *GetTopmostPointer();

	static CMenuStackManager &GetManager();

protected:
	std::vector<CGUIWindow*> m_stack;

	float m_transitionDuration;
	int m_windowDistance;
};

#endif // __CMENUSTACKMANAGER_H__