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

#include "IniFile.h"

#include <fstream>
#include <sstream>
using namespace std;

CIniFile::CIniFile() : m_bReadSuccesfull(false), m_currentSectionIndex(-1)
{
}

CIniFile::~CIniFile()
{
}

bool CIniFile::ReadFile(const string filename)
{
	m_filename = filename;

	ifstream file(filename.c_str());
	if(!file.is_open())
		return false;

	char buffer[1024];
	while(file.peek() != EOF)
	{
		file.getline(buffer, 1023);
		buffer[1023] = '\0';
		string line(buffer);
		ParseLine(line);
	}

	m_bReadSuccesfull = true;

	file.close();

	return true;
}

void CIniFile::ParseLine(const string &line)
{
	if(line.empty())
		return;

	if(line[0] == '[')
	{
		// Section definitions
		int pos;
		if((pos = (int)line.find(']')) == (int)string::npos)
			return;
		AddSection(line.substr(1, pos-1));
	}
	else if(isalpha(line[0]))
	{
		// Key definition
		int pos;
		if((pos = (int)line.find('=')) == (int)string::npos)
			return;
		AddKey(SValue(line.substr(0, pos), line.substr(pos+1)));
	}
}

bool CIniFile::WriteFile(const string filename)
{
	string fName(filename);
	if(fName == "")
		fName = m_filename;

	ofstream f(fName.c_str());
	if(!f.is_open())
		return false;

	vector<SSection>::iterator it = m_sections.begin();
	vector<SValue>::iterator vIt;
	for(; it != m_sections.end(); ++it)
	{
		SSection &s = *it;
		f << endl << '[' << s.name << ']' << endl;

		for(vIt = s.values.begin(); vIt != s.values.end(); ++vIt)
		{
			f << vIt->key << "=" << vIt->value << endl;
		}
	}
	f.close();

	return true;
}

void CIniFile::AddSection(const string name)
{
	vector<SSection>::iterator it = m_sections.begin();
	for(int i=0; it != m_sections.end(); ++it, ++i)
		if(it->name == name)
		{
			m_currentSectionIndex = i;
			return;
		}
	m_sections.push_back(SSection(name));
	m_currentSectionIndex = (int)m_sections.size()-1;
}

void CIniFile::AddKey(SValue keyValue)
{
	if(m_currentSectionIndex == -1)
		return;

	SSection &s = m_sections[m_currentSectionIndex];
	vector<SValue>::iterator it = s.values.begin();
	for(; it != s.values.end(); ++it)
		if(it->key == keyValue.key)
		{
			it->value = keyValue.value;
			return;
		}
	s.values.push_back(keyValue);
}

CIniFile::SValue *CIniFile::FindKey(string section, string key)
{
	bool foundSection = false;
	vector<SSection>::iterator it = m_sections.begin();
	for(int i=0; it != m_sections.end(); ++it, ++i)
		if(it->name == section)
		{
			foundSection = true;
			break;
		}
	if(!foundSection)
		return 0;

	SSection &s = *it;
	vector<SValue>::iterator it2 = s.values.begin();
	for(; it2 != s.values.end(); ++it2)
		if(it2->key == key)
			return &*it2;
	return 0;
}

bool CIniFile::ReadBoolValue(std::string section, std::string key, bool defaultValue)
{
	SValue *v = FindKey(section, key);
	if(v == 0)
		return defaultValue;

	if(v->value.empty())
		return defaultValue;

	if(v->value == "0")
		return false;
	if(v->value == "1")
		return true;
	if(v->value == "true")
		return true;
	if(v->value == "false")
		return false;

	return defaultValue;
}

std::string CIniFile::ReadStringValue(std::string section, std::string key, std::string defaultValue)
{
	SValue *v = FindKey(section, key);
	if(v == 0)
		return defaultValue;
	return v->value;
}

int CIniFile::ReadIntValue(std::string section, std::string key, int defaultValue)
{
	SValue *v = FindKey(section, key);
	if(v == 0)
		return defaultValue;

	int num = atoi(v->value.c_str());
	return num;
}

double CIniFile::ReadDoubleValue(std::string section, std::string key, double defaultValue)
{
	SValue *v = FindKey(section, key);
	if(v == 0)
		return defaultValue;

	double num = atof(v->value.c_str());
	return num;
}

bool CIniFile::DoesKeyExist(std::string section, std::string key)
{
	SValue *v = FindKey(section, key);
	if(v == 0)
		return false;
	return true;
}

void CIniFile::WriteBoolValue(std::string section, std::string key, bool value)
{
	AddSection(section);
	if(value)
		AddKey(SValue(key, "1"));
	else
		AddKey(SValue(key, "0"));
}

void CIniFile::WriteStringValue(std::string section, std::string key, std::string value)
{
	AddSection(section);
	AddKey(SValue(key, value));
}

void CIniFile::WriteIntValue(std::string section, std::string key, int value)
{
	AddSection(section);
	stringstream s;
	s << value;
	AddKey(SValue(key, s.str()));
}

void CIniFile::WriteDoubleValue(std::string section, std::string key, double value)
{
	AddSection(section);
	stringstream s;
	s << value;
	AddKey(SValue(key, s.str()));
}