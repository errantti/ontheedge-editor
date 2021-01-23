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

#include "InputJoystickEffect.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputJoystickEffect::CInputJoystickEffect()
: m_pJoystick(NULL)
{
	::memset(&m_effectList, 0, sizeof(EFFECT_NODE));
	m_effectList.pNext = &m_effectList;
}

CInputJoystickEffect::~CInputJoystickEffect()
{
}

HRESULT CInputJoystickEffect::Create(CInputJoystick *pJoystick, LPCTSTR strFilename)
{
	Release();

	m_pJoystick = pJoystick;

	HRESULT hres;
	hres = m_pJoystick->GetDevice()->EnumEffectsInFile(strFilename, 
		EnumFunction, this, DIFEF_MODIFYIFNEEDED);

	if(FAILED(hres))
	{
		::OutputDebugString("EFFECT - EnumEffectsInFile failed");
		return hres;
	}

	if(m_effectList.pNext == &m_effectList)
	{
		// no effects loaded...
		::OutputDebugString("EFFECT - no effects in file.\r\n");
		return E_FAIL;
	}
	return S_OK;
}

void CInputJoystickEffect::Release()
{
	if(m_effectList.pNext != &m_effectList)
	{
		EFFECT_NODE *pNode = m_effectList.pNext;
		EFFECT_NODE *pDelete;

		while(pNode != &m_effectList)
		{
			pDelete = pNode;
			pNode = pNode->pNext;

			if(pDelete->pEffect)
			{
				pDelete->pEffect->Release();
			}
			delete pDelete;
		}

		m_effectList.pNext = &m_effectList;
	}
}


BOOL CALLBACK CInputJoystickEffect::EnumFunction(LPCDIFILEEFFECT pFileEffect, void *pParam)
{
	CInputJoystickEffect *pEffect = (CInputJoystickEffect*)pParam;

	HRESULT hres;
	LPDIRECTINPUTEFFECT pNewEffect = NULL;
	LPDIRECTINPUTDEVICE8 pDevice = pEffect->m_pJoystick->GetDevice();

	hres = pDevice->CreateEffect(pFileEffect->GuidEffect, pFileEffect->lpDiEffect, &pNewEffect, NULL);
	if(FAILED(hres))
	{
		::OutputDebugString("Enum - failed to create effect.\r\n");
		return DIENUM_CONTINUE;
	}

	// allocate new node for the linked list
	EFFECT_NODE *pNode = new EFFECT_NODE;
	if(pNode == NULL)
		return DIENUM_STOP;

	// fill the new effect node
	::memset(pNode, 0, sizeof(EFFECT_NODE));
	pNode->pEffect = pNewEffect;
	pNode->dwRepeatCount = 1;

	// link new node to the list
	pNode->pNext = pEffect->GetEffectList()->pNext;
	pEffect->GetEffectList()->pNext = pNode;
	return DIENUM_CONTINUE;
}

// plays the force feedback effect. Any previous
// effects are stopped first
HRESULT CInputJoystickEffect::Play()
{
	HRESULT hres;
	LPDIRECTINPUTEFFECT pEffect = NULL;
	EFFECT_NODE *pNode = m_effectList.pNext;

	Stop();

	// iterate through the effect list and play 'em all
	while(pNode != &m_effectList)
	{
		pEffect = pNode->pEffect;

		if(pEffect)
		{
			hres = pEffect->Start(pNode->dwRepeatCount, 0);
			if(FAILED(hres))
			{
				return hres;
			}
		}

		pNode = pNode->pNext;
	}
	return S_OK;
}

void CInputJoystickEffect::Stop()
{
	// stop all playing effects
	if(m_pJoystick)
	{
		m_pJoystick->GetDevice()->SendForceFeedbackCommand(DISFFC_STOPALL);
	}
}
