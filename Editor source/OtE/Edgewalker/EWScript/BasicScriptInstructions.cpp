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

#include "BasicScriptInstructions.h"
#include "ScriptEngine.h"
#include "..\\Edgewalker.h"

void CBasicScriptInstructions::AddBasicCommands(CScriptEngine *scriptEngine)
{
	if(scriptEngine == NULL)
		return;

	CScriptEngine &se = *scriptEngine;

	se.AddVariableType("NUM");
	se.AddVariableType("STR");
	se.AddVariableType("BOOL");
	se.AddVariableType("VEC3");
	se.AddVariableType("VARTYPE");

	int id = 1;
	const unsigned int strLength = MAX_SCRIPT_STRING_LENGTH * MAX_ATTRIBUTES + MAX_ATTRIBUTES;

	/**
	 *	END_IF
	 **/
	char str_end_if[strLength] = "";
	CInstructionTemplate end_if(NULL, scriptEngine, "endIf", INSTRUCTION_TYPE_END_IF, id,
	str_end_if);
	se.AddInstructionTemplate(end_if);
	id++;

	/**
	 *	IF_BOOL
	 **/
	char str_if_bool[strLength] = "BOOL";
	CInstructionTemplate if_bool(new Script_IF_BOOL(), scriptEngine, "if_true", INSTRUCTION_TYPE_IF, id,
	str_if_bool);
	se.AddInstructionTemplate(if_bool);
	id++;

	/**
	 *	IF_NBOOL
	 **/
	char str_if_NBOOL[strLength] = "BOOL";
	CInstructionTemplate if_NBOOL(new Script_IF_NBOOL(), scriptEngine, "if_notTrue", INSTRUCTION_TYPE_IF, id,
	str_if_NBOOL);
	se.AddInstructionTemplate(if_NBOOL);
	id++;

	/**
	 *	IF_INTEQ
	 **/
	char str_if_INTEQ[strLength] = "NUM|NUM";
	CInstructionTemplate if_INTEQ(new Script_IF_INTEQ(), scriptEngine, "if_numEq", INSTRUCTION_TYPE_IF, id,
	str_if_INTEQ);
	se.AddInstructionTemplate(if_INTEQ);
	id++;

	/**
	 *	IF_NINTEQ
	 **/
	char str_if_NINTEQ[strLength] = "NUM|NUM";
	CInstructionTemplate if_NINTEQ(new Script_IF_NINTEQ(), scriptEngine, "if_notNumEq", INSTRUCTION_TYPE_IF, id,
	str_if_NINTEQ);
	se.AddInstructionTemplate(if_NINTEQ);
	id++;

	/**
	 *	IF_STREQ
	 **/
	char str_if_STREQ[strLength] = "STR|STR";
	CInstructionTemplate if_STREQ(new Script_IF_STREQ(), scriptEngine, "if_strEq", INSTRUCTION_TYPE_IF, id,
	str_if_STREQ);
	se.AddInstructionTemplate(if_STREQ);
	id++;

	/**
	 *	IF_NSTREQ
	 **/
	char str_if_NSTREQ[strLength] = "STR|STR";
	CInstructionTemplate if_NSTREQ(new Script_IF_NSTREQ(), scriptEngine, "if_notStrEq", INSTRUCTION_TYPE_IF, id,
	str_if_NSTREQ);
	se.AddInstructionTemplate(if_NSTREQ);
	id++;

	/**
	 *	IF_GENERIC
	 **/
	char str_IF_GENERIC[strLength] = "ANY|ANY";
	CInstructionTemplate IF_GENERIC(new Script_IF_GENERIC(), scriptEngine, "if_generic", INSTRUCTION_TYPE_IF, id,
	str_IF_GENERIC);
	se.AddInstructionTemplate(IF_GENERIC);
	id++;

	/**
	 *	IF_NGENERIC
	 **/
	char str_IF_NGENERIC[strLength] = "ANY|ANY";
	CInstructionTemplate IF_NGENERIC(new Script_IF_NGENERIC(), scriptEngine, "if_notGeneric", INSTRUCTION_TYPE_IF, id,
	str_IF_NGENERIC);
	se.AddInstructionTemplate(IF_NGENERIC);
	id++;

	/**
	 *	IF_GREATERTHAN
	 **/
	char str_IF_GREATERTHAN[strLength] = "NUM|NUM";
	CInstructionTemplate IF_GREATERTHAN(new Script_IF_GREATERTHAN(), scriptEngine, "if_>", INSTRUCTION_TYPE_IF, id,
	str_IF_GREATERTHAN);
	se.AddInstructionTemplate(IF_GREATERTHAN);
	id++;

	/**
	 *	IF_LESSERTHAN
	 **/
	char str_IF_LESSERTHAN[strLength] = "NUM|NUM";
	CInstructionTemplate IF_LESSERTHAN(new Script_IF_LESSERTHAN(), scriptEngine, "if_<", INSTRUCTION_TYPE_IF, id,
	str_IF_LESSERTHAN);
	se.AddInstructionTemplate(IF_LESSERTHAN);
	id++;

	/**
	 *	WHILE
	 **/
	char str_WHILE[strLength] = "BOOL";
	CInstructionTemplate WHILE(new Script_WHILE(), scriptEngine, "while", INSTRUCTION_TYPE_LOOP, id,
	str_WHILE);
	se.AddInstructionTemplate(WHILE);
	id++;

	/**
	 *	FOR
	 **/
	char str_FOR[strLength] = "NUM|NUM";
	CInstructionTemplate FOR(new Script_FOR(), scriptEngine, "for", INSTRUCTION_TYPE_LOOP, id,
	str_FOR);
	se.AddInstructionTemplate(FOR);
	id++;

	/**
	 *	SET_STRING
	 **/
	char str_SET_STRING[strLength] = "STR|STR";
	CInstructionTemplate SET_STRING(NULL, scriptEngine, "setString", INSTRUCTION_TYPE_SET_VAR, id,
	str_SET_STRING);
	se.AddInstructionTemplate(SET_STRING);
	id++;

	/**
	 *	SET_NUMERIC
	 **/
	char str_SET_NUMERIC[strLength] = "NUM|NUM";
	CInstructionTemplate SET_NUMERIC(NULL, scriptEngine, "setNumeric", INSTRUCTION_TYPE_SET_VAR, id,
	str_SET_NUMERIC);
	se.AddInstructionTemplate(SET_NUMERIC);
	id++;

	/**
	 *	SET_BOOL
	 **/
	char str_SET_BOOL[strLength] = "BOOL|BOOL";
	CInstructionTemplate SET_BOOL(NULL, scriptEngine, "setBool", INSTRUCTION_TYPE_SET_VAR, id,
	str_SET_BOOL);
	se.AddInstructionTemplate(SET_BOOL);
	id++;

	/**
	 *	SET_VECTOR3
	 **/
	char str_SET_VECTOR3[strLength] = "VEC3|VEC3";
	CInstructionTemplate SET_VECTOR3(NULL, scriptEngine, "setVector3", INSTRUCTION_TYPE_SET_VAR, id,
	str_SET_VECTOR3);
	se.AddInstructionTemplate(SET_VECTOR3);
	id++;

	/**
	 *	constructVector3
	 **/
	char str_constructVector3[strLength] = "VEC3|NUM|NUM|NUM";
	CInstructionTemplate constructVector3(new Script_constructVector3(), scriptEngine, "constructVector3", INSTRUCTION_TYPE_MODIFY_VAR, id,
	str_constructVector3);
	se.AddInstructionTemplate(constructVector3);
	id++;

	/**
	 *	INCREMENT
	 **/
	char str_INCREMENT[strLength] = "NUM|NUM";
	CInstructionTemplate INCREMENT(new Script_INCREMENT(), scriptEngine, "increment", INSTRUCTION_TYPE_MODIFY_VAR, id,
	str_INCREMENT);
	se.AddInstructionTemplate(INCREMENT);
	id++;

	/**
	 *	subtract
	 **/
	char str_subtract[strLength] = "NUM|NUM";
	CInstructionTemplate subtract(new Script_subtract(), scriptEngine, "subtract", INSTRUCTION_TYPE_MODIFY_VAR, id,
	str_subtract);
	se.AddInstructionTemplate(subtract);
	id++;

	/**
	 *	MULTIPLY
	 **/
	char str_MULTIPLY[strLength] = "NUM|NUM";
	CInstructionTemplate MULTIPLY(new Script_MULTIPLY(), scriptEngine, "multiply", INSTRUCTION_TYPE_MODIFY_VAR, id,
	str_MULTIPLY);
	se.AddInstructionTemplate(MULTIPLY);
	id++;

	/**
	 *	round
	 **/
	char str_round[strLength] = "NUM|NUM";
	CInstructionTemplate round(new Script_round(), scriptEngine, "round", INSTRUCTION_TYPE_MODIFY_VAR, id,
	str_round);
	se.AddInstructionTemplate(round);
	id++;

	/**
	 *	END_SCRIPT
	 **/
	char str_END_SCRIPT[strLength] = "BOOL";
	CInstructionTemplate END_SCRIPT(new Script_END_SCRIPT(), scriptEngine, "endScript", INSTRUCTION_TYPE_SCRIPT_END, id,
	str_END_SCRIPT);
	se.AddInstructionTemplate(END_SCRIPT);
	id++;

	/**
	 *	CREATE_VAR
	 **/
	char str_CREATE_VAR[strLength] = "STR|VARTYPE";
	CInstructionTemplate CREATE_VAR(NULL, scriptEngine, "createVar", INSTRUCTION_TYPE_CREATE_VAR, id,
	str_CREATE_VAR);
	se.AddInstructionTemplate(CREATE_VAR);
	id++;

	/**
	 *	concatenate
	 **/
	char str_concatenate[strLength] = "STR|ANY";
	CInstructionTemplate concatenate(new Script_concatenate(), scriptEngine, 
		"concatenate", INSTRUCTION_TYPE_MODIFY_VAR, id, str_concatenate);
	se.AddInstructionTemplate(concatenate);
	id++;

	/**
	 *	concatenateInt
	 **/
	char str_concatenateInt[strLength] = "STR|NUM";
	CInstructionTemplate concatenateInt(new Script_concatenateInt(), scriptEngine, 
		"concatenateInt", INSTRUCTION_TYPE_MODIFY_VAR, id, str_concatenateInt);
	se.AddInstructionTemplate(concatenateInt);
	id++;

	/**
	 *	randomInt
	 **/
	CInstructionTemplate randomInt(new Script_randomInt(), scriptEngine, 
		"randomInt", INSTRUCTION_TYPE_MODIFY_VAR, id, "NUM|NUM|NUM");
	se.AddInstructionTemplate(randomInt);
	id++;

	/**
	 *	randomDecimal
	 **/
	CInstructionTemplate randomDecimal(new Script_randomDecimal(), scriptEngine, 
		"randomDecimal", INSTRUCTION_TYPE_MODIFY_VAR, id, "NUM|NUM|NUM");
	se.AddInstructionTemplate(randomDecimal);
	id++;

	/**
	 *	getVector3Length
	 **/
	CInstructionTemplate getVector3Length(new Script_getVector3Length(), scriptEngine, 
		"getVector3Length", INSTRUCTION_TYPE_MODIFY_VAR, id, "NUM|VEC3");
	se.AddInstructionTemplate(getVector3Length);
	id++;

	/**
	 *	getVec3X
	 **/
	CInstructionTemplate getVec3X(new Script_getVec3X(), scriptEngine, 
		"getVec3X", INSTRUCTION_TYPE_MODIFY_VAR, id, "NUM|VEC3");
	se.AddInstructionTemplate(getVec3X);
	id++;

	/**
	 *	getVec3Y
	 **/
	CInstructionTemplate getVec3Y(new Script_getVec3Y(), scriptEngine, 
		"getVec3Y", INSTRUCTION_TYPE_MODIFY_VAR, id, "NUM|VEC3");
	se.AddInstructionTemplate(getVec3Y);
	id++;

	/**
	 *	getVec3Z
	 **/
	CInstructionTemplate getVec3Z(new Script_getVec3Z(), scriptEngine, 
		"getVec3Z", INSTRUCTION_TYPE_MODIFY_VAR, id, "NUM|VEC3");
	se.AddInstructionTemplate(getVec3Z);
	id++;

	/**
	 *	addLongString
	 **/
	CInstructionTemplate addLongString(new Script_addLongString(), scriptEngine, 
		"addLongString", INSTRUCTION_TYPE_MODIFY_VAR, id, "STR|STR|STR|STR");
	se.AddInstructionTemplate(addLongString);
	id++;
}



