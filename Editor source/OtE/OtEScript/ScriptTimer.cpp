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

#include "ScriptTimer.h"
#include "ScriptManager.h"

CScriptTimer::CScriptTimer(long id) :
m_duration(10.f), m_endScript(NULL), m_ID(id), m_interval(5.f), m_updateScript(NULL), 
m_bEnabled(true), m_bCounting(false), m_timeFromLastScriptUpdate(0.f), m_timeFromStart(0.f)
{
}

bool CScriptTimer::Update(float frameTime)
{
	if(!m_bEnabled)
		return true;

	if(!m_bCounting)
		return false;

	m_timeFromStart += frameTime;
	m_timeFromLastScriptUpdate += frameTime;

	if(m_timeFromStart >= m_duration)
	{
		CScriptManager::Execute(m_endScript, SCRIPT_CALLER_NONE, 0, NULL);

		m_bEnabled = false;
		return true;
	}

	if(m_timeFromLastScriptUpdate >= m_interval)
	{
		CScriptManager::Execute(m_updateScript, SCRIPT_CALLER_NONE, 0, NULL);

		m_timeFromLastScriptUpdate = 0;
	}

	return false;
}