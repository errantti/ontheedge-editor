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

#include "TimerInstructions.h"
#include "ScriptTimer.h"
#include "ScriptManager.h"
#include "..\\OnTheEdge.h"

void CTimerInstructions::AddTimerCommands(CScriptEngine *scriptEngine, int &id)
{
	if(scriptEngine == NULL)
		return;

	CScriptEngine &se = *scriptEngine;

	/**
	 *	createTimer
	 **/
	CInstructionTemplate createTimer(new Script_createTimer, scriptEngine, "createTimer", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "TIMER|NUM");
	se.AddInstructionTemplate(createTimer);

	/**
	 *	removeTimer
	 **/
	CInstructionTemplate removeTimer(new Script_removeTimer, scriptEngine, "removeTimer", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "TIMER");
	se.AddInstructionTemplate(removeTimer);

	/**
	 *	getTimer
	 **/
	CInstructionTemplate getTimer(new Script_getTimer, scriptEngine, "getTimer", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "TIMER|NUM");
	se.AddInstructionTemplate(getTimer);

	/**
	 *	getTimerTime
	 **/
	CInstructionTemplate getTimerTime(new Script_getTimerTime, scriptEngine, "getTimerTime", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "NUM|TIMER");
	se.AddInstructionTemplate(getTimerTime);

	/**
	 *	setTimerTime
	 **/
	CInstructionTemplate setTimerTime(new Script_setTimerTime, scriptEngine, "setTimerTime", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "TIMER|NUM");
	se.AddInstructionTemplate(setTimerTime);

	/**
	 *	startTimer
	 **/
	CInstructionTemplate startTimer(new Script_startTimer, scriptEngine, "startTimer", 
		INSTRUCTION_TYPE_EXECUTE, id++, "TIMER");
	se.AddInstructionTemplate(startTimer);

	/**
	 *	stopTimer
	 **/
	CInstructionTemplate stopTimer(new Script_stopTimer, scriptEngine, "stopTimer", 
		INSTRUCTION_TYPE_EXECUTE, id++, "TIMER");
	se.AddInstructionTemplate(stopTimer);

	/**
	 *	setTimerUpdateFrequency
	 **/
	CInstructionTemplate setTimerUpdateFrequency(new Script_setTimerUpdateFrequency, scriptEngine, "setTimerUpdateFrequency", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "TIMER|NUM");
	se.AddInstructionTemplate(setTimerUpdateFrequency);

	/**
	 *	setTimerUpdateScript
	 **/
	CInstructionTemplate setTimerUpdateScript(new Script_setTimerUpdateScript, scriptEngine, "setTimerUpdateScript", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "TIMER|STR");
	se.AddInstructionTemplate(setTimerUpdateScript);

	/**
	 *	setTimerFinishScript
	 **/
	CInstructionTemplate setTimerFinishScript(new Script_setTimerFinishScript, scriptEngine, "setTimerFinishScript", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "TIMER|STR");
	se.AddInstructionTemplate(setTimerFinishScript);

	/**
	 *	setTimerDuration
	 **/
	CInstructionTemplate setTimerDuration(new Script_setTimerDuration, scriptEngine, "setTimerDuration", 
		INSTRUCTION_TYPE_MODIFY_VAR, id++, "TIMER|NUM");
	se.AddInstructionTemplate(setTimerDuration);

	/**
	 *	displayTimer
	 **/
	CInstructionTemplate displayTimer(new Script_displayTimer, scriptEngine, "displayTimer", 
		INSTRUCTION_TYPE_EXECUTE, id++, "TIMER|BOOL");
	se.AddInstructionTemplate(displayTimer);

	/**
	 *	hideTimer
	 **/
	CInstructionTemplate hideTimer(new Script_hideTimer, scriptEngine, "hideTimer", 
		INSTRUCTION_TYPE_EXECUTE, id++, "TIMER");
	se.AddInstructionTemplate(hideTimer);
}





////////////////////////////
/// Instruction handlers ///
////////////////////////////

ScriptReturn CTimerInstructions::Script_createTimer::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CScriptTimer *timer = new CScriptTimer((long)param[1].num_value);
	modifyVar->custom_ref = (void*)timer;
	CScriptManager::GetManager().AddTimer(timer);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_removeTimer::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
		CScriptManager::GetManager().RemoveTimer(((CScriptTimer*)param[0].custom_ref)->GetID());
		modifyVar->custom_ref = NULL;
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_getTimer::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->custom_ref = CScriptManager::GetManager().GetTimer((long)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_setTimerTime::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
		((CScriptTimer*)param[0].custom_ref)->SetTime((float)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_getTimerTime::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[1].custom_ref)
		modifyVar->num_value = ((CScriptTimer*)param[0].custom_ref)->GetTime();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_startTimer::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
		((CScriptTimer*)param[0].custom_ref)->Continue();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_stopTimer::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
		((CScriptTimer*)param[0].custom_ref)->Stop();

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_setTimerUpdateFrequency::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
		((CScriptTimer*)param[0].custom_ref)->SetUpdateInterval((float)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_setTimerUpdateScript::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
		CScript *script = CScriptManager::GetManager().GetScript(param[1].str_value);
		((CScriptTimer*)param[0].custom_ref)->SetUpdateScript(script);
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_setTimerFinishScript::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
	{
		CScript *script = CScriptManager::GetManager().GetScript(param[1].str_value);
		((CScriptTimer*)param[0].custom_ref)->SetEndScript(script);
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_setTimerDuration::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
		((CScriptTimer*)param[0].custom_ref)->SetDuration((float)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_displayTimer::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
		CScriptManager::GetManager().DisplayTimer((CScriptTimer*)param[0].custom_ref, param[1].bool_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CTimerInstructions::Script_hideTimer::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].custom_ref)
		CScriptManager::GetManager().HideVisualTimer();

	return SCRIPT_RETURN_NOVALUE;
}