////////////////////////////
/// Instruction handlers ///
////////////////////////////

ScriptReturn CBasicScriptInstructions::Script_IF_BOOL::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].bool_value == param[1].bool_value)
		return SCRIPT_RETURN_TRUE;
	else
		return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_IF_NBOOL::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].bool_value != param[1].bool_value)
		return SCRIPT_RETURN_TRUE;
	else
		return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_IF_STREQ::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(!strcmp(param[0].str_value, param[1].str_value))
		return SCRIPT_RETURN_TRUE;
	else
		return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_IF_NSTREQ::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(strcmp(param[0].str_value, param[1].str_value))
		return SCRIPT_RETURN_TRUE;
	else
		return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_IF_INTEQ::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].num_value == param[1].num_value)
		return SCRIPT_RETURN_TRUE;
	else
		return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_IF_NINTEQ::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].num_value != param[1].num_value)
		return SCRIPT_RETURN_TRUE;
	else
		return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_IF_GENERIC::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(strcmp(param[0].type, param[1].type))
		return SCRIPT_RETURN_ERROR;
	if(strcmp(param[0].type, "STR"))
	{
		for(int i=0; i<8; ++i)
			if(param[0].str_value[i] != param[1].str_value[i])
				return SCRIPT_RETURN_FALSE;
		return SCRIPT_RETURN_TRUE;
	}
	else
	{
		if(strncmp(param[0].str_value, param[1].str_value, MAX_SCRIPT_MSG_LENGTH))
			return SCRIPT_RETURN_FALSE;
		else
			return SCRIPT_RETURN_TRUE;
	}
}

