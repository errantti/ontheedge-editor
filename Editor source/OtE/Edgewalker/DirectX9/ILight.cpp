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

// ILight.cpp: implementation of the ILight class.
//
//////////////////////////////////////////////////////////////////////

#include "ILight.h"
#include "DirectX9.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ILight *ILight::m_lights[MAXIMUM_LIGHT_COUNT] = {0, 0, 0, 0};

ILight::ILight()
: m_bEnabled(FALSE), m_iIndex(-1), m_bSet(FALSE), m_bStatic(TRUE)
{
	bool foundSpot = false;
	for(int i=0; i<MAXIMUM_LIGHT_COUNT; ++i)
		if(m_lights[i] == 0)
		{
			m_lights[i] = this;
			UpdateLightIndex(i);
			foundSpot = true;
			break;
		}
	if(!foundSpot)
	{
		CPrinter::logError("Failed creating light: too many lights in the scene");
	}
}

ILight::~ILight()
{
	if(m_iIndex >= 0)
	{
		for(int i=0; i<MAXIMUM_LIGHT_COUNT; ++i)
			if(m_lights[i] == this)
			{
				m_lights[i] = 0;
				Enable(false, GetDXApp()->GetDevice());
				m_iIndex = -1;
			}
	}
}

void ILight::DisableDynamicLights(IDirect3DDevice9 *pd3dd)
{
	for(int i=0; i<MAXIMUM_LIGHT_COUNT; ++i)
		if(m_lights[i] != 0)
		{
			if(!m_lights[i]->GetStatic())
				m_lights[i]->Enable(false, pd3dd);
		}
}

void ILight::Enable(bool bEnabled, IDirect3DDevice9 *pd3dd)
{
	if(!m_bSet || m_iIndex < 0 || m_iIndex >= MAXIMUM_LIGHT_COUNT)
		return;

	if(bEnabled == FALSE)
	{
		if(m_bEnabled == FALSE)
			return;
		m_bEnabled = FALSE;
		if(pd3dd)
			pd3dd->LightEnable(m_iIndex, FALSE);
	}
	else
	{
		if(pd3dd)
		{
			m_bEnabled = TRUE;
			pd3dd->SetLight(m_iIndex, &m_light);
			pd3dd->LightEnable(m_iIndex, TRUE);
		}
		else
			m_bEnabled = FALSE;
	}
}