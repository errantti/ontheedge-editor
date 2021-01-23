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

#include "InstructionTemplate.h"
#include "ScriptEngine.h"

ScriptReturn CInstructionTemplate::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(m_handler != NULL)
		return m_handler->ExecuteInstruction(param, modifyVar);
	else
		return SCRIPT_RETURN_ERROR;
}

char *CInstructionTemplate::GetAttributeType(int index)
{
	if(index < 0 || index >= MAX_ATTRIBUTES)
		return "";
	else
		return m_attributeTypes[index];
}

void CInstructionTemplate::SetAttributeType(int index, char *type)
{
	if(index < 0 || index >= MAX_ATTRIBUTES)
		return;
	if(!m_scriptEngine->ValidateVariableType(type, true))
		return;
	strcpy(m_attributeTypes[index], type);
}

void CInstructionTemplate::SetName(char *text)
{
	if(text == 0)
		return;
	if(strlen(text)>=MAX_SCRIPT_STRING_LENGTH-1)
		return;
	strcpy(m_name, text);
}

void CInstructionTemplate::SetAttributeTypes(char *types)
{
	if(types == NULL)
		return;

	// Initialize all attributes to zero
	for(int i=0; i<MAX_ATTRIBUTES; ++i)
		m_attributeTypes[i][0] = '\0';

	char type[MAX_SCRIPT_STRING_LENGTH];
	int len = (int)strlen(types);
	int attIndex = 0;
	int k = 0;
	for(int i=0;i<len;++i)
	{
		if(types[i] != '|')
		{
			type[k] = types[i];
			++k;
		}
		else
		{
			type[k] = '\0';
			if(k>1)
				SetAttributeType(attIndex++, type);
			k = 0;
		}
	}
	if(k>1)
	{
		type[k] = '\0';
		SetAttributeType(attIndex++, type);
	}
}