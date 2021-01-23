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

#include "InputMouse.h"
#include "..\\DirectX9\\DirectX9.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputMouse::CInputMouse()
: m_pDevice(NULL), m_dwButtonCount(0), m_bSynchronize(FALSE)
{
	::memset(&m_state, 0, sizeof(DIMOUSESTATE2));
	m_position.x = m_position.y = 0;
}

CInputMouse::~CInputMouse()
{
	Release();
}

/**
 * initialize the input object
 **/
HRESULT CInputMouse::Create(CInputEngine::INPUT_DEVICE *pDevice, DWORD dwFlags)
{
	// get the direct input
	LPDIRECTINPUT8 pDI = CInputEngine::GetInputEngine()->GetDirectInput();
	if(pDI == NULL)
		return E_FAIL;

	HRESULT hres;
	GUID gdDevice;

	// init the mouse GUID
	gdDevice = GUID_SysMouse;
	if(pDevice)
		gdDevice = pDevice->gdDevice;

	hres = pDI->CreateDevice(gdDevice, &m_pDevice, NULL);

	if(FAILED(hres))
	{
		::OutputDebugString("MOUSE - failed to create direct input device.\r\n");
		return hres;
	}

	// set the input device data format
	hres = m_pDevice->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(hres))
	{
		::OutputDebugString("MOUSE - SetDataFormat failed");
		return hres;
	}

	// set the co-operative level
	if(dwFlags == 0)
	{
		// NONEXCLUSIVE:
		// gives access to input device also to other applications
		// FOREGROUND:
		// we get keyboard data
		dwFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
	}

	hres = m_pDevice->SetCooperativeLevel(CInputEngine::GetInputEngine()->GetWindow(), dwFlags);
	if(FAILED(hres))
	{
		::OutputDebugString("MOUSE - SetCooperativeLevel failed");
		return hres;
	}

	// count the number of buttons this mouse device has
	m_dwButtonCount = CountButtons();

	InitButtonStates();

	// acquire the input device
	hres = m_pDevice->Acquire();

	return S_OK;
}

void CInputMouse::Release()
{
	if(m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}
}


// get the current keyboard state
BOOL CInputMouse::Update()
{
	if(m_pDevice == NULL)
		return FALSE;

	HRESULT hres;

	hres = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_state);

	// if getting the device state fails, most likely
	// another application has gained the mouse
	// focus and we have lost it.
	if(FAILED(hres))
	{
		// failed to get keyboard state
		// try to acquire the device back to us.
		m_pDevice->Acquire();

		hres = m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_state);

		if(FAILED(hres))
		{
			// nothing much we can do about this..
			::memset(&m_state, 0, sizeof(DIMOUSESTATE2));
			UpdatePositionFromWindows();
			return FALSE;
		}
	}

	UpdateButtonStates();
	m_position.x += GetRelativeX();
	m_position.y += GetRelativeY();

	if(m_bSynchronize)
		UpdatePositionFromWindows();

	CDirectX9 *dx = GetDXApp();

	if(m_position.x < 0)
		m_position.x = 0;
	if(m_position.x > dx->GetWidth())
		m_position.x = dx->GetWidth();
	if(m_position.y < 0)
		m_position.y = 0;
	if(m_position.y > dx->GetHeight())
		m_position.y = dx->GetHeight();

	return TRUE;
}

void CInputMouse::UpdatePositionFromWindows()
{
	POINT ptCursor;
    GetCursorPos( &ptCursor );
    ScreenToClient( GetDXApp()->GetWindow(), &ptCursor );
	m_position.x = ptCursor.x;
	m_position.y = ptCursor.y;
}

void CInputMouse::UpdateButtonStates()
{
	for(int i=0;i<m_dwButtonCount;i++)
	{
		if(m_state.rgbButtons[i] != 0)
		{
			if(m_buttonStates[i] == MOUSE_BUTTON_PRESSED)
				m_buttonStates[i] = MOUSE_BUTTON_DOWN;
			else if(m_buttonStates[i] == MOUSE_BUTTON_UP)
				m_buttonStates[i] = MOUSE_BUTTON_PRESSED;
			else
				m_buttonStates[i] = MOUSE_BUTTON_DOWN;
		}
		else
		{
			if(m_buttonStates[i] == MOUSE_BUTTON_RELEASED)
				m_buttonStates[i] = MOUSE_BUTTON_UP;
			else if(m_buttonStates[i] == MOUSE_BUTTON_DOWN)
				m_buttonStates[i] = MOUSE_BUTTON_RELEASED;
			else
				m_buttonStates[i] = MOUSE_BUTTON_UP;
		}
	}
}

void CInputMouse::InitButtonStates()
{
	for(int i=0;i<m_dwButtonCount;i++)
	{
		m_buttonStates[i] = MOUSE_BUTTON_UP;
	}
}

DWORD CInputMouse::CountButtons()
{
	DIDEVICEOBJECTINSTANCE	didoi;

	::memset(&didoi, 0, sizeof(DIDEVICEOBJECTINSTANCE));
	didoi.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

	DWORD dwButtons = 0;

	// try to get information from 8 mouse buttons
	// if GetObjectInfo fails, there is not button
	// requested
	int i;
	for(i=0;i<8;i++)
	{
		if(SUCCEEDED(m_pDevice->GetObjectInfo(&didoi, DIMOFS_BUTTON0 + i, DIPH_BYOFFSET)))
			dwButtons++;
	}

	return dwButtons;
}

/**
 * returns TRUE if mouse button is down
 **/
BOOL CInputMouse::GetButtonDown(DWORD dwButton)
{
	return m_state.rgbButtons[dwButton] ? TRUE : FALSE;
}

void CInputMouse::ResetMouseState()
{
	for(int i=0;i<8;++i)
		m_buttonStates[i] = MOUSE_BUTTON_UP;
}