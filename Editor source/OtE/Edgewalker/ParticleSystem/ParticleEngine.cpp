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

#include "ParticleEngine.h"
#include <algorithm>

using namespace std;

CParticleEngine *CParticleEngine::m_this = 0;

CParticleEngine::CParticleEngine(ISceneManager &manager) : m_scnManager(manager), 
	m_spriteParticleCount(0), m_pD3DDevice(NULL)
{
	if(m_this == NULL)
		m_this = this;
}

CParticleEngine::~CParticleEngine()
{
	if(m_this == this)
		m_this = NULL;
}

void CParticleEngine::InitCache()
{
	m_cache.CreateBuffer();
	m_pD3DDevice = GetDXApp()->GetDevice();
}

void CParticleEngine::RegisterEmitterForRendering(CParticleEmitter &emitter)
{
	if(emitter.GetParticleCount() == 0)
		return;

	if(emitter.IsMeshEmitter())
	{
		/**
		 * Mesh emitters
		 **/
		m_meshEmitters.push_back(emitter);
	}
	else
	{
		/**
		 * Point sprite emitters
		 **/
		m_spriteParticleCount += emitter.GetParticleCount();

		m_spriteEmitters.push_back(emitter);
	}
}

void CParticleEngine::Render()
{
	if(m_meshEmitters.size() != 0)
	{
		/**
		* Mesh emitters
		**/
		sort(m_meshEmitters.begin(), m_meshEmitters.end());

		// Render
		vector<SMeshEmitterRef>::iterator it = m_meshEmitters.begin();
		CTexture *lastTexture = NULL;

		SetMeshParticleRenderStates();

		for(; it != m_meshEmitters.end(); ++it)
		{
			if(lastTexture != it->texture)
			{
				lastTexture = it->texture;
				if(FAILED(m_pD3DDevice->SetTexture(0, lastTexture->GetTexture())))
				{
					//CPrinter::log("Failed setting mesh particle texture");
				}
			}
			it->emitter->RenderMeshes();
		}

		m_meshEmitters.clear();
	}

	/**
	 * Sprite emitters
	 **/
	m_cache.SetParticleRenderStates(0.f);
	RenderSpriteEmittersOptimized();

	m_cache.RestoreDefaultRenderStates();/**/
}

void CParticleEngine::RenderSpriteEmittersOptimized()
{
	if(m_spriteParticleCount == 0 || m_spriteEmitters.size() == 0)
		return;

	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &m);

	/**
	 * Sprite emitters
	 **/
	sort(m_spriteEmitters.begin(), m_spriteEmitters.end());

	if(!m_cache.Lock(m_spriteParticleCount))
	{
		CPrinter::log("RenderSpriteEmittersOptimized: failed locking vertex buffer", LOG_ERROR);
		return;
	}

	BYTE *ptr = m_cache.GetVertexBufferDataPointer();
	CParticleCache::POINTSPRITEVERTEX *particleBuffer = (CParticleCache::POINTSPRITEVERTEX*)ptr;
	int partAmount = 0;

	// Fill vertex buffer data and hope there isn't overflow
	vector<SSpriteEmitterRef>::iterator it = m_spriteEmitters.begin();
	for(; it != m_spriteEmitters.end(); ++it)
	{
		partAmount = it->emitter->PopulateParticleVertexBuffer(particleBuffer);
		particleBuffer += partAmount;
	}

	m_cache.Unlock();

	// Render, minimize the amount of DrawPrimitive calls
	unsigned int offset = 0, amount = 0;
	CTexture *lastTexture = m_spriteEmitters.front().texture;
	bool changed = false;
	bool lastAlphaModeAdditive = m_spriteEmitters.front().additiveBlend;

	// Start rendering after setting initial settings
	m_pD3DDevice->SetTexture(0, lastTexture->GetTexture());
	m_cache.EnabledAdditiveBlend(lastAlphaModeAdditive);

	m_cache.StartRender();

	for(it = m_spriteEmitters.begin(); it != m_spriteEmitters.end(); ++it)
	{
		if(it->texture != lastTexture)
			changed = true;
		if(it->additiveBlend != lastAlphaModeAdditive)
			changed = true;

		if(changed)
		{
			// Render the batch
			m_cache.Render(offset, amount);
			offset += amount;
			amount = 0;
			changed = false;
			if(it->texture != lastTexture)
			{
				lastTexture = it->texture;
				if(FAILED(m_pD3DDevice->SetTexture(0, lastTexture->GetTexture())))
				{
					CPrinter::log("Failed setting particle texture", LOG_ERROR);
				}
			}
			if(it->additiveBlend != lastAlphaModeAdditive)
			{
				lastAlphaModeAdditive = it->additiveBlend;
				m_cache.EnabledAdditiveBlend(lastAlphaModeAdditive);
			}
		}
		amount += it->emitter->GetParticleCount();
	}
	if(amount > 0)
	{
		// Render the last batch
		m_cache.Render(offset, amount);
	}

	m_spriteParticleCount = 0;
	m_spriteEmitters.clear();
}

void CParticleEngine::SetMeshParticleRenderStates()
{
	IDirect3DDevice9 *pDevice = GetDXApp()->GetDevice();

	// set the 3d device settings for the point sprites
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// Blend options
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
	//pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	//pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
}