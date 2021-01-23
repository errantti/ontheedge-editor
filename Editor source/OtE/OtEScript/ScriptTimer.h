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

#ifndef __CSCRIPTTIMER_H__
#define __CSCRIPTTIMER_H__

#include "..\\Edgewalker\\EWScript\\Script.h"

/**
 * A timer that executes a specified script at certain intervals
 * and another script when the timer reaches maximum duration
 */
class CScriptTimer
{
public:
	CScriptTimer(long id);

	/**
	 * Update returns true to signal that this timer is ready to be removed
	 */
	bool Update(float frameTime);

	bool IsCounting() {return m_bCounting;}
	void Stop() {m_bCounting = false;}
	void Continue() {m_bCounting = true;}

	float GetTime() {return m_timeFromStart;}
	void SetTime(float time) {m_timeFromStart = time;}

	void SetDuration(float time) {m_duration = time;}
	void SetEndScript(CScript *script) {m_endScript = script;}
	void SetUpdateScript(CScript *script) {m_updateScript = script;}
	void SetUpdateInterval(float time) {m_interval = time;}

	inline long GetID() {return m_ID;}

private:
	CScript *m_endScript, *m_updateScript;
	long m_ID;
	float m_duration, m_timeFromStart;
	float m_interval, m_timeFromLastScriptUpdate;

	bool m_bEnabled, m_bCounting;
};

#endif // __CSCRIPTTIMER_H__