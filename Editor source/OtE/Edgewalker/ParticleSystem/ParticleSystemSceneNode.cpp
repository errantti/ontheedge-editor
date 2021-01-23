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

#include "ParticleSystemSceneNode.h"
#include "ParticleEngine.h"
#include "ParticleEmitter.h"
#include "..\\Scene\\SceneManager.h"

//long CParticleSystemSceneNode::m_emitterCount = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParticleSystemSceneNode::CParticleSystemSceneNode(ISceneNode *parent, ISceneManager *manager, long id, 
		D3DXVECTOR3 pos, bool autoDestruct, float duration)
	: IParticleSystemSceneNode(parent, manager, id, pos, autoDestruct, duration), 
	m_speed(D3DXVECTOR3(0.f, 0.f, 0.f)), m_timeFromCreation(0.f)
{
	//SetActive(FALSE);
	//++m_emitterCount;
}

CParticleSystemSceneNode::~CParticleSystemSceneNode()
{
	Release();
	//--m_emitterCount;
}

void CParticleSystemSceneNode::CreateEmitter(sEMITTER_PARAMS &params)
{
	CParticleEmitter *emitter = new CParticleEmitter(params, *(this));
	m_emitters.push_back(emitter);
}

void CParticleSystemSceneNode::Release()
{
	std::vector<CParticleEmitter*>::iterator it = m_emitters.begin();
	for(; it != m_emitters.end(); ++it)
		delete (*it);
	m_emitters.clear();
}

void CParticleSystemSceneNode::Update(float fFrametime)
{
	if (m_bActive)
	{
		m_timeFromCreation += fFrametime;
		std::vector<CParticleEmitter*>::iterator it = m_emitters.begin();
		for(; it != m_emitters.end(); ++it)
			(*it)->Update(fFrametime, m_timeFromCreation);
		if(m_bAutoDestruct)
		{
			m_duration -= fFrametime;
			if(m_duration < 0.f)
			{
				// Kill this object
				m_manager->AddNodeToRemoveList(this);
			}
		}
	}
}

// This should never be called
void CParticleSystemSceneNode::Render(bool isShadowPass)
{
}

void CParticleSystemSceneNode::Emit()
{
	std::vector<CParticleEmitter*>::iterator it = m_emitters.begin();
	for(; it != m_emitters.end(); ++it)
		(*it)->Emit();
}

// InstantEmit
// Emit some sPARTICLEs based on m_params structure
void CParticleSystemSceneNode::InstantEmit()
{
	std::vector<CParticleEmitter*>::iterator it = m_emitters.begin();
	for(; it != m_emitters.end(); ++it)
		(*it)->InstantEmit();
}

void CParticleSystemSceneNode::InstantEmit(int index)
{
	if(index < 0 || index >= m_emitters.size())
		return;
	m_emitters[index]->InstantEmit();
}

void CParticleSystemSceneNode::OnPreRender(bool isShadowPass)
{
	if(IsActive() && !isShadowPass)
	{
		if(!((CSceneManager*)m_manager)->GetIsCulled(this))
		{
			CParticleEngine *e = CParticleEngine::GetEngine();
			std::vector<CParticleEmitter*>::iterator it = m_emitters.begin();
			for(; it != m_emitters.end(); ++it)
				e->RegisterEmitterForRendering(*(*it));
		}
	}
}