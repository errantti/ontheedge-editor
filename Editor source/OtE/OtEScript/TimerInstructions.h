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

#ifndef __CTIMERINSTRUCTIONS_H__
#define __CTIMERINSTRUCTIONS_H__

#include "..\\Edgewalker\\EWScript\\ScriptEngine.h"

class CTimerInstructions
{
public:
	static void AddTimerCommands(CScriptEngine *scriptEngine, int &id);

private:
	class Script_createTimer : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_removeTimer : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getTimer : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setTimerTime : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getTimerTime : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_startTimer : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_stopTimer : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setTimerUpdateFrequency : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setTimerUpdateScript : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setTimerFinishScript : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_setTimerDuration : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_displayTimer : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_hideTimer : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};
};

#endif // #ifndef __CTIMERINSTRUCTIONS_H__