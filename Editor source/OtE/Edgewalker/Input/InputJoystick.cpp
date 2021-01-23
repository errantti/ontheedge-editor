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

#include "InputJoystick.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputJoystick::CInputJoystick()
: m_bIsForce(FALSE), m_pDevice(NULL), m_dwButtonCount(0)
{
	::memset(&m_state, 0, sizeof(DIJOYSTATE2));
}

CInputJoystick::~CInputJoystick()
{
}

void CInputJoystick::Release()
{
	if(m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}
}

BOOL CInputJoystick::Update()
{
	if(m_pDevice == NULL)
		return FALSE;

	HRESULT hres;

	hres = m_pDevice->GetDeviceState(sizeof(DIJOYSTATE2), &m_state);

	// if getting the device state fails, most likely
	// another application has gained the joystick
	// focus and we have lost it.
	if(FAILED(hres))
	{
		// failed to get joystick state
		// try to acquire the device back to us.
		m_pDevice->Acquire();

		hres = m_pDevice->GetDeviceState(sizeof(DIJOYSTATE2), &m_state);

		if(FAILED(hres))
		{
			// nothing much we can do about this..
			::memset(&m_state, 0, sizeof(DIJOYSTATE2));
			return FALSE;
		}
	}
	/*
	for(int i=0;i<20;++i)
	{
		if(m_state.rgbButtons[i] != 0)
			int b=1;
	}*/

	return TRUE;
}

BOOL CInputJoystick::GetButtonDown(DWORD dwButton)
{
	return m_state.rgbButtons[dwButton] ? TRUE : FALSE;
}

/**
 * initialize the input object
 **/
HRESULT CInputJoystick::Create(CInputEngine::INPUT_DEVICE *pDevice, DWORD dwFlags)
{
	// get the direct input
	LPDIRECTINPUT8 pDI = CInputEngine::GetInputEngine()->GetDirectInput();
	if(pDI == NULL)
		return E_FAIL;

	HRESULT hres;
	GUID gdDevice;

	gdDevice = pDevice->gdDevice;

	hres = pDI->CreateDevice(gdDevice, &m_pDevice, NULL);

	if(FAILED(hres))
	{
		::OutputDebugString("JOYSTICK - failed to create direct input device.\r\n");
		return hres;
	}

	// set the input device data format
	hres = m_pDevice->SetDataFormat(&c_dfDIJoystick2);
	if(FAILED(hres))
	{
		::OutputDebugString("JOYSTICK - SetDataFormat failed");
		return hres;
	}

	// set the co-operative level
	if(dwFlags == 0)
	{
		// NONEXCLUSIVE:
		// gives access to input device also to other applications
		// FOREGROUND:
		// we get joystick data only when in foreground
		dwFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
	}

	hres = m_pDevice->SetCooperativeLevel(CInputEngine::GetInputEngine()->GetWindow(), dwFlags);
	if(FAILED(hres))
	{
		::OutputDebugString("JOYSTICK - SetCooperativeLevel failed");
		return hres;
	}

	// check whether this joystick supports force feedback
	DIDEVCAPS caps;
	::memset(&caps, 0, sizeof(DIDEVCAPS));
	caps.dwSize = sizeof(DIDEVCAPS);

	m_pDevice->GetCapabilities(&caps);
	if(caps.dwFlags & DIDC_FORCEFEEDBACK)
	{
		m_bIsForce = TRUE;
	}

	// count the number of buttons this joystick device has
	m_dwButtonCount = CountButtons();

	// acquire the input device
	hres = m_pDevice->Acquire();

	return S_OK;
}

DWORD CInputJoystick::CountButtons()
{
	DIDEVICEOBJECTINSTANCE	didoi;

	::memset(&didoi, 0, sizeof(DIDEVICEOBJECTINSTANCE));
	didoi.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);

	DWORD dwButtons = 0;

	// try to get information from buttons
	// if GetObjectInfo fails, there is not button
	// requested
	int i;
	for(i=0;i<128;i++)
	{
		if(SUCCEEDED(m_pDevice->GetObjectInfo(&didoi, DIJOFS_BUTTON(i), DIPH_BYOFFSET)))
			dwButtons++;
	}

	return dwButtons;
}

void CInputJoystick::SetAxisRange(DWORD dwAxis, int iMin, int iMax)
{
	if(m_pDevice == NULL)
	{
		return;
	}

	// device must be unacquired when changing parameters
	m_pDevice->Unacquire();

	DIPROPRANGE range;
	::memset(&range, 0, sizeof(DIPROPRANGE));
	range.diph.dwSize = sizeof(DIPROPRANGE);
	range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	range.diph.dwHow = DIPH_BYOFFSET;
	range.diph.dwObj = dwAxis;

	range.lMax = iMax;
	range.lMin = iMin;

	m_pDevice->SetProperty(DIPROP_RANGE, &range.diph);
	m_pDevice->Acquire();
}

void CInputJoystick::SetDeadZone(DWORD dwAxis, int iDeadzone)
{
	if(m_pDevice == NULL)
	{
		return;
	}

	// device must be unacquired when changing parameters
	m_pDevice->Unacquire();

	DIPROPDWORD prop;
	::memset(&prop, 0, sizeof(DIPROPDWORD));
	prop.diph.dwSize = sizeof(DIPROPDWORD);
	prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	prop.diph.dwHow = DIPH_BYOFFSET;
	prop.diph.dwObj = dwAxis;

	prop.dwData = iDeadzone;

	m_pDevice->SetProperty(DIPROP_DEADZONE, &prop.diph);
	m_pDevice->Acquire();
}

HRESULT CInputJoystick::FF_Reset()
{
	if(IsForce() == FALSE || m_pDevice == NULL)
	{
		return DIERR_UNSUPPORTED;
	}

	HRESULT hres;
	m_pDevice->Unacquire();

	// turn off the autocenter spring
	DIPROPDWORD prop;
	::memset(&prop, 0, sizeof(DIPROPDWORD));
	prop.diph.dwSize = sizeof(DIPROPDWORD);
	prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	prop.diph.dwHow = DIPH_DEVICE;

	hres = m_pDevice->SetProperty(DIPROP_AUTOCENTER, &prop.diph);
	if(FAILED(hres))
	{
		::OutputDebugString(_T("JOYSTICK - SetProperty(autocenter) failed\r\n"));
		return hres;
	}

	hres = m_pDevice->SendForceFeedbackCommand(DISFFC_RESET);
	if(FAILED(hres))
	{
		::OutputDebugString(_T("JOYSTICK - FF reset failed failed\r\n"));
		return hres;
	}

	m_pDevice->Acquire();
	return S_OK;
}
