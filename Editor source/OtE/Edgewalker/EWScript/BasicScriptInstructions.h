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

#ifndef __BASICSCRIPTINSTRUCTIONS_H__
#define __BASICSCRIPTINSTRUCTIONS_H__

#include "InstructionTemplate.h"

class CBasicScriptInstructions
{
public:
	void AddBasicCommands(CScriptEngine *scriptEngine);

private:
	class Script_IF_BOOL : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_IF_NBOOL : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_IF_INTEQ : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_IF_NINTEQ : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_IF_STREQ : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_IF_NSTREQ : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_IF_GENERIC : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_IF_NGENERIC : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_IF_LESSERTHAN : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_IF_GREATERTHAN : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_WHILE : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_FOR : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_INCREMENT : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_subtract : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_MULTIPLY : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_round : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_END_SCRIPT : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_constructVector3 : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_concatenate : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_concatenateInt : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_randomInt : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_randomDecimal : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getVector3Length : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getVec3X : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getVec3Y : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_getVec3Z : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};

	class Script_addLongString : public CInstructionTemplate::IInstructionHandler
	{
	public:
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);
	};
};

#endif // #ifndef __BASICSCRIPTINSTRUCTIONS_H__