ScriptReturn CBasicScriptInstructions::Script_IF_NGENERIC::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(strcmp(param[0].type, param[1].type))
		return SCRIPT_RETURN_ERROR;
	if(strcmp(param[0].type, "STR"))
	{
		for(int i=0; i<8; ++i)
			if(param[0].str_value[i] != param[1].str_value[i])
				return SCRIPT_RETURN_TRUE;
		return SCRIPT_RETURN_FALSE;
	}
	else
	{
		if(strncmp(param[0].str_value, param[1].str_value, MAX_SCRIPT_MSG_LENGTH))
			return SCRIPT_RETURN_TRUE;
		else
			return SCRIPT_RETURN_FALSE;
	}
}

ScriptReturn CBasicScriptInstructions::Script_IF_LESSERTHAN::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].num_value < param[1].num_value)
		return SCRIPT_RETURN_TRUE;
	return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_IF_GREATERTHAN::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].num_value > param[1].num_value)
		return SCRIPT_RETURN_TRUE;
	return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_WHILE::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].bool_value)
		return SCRIPT_RETURN_TRUE;
	return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_FOR::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	// Autoincrement
	modifyVar->num_value += 1;
	// Continue loop as long as param1 is less than param2
	if(param[0].num_value < param[1].num_value)
		return SCRIPT_RETURN_TRUE;
	return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_INCREMENT::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = param[0].num_value + param[1].num_value;

	return SCRIPT_RETURN_TRUE;
}

