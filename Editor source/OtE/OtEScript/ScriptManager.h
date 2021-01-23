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

#ifndef __CSCRIPTMANAGER_H__
#define __CSCRIPTMANAGER_H__

#include "..\\Edgewalker\\EWScript\\ScriptEngine.h"
#include "..\\Edgewalker\\EWScript\\ScriptParser.h"
#include "OtEScriptInstructions.h"
#include "ScriptTimer.h"

enum SCRIPT_CALLER
{
	SCRIPT_CALLER_NONE,
	SCRIPT_CALLER_OBJECT,
	SCRIPT_CALLER_DIALOGUE,
	SCRIPT_CALLER_TERMINAL,
	SCRIPT_CALLER_SCRIPT,
	SCRIPT_CALLER_GADGET,
	SCRIPT_CALLER_CAMPAIGN
};

class CScriptManager
{
public:
	CScriptManager();
	~CScriptManager();

	//void SetScriptEngineRef(CScriptEngine &engine, bool bLoadInstructions = true);
	void InitializeScriptEngine();

	CScript *GetScript(const std::string name);

	bool LoadScriptDatabase(const std::string filename, bool temporary);

	/**
	 * Saves the current script collection into a file
	 * /param filename - name and path of the script collection file
	 * /param saveAll - true if each script is to be saved into the collection
	 *	 regardless of whether the script is temporary
	 * /param temporaryOnly - if saveAll is false then this value determines whether
	 *	 it is the temporary or non-temporary scripts that will be saved
	 */
	bool SaveScriptDatabase(const std::string filename, bool saveAll, bool temporaryOnly);

	// Unloads all scripts
	void ClearDatabase();

	void FillDefaultScripts();

	CScriptEngine &GetScriptEngine() {return m_scriptEngine;}

	void *GetCaller() {return m_caller;}
	SCRIPT_CALLER GetCallerType() {return m_callerType;}
	long GetCallerTypeID() {return m_callerTypeID;}

	void AddTimer(CScriptTimer *timer);
	void RemoveTimer(long id);
	void UpdateTimers(float frameTime);
	CScriptTimer *GetTimer(long id);

	void DisplayTimer(CScriptTimer *timer, bool countForward);
	void HideVisualTimer();

	void RemoveTimers();

	/**
	 * This method should be always used to execute script in OtE
	 */
	static bool Execute(CScript *script, SCRIPT_CALLER caller, long typeID, void *source, bool *outReturnValue = NULL);
	static CScriptManager &GetManager() {return *m_this;}

private:
	void InitCaller();

	static CScriptManager *m_this;

	CScriptEngine m_scriptEngine;

	SCRIPT_CALLER m_callerType;
	long m_callerTypeID;
	void *m_caller;

	bool m_bVisualTimerCountsForward;
	bool m_bCriticalSection;

	CScriptTimer *m_visualTimer;

	std::vector<CScriptTimer*> m_timers, m_toBeAddedTimers;

	COtEScriptInstructions m_instructions;
};

#endif // #ifndef __CSCRIPTMANAGER_H__