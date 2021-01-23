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

#ifndef __COTESCRIPTINSTRUCTIONS_H__
#define __COTESCRIPTINSTRUCTIONS_H__

#include "..\\Edgewalker\\EWScript\\ScriptEngine.h"

class COtEScriptInstructions
{
public:
	static void AddOtEScriptCommands(CScriptEngine *scriptEngine);

private:
	class Script_if_ptrNull : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_if_ptrNotNull : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_screenDisplayStr : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_screenDisplayStrEx : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_randomizeLocation : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	/**
	 * Scripts
	 **/
	class Script_isCallerObject : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_isCallerGadget : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_isCallerScript : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_isCallerCampaign : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getCallerObject : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getCallerGadget : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getCallerObjectTypeID : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_executeScript : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};
};

#endif // #ifndef __COTESCRIPTINSTRUCTIONS_H__