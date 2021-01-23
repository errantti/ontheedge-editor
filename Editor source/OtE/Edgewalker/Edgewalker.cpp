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

// Edgewalker.cpp: implementation of the CEdgewalker class.
//
//////////////////////////////////////////////////////////////////////

#include "Edgewalker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEdgewalker::CEdgewalker()
	:	m_input(new CInputEngine), m_mouse(new CInputMouse), m_keyboard(new CInputKeyboard),
		m_sceneManager(new CSceneManager), m_guiEnvironment(new CGUIEnvironment),
		m_physicsEngine(new CPhysicsEngine), m_winMouseX(0), m_winMouseY(0), 
		m_joystick(new CInputJoystick)
{
}

CEdgewalker::~CEdgewalker()
{
	if(m_sceneManager)
	{
		delete m_sceneManager;
		m_sceneManager = NULL;
	}
	if(m_guiEnvironment)
	{
		delete m_guiEnvironment;
		m_guiEnvironment = NULL;
	}
	if(m_physicsEngine)
	{
		delete m_physicsEngine;
		m_physicsEngine = NULL;
	}
}

HRESULT CEdgewalker::Create(int iWidth, int iHeight, int iBPP, bool bWindowed, LPCTSTR strTitle, 
						  DWORD dwFlags, bool bVSync, bool bZBuffer, bool bMaximizeBox)
{
	HRESULT hres = CDirectX9::Create(iWidth, iHeight, iBPP, bWindowed, strTitle, dwFlags, bVSync, bZBuffer, bMaximizeBox);
	if(FAILED(hres))
		return hres;

	if(!CreateInputEngine())
		return E_FAIL;

	CParticleEngine::GetEngine()->InitCache();
	CResourceManager::GetManager().SetDevice(GetDevice());

	//if(m_bWindowed)
	//	m_mouse->SetSynchronizeWithWindows(TRUE);

	m_guiEnvironment->LoadDefaultFont();

	//if(!m_guiEnvironment->LoadCursorImage("imgs\\DefaultCursor.bmp"))
	if(!m_guiEnvironment->LoadCursorImage("imgs\\cursor-2.png"))
		CPrinter::log("Failed loading default cursor image", LOG_ERROR);

	return S_OK;
}

void CEdgewalker::Release()
{
	ReleaseInputEngine();

	CDirectX9::Release();
}

/*******************
 * Input
 *******************/

bool CEdgewalker::CreateInputEngine()
{
	// Create Input engine (using Direct Input)
	if(FAILED(m_input->Create(GetWindow())))
	{
		CPrinter::log("Failed creating input engine", LOG_ERROR);
		return FALSE;
	}
	if(FAILED(m_mouse->Create(NULL, 0)))
		CPrinter::log("Failed creating mouse input object", LOG_ERROR);

	if(FAILED(m_keyboard->Create(NULL, 0)))
		CPrinter::log("Failed creating keyboard input object", LOG_ERROR);

	// Create joystick if one is attached to the system
	vector<CInputEngine::INPUT_DEVICE> arrJoysticks;
	m_input->GetJoysticks(&arrJoysticks);

	HRESULT hr;

	// See if there are any joysticks / gamepads / wheels attached
	if(arrJoysticks.size() != 0)
	{
		CPrinter::log("Found a joystick, gamepad or wheel. Trying to initialize...");

		// Create the joystick object from the first enumerated device
		// NOTE! If there are several joysticks / gamepads / wheels attached
		// using this creation method only one of them can be used
		hr = m_joystick->Create(&arrJoysticks[0], DISCL_EXCLUSIVE | DISCL_BACKGROUND);

		if(FAILED(hr))
		{
			CPrinter::log("Failed to create joystick, gamepad or wheel device.", LOG_ERROR);
		}
		else
		{
			// Set default joystick axis ranges and dead zones
			// Axes to range of -100 to 100
			m_joystick->SetAxisRange(DIJOFS_X, -100, 100);
			m_joystick->SetAxisRange(DIJOFS_Y, -100, 100);
			m_joystick->SetAxisRange(DIJOFS_Z, -100, 100);

			// and dead zones to 0 - 10000
			m_joystick->SetDeadZone(DIJOFS_X, 500);
			m_joystick->SetDeadZone(DIJOFS_Y, 500);
			m_joystick->SetDeadZone(DIJOFS_Z, 500);
		}
	}

	arrJoysticks.clear();

	CPrinter::log("Input engine created succesfully");

	return TRUE;
}

void CEdgewalker::ReleaseInputEngine()
{
	if(m_mouse)
	{
		m_mouse->Release();
		delete m_mouse;
		m_mouse = NULL;
	}
	if(m_keyboard)
	{
		m_keyboard->Release();
		delete m_keyboard;
		m_keyboard = NULL;
	}
	if(m_input)
	{
		m_input->Release();
		delete m_input;
		m_input = NULL;
	}
	if(m_joystick)
	{
		m_joystick->Release();
		delete m_joystick;
		m_joystick = NULL;
	}
}

void CEdgewalker::UpdateInputDevices()
{
	if(m_mouse)
		m_mouse->Update();
	if(m_keyboard)
		m_keyboard->Update();
	if(m_joystick)
		m_joystick->Update();
}

void CEdgewalker::OnMouseMove(int xpos, int ypos)
{
	m_winMouseX = xpos;
	m_winMouseY = ypos;
}