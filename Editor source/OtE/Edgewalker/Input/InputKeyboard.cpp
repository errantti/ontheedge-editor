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

#include "InputKeyboard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputKeyboard::CInputKeyboard()
: m_pDevice(NULL)
{
	::memset(m_byKeys, 0, 256);
}

CInputKeyboard::~CInputKeyboard()
{
}

/**
 * initialize the input object
 **/
HRESULT CInputKeyboard::Create(CInputEngine::INPUT_DEVICE *pDevice, DWORD dwFlags)
{
	// get the direct input
	LPDIRECTINPUT8 pDI = CInputEngine::GetInputEngine()->GetDirectInput();
	if(pDI == NULL)
		return E_FAIL;

	HRESULT hres;
	GUID gdDevice;

	// init the keyboard GUID
	gdDevice = GUID_SysKeyboard;
	if(pDevice)
		gdDevice = pDevice->gdDevice;

	hres = pDI->CreateDevice(gdDevice, &m_pDevice, NULL);

	if(FAILED(hres))
	{
		::OutputDebugString("KEYBOARD - failed to create direct input device.\r\n");
		return hres;
	}

	// set the input device data format
	hres = m_pDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hres))
	{
		::OutputDebugString("KEYBOARD - SetDataFormat failed");
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
		::OutputDebugString("KEYBOARD - SetCooperativeLevel failed");
		return hres;
	}

	// acquire the input device
	hres = m_pDevice->Acquire();
	::memset(m_byKeys, 0, 256);

	return S_OK;
}

void CInputKeyboard::Release()
{
	if(m_pDevice)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}
}

// get the current keyboard state
BOOL CInputKeyboard::Update()
{
	if(m_pDevice == NULL)
		return FALSE;

	HRESULT hres;

	hres = m_pDevice->GetDeviceState(256, m_byKeys);

	// if getting the device state fails, most likely
	// another application has gained the keyboard
	// focus and we have lost it.
	if(FAILED(hres))
	{
		// failed to get keyboard state
		// try to acquire the device back to us.
		m_pDevice->Acquire();

		hres = m_pDevice->GetDeviceState(256, m_byKeys);

		if(FAILED(hres))
		{
			// nothing much we can do about this..
			::memset(m_byKeys, 0, 256);
			return FALSE;
		}
	}

	return TRUE;
}

// return TRUE if button dwDIK is down
BOOL CInputKeyboard::GetButtonDown(DWORD dwDIK)
{
	return m_byKeys[dwDIK] ? TRUE : FALSE;
}
