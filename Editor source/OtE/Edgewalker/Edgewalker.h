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

#ifndef __CEDGEWALKER_H__
#define __CEDGEWALKER_H__

// DirectX graphics includes
#include "DirectX9\\DirectX9.h"

// input includes
#include "Input\\InputMouse.h"
#include "Input\\InputKeyboard.h"
#include "Input\\InputJoystickEffect.h"

// Scene includes
#include "Scene\\SceneManager.h"
#include "Scene\\LightSceneNode.h"
#include "Scene\\CameraSceneNode.h"
#include "Scene\\CameraFPSSceneNode.h"
#include "Scene\\CameraFollowSceneNode.h"
#include "Scene\\SkyBoxSceneNode.h"
#include "Scene\\MeshSceneNode.h"
#include "Scene\\ObjectManagerSceneNode.h"

#include "ParticleSystem\\ParticleSystemSceneNode.h"
#include "ParticleSystem\\ParticleEngine.h"

#include "Terrain\\TiledTerrain.h"

// GUI includes
#include "GUI\\GUIEnvironment.h"

// Physics engine includes
#include "PhysicsEngine.h"

#define GetEdgewalkerApp() ((CEdgewalker*)CEdgewalker::GetApp())

class CEdgewalker : public CDirectX9  
{
public:
	CEdgewalker();
	virtual ~CEdgewalker();

	virtual HRESULT Create(int iWidth, int iHeight, int iBPP, bool bWindowed, LPCTSTR strTitle, DWORD dwFlags, bool bVSync=FALSE, bool bZBuffer=TRUE, bool bMaximizeBox=FALSE);
	virtual void Release();

	virtual HRESULT OnCreate() {return S_OK;}
	virtual void OnRelease() { }
	virtual void OnFlip() { }
	virtual bool OnRestore() { return CDirectX9::OnRestore(); }

	virtual void OnMouseMove(int xpos, int ypos);

	// Input
	CInputMouse *GetMouse() {return m_mouse;}
	CInputKeyboard *GetKeyboard() {return m_keyboard;}
	CInputJoystick *GetJoystick() {return m_joystick;}

	inline int GetWinMouseX() const {return m_winMouseX;}
	inline int GetWinMouseY() const {return m_winMouseY;}

	virtual void UpdateInputDevices();

	// Scene
	CSceneManager *GetSceneManager() {return m_sceneManager;}

	// GUI
	CGUIEnvironment *GetGUIEnvironment() {return m_guiEnvironment;}

	// Physics engine
	CPhysicsEngine *GetPhysicsEngine() {return m_physicsEngine;}

protected:
	// Input
	CInputEngine	*m_input;		// Input engine
	CInputMouse		*m_mouse;		// Mouse input object
	CInputKeyboard	*m_keyboard;	// Keyboard input object
	CInputJoystick  *m_joystick;	// Joystick / gamepad / wheel object

	bool CreateInputEngine();
	void ReleaseInputEngine();

	// Scene manager
	CSceneManager	*m_sceneManager;

	// GUI
	CGUIEnvironment *m_guiEnvironment;

	// Physics engine
	CPhysicsEngine *m_physicsEngine;

	int m_winMouseX, m_winMouseY;
};

#endif // #ifndef __CEDGEWALKER_H__
