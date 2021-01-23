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

#include "LightSceneNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightSceneNode::CLightSceneNode(ISceneNode *parent, ISceneManager *manager, bool bStatic,
		D3DXVECTOR3 pos, D3DCOLOR color, float radius, DWORD id)
	: ISceneNode(parent, manager, id, pos), m_attenuation1Coefficient(0.5f)
{
	m_bStatic = bStatic;
}

CLightSceneNode::~CLightSceneNode()
{
}

void CLightSceneNode::Render(bool isShadowPass)
{
	CDirectX9 *dx = GetDXApp();
	if(dx == NULL)
		return;

	if(m_bStatic)
	{
		if(!m_bEnabled && m_bSet)
		{
			Enable(TRUE, dx->GetDevice());
			return;
		}
	}
	else
	{
		// Dynamic lights (only point light enabled)
		if(!m_params)
			return;
		Enable(true, dx->GetDevice());
	}
}

void CLightSceneNode::Update(float frameTime)
{
	if(!m_params || m_bStatic)
		return;

	// Update the light data
	m_light.Position.x = m_absoluteTransformation._41;
	m_light.Position.y = m_absoluteTransformation._42;
	m_light.Position.z = m_absoluteTransformation._43;

	/*D3DXVec3TransformCoord(&m_light.Position, &m_light.Position, m_parent->GetAbsoluteTransformation());*/

	int newState = 1;

	if(m_params->stateChangeEnabled)
	{
		m_duration -= frameTime;
		if(m_bTransitionPhase)
		{
			if(m_duration < 0.f)
			{
				if(m_lastState == 1)
					newState = 0;
				m_light.Range = m_params->states[newState].range;
				m_light.Attenuation1 = m_attenuation1Coefficient / m_params->states[newState].range;
				m_light.Specular = m_light.Diffuse = m_params->states[newState].color;
				m_duration = GetDXApp()->RandFloat(m_params->states[newState].durationMin, m_params->states[newState].durationMax);
				m_lastState = newState;
				m_bTransitionPhase = false;
			}
			else
			{
				m_light.Range += frameTime * m_interpolateRange;
				m_light.Diffuse.r += frameTime * m_interpolateColor.r;
				m_light.Diffuse.g += frameTime * m_interpolateColor.g;
				m_light.Diffuse.b += frameTime * m_interpolateColor.b;
				m_light.Diffuse.a += frameTime * m_interpolateColor.a;
				//m_light.Ambient = m_light.Specular = m_light.Diffuse;
				m_light.Attenuation1 = m_attenuation1Coefficient / m_light.Range;
				//m_light.Attenuation1 += frameTime * m_interpolateAttenuation;
				//m_light.Specular = m_light.Diffuse;
			}
		}
		else
		{
			if(m_duration < 0.f)
			{
				m_duration = GetDXApp()->RandFloat(m_params->transitionDurMin, m_params->transitionDurMax);
				if(m_lastState == 1)
					newState = 0;

				if(m_duration > 0.1f)
				{
					// Step into transition phase
					m_bTransitionPhase = true;
					float ratio = 1.f / m_duration;
					m_interpolateRange = (m_params->states[newState].range - m_params->states[m_lastState].range) * ratio;
					m_interpolateAttenuation = (m_attenuation1Coefficient / m_params->states[newState].range - 
						m_attenuation1Coefficient / m_params->states[m_lastState].range) * ratio;
					m_interpolateColor.r = (m_params->states[newState].color.r - m_params->states[m_lastState].color.r) * ratio;
					m_interpolateColor.g = (m_params->states[newState].color.g - m_params->states[m_lastState].color.g) * ratio;
					m_interpolateColor.b = (m_params->states[newState].color.b - m_params->states[m_lastState].color.b) * ratio;
					m_interpolateColor.a = (m_params->states[newState].color.a - m_params->states[m_lastState].color.a) * ratio;
					return;
				}

				// Simple change state immediately
				m_light.Range = m_params->states[newState].range;
				m_light.Specular = m_light.Diffuse = m_params->states[newState].color;
				m_duration = GetDXApp()->RandFloat(m_params->states[newState].durationMin, m_params->states[newState].durationMax);
				m_lastState = newState;
			}
		}
	}
}

void CLightSceneNode::OnPreRender(bool isShadowPass)
{
	if(GetVisible() && !isShadowPass)
	{
		m_manager->RegisterNodeForRendering(this, SNG_LIGHT);
		ISceneNode::OnPreRender(isShadowPass);
	}
}

void CLightSceneNode::InitDynamicPointLight(SDynamicPointLightParams &p)
{
	m_bStatic = false;
	m_params = &p;
	m_lastState = 0;
	m_bTransitionPhase = false;
	m_attenuation1Coefficient = p.attenuation1Coef;
	m_duration = GetDXApp()->RandFloat(p.states[0].durationMin, p.states[0].durationMax);
	ZeroMemory(&m_light, sizeof(D3DLIGHT9));
	m_light.Falloff = 10.f;
	m_light.Range = p.states[0].range;
	m_light.Specular = p.states[0].color;
	//m_light.Ambient = p.states[0].color;
	m_light.Diffuse = p.states[0].color;
	m_light.Type = D3DLIGHT_POINT;
	m_light.Position.x = m_absoluteTransformation._41;
	m_light.Position.y = m_absoluteTransformation._42;
	m_light.Position.z = m_absoluteTransformation._43;
	m_light.Attenuation0 = 0.f;
	m_light.Attenuation1 = m_attenuation1Coefficient / m_light.Range;
	m_light.Attenuation2 = 0.f;
	m_bSet = true;
}

void CLightSceneNode::OnDeviceLost()
{
}

void CLightSceneNode::OnDeviceReset()
{
	if(GetVisible() && m_bSet && m_bEnabled)
	{
		Enable(TRUE, GetDXApp()->GetDevice());
	}
}