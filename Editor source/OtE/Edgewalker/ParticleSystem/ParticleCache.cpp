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

#include "ParticleCache.h"
#include "..\\DirectX9\\DirectX9.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticleCache::CParticleCache() : m_vb(NULL), m_bufferPtr(NULL),
	m_bufferSize(0)
{
}

CParticleCache::~CParticleCache()
{
	Release();
}

void CParticleCache::Release()
{
	ReleaseVertexBuffer();
	/*if(m_temporaryBuffer)
	{
		delete [] m_temporaryBuffer;
		m_temporaryBuffer = NULL;
	}*/
}

void CParticleCache::ReleaseVertexBuffer()
{
	if(m_bufferPtr)
		Unlock();
	if(m_vb)
	{
		m_vb->Release();
		m_vb = NULL;
	}
}

bool CParticleCache::CreateBuffer(unsigned int size)
{
	Release();

	m_pD3DDevice = GetDXApp()->GetDevice();
	if(m_pD3DDevice == NULL)
		return FALSE;
	if(size < BUFFER_BLOCK_SIZE)
		size = BUFFER_BLOCK_SIZE;
	if(size > MAXIMUM_PARTICLE_COUNT)
		size = MAXIMUM_PARTICLE_COUNT;

	m_bufferSize = size;

	if(CreateVertexBuffer() == FALSE)
		return FALSE;

	/*m_temporaryBuffer = new POINTSPRITEVERTEX[size];
	if(m_temporaryBuffer == NULL)
		return FALSE;

	m_nextTemporaryIndex = 0;*/

	return TRUE;
}

bool CParticleCache::CreateVertexBuffer()
{
	ReleaseVertexBuffer();

	IDirect3DDevice9 *device = GetDXApp()->GetDevice();
	if(device == NULL || m_bufferSize<1)
		return FALSE;

	if(FAILED(device->CreateVertexBuffer(sizeof(POINTSPRITEVERTEX)*m_bufferSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
		D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE, D3DPOOL_SYSTEMMEM, &m_vb, NULL)))
	{
		m_vb = NULL;
		CPrinter::log("Failed creating particle cache dynamic vertex buffer", LOG_ERROR);
		return FALSE;
	}

	return TRUE;
}

bool CParticleCache::Lock(int size)
{
	/*static int num = 0;
	++num;
	if(num > 6)
		num = num;*/
	// If capacity requirement is greater than the current size
	// automatically recreate the buffer one block larger
	if(size > m_bufferSize)
	{
		if(size <= MAXIMUM_PARTICLE_COUNT)
		{
			ReleaseVertexBuffer();
			m_bufferSize += BUFFER_BLOCK_SIZE;
			if(!CreateVertexBuffer())
				return false;
		}
	}
	if(FAILED(m_vb->Lock(0, sizeof(POINTSPRITEVERTEX)*size, (VOID**)&m_bufferPtr, D3DLOCK_DISCARD)))
	//if(FAILED(m_vb->Lock(0, 0, (VOID**)&m_bufferPtr, D3DLOCK_DISCARD)))
	{
		m_bufferPtr = NULL;
		CPrinter::log("Failed locking particle cache vertex buffer", LOG_ERROR);
		return FALSE;
	}
	return TRUE;
}

void CParticleCache::Unlock()
{
	m_vb->Unlock();
	m_bufferPtr = NULL;
}

/**
 * 
 *
void CParticleCache::AddParticle(POINTSPRITEVERTEX &p)
{
	m_temporaryBuffer[m_nextTemporaryIndex] = p;
	++m_nextTemporaryIndex;
}

void CParticleCache::AddParticleSafe(POINTSPRITEVERTEX &p)
{
	if(m_nextTemporaryIndex >= m_bufferSize || m_temporaryBuffer == NULL)
		return;
	m_temporaryBuffer[m_nextTemporaryIndex] = p;
	++m_nextTemporaryIndex;
}

void CParticleCache::Render()
{
	IDirect3DDevice9 *device = GetDXApp()->GetDevice();

	if(m_temporaryBuffer == NULL || m_nextTemporaryIndex < 1 || m_vb == NULL)
	{
		m_nextTemporaryIndex = 0;
		return;
	}

	if(!Lock())
	{
		m_nextTemporaryIndex = 0;
		return;
	}
	memcpy(m_bufferPtr, m_temporaryBuffer, sizeof(POINTSPRITEVERTEX)*m_nextTemporaryIndex);

	Unlock();

	// Render
	device->SetStreamSource(0, m_vb, 0, sizeof(POINTSPRITEVERTEX));
	device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE);
	//device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	device->DrawPrimitive(D3DPT_POINTLIST, 0, m_nextTemporaryIndex);

	// "Empty" the temporary buffer
	m_nextTemporaryIndex = 0;
}*/

void CParticleCache::StartRender()
{
	m_pD3DDevice->SetStreamSource(0, m_vb, 0, sizeof(POINTSPRITEVERTEX));
	m_pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_PSIZE);
}

void CParticleCache::Render(unsigned int offset, unsigned int amount)
{
	m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, offset, amount);
}

void CParticleCache::EnabledAdditiveBlend(bool enabled)
{
	IDirect3DDevice9 *pDevice = GetDXApp()->GetDevice();

	if(enabled)
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	else
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void CParticleCache::SetParticleRenderStates(float fMinSize)
{
	IDirect3DDevice9 *pDevice = GetDXApp()->GetDevice();

	// set the 3d device settings for the point sprites
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	//pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Blend options
	//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	//pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	//pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE4X);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	// set the point sprite settings
	pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

	pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&fMinSize));
	float max = 1000.f;
	pDevice->SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&max));
	pDevice->SetRenderState(D3DRS_POINTSCALE_A, *((DWORD*)&fMinSize));
	pDevice->SetRenderState(D3DRS_POINTSCALE_B, *((DWORD*)&fMinSize));
	fMinSize = 1.0f;
	pDevice->SetRenderState(D3DRS_POINTSCALE_C, *((DWORD*)&fMinSize));/**/

	fMinSize = 10.f;
	pDevice->SetRenderState(D3DRS_POINTSIZE, *((DWORD*)&fMinSize));
}

void CParticleCache::RestoreDefaultRenderStates()
{
	IDirect3DDevice9 *pDevice = GetDXApp()->GetDevice();

	// set the basic 3d device settings
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// set the point sprite settings
	pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);

	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
}

void CParticleCache::OnDeviceLost()
{
	ReleaseVertexBuffer();
}

void CParticleCache::OnDeviceReset()
{
	CreateVertexBuffer();
}