ScriptReturn CBasicScriptInstructions::Script_subtract::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = param[0].num_value - param[1].num_value;

	return SCRIPT_RETURN_TRUE;
}

ScriptReturn CBasicScriptInstructions::Script_MULTIPLY::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = param[0].num_value * param[1].num_value;

	return SCRIPT_RETURN_TRUE;
}

ScriptReturn CBasicScriptInstructions::Script_round::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = (double)(long)(param[1].num_value+0.5);

	return SCRIPT_RETURN_TRUE;
}

ScriptReturn CBasicScriptInstructions::Script_END_SCRIPT::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	if(param[0].bool_value)
		return SCRIPT_RETURN_TRUE;
	else
		return SCRIPT_RETURN_FALSE;
}

ScriptReturn CBasicScriptInstructions::Script_constructVector3::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->vec3_value.x = (float)param[1].num_value;
	modifyVar->vec3_value.y = (float)param[2].num_value;
	modifyVar->vec3_value.z = (float)param[3].num_value;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CBasicScriptInstructions::Script_concatenate::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	int len = (int)strlen(param[0].str_value);
	
	if(!strcmp(param[1].type, "STR"))
	{
		strncat(modifyVar->str_value, param[1].str_value, MAX_SCRIPT_MSG_LENGTH - len - 1);
	}
	else if(!strcmp(param[1].type, "NUM"))
	{
		_snprintf(modifyVar->str_value, MAX_SCRIPT_MSG_LENGTH-1, "%s%f", param[0].str_value, param[1].num_value);
	}
	else if(!strcmp(param[1].type, "VEC3"))
	{
		_snprintf(modifyVar->str_value, MAX_SCRIPT_MSG_LENGTH -1, "%s{%f,%f,%f}", param[0].str_value, param[1].vec3_value.x, param[1].vec3_value.y, param[1].vec3_value.z);
	}

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CBasicScriptInstructions::Script_concatenateInt::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	_snprintf(modifyVar->str_value, MAX_SCRIPT_MSG_LENGTH-1, "%s%i", param[0].str_value, (int)param[1].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CBasicScriptInstructions::Script_randomInt::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = GetEdgewalkerApp()->RandInt((int)param[1].num_value, (int)param[2].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CBasicScriptInstructions::Script_randomDecimal::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = GetEdgewalkerApp()->RandFloat((float)param[1].num_value, (float)param[2].num_value);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CBasicScriptInstructions::Script_getVector3Length::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	CVector3D &v = param[1].vec3_value;
	modifyVar->num_value = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CBasicScriptInstructions::Script_getVec3X::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = param[1].vec3_value.x;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CBasicScriptInstructions::Script_getVec3Y::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = param[1].vec3_value.y;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CBasicScriptInstructions::Script_getVec3Z::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	modifyVar->num_value = param[1].vec3_value.z;

	return SCRIPT_RETURN_NOVALUE;
}

ScriptReturn CBasicScriptInstructions::Script_addLongString::ExecuteInstruction(SScriptVariable *param, SScriptVariable *modifyVar)
{
	sprintf(modifyVar->str_value, "%s%s%s%s", param[0].str_value, param[1].str_value, param[2].str_value, param[3].str_value);

	return SCRIPT_RETURN_NOVALUE;
}