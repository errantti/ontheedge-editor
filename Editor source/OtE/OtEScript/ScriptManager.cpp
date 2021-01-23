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

#include "..\\Edgewalker\\Printer.h"
#include "..\\Edgewalker\\StandardLogWriter.h"
#include "ScriptManager.h"

CScriptManager *CScriptManager::m_this = 0;

CScriptManager::CScriptManager() : m_visualTimer(NULL), m_bCriticalSection(false)
{
	InitCaller();
	if(m_this == 0)
		m_this = this;
}

CScriptManager::~CScriptManager()
{
	RemoveTimers();
	if(m_this == this)
		m_this = 0;
}

void CScriptManager::InitializeScriptEngine()
{
	static CStandardLogWriter logger;
	m_scriptEngine.SetLogWriter(&logger);

	m_instructions.AddOtEScriptCommands(&m_scriptEngine);
}

void CScriptManager::FillDefaultScripts()
{
	// Load, parse and validate a test script
	CScriptParser parser(&m_scriptEngine);

	CScript *s = parser.ParseFile("test.ews", "TestScript");
	m_scriptEngine.AddScript(s);
}

CScript *CScriptManager::GetScript(const std::string name)
{
	return m_scriptEngine.GetScript(name);
}

bool CScriptManager::LoadScriptDatabase(const std::string filename, bool temporary)
{
	return m_scriptEngine.LoadScriptCollection(filename, false, true, temporary);
}

bool CScriptManager::SaveScriptDatabase(const std::string filename, bool saveAll, bool temporaryOnly)
{
	return m_scriptEngine.SaveScriptCollection(filename, saveAll, temporaryOnly);
}

void CScriptManager::ClearDatabase()
{
	RemoveTimers();
	m_scriptEngine.UnloadCurrentScripts();
}

void CScriptManager::InitCaller()
{
	m_caller = NULL;
	m_callerTypeID = -1;
	m_callerType = SCRIPT_CALLER_SCRIPT;
}

bool CScriptManager::Execute(CScript *script, SCRIPT_CALLER caller, long typeID, void *source, bool *outReturnValue)
{
	if(script == NULL)
		return false;

	m_this->m_caller = source;
	m_this->m_callerTypeID = typeID;
	m_this->m_callerType = caller;

	ScriptReturn r = script->Execute();
	m_this->InitCaller();
	if(r == SCRIPT_RETURN_ERROR || r == SCRIPT_RETURN_NOT_VALIDATED)
	{
		CPrinter::logError("Failed executing script");
		return false;
	}

	if(r != SCRIPT_RETURN_NOVALUE && outReturnValue != NULL)
	{
		*outReturnValue = false;
		if(r == SCRIPT_RETURN_TRUE)
			*outReturnValue = true;
	}

	return true;
}

void CScriptManager::AddTimer(CScriptTimer *timer)
{
	if(timer == NULL)
		return;
	if(!m_bCriticalSection)
		m_timers.push_back(timer);
	else
		m_toBeAddedTimers.push_back(timer);
}

void CScriptManager::RemoveTimer(long id)
{
	vector<CScriptTimer*>::iterator it = m_timers.begin();
	for(; it != m_timers.end(); ++it)
		if((*it)->GetID() == id)
		{
			delete (*it);
			m_timers.erase(it);
			return;
		}
}

void CScriptManager::UpdateTimers(float frameTime)
{
	m_bCriticalSection = true;
	vector<CScriptTimer*>::iterator it = m_timers.begin();
	for(; it != m_timers.end();)
	{
		if((*it)->Update(frameTime))
		{
			delete *it;
			it = m_timers.erase(it);
		}
		else
			++it;
	}
	m_bCriticalSection = false;

	// Add timers to main vector (those that were added during the critical section)
	for(it = m_toBeAddedTimers.begin(); it != m_toBeAddedTimers.end(); ++it)
		m_timers.push_back(*it);
	m_toBeAddedTimers.clear();

	// Update visual timer
	if(m_visualTimer != NULL)
	{
		// TODO: visual timer
	}
}

CScriptTimer *CScriptManager::GetTimer(long id)
{
	vector<CScriptTimer*>::iterator it = m_timers.begin();
	for(; it != m_timers.end(); ++it)
		if((*it)->GetID() == id)
			return (*it);
	return NULL;
}

void CScriptManager::DisplayTimer(CScriptTimer *timer, bool countForward)
{
	m_visualTimer = timer;
	m_bVisualTimerCountsForward = countForward;
}

void CScriptManager::HideVisualTimer()
{
	m_visualTimer = NULL;
}

void CScriptManager::RemoveTimers()
{
	vector<CScriptTimer*>::iterator it = m_timers.begin();
	for(; it != m_timers.end(); ++it)
		delete (*it);
	m_timers.clear();
}