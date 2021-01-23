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

#ifndef __INSTRUCTIONTYPE_H__
#define __INSTRUCTIONTYPE_H__

#include "ScriptEngineDefines.h"
#include <stdlib.h>

class CInstructionTemplate
{
public:
	class IInstructionHandler
	{
	public:
		IInstructionHandler() {}
		~IInstructionHandler() {}

		/**
		 * ExecuteInstruction
		 * Called to execute a particular instruction
		 * /param - list of n parameters
		 * /modifyVar - the variable to which the instruction makes modifications (if at all)
		 **/
		virtual ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar) = 0;
	};

	CInstructionTemplate(IInstructionHandler *handler, CScriptEngine *scriptEngine) :
	  m_handler(handler), m_scriptEngine(scriptEngine)
	{
		for(int i=0; i<MAX_ATTRIBUTES; ++i)
			m_attributeTypes[i][0] = '\0';
	}

	CInstructionTemplate(IInstructionHandler *handler, CScriptEngine *scriptEngine,
		char *name, InstructionType type, int id, char *types) :
		m_handler(handler), m_scriptEngine(scriptEngine), m_type(type), m_ID(id)
	{
		SetName(name);
		if(types == 0)
			return;
		SetAttributeTypes(types);
	}

	ScriptReturn ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar);

	InstructionType GetType() {return m_type;}
	void SetType(InstructionType type) {m_type = type;}

	char *GetAttributeType(int index);

	void SetAttributeType(int index, char *type);

	char *GetName() {return m_name;}
	void SetName(char *text);

	int GetID() {return m_ID;}
	void SetID(int id) {m_ID = id;}

private:
	IInstructionHandler *m_handler;
	InstructionType m_type;
	char m_attributeTypes[MAX_ATTRIBUTES][MAX_SCRIPT_STRING_LENGTH];
	char m_name[MAX_SCRIPT_STRING_LENGTH];
	int m_ID;

	CScriptEngine *m_scriptEngine;

	void SetAttributeTypes(char *types);
};

#endif // #ifndef __INSTRUCTIONTYPE_H__