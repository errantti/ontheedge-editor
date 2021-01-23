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

#ifndef __SCRIPTPARSER_H__
#define __SCRIPTPARSER_H__

#include "ScriptEngineDefines.h"
#include "Script.h"
#include <string>

class CScriptParser
{
public:
	CScriptParser(CScriptEngine *scriptEngine) : m_scriptEngine(scriptEngine), m_current(0) {}
	~CScriptParser();

	void BeginParse(std::string scriptName);
	void BeginParse(CScript *script);
	bool ParseLine(const char *line);
	CScript *EndParse();

	void InterruptParse();

	CScript *ParseFile(const char *filename, const char *scriptName);

private:
	CScript *m_current;
	CScriptEngine *m_scriptEngine;

	bool m_bParsingExternalScript;

	bool GetWord(const char *line, int wordIndex, char *output);
	bool ExtractAttribute(char *word, CScript::SAttribute &att, char *type);

	bool VerifyVariableNameSyntax(char *name);
	bool VerifyStringSyntax(char *str);

	void ReportError(std::string errormsg);
};

#endif // #ifndef __SCRIPTPARSER_H__