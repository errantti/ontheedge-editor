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

#ifndef __CEVENTACTIONE_H__
#define __CEVENTACTIONE_H__

#include "Edgewalker\\EWScript\\Script.h"
#include "ParticleEffects.h"
#include "Sound\\Sound.h"

/**
 * Struct that encapsulates the essential contents
 * of an action event
 */
struct SActionEvent
{
	long soundID;
	long particleEffectID;
	char scriptName[32];

	void FillDefaults()
	{
		soundID = -1;
		particleEffectID = -1;
		scriptName[0] = '\0';
	}
};

class IGameObject;

/**
 * Handles and stores a single event action. An event action
 * contains a particle effect, sound effect and script along
 * with other features which will all be triggered when the
 * event action is triggered
 */ 
class CActionEvent
{
public:
	/// Default constructor
	CActionEvent() : m_particleEffect(0), m_sound(0), m_script(0) {}

	/**
	 * Fetches the pointers of particle effect, script and
	 * sound for fast triggering
	 */
	CActionEvent(SActionEvent &data);
	~CActionEvent() {}

	SActionEvent GetData();

	/**
	 * Fetches the pointers of particle effect, script and
	 * sound for fast triggering
	 */
	void SetData(SActionEvent &data);

	bool SetScript(char *scriptName);
	bool SetSound(long id);
	bool SetParticleEffect(long id);

	/**
	 * Triggers the event action at given object
	 */
	void Trigger(IGameObject *source);
	/**
	 * Trigger the event at given position
	 */
	void Trigger(D3DXVECTOR3 position, D3DXVECTOR3 rotation = D3DXVECTOR3(0.f, 0.f, 0.f));

	void PlaySound(D3DXVECTOR3 position = D3DXVECTOR3(0.f, 0.f, 0.f));
	void TriggerScript(IGameObject *source);
	void TriggerParticleEffect(IGameObject *source);

private:
	SParticleSystemInfo *m_particleEffect;
	CScript *m_script;
	CSound *m_sound;
};

#endif // #ifndef __CEVENTACTIONE_H__