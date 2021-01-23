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

#include "ScriptEngine.h"
#include <iostream>
#include <fstream>

CScriptEngine::CScriptEngine(bool ignoreBasicCommands)
{
	// Adds the very basic instructions (if, endIf, createVar, etc..)
	if(!ignoreBasicCommands)
		AddBasicCommands();
}

CScriptEngine::~CScriptEngine()
{
	UnloadCurrentScripts();
}

void CScriptEngine::AddBasicCommands()
{
	m_basicInsts.AddBasicCommands(this);
}

bool CScriptEngine::AddInstructionTemplate(CInstructionTemplate t)
{
	for(int i=0;i<MAX_ATTRIBUTES;++i)
		if(!ValidateVariableType(t.GetAttributeType(i), true))
			return false;
	m_instTemplates.push_back(t);
	return true;
}

void CScriptEngine::AddVariableType(char *typeName)
{
	if(typeName == NULL)
		return;
	if(strlen(typeName) < MIN_SCRIPT_STRING_LENGTH || strlen(typeName) >= MAX_SCRIPT_STRING_LENGTH)
		return;
	std::string str(typeName);
	m_variableTypes.push_back(str);
}

bool CScriptEngine::ValidateVariableType(char *type, bool templateSource)
{
	if(type == NULL)
		return false;
	// An empty attribute is valid
	if(type[0] == '\0')
		return true;

	if(templateSource)
	{
		// Allow the core type 'ANY' for template attributes
		if(!strcmp(type, "ANY"))
			return true;
	}
	std::string str(type);
	std::vector<std::string>::iterator it = m_variableTypes.begin();
	for(;it!=m_variableTypes.end();++it)
		if(!((std::string)*it).compare(str))
			return true;
	return false;
}

CInstructionTemplate *CScriptEngine::GetInstructionTemplate(int id, int &outIndex)
{
	std::vector<CInstructionTemplate>::iterator it = m_instTemplates.begin();
	outIndex = 0;
	for(;it!=m_instTemplates.end();++it, ++outIndex)
		if(((CInstructionTemplate)*it).GetID()==id)
			return (CInstructionTemplate*)&(*it);
	outIndex = -1;
	return NULL;
}

CInstructionTemplate *CScriptEngine::GetInstructionTemplate(char *name, int &outIndex)
{
	std::vector<CInstructionTemplate>::iterator it = m_instTemplates.begin();
	outIndex = 0;
	for(;it!=m_instTemplates.end();++it, ++outIndex)
		if(!strcmp(((CInstructionTemplate)*it).GetName(), name))
			return (CInstructionTemplate*)&(*it);
	outIndex = -1;
	return NULL;
}

CInstructionTemplate *CScriptEngine::GetInstructionTemplateAt(const int index)
{
	if(index<0 || index>=(int)m_instTemplates.size())
		return NULL;
	return &m_instTemplates[index];
}


ScriptReturn CScriptEngine::ExecuteScript(const std::string name)
{
	std::vector<CScript*>::iterator it = m_scripts.begin();
	for(;it!=m_scripts.end();++it)
		if(!(*it)->GetName().compare(name))
			return (*it)->Execute();
	return SCRIPT_RETURN_ERROR;
}

bool CScriptEngine::AddScript(CScript *script)
{
	if(script == NULL)
		return false;
	m_scripts.push_back(script);
	int errorLine;
	return script->Validate(errorLine);
}

bool CScriptEngine::RemoveScript(const std::string name)
{
	std::vector<CScript*>::iterator it = m_scripts.begin();
	for(;it!=m_scripts.end();++it)
		if(!(*it)->GetName().compare(name))
		{
			delete (*it);
			m_scripts.erase(it);
			return true;
		}
	return false;
}

void CScriptEngine::UnloadCurrentScripts()
{
	std::vector<CScript*>::iterator it = m_scripts.begin();
	for(;it!=m_scripts.end();++it)
		delete (*it);
	m_scripts.clear();
}

void CScriptEngine::UnloadTemporaryScripts()
{
	std::vector<CScript*>::iterator it = m_scripts.begin();
	for(;it!=m_scripts.end();)
	{
		if((*it)->IsTemporary())
		{
			delete (*it);
			it = m_scripts.erase(it);
		}
		else
			++it;
	}
}

bool CScriptEngine::LoadScriptCollection(const std::string filename, bool unloadCurrentScripts, bool autoValidate, bool temporary)
{
	using namespace std;

	if(unloadCurrentScripts)
		UnloadCurrentScripts();

	fstream f;
	f.open(filename.c_str(), ios::in | ios::binary);

	if(!f.is_open())
	{
		ReportError("LoadScriptCollection error: Couldn't open file (file may not exist)");
		return false;
	}

	int version = 0;
	try {
		f.read((char*)&version, sizeof(int));

		if(version != EWSCRIPT_VERSION)
		{
			ReportError("LoadScriptCollection error: version mismatch, file is corrupt or from an old version");
			f.close();
			return false;
		}

		while(f.peek() != EOF)
			if(!AddNewScriptFromCollection(f, temporary))
				throw exception("failed loading script");
	} catch (exception e) {
		string msg = "LoadScriptCollection error: ";
		msg += e.what();
		ReportError(msg);

		f.close();
		return false;
	}

	f.close();

	if(autoValidate)
	{
		int errorLine = 0;
		string msg;
		vector<CScript*>::iterator it = m_scripts.begin();
		for(; it != m_scripts.end(); ++it)
		{
			if(!(*it)->Validate(errorLine))
			{
				msg = "Validation error at ";
				msg += (*it)->GetName();
				msg += " line ";
				msg += errorLine;
				ReportError(msg);
			}
		}
	}

	return true;
}

bool CScriptEngine::SaveScriptCollection(const std::string filename, bool saveAll, bool temporaryOnly)
{
	using namespace std;

	fstream f;
	f.open(filename.c_str(), ios::out | ios::binary);

	if(!f.is_open())
	{
		ReportError("SaveScriptCollection error: Couldn't open file (file may not exist)");
		return false;
	}

	int version = EWSCRIPT_VERSION;
	try {
		f.write((const char*)&version, sizeof(int));

		vector<CScript*>::iterator it = m_scripts.begin();
		for(; it != m_scripts.end(); ++it)
		{
			if(saveAll)
				(*it)->SaveScript(f);
			else
			{
				if(temporaryOnly && (*it)->IsTemporary())
					(*it)->SaveScript(f);
				else if(!temporaryOnly && !(*it)->IsTemporary())
					(*it)->SaveScript(f);
			}
		}
	} catch (exception e) {
		string msg = "SaveScriptCollection error: ";
		msg += e.what();
		ReportError(msg);

		f.close();
		return false;
	}

	f.close();

	return true;
}

bool CScriptEngine::AddNewScriptFromCollection(std::fstream &stream, bool temporary)
{
	CScript *s = new CScript(this);
	if(!s->LoadScript(stream))
	{
		delete s;
		return false;
	}
	s->SetIsTemporary(temporary);
	m_scripts.push_back(s);
	return true;
}

CScript *CScriptEngine::GetScript(const std::string name)
{
	std::vector<CScript*>::iterator it = m_scripts.begin();
	for(;it!=m_scripts.end();++it)
		if((*it)->GetName().compare(name) == 0)
			return (*it);
	return NULL;
}

void CScriptEngine::ReportError(const std::string errorMsg)
{
	if(m_logger)
		m_logger->LogError(errorMsg);
}