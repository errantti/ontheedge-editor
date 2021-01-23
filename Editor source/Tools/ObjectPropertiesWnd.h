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

#ifndef __COBJECTPROPERTIESWND_H__
#define __COBJECTPROPERTIESWND_H__

#include <windows.h>
#include "..\\OtE\\Level.h"

/**
 * Handles a window that let's edit
 * a single object instance's properties (ID and targetID)
 */
class CObjectPropertiesWnd
{
public:
	CObjectPropertiesWnd();
	~CObjectPropertiesWnd();

	bool Open(HWND parent, SObjectInstanceInfo *info, IGameObject *object);

private:
	void SetObjectType(HWND hWnd);
	void DisableScriptSelection(HWND hWnd);
	void EnableTargetSelection(HWND hWnd, std::string name, bool editable = true, bool enableBrowse = true);
	void BrowseTarget(HWND hWnd);
	void BrowseScript(HWND hWnd);
	void BrowsePEffect(HWND hWnd);
	void BrowseLEffect(HWND hWnd);
	void BrowseController(HWND hWnd);
	void SetEffectPos(HWND hWnd);

	static BOOL CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	static CObjectPropertiesWnd *m_pointer;

	SObjectInstanceInfo m_template;

	int m_type;
	STATIC_OBJECT_TYPE m_soType;

	IGameObject *m_object;
	bool m_bModified;
};

#endif // #ifndef __COBJECTPROPERTIESWND_H__

