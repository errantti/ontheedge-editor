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

#include "ScriptParser.h"
#include "ScriptEngine.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

CScriptParser::~CScriptParser()
{
	InterruptParse();
}

void CScriptParser::InterruptParse()
{
	if(m_current != 0)
	{
		if(!m_bParsingExternalScript)
			delete m_current;
		m_current = 0;
	}
}

void CScriptParser::BeginParse(std::string scriptName)
{
	InterruptParse();

	m_current = new CScript(m_scriptEngine);
	m_current->SetName(scriptName);

	m_bParsingExternalScript = false;
}

void CScriptParser::BeginParse(CScript *script)
{
	if(script == NULL)
	{
		m_current = NULL;
		return;
	}

	InterruptParse();

	script->ClearScript();
	m_current = script;
	m_bParsingExternalScript = true;
}

CScript *CScriptParser::EndParse()
{
	CScript *finished = m_current;
	m_current = NULL;
	return finished;
}

bool CScriptParser::ParseLine(const char *line)
{
	if(m_current == NULL)
	{
		ReportError("Internal error 1000");
		return false;
	}
	if(line == NULL)
	{
		ReportError("Internal error 1001");
		return false;
	}

	// Check if the line is empty
	if(strlen(line)<1)
		return true;

	// Check if the line is a comment
	if(line[0] == '/')
		return true;

	char buffer[MAX_PARAM_STRING_LENGTH];

	std::string errorMsg = "Error at: ";
	if(!GetWord(line, 0, buffer))
	{
		errorMsg += line;
		ReportError(errorMsg);
		return false;
	}


	int index;
	CInstructionTemplate *temp = m_scriptEngine->GetInstructionTemplate(buffer, index);
	if(temp == NULL)
	{
		ReportError("Unknown command");
		errorMsg += line;
		ReportError(errorMsg);
		return false;
	}

	CScript::SInstruction inst;
	inst.instructionID = temp->GetID();
	inst.instructionIndex = index;	// Not used

	int word = 1;
	for(; word<=MAX_ATTRIBUTES; ++word)
	{
		if(!strcmp(temp->GetAttributeType(word-1), ""))
			break;

		if(!GetWord(line, word, buffer))
		{
			ReportError("Missing parameter(s)");
			errorMsg += line;
			ReportError(errorMsg);
			return false;
		}

		if(!ExtractAttribute(buffer, inst.att[word-1], temp->GetAttributeType(word-1)))
		{
			ReportError("Failed parsing parameter");
			errorMsg += line;
			ReportError(errorMsg);
			return false;
		}
	}

	m_current->AddInstruction(inst);

	return true;
}

bool CScriptParser::GetWord(const char *line, int wordIndex, char *output)
{
	if(line == NULL || output == NULL)
		return false;

	int word = 0, i = 0, currentWordLen = 0;
	int len = (int)strlen(line);
	bool insideQuotes = false;

	for(;i<len && word < wordIndex;++i)
	{
		if(line[i] == ' ' || line[i] == '\t')
		{
			if(insideQuotes)
				continue;

			if(currentWordLen == 0)
				continue;
			else
			{
				word++;
				currentWordLen = 0;
			}
		}
		else
		{
			if(line[i] == '"')
				insideQuotes = !insideQuotes;
			currentWordLen++;
		}
	}

	if(word != wordIndex)
		return false;

	insideQuotes = false;
	int k;
	for(k=0; i<len, k<MAX_PARAM_STRING_LENGTH-1; ++i)
	{
		if(!insideQuotes)
			if(line[i] == ' ' || line[i] == '\t')
			{
				if(k == 0)
					continue;
				else
					break;
			}
		if(line[i] == '"')
			insideQuotes = !insideQuotes;
		output[k] = line[i];
		++k;
	}
	output[k] = '\0';
	if(k == 0)
		return false;
	return true;
}

bool CScriptParser::ExtractAttribute(char *word, CScript::SAttribute &att, char *type)
{
	if(word == NULL || type == NULL)
		return false;

	if(strlen(word)<1)
	{
		ReportError("Too short attribute");
		return false;
	}

	char *w = word;
	att.variable = false;

	/// Variable
	if(word[0] == '#')
	{
		w++;
		if(!VerifyVariableNameSyntax(w))
		{
			ReportError("Variable name syntax error");
			return false;
		}
		strcpy(att.str_value, w);
		att.variable = true;
		strcpy(att.type, "");
	}
	/// Raw value
	else if(isdigit(word[0]) || word[0]=='-')
	{
		att.num_value = atof(word);
		strcpy(att.type, "NUM");
	}
	else if(word[0] == '$')
	{
		w++;
		if(!strcmp(w, "TRUE"))
		{
			att.bool_value = true;
			strcpy(att.type, "BOOL");
		}
		else if(!strcmp(w, "FALSE"))
		{
			att.bool_value = false;
			strcpy(att.type, "BOOL");
		}
		else
		{
			ReportError("Invalid value type");
			return false;
		}
	}
	/// STRING
	else if(word[0] == '"')
	{
		if(!VerifyStringSyntax(word))
		{
			ReportError("String syntax verification failure");
			return false;
		}
		w++;
		w[strlen(w)-1] = '\0';
		strcpy(att.str_value, w);
		strcpy(att.type, "STR");
	}
	/// VARTYPE
	else
	{
		if(!m_scriptEngine->ValidateVariableType(word))
		{
			ReportError("Unknown variable type");
			return false;
		}
		strcpy(att.str_value, word);
		strcpy(att.type, "VARTYPE");
	}

	if(!att.variable && strcmp(type, "ANY"))
	{
		if(strcmp(att.type, type))
		{
			ReportError("Variable type mismatch");
			return false;
		}
	}

	return true;
}

bool CScriptParser::VerifyVariableNameSyntax(char *name)
{
	if(name == NULL)
		return false;
	int len = (int)strlen(name);
	for(int i=0; i<len; ++i)
		if(!isalpha(name[i]) && !isdigit(name[i]) && name[i] != '_')
			return false;
	return true;
}

bool CScriptParser::VerifyStringSyntax(char *str)
{
	if(str == NULL)
		return false;
	int len = (int)strlen(str);
	if(str[0] != '"')
		return false;
	for(int i=1;i<len;++i)
		if(str[i] == '"')
		{
			if(i==len-1)
				return true;
			else
				return false;
		}
	return false;
}

CScript *CScriptParser::ParseFile(const char *filename, const char *scriptName)
{
	if(filename == NULL)
		return false;
	std::ifstream in;
	in.open(filename);

	if(!in.is_open())
	{
		ReportError("Failed opening script file");
		return false;
	}

	char line[1024];
	if(scriptName == NULL)
		BeginParse(filename);
	else
		BeginParse(scriptName);

	m_current->SetFilename(filename);

	while(in.peek() != EOF)
	{
		in.getline(line, 1024, '\n');
		if(!ParseLine(line))
		{
			in.close();
			InterruptParse();
			return false;
		}
	}
	in.close();
	return EndParse();
}

void CScriptParser::ReportError(std::string errormsg)
{
	m_scriptEngine->ReportError(errormsg);
}