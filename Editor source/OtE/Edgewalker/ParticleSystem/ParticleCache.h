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

#ifndef __CPARTICLECACHE_H__
#define __CPARTICLECACHE_H__

#include <d3dx9.h>
#include "..\\DirectX9\\IDirectXResource.h"

#define BUFFER_BLOCK_SIZE		4000
#define MAXIMUM_PARTICLE_COUNT	50000

class CParticleCache : public IDirectXResource
{
public:
	struct POINTSPRITEVERTEX
	{
		D3DXVECTOR3		vPosition;
		float			pointSize;
		DWORD			dwDiffuse;
	};

	CParticleCache();
	~CParticleCache();

	void SetParticleRenderStates(float fMinSize = 0.f);
	void RestoreDefaultRenderStates();
	void EnabledAdditiveBlend(bool enabled);
	void Release();
	//void Render();

	bool Lock(int size);
	void Unlock();
	void StartRender();
	void Render(unsigned int offset, unsigned int amount);

	inline int GetBufferSize() {return m_bufferSize;}
	inline BYTE *GetVertexBufferDataPointer() {return m_bufferPtr;}

	//void AddParticle(POINTSPRITEVERTEX &p);
	//void AddParticleSafe(POINTSPRITEVERTEX &p);
	bool CreateBuffer(unsigned int size = 0);

	virtual void OnDeviceLost();
	virtual void OnDeviceReset();

private:
	// Create and release vertex buffer affect only the vertex buffer, not temporary buffer
	bool CreateVertexBuffer();
	void ReleaseVertexBuffer();

	IDirect3DVertexBuffer9 *m_vb;
	IDirect3DDevice9 *m_pD3DDevice;
	
	BYTE *m_bufferPtr;
	//POINTSPRITEVERTEX *m_temporaryBuffer;
	//int m_nextTemporaryIndex;
	unsigned int m_bufferSize;
};

#endif // #ifndef __CPARTICLECACHE_H__
