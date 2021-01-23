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

#ifndef __SCRIPTENGINE_H__
#define __SCRIPTENGINE_H__

#include "ScriptEngineDefines.h"
#include "Script.h"
#include "InstructionTemplate.h"
#include "BasicScriptInstructions.h"
#include "..\\ILogWriter.h"

#include <string>

/**
 * The core of the scripting engine. Stores scripts,
 * instruction templates and variable types
 */
class CScriptEngine
{
public:
	CScriptEngine(bool ignoreBasicCommands = false);

	~CScriptEngine();

	/**
	 * AddInstructionTemplate
	 * Adds an instruction template to the script system
	 */
	bool AddInstructionTemplate(CInstructionTemplate t);
	/**
	 * AddVariableType
	 * Adds a user variable type to the script system
	 */
	void AddVariableType(char *typeName);

	CInstructionTemplate *GetInstructionTemplate(int id, int &outIndex);
	CInstructionTemplate *GetInstructionTemplate(char *name, int &outIndex);
	CInstructionTemplate *GetInstructionTemplateAt(const int index);

	/**
	 * Verify the validity of a variable type name (STR, etc...)
	 * If templateSource is true then metatypes ANY and NONPTR are
	 * included in the list of possible types but only instruction
	 * templates are allowed to use those types
	 */
	bool ValidateVariableType(char *type, bool templateSource = false);

	bool AddScript(CScript *script);
	bool RemoveScript(const std::string name);
	CScript *GetScript(const std::string name);
	ScriptReturn ExecuteScript(const std::string name);

	std::vector<CScript*> &GetCurrentScriptCollection() {return m_scripts;}

	/**
	 * Loads a collection of scripts from a binary database file
	 * /param filename - filename of the collections
	 * /param unloadCurrentScripts - unloads currently loaded scripts before loading collection
	 * /param autoValidate - automatically validates all loaded scripts
	 * /param temporary - marks the loaded scripts temporary (can be easily removed later on)
	 */
	bool LoadScriptCollection(const std::string filename, bool unloadCurrentScripts, bool autoValidate, bool temporary);
	/**
	 * Saves the current script collection into a binary file
	 * /param filename - name and path of the script collection file
	 * /param saveAll - true if each script is to be saved into the collection
	 *	 regardless of whether the script is temporary
	 * /param temporaryOnly - if saveAll is false then this value determines whether
	 *	 it is the temporary or non-temporary scripts that will be saved
	 */
	bool SaveScriptCollection(const std::string filename, bool saveAll, bool temporaryOnly);
	/**
	 * Unloads all currently loaded scripts
	 */
	void UnloadCurrentScripts();

	/**
	 * Unloads all scripts marked as temporary
	 */
	void UnloadTemporaryScripts();

	void ReportError(const std::string errorMsg);

	inline void SetLogWriter(ILogWriter *logger) {m_logger = logger;}

	static int GetFirstCustomInstructionID() {return 1000;}

private:
	std::vector<CInstructionTemplate> m_instTemplates;
	std::vector<std::string> m_variableTypes;
	std::vector<CScript*> m_scripts;

	ILogWriter *m_logger;

	void AddBasicCommands();

	bool AddNewScriptFromCollection(std::fstream &stream, bool temporary);

	CBasicScriptInstructions m_basicInsts;
};

#endif // #ifndef __SCRIPTENGINE_H__