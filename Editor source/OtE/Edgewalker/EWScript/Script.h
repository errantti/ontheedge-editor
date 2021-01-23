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

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <vector>
#include <string>
#include <fstream>
#include <windows.h>

#include "ScriptEngineDefines.h"
#include "InstructionTemplate.h"

#define SCRIPT_NAME_LENGTH		32
#define SCRIPT_FILENAME_LENGTH	128

/**
 * Stores a single script. Handles execution
 * and validation.
 */
class CScript
{
public:
	CScript(CScriptEngine *ref, bool requiresManualValidation = true) : 
	  m_scriptEngine(ref), m_bRequiresManualValidation(requiresManualValidation),
	  m_bTemporary(false)
	{
	}

	~CScript()
	{
	}

	/**
	 * SAttribute
	 * Stores a single attribute parameter
	 */
	struct SAttribute
	{
		/**
		 * variable
		 * True if this attribute points to a run-time parameter, if that's
		 * the case, the variable's name is stored in str_value
		 */
		bool variable;
		/**
		 * Type of the parameter. If this attribute references a variable,
		 * this type is trivial
		 */
		char type[MAX_SCRIPT_STRING_LENGTH];
		/**
		 * Data stored in the variable.
		 */
		union
		{
			double num_value;
			char str_value[MAX_PARAM_STRING_LENGTH];
			bool bool_value;
			CVector3D vec3_value;
			void *custom_ref;
		};

		void Init()
		{
			variable = false;
			type[0] = '\0';
			ZeroMemory(str_value, sizeof(char)*MAX_PARAM_STRING_LENGTH);
		}
	};

	/**
	 * SInstruction
	 * Stores the info of a single instruction and its parameters
	 */
	struct SInstruction
	{
		int instructionID;
		int instructionIndex;		// Optimization, this is set during validation so instruction
									// templates can be fetched a lot faster during execution
		SAttribute att[MAX_ATTRIBUTES];

		void Init()
		{
			for(int i=0;i<MAX_ATTRIBUTES;++i)
				att[i].Init();
			instructionID = 0;
		}
	};

	/**
	 * Validate
	 * Check the validity of the script
	 * /errorLineNumber - reference to int to which error line number is stored if
	 *		the validity check fails
	 * Returns true if the script is found valid
	 */
	bool Validate(int &errorLineNumber);

	/** 
	 * Execute the script. If manual validity check
	 * is forced, the script won't be executed
	 */
	ScriptReturn Execute();

	/// Script name (not filename)
	std::string GetName() {return m_name;}
	void SetName(std::string name) {m_name = name;}

	std::string GetFilename() {return m_filename;}
	void SetFilename(std::string filename) {m_filename = filename;}

	bool IsValidated() {return m_bValidated;}

	void AddInstruction(SInstruction inst) {m_instructions.push_back(inst);}
	bool RemoveInstruction(int lineNumber);
	void ClearScript() {m_instructions.clear();}

	bool LoadScript(std::fstream &stream);
	bool LoadScript(std::string filename);

	bool SaveScript(std::fstream &stream);
	bool SaveScript(std::string filename);

	inline bool IsTemporary() {return m_bTemporary;}
	inline void SetIsTemporary(bool value) {m_bTemporary = value;}

	bool ConstructText(std::string &str, std::string lineTerminator);

protected:

	/// Name of the script, identifier purpose
	std::string m_name;
	/// The original filename of the script (the text file from which it was parsed)
	std::string m_filename;
	/// Variables, used run-time
	std::vector<SScriptVariable> m_variables;
	/// Instructions and their attributes
	std::vector<SInstruction> m_instructions;
	std::vector<std::string> m_longStrings;			// TODO: long string variables

	bool m_bTemporary;

	CScriptEngine *m_scriptEngine;

	bool m_bValidated;
	/**
	 * m_bRequiresManualValidation
	 * If true, the script can't be executed before manually
	 * calling Validate
	 */
	bool m_bRequiresManualValidation;

	/**
	 * Executes a single instruction
	 */
	ScriptReturn ExecuteInstruction(CInstructionTemplate *temp, SAttribute *atts, bool modifyFirstParam);
	/**
	 * Checks the validity of a single instruction
	 */
	ScriptReturn ValidateInstruction(CInstructionTemplate *temp, SAttribute *atts, bool modifyFirstParam);
	/**
	 * Returns a run-time variable
	 */
	SScriptVariable *GetVariable(char *name);
	/**
	 * Checks whether the given string matches any variable type name
	 */
	bool ValidateVariableType(char *type);

	/**
	 * Creates a new run-time variable
	 */
	ScriptReturn ScriptCreateVariable(SAttribute *atts, bool validate);
	/**
	 * Sets the value of a run-time variable
	 */
	ScriptReturn ScriptSetVariable(SAttribute *atts, bool validate);

	void ResetVariables();
	void ReportError(std::string errormsg);

	std::string ConstructText(SInstruction &inst);
};

#endif // #ifndef __SCRIPT_H__