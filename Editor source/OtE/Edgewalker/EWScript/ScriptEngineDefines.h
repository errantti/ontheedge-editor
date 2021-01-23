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

#ifndef __SCRIPTENGINEDEFINES_H__
#define __SCRIPTENGINEDEFINES_H__

#include "Vector3D.h"

// Assumptions:
// 16 <= MAX_SCRIPT_STRING_LENGTH <= MAX_PARAM_STRING_LENGTH <= MAX_SCRIPT_MSG_LENGTH

#define MAX_ATTRIBUTES				4		// Number of attributes (parameters) per instruction
#define MAX_SCRIPT_STRING_LENGTH	32		// Max length of in-script string (variable names, types, commands)
#define MAX_PARAM_STRING_LENGTH		64		// Strings that can be stored in variables inside instructions
#define MAX_SCRIPT_MSG_LENGTH		1024	// Messages that are stored outside of the actual instructions

#define MIN_SCRIPT_STRING_LENGTH	2

#define EWSCRIPT_VERSION			1010

#define MAX_LOOP_COUNT				100		// Define the number of times a single loop can run 
	//through before being forced to be terminated (avoiding run-time infinite script loops)

/**
 * InstructionType
 * Indicates whether an instruction has any special
 * meaning to the script engine besides execution
 **/
enum InstructionType {
	INSTRUCTION_TYPE_EXECUTE=0,		// Executes
	INSTRUCTION_TYPE_IF,
	INSTRUCTION_TYPE_END_IF,
	INSTRUCTION_TYPE_SCRIPT_END,
	INSTRUCTION_TYPE_CREATE_VAR,
	INSTRUCTION_TYPE_SET_VAR,
	INSTRUCTION_TYPE_MODIFY_VAR,
	INSTRUCTION_TYPE_LOOP
};

/**
 * ScriptReturn
 * Return values for several different script-handling
 * sub-operations.
 **/
enum ScriptReturn {
	SCRIPT_RETURN_WARNING=-3,
	SCRIPT_RETURN_NOT_VALIDATED=-2,
	SCRIPT_RETURN_ERROR=-1,
	SCRIPT_RETURN_FALSE=0,
	SCRIPT_RETURN_TRUE=1,
	SCRIPT_RETURN_NOVALUE
};

class CScriptEngine;

/**
 * SScriptVariable
 * Stores a single run-time script-system
 * variable and its type information
 **/
struct SScriptVariable
{
	char name[MAX_SCRIPT_STRING_LENGTH];
	char type[MAX_SCRIPT_STRING_LENGTH];
	union
	{
		char str_value[MAX_SCRIPT_MSG_LENGTH];
		bool bool_value;
		double num_value;
		CVector3D vec3_value;
		void *custom_ref;
	};
};

#endif // #ifndef __SCRIPTENGINEDEFINES_H__