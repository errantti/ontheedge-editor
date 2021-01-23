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

#include "Script.h"
#include "ScriptEngine.h"
#include "InstructionTemplate.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>

ScriptReturn CScript::Execute()
{
	if(!m_bValidated)
	{
		int i=0;
		if(!m_bRequiresManualValidation)
			if(!Validate(i))
				return SCRIPT_RETURN_NOT_VALIDATED;
	}

	ResetVariables();

	std::vector<SInstruction>::iterator it = m_instructions.begin();
	int iterationLevel = 0, targetLevel = 0, loopStartIterationLevel = 0;
	bool lookingForBlockEnd = false;
	bool insideLoop = false;
	std::vector<SInstruction>::iterator loopStart = m_instructions.end();

	int loopCount = 0;

	for(;it!=m_instructions.end();)
	{
		//CInstructionTemplate *temp = m_scriptEngine->GetInstructionTemplate(((SInstruction)*it).instructionID);
		CInstructionTemplate *temp = m_scriptEngine->GetInstructionTemplateAt(((SInstruction)*it).instructionIndex);
		if(temp == NULL)
		{
			ReportError("CScript::Execute: Couldn't find instruction template");
			// This error should never happen. If it does then the invalid command has somehow passed
			// the validation process and there's something seriously wrong
			break;
		}

		bool incrementIterator = true;
		if(lookingForBlockEnd)
		{
			if(temp->GetType() == INSTRUCTION_TYPE_IF)
			{
				iterationLevel++;
			}
			else if(temp->GetType() == INSTRUCTION_TYPE_LOOP)
			{
				iterationLevel++;
			}
			else if(temp->GetType() == INSTRUCTION_TYPE_END_IF)
			{
				iterationLevel--;
				if(iterationLevel <= targetLevel)
					lookingForBlockEnd = false;
			}
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_END_IF)
		{
			if(iterationLevel > 0)
				iterationLevel--;
			if(insideLoop)
			{
				if(iterationLevel <= loopStartIterationLevel)
				{
					loopCount++;
					if(loopCount < MAX_LOOP_COUNT)
					{
						// Move back to loop instruction
						it = loopStart;
						incrementIterator = false;
						insideLoop = false;
					}
					else
					{
						// Maximum loop count exceeded, jump out of loop and reset loop count
						loopCount = 0;
						ReportError("Maximum loop count exceeded. Forced out of loop. Possible infinite loop.");
					}
				}
			}
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_IF)
		{
			ScriptReturn r = ExecuteInstruction(temp, ((SInstruction)*it).att, false);
			if(r == SCRIPT_RETURN_FALSE)
			{
				lookingForBlockEnd = true;
				targetLevel = iterationLevel;
			}
			iterationLevel++;
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_LOOP)
		{
			// Don't allow loops inside loops
			if(!insideLoop)
			{
				ScriptReturn r = ExecuteInstruction(temp, ((SInstruction)*it).att, true);
				if(r == SCRIPT_RETURN_FALSE)
				{
					lookingForBlockEnd = true;
					targetLevel = iterationLevel;
					loopCount = 0;
				}
				else
				{
					insideLoop = true;
					loopStart = it;
					loopStartIterationLevel = iterationLevel;
				}
			}
			iterationLevel++;
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_SCRIPT_END)
		{
			ScriptReturn r = ExecuteInstruction(temp, ((SInstruction)*it).att, false);
			if(r != SCRIPT_RETURN_ERROR && r != SCRIPT_RETURN_NOVALUE)
				return r;
			//else
			//	return SCRIPT_RETURN_NOVALUE;
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_CREATE_VAR)
		{
			ScriptCreateVariable(((SInstruction)*it).att, false);
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_SET_VAR)
		{
			ScriptSetVariable(((SInstruction)*it).att, false);
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_MODIFY_VAR)
		{
			ExecuteInstruction(temp, ((SInstruction)*it).att, true);
		}
		else
		{
			ExecuteInstruction(temp, ((SInstruction)*it).att, false);
		}

		if(incrementIterator)
			++it;
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CScript::ExecuteInstruction(CInstructionTemplate *temp, SAttribute *atts, bool modifyFirstParam)
{
	if(temp == NULL)
		return SCRIPT_RETURN_ERROR;
	SScriptVariable vars[MAX_ATTRIBUTES];
	SScriptVariable *modifyVar = NULL;

	if(modifyFirstParam)
	{
		if(!atts[0].variable)
			return SCRIPT_RETURN_ERROR;
		modifyVar = GetVariable(atts[0].str_value);
		if(modifyVar == NULL)
			return SCRIPT_RETURN_ERROR;
	}
	for(int i=0;i<MAX_ATTRIBUTES;++i)
	{
		char *type = temp->GetAttributeType(i);
		if(!strcmp(type, ""))
			break;
		if(atts[i].variable)
		{
			SScriptVariable *var2 = GetVariable(atts[i].str_value);
			if(var2 == NULL)
			{
				ReportError("Variable not declared");
				return SCRIPT_RETURN_ERROR;
			}
			if(strcmp(type, var2->type))
			{
				if(strcmp(type, "ANY"))
				{
					ReportError("Variable type mismatch");
					return SCRIPT_RETURN_ERROR;
				}
			}

			memcpy(&vars[i], var2, sizeof(SScriptVariable));
		}
		else
		{
			memcpy(vars[i].str_value, atts[i].str_value, sizeof(char)*MAX_PARAM_STRING_LENGTH);
			vars[i].str_value[MAX_PARAM_STRING_LENGTH-1] = '\0';
			strcpy(vars[i].type, atts[i].type);
		}

		/*if(!strcmp(type, "NUM"))
			vars[i].num_value = atts[i].num_value;
		else if(!strcmp(type, "STR"))
			vars[i].str_value = atts[i].str_value;
		else if(!strcmp(type, "NUM"))
			vars[i].num_value = atts[i].num_value;
		else if(!strcmp(type, "BOOL"))
			vars[i].bool_value = atts[i].bool_value;
		else if(!strcmp(type, "VEC3"))
			vars[i].vec3_value = atts[i].vec3_value;
		else
			vars[i].custom_ref = atts[i].custom_ref;*/
	}

	return temp->ExecuteInstruction(vars, modifyVar);
}

void CScript::ResetVariables()
{
	m_variables.clear();

	// TODO: add long strings
}

ScriptReturn CScript::ScriptCreateVariable(SAttribute *atts, bool validate)
{
	if(validate)
	{
		if(strlen(atts[0].str_value) >= MAX_SCRIPT_STRING_LENGTH || strlen(atts[0].str_value) == 0)
		{
			ReportError("Validation error: parameter 1 out of bounds");
			return SCRIPT_RETURN_ERROR;
		}
		if(strlen(atts[1].str_value) >= MAX_SCRIPT_STRING_LENGTH)
		{
			ReportError("Validation error: parameter 2 out of bounds");
			return SCRIPT_RETURN_ERROR;
		}
		if(!ValidateVariableType(atts[1].str_value))
		{
			ReportError("Validation error: invalid variable type");
			return SCRIPT_RETURN_ERROR;
		}
		SScriptVariable *v = GetVariable(atts[0].str_value);
		if(v != NULL)
		{
			ReportError("Validation error: variable with that name already exists");
			return SCRIPT_RETURN_ERROR;
		}
	}

	SScriptVariable var;
	strcpy(var.name, atts[0].str_value);
	strcpy(var.type, atts[1].str_value);
	ZeroMemory(var.str_value, sizeof(char)*MAX_SCRIPT_MSG_LENGTH);
	m_variables.push_back(var);

	return SCRIPT_RETURN_TRUE;
}

ScriptReturn CScript::ScriptSetVariable(SAttribute *atts, bool validate)
{
	if(validate)
	{
		if(strlen(atts[0].str_value) >= MAX_SCRIPT_STRING_LENGTH || strlen(atts[0].str_value) == 0)
		{
			ReportError("Validation error: parameter 1 out of bounds");
			return SCRIPT_RETURN_ERROR;
		}
		if(atts[1].variable)
			if(strlen(atts[1].str_value) >= MAX_SCRIPT_STRING_LENGTH)
			{
				ReportError("Validation error: parameter 2 out of bounds");
				return SCRIPT_RETURN_ERROR;
			}
	}

	SScriptVariable *var = GetVariable(atts[0].str_value);
	if(var == NULL)
	{
		ReportError("Variable not declared");
		return SCRIPT_RETURN_ERROR;
	}

	if(atts[1].variable)
	{
		SScriptVariable *var2 = GetVariable(atts[1].str_value);
		if(var2 == NULL)
		{
			ReportError("Variable not declared");
			return SCRIPT_RETURN_ERROR;
		}
		// Variable types have to match
		if(strcmp(var2->type, var->type))
		{
			ReportError("Variable type mismatch");
			return SCRIPT_RETURN_ERROR;
		}

		memcpy(var->str_value, var2->str_value, sizeof(char)*MAX_SCRIPT_MSG_LENGTH);
	}
	else
	{
		if(strcmp(var->type, atts[1].type))
		{
			ReportError("Value type mismatch");
			return SCRIPT_RETURN_ERROR;
		}
		memcpy(var->str_value, atts[1].str_value, sizeof(char)*MAX_PARAM_STRING_LENGTH);
		var->str_value[MAX_PARAM_STRING_LENGTH-1] = '\0';
	}
	return SCRIPT_RETURN_TRUE;
}

SScriptVariable *CScript::GetVariable(char *name)
{
	std::vector<SScriptVariable>::iterator it = m_variables.begin();
	for(;it!=m_variables.end();++it)
		if(!strcmp(((SScriptVariable)*it).name, name))
			return (SScriptVariable*)&(*it);
	return NULL;
}

bool CScript::Validate(int &errorLineNumber)
{
	ResetVariables();

	std::vector<SInstruction>::iterator it = m_instructions.begin();
	int iterationLevel = 0, loopStartIterationLevel = 0;
	bool insideLoop = false;

	std::string strErrorLine = "Validation error:\n\t";

	errorLineNumber = 0;
	ScriptReturn r;
	for(;it!=m_instructions.end();++it)
	{
		errorLineNumber++;
		int index;
		CInstructionTemplate *temp = m_scriptEngine->GetInstructionTemplate(it->instructionID, index);
		it->instructionIndex = index;

		if(temp == NULL)
		{
			strErrorLine += ConstructText((*it));
			strErrorLine += ": unknown command";
			ReportError(strErrorLine);
			return false;
		}
		if(temp->GetType() == INSTRUCTION_TYPE_END_IF)
		{
			iterationLevel--;
			if(iterationLevel<0)
			{
				strErrorLine += ConstructText((*it));
				strErrorLine += ": endIf count mismatch";
				ReportError(strErrorLine);
				return false;
			}
			if(insideLoop)
			{
				if(iterationLevel <= loopStartIterationLevel)
				{
					insideLoop = false;
				}
			}
			continue;
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_IF)
		{
			r = ValidateInstruction(temp, ((SInstruction)*it).att, false);
			if(r == SCRIPT_RETURN_ERROR)
			{
				strErrorLine += ConstructText((*it));
				strErrorLine += ": instruction validation error";
				ReportError(strErrorLine);
				return false;
			}
			iterationLevel++;
			continue;
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_LOOP)
		{
			if(insideLoop)
			{
				strErrorLine += ConstructText((*it));
				strErrorLine += ": nested loops detected";
				ReportError(strErrorLine);
				return false;
			}
			else
			{
				r = ValidateInstruction(temp, ((SInstruction)*it).att, true);
				if(r == SCRIPT_RETURN_ERROR)
				{
					strErrorLine += ConstructText((*it));
					strErrorLine += ": instruction validation error";
					ReportError(strErrorLine);
					return false;
				}
				insideLoop = true;
				loopStartIterationLevel = iterationLevel;
			}
			iterationLevel++;
			continue;
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_SCRIPT_END)
		{
			r = ValidateInstruction(temp, ((SInstruction)*it).att, false);
			if(r == SCRIPT_RETURN_ERROR)
			{
				strErrorLine += ConstructText((*it));
				strErrorLine += ": instruction validation error";
				ReportError(strErrorLine);
				return false;
			}
			else
				return true;
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_CREATE_VAR)
		{
			r = ScriptCreateVariable(((SInstruction)*it).att, true);
			if(r == SCRIPT_RETURN_ERROR)
			{
				strErrorLine += ConstructText((*it));
				strErrorLine += ": instruction validation error";
				ReportError(strErrorLine);
				return false;
			}
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_SET_VAR)
		{
			r = ScriptSetVariable(((SInstruction)*it).att, true);
			if(r == SCRIPT_RETURN_ERROR)
			{
				strErrorLine += ConstructText((*it));
				strErrorLine += ": instruction validation error";
				ReportError(strErrorLine);
				return false;
			}
		}
		else if(temp->GetType() == INSTRUCTION_TYPE_MODIFY_VAR)
		{
			r = ValidateInstruction(temp, ((SInstruction)*it).att, true);
			if(r == SCRIPT_RETURN_ERROR)
			{
				strErrorLine += ConstructText((*it));
				strErrorLine += ": instruction validation error";
				ReportError(strErrorLine);
				return false;
			}
		}
		else
		{
			r = ValidateInstruction(temp, ((SInstruction)*it).att, false);
			if(r == SCRIPT_RETURN_ERROR)
			{
				strErrorLine += ConstructText((*it));
				strErrorLine += ": instruction validation error";
				ReportError(strErrorLine);
				return false;
			}
		}
	}
	if(iterationLevel != 0)
	{
		ReportError("Unmatching amount of IFs and END_IFs");
		return false;
	}

	return true;
}

ScriptReturn CScript::ValidateInstruction(CInstructionTemplate *temp, SAttribute *atts, bool modifyFirstParam)
{
	if(temp == NULL)
	{
		ReportError("Strange internal error");
		return SCRIPT_RETURN_ERROR;
	}
	SScriptVariable *modifyVar = NULL;

	if(modifyFirstParam)
	{
		if(!atts[0].variable)
		{
			ReportError("Modifiable parameter not variable");
			return SCRIPT_RETURN_ERROR;
		}
		modifyVar = GetVariable(atts[0].str_value);
		if(modifyVar == NULL)
		{
			ReportError("Variable not declared");
			return SCRIPT_RETURN_ERROR;
		}
	}
	for(int i=0;i<MAX_ATTRIBUTES;++i)
	{
		char *type = temp->GetAttributeType(i);
		if(!strcmp(type, ""))
			break;
		if(atts[i].variable)
		{
			SScriptVariable *var2 = GetVariable(atts[i].str_value);
			if(var2 == NULL)
			{
				ReportError("Variable not declared");
				return SCRIPT_RETURN_ERROR;
			}
			if(strcmp(type, var2->type))
			{
				if(strcmp(type, "ANY"))
				{
					ReportError("Variable type mismatch");
					return SCRIPT_RETURN_ERROR;
				}
			}
		}
		else
		{
			if(strcmp(atts[i].type, type))
			{
				if(strcmp(type, "ANY"))
				{
					ReportError("Variable type mismatch");
					return SCRIPT_RETURN_ERROR;
				}
			}
		}
	}

	return SCRIPT_RETURN_TRUE;
}

bool CScript::ValidateVariableType(char *type)
{
	if(type == NULL)
		return false;

	return m_scriptEngine->ValidateVariableType(type);
}

bool CScript::RemoveInstruction(int lineNumber)
{
	std::vector<SInstruction>::iterator it = m_instructions.begin();
	for(int i=1; it != m_instructions.end(); ++it, ++i)
		if(i == lineNumber)
		{
			m_instructions.erase(it);
			return true;
		}
	return false;
}

bool CScript::LoadScript(std::fstream &stream)
{
	char nameBuffer[128];

	stream.read(nameBuffer, sizeof(char)*SCRIPT_NAME_LENGTH);
	m_name = nameBuffer;
	stream.read(nameBuffer, sizeof(char)*SCRIPT_FILENAME_LENGTH);
	m_filename = nameBuffer;

	int numInstructions = 0;
	stream.read((char*)&numInstructions, sizeof(int));
	if(numInstructions < 0)
		return false;

	SInstruction sinst;
	for(int i=0; i<numInstructions; ++i)
	{
		stream.read((char*)&sinst, sizeof(SInstruction));
		// Update the instruction index to preserve backwards binary compatibility when new script commands are added
		m_scriptEngine->GetInstructionTemplate(sinst.instructionID, sinst.instructionIndex);
		m_instructions.push_back(sinst);
	}

	return true;
}

bool CScript::LoadScript(std::string filename)
{
	using namespace std;

	fstream f;
	f.open(filename.c_str(), ios::in | ios::binary);

	if(!f.is_open())
	{
		ReportError("LoadScript error: Couldn't open file (file may not exist)");
		return false;
	}

	int version = 0;

	bool result;
	try {
		f.read((char*)&version, sizeof(int));

		if(version != EWSCRIPT_VERSION)
		{
			ReportError("LoadScript error: version mismatch, file is corrupt or from an old version");
			f.close();
			return false;
		}

		result = LoadScript(f);
	} catch (exception e) {
		string msg = "LoadScript error: ";
		msg += e.what();
		ReportError(msg);

		f.close();
		return false;
	}

	f.close();

	return result;
}

bool CScript::SaveScript(std::fstream &stream)
{
	char nameBuffer[128];

	strncpy(nameBuffer, m_name.c_str(), SCRIPT_NAME_LENGTH-1);
	stream.write(nameBuffer, SCRIPT_NAME_LENGTH);

	strncpy(nameBuffer, m_filename.c_str(), SCRIPT_FILENAME_LENGTH-1);
	stream.write(nameBuffer, SCRIPT_FILENAME_LENGTH);

	int numInstructions = (int)m_instructions.size();
	stream.write((const char*)&numInstructions, sizeof(int));

	std::vector<SInstruction>::iterator it = m_instructions.begin();
	for(; it != m_instructions.end(); ++it)
		stream.write((const char*)&(*it), sizeof(SInstruction));

	return true;
}

bool CScript::SaveScript(std::string filename)
{
	using namespace std;

	fstream f;
	f.open(filename.c_str(), ios::out | ios::binary);

	if(!f.is_open())
	{
		ReportError("SaveScript error: Couldn't open file (file may not exist)");
		return false;
	}

	bool result;

	int version = EWSCRIPT_VERSION;
	try {
		f.write((const char*)&version, sizeof(int));

		result = SaveScript(f);
	} catch (exception e) {
		string msg = "SaveScript error: ";
		msg += e.what();
		ReportError(msg);

		f.close();
		return false;
	}

	f.close();

	return result;
}

/**
 * ConstructText
 * Converts a binary instruction back into a text line
 * for debugging purposes
 **/
std::string CScript::ConstructText(SInstruction &inst)
{
	int index;
	CInstructionTemplate *t = m_scriptEngine->GetInstructionTemplate(inst.instructionID, index);
	if(t == NULL)
		return "";

	std::string line = "";
	line += t->GetName();
	for(int i=0; i<MAX_ATTRIBUTES; ++i)
	{
		if(t->GetAttributeType(i)[0] == '\0')
			break;
		if(inst.att[i].variable)
		{
			line += " #";
			line += inst.att[i].str_value;
			continue;
		}
		else if(!strcmp(inst.att[i].type, "STR"))
		{
			line += " \"";
			line += inst.att[i].str_value;
			line += "\"";
		}
		else if(!strcmp(inst.att[i].type, "VARTYPE"))
		{
			line += " ";
			line += inst.att[i].str_value;
		}
		else if(!strcmp(inst.att[i].type, "NUM"))
		{
			std::stringstream ss;

			line += " ";
			ss << inst.att[i].num_value;
			line += ss.str();
			//line += (int)inst.att[i].num_value; // seems like string += doesn't support double o.O
			//sprintf(numBuffer, "%f", inst.att[i].num_value);
			//line += numBuffer;
		}
		else if(!strcmp(inst.att[i].type, "BOOL"))
		{
			if(inst.att[i].bool_value)
				line += " $TRUE";
			else
				line += " $FALSE";
		}
	}
	return line;
}

bool CScript::ConstructText(std::string &str, std::string lineTerminator)
{
	str.clear();

	std::vector<SInstruction>::iterator it = m_instructions.begin();
	for(; it != m_instructions.end(); ++it)
		str += ConstructText(*it) + lineTerminator;
	return true;
}

void CScript::ReportError(std::string errorMsg)
{
	m_scriptEngine->ReportError(errorMsg);
}