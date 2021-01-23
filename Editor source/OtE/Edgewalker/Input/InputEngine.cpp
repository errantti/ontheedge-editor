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

#include "InputEngine.h"
using namespace std;

// declare the static member pointer
CInputEngine *CInputEngine::m_pInputEngine = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputEngine::CInputEngine()
: m_pDI(NULL), m_hWnd(NULL)
{
	m_pInputEngine = this;
}

CInputEngine::~CInputEngine()
{
	Release();
	m_pInputEngine = NULL;
}

HRESULT CInputEngine::Create(HWND hWnd)
{
	HRESULT hres;
	HINSTANCE hInst;

	m_hWnd = hWnd;
	hInst = ::GetModuleHandle(NULL);

	hres = ::DirectInput8Create(hInst, 0x800, IID_IDirectInput8, (void**)&m_pDI, NULL);

	return hres;
}

void CInputEngine::Release()
{
	if(m_pDI)
	{
		m_pDI->Release();
		m_pDI = NULL;
	}
}

/**
 * gets all attached keyboard devices
 * returns number of devices found
 **/
int CInputEngine::GetKeyboards(vector<INPUT_DEVICE> *parrDevices)
{
	if(m_pDI == NULL)
		return 0;

	m_pDI->EnumDevices(DI8DEVCLASS_KEYBOARD, EnumDeviceCallback, parrDevices, DIEDFL_ATTACHEDONLY);

	return parrDevices->size();
}

/**
 * gets all attached mouse devices
 * returns number of devices found
 **/
int CInputEngine::GetMice(vector<INPUT_DEVICE> *parrDevices)
{
	if(m_pDI == NULL)
		return 0;

	m_pDI->EnumDevices(DI8DEVCLASS_POINTER, EnumDeviceCallback, parrDevices, DIEDFL_ATTACHEDONLY);

	return parrDevices->size();
}

/**
 * gets all attached joystick devices
 * returns number of devices found
 **/
int CInputEngine::GetJoysticks(vector<INPUT_DEVICE> *parrDevices)
{
	if(m_pDI == NULL)
		return 0;

	m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDeviceCallback, parrDevices, DIEDFL_ATTACHEDONLY);

	return parrDevices->size();
}

int WINAPI CInputEngine::EnumDeviceCallback(LPCDIDEVICEINSTANCE pInst, void *pParam)
{
	// Get the array of devices from the parameter
	vector<INPUT_DEVICE> *parrDevices = (vector<INPUT_DEVICE>*)pParam;

	// copy the nbeede information from the pInstance
	// into our own structure
	INPUT_DEVICE d;
	memcpy(&d.gdDevice, &pInst->guidInstance, sizeof(GUID));
	_tcsncpy(d.strName, pInst->tszProductName, 256);
	parrDevices->push_back(d);

	return DIENUM_CONTINUE;
}

void CInputEngine::AddInputReceiver(IInputReceiver *receiver)
{
	if(receiver == 0)
		return;
	if(m_inputStack.size() > 0)
		m_inputStack[m_inputStack.size()-1]->OnInputDeActivation();
	m_inputStack.push_back(receiver);
	receiver->OnInputActivation();
}

void CInputEngine::RemoveInputReceiver(IInputReceiver *receiver)
{
	if(receiver == 0)
		return;
	vector<IInputReceiver*>::iterator it = m_inputStack.begin(), it2;
	for(; it != m_inputStack.end(); ++it)
	{
		if(*it == receiver)
		{
			it2 = it;
			if(++it2 == m_inputStack.end())
			{
				(*it)->OnInputDeActivation();
				m_inputStack.erase(it);
				if(m_inputStack.size() > 0)
					m_inputStack[m_inputStack.size()-1]->OnInputActivation();
				return;
			}
			else
			{
				m_inputStack.erase(it);
				return;
			}
		}
	}
}

void CInputEngine::PriorizeReceiver(IInputReceiver *receiver)
{
	if(m_inputStack.size() == 0)
		return;
	vector<IInputReceiver*>::iterator it = m_inputStack.begin(), it2;
	for(; it != m_inputStack.end(); ++it)
		if(*it == receiver)
		{
			m_inputStack[m_inputStack.size()-1]->OnInputDeActivation();
			m_inputStack.erase(it);
			m_inputStack.push_back(receiver);
			receiver->OnInputActivation();
			return;
		}
}

void CInputEngine::UpdateReceivers()
{
	if(m_inputStack.size() > 0)
		m_inputStack[m_inputStack.size()-1]->ProcessInput();
}