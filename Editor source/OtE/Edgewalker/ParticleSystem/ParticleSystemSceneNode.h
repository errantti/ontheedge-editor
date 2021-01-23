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

#ifndef __CPARTICLEEMITTERSCENENODE_H__
#define __CPARTICLEEMITTERSCENENODE_H__

#include <vector>
#include "IParticleSystemSceneNode.h"

class CParticleSystemSceneNode : public IParticleSystemSceneNode
{
public:
	CParticleSystemSceneNode(ISceneNode *parent, ISceneManager *manager, long id, 
		D3DXVECTOR3 pos, bool autoDestruct = FALSE, float duration = 4.f);
	virtual ~CParticleSystemSceneNode();

	virtual void CreateEmitter(sEMITTER_PARAMS &params);
	virtual void Release();

	virtual void Render(bool isShadowPass);
	virtual void OnPreRender(bool isShadowPass);
	virtual void Update(float fFrametime);

	virtual void Emit();
	virtual void InstantEmit();
	virtual void InstantEmit(int index);

	int GetEmitterCount() {return (int)m_emitters.size();}

	virtual D3DXVECTOR3 GetSpeed() {return m_speed;}

	/*bool IsRenderingOldWay() {return m_bOldRenderMode;}
	void SetRenderMode(bool oldWay) {m_bOldRenderMode = oldWay;}*/

	//static long GetEmitterCount() {return m_emitterCount;}

protected:
	std::vector<CParticleEmitter*> m_emitters;

	D3DXVECTOR3 m_speed;
	float m_timeFromCreation;

	//bool m_bOldRenderMode;

	/*struct POINTSPRITEVERTEX
	{
		D3DXVECTOR3		vPosition;
		DWORD			dwDiffuse;
	};

	// particle members
	sPARTICLE				*m_pParticles;
	DWORD					m_dwNumParticles;
	DWORD					m_dwNextFreeParticle;
	DWORD					m_activeParticles;

	// texture members, for the billboad animation
	DWORD					m_dwNumTextures;
	PTEXTURE				*m_pTextures;

	DWORD					m_dwFlags;

	float					m_fEmitCounter;

	// get the application pointer for the constant use of
	// random functions.
	IApplication			*m_pApp;

	D3DXVECTOR3				m_gravity;

	CParticleCache			m_cache;
	bool					m_bUsingCache;

	static long m_emitterCount;*/
};

#endif // #ifndef __CPARTICLEEMITTERSCENENODE_H__
