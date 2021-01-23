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

#include "OnTheEdge.h"
#include "ActionEvent.h"

CActionEvent::CActionEvent(SActionEvent &data)
{
	SetData(data);
}

void CActionEvent::SetData(SActionEvent &data)
{
	m_particleEffect = GetOTE()->GetParticleEffects()->GetEffectData(data.particleEffectID);
	m_sound = CSoundManager::GetManager().GetSound(data.soundID);
	m_script = CScriptManager::GetManager().GetScript(data.scriptName);
}

SActionEvent CActionEvent::GetData()
{
	SActionEvent e;
	e.FillDefaults();
	if(m_particleEffect)
		e.particleEffectID = m_particleEffect->ID;
	if(m_sound)
		e.soundID = m_sound->GetID();
	// TODO: make sure this works O.o
	if(m_script)
		strcpy(e.scriptName, m_script->GetFilename().c_str());
	return e;
}

bool CActionEvent::SetScript(char *scriptName)
{
	if(scriptName == NULL)
	{
		m_script = NULL;
		return true;
	}

	if((m_script = CScriptManager::GetManager().GetScript(scriptName)) == NULL)
		return false;
	return true;
}

bool CActionEvent::SetSound(long id)
{
	if(id == -1)
	{
		m_sound = NULL;
		return true;
	}

	if((m_sound = CSoundManager::GetManager().GetSound(id)) == NULL)
		return false;
	return true;
}

bool CActionEvent::SetParticleEffect(long id)
{
	if(id == -1)
	{
		m_particleEffect = NULL;
		return true;
	}

	if((m_particleEffect = GetOTE()->GetParticleEffects()->GetEffectData(id)) == NULL)
		return false;
	return true;
}

void CActionEvent::Trigger(IGameObject *source)
{
	if(m_particleEffect != NULL)
		GetOTE()->GetParticleEffects()->StartEffect(source, *m_particleEffect);
	if(m_sound != NULL)
		m_sound->Play();
	if(m_script != NULL)
		CScriptManager::Execute(m_script, SCRIPT_CALLER_OBJECT, source->GetType(), source);
}

void CActionEvent::Trigger(D3DXVECTOR3 position, D3DXVECTOR3 rotation)
{
	if(m_particleEffect != NULL)
		GetOTE()->GetParticleEffects()->StartEffect(position, rotation, *m_particleEffect);
	if(m_sound != NULL)
		m_sound->Play();
	if(m_script != NULL)
		CScriptManager::Execute(m_script, SCRIPT_CALLER_NONE, 0, NULL);
}

void CActionEvent::PlaySound(D3DXVECTOR3 position)
{
	if(m_sound != NULL)
		m_sound->Play();
}