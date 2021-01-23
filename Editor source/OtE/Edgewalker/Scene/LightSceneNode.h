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

#ifndef __CLIGHTSCENENODE_H__
#define __CLIGHTSCENENODE_H__

#include "ISceneNode.h"
#include "..\\DirectX9\\ILight.h"
#include "..\\DirectX9\\IDirectXResource.h"

struct SDynamicPointLightParams
{
	struct SState
	{
		D3DCOLORVALUE color;
		float range;
		float durationMin, durationMax;
	};

	char name[32];
	SState states[2];
	float transitionDurMin, transitionDurMax;
	float attenuation1Coef, attenuation0;
	bool stateChangeEnabled;
	long id;

	void FillDefaults()
	{
		name[0] = '\0';
		states[0].color = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
		states[0].durationMin = states[0].durationMax = 0.5f;
		states[0].range = 150.f;
		states[1].color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		states[1].durationMin = states[1].durationMax = 0.5f;
		states[1].range = 250.f;
		transitionDurMax = transitionDurMin = 1.5f;
		attenuation1Coef = 0.5f;
		attenuation0 = 0.f;
		stateChangeEnabled = true;
		id = -1;
	}
};

class CLightSceneNode : public ISceneNode, public ILight, public IDirectXResource
{
public:
	CLightSceneNode(ISceneNode *parent = 0, ISceneManager *manager = 0, bool bStatic = TRUE,
		D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DCOLOR color = D3DCOLOR_RGBA(255, 255, 255, 255),
		float radius = 500.0f, DWORD id = -1);
	virtual ~CLightSceneNode();

	void Render(bool isShadowPass);
	aabbox3d &GetBoundingBox() {return m_bbox;}

	void OnPreRender(bool isShadowPass);
	void Update(float frameTime);

	void OnDeviceLost();
	void OnDeviceReset();

	void InitDynamicPointLight(SDynamicPointLightParams &p);

protected:
	aabbox3d m_bbox;

	SDynamicPointLightParams *m_params;
	D3DCOLORVALUE m_interpolateColor;
	float m_interpolateRange;
	float m_interpolateAttenuation;
	float m_attenuation1Coefficient;

	int m_lastState;
	bool m_bTransitionPhase;
	float m_duration;
};

#endif // #ifndef __CLIGHTSCENENODE_H__
