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

#ifndef __CPARTICLEENGINE_H__
#define __CPARTICLEENGINE_H__

#include "..\\Scene\\ISceneManager.h"
#include "ParticleCache.h"
#include "ParticleEmitter.h"

class CParticleEngine
{
public:
	CParticleEngine(ISceneManager &manager);
	~CParticleEngine();

	void InitCache();
	void RegisterEmitterForRendering(CParticleEmitter &emitter);
	void Render();

	static CParticleEngine *GetEngine() {return m_this;}

private:
	struct SSpriteEmitterRef
	{
		CParticleEmitter *emitter;
		CTexture *texture;
		int priority;				// (int)emitter + (int)additiveBlend, gives an int 
									// that identifies certain texture/blend combination
		bool additiveBlend;

		SSpriteEmitterRef(CParticleEmitter &emitter) : emitter(&emitter),
			texture(emitter.GetTexture()), additiveBlend(emitter.IsAdditiveBlend())
		{
			priority = (int)texture + (int)additiveBlend;
		}

		bool operator < (const SSpriteEmitterRef &other) const
		{
			return (priority < other.priority);
		}
	};

	struct SMeshEmitterRef
	{
		CParticleEmitter *emitter;
		CTexture *texture;

		SMeshEmitterRef(CParticleEmitter &emitter) : emitter(&emitter)
		{
			texture = emitter.GetMesh()->GetTexture();
		}

		// TODO: 
		bool operator < (const SMeshEmitterRef &other) const
		{
			return (texture < other.texture);
		}
	};
	ISceneManager &m_scnManager;

	std::vector<SMeshEmitterRef> m_meshEmitters;
	std::vector<SSpriteEmitterRef> m_spriteEmitters;

	DWORD m_spriteParticleCount;

	IDirect3DDevice9 *m_pD3DDevice;

	CParticleCache m_cache;

	void RenderSpriteEmittersOptimized();
	void SetMeshParticleRenderStates();

	static CParticleEngine *m_this;
};

#endif // #ifndef __CPARTICLEENGINE_H__