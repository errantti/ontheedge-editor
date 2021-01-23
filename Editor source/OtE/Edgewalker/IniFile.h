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

#include <string>
#include <vector>

/**
 * Class for easily reading and writing ini-files
 */
class CIniFile
{
public:
	CIniFile();
	~CIniFile();

	bool ReadFile(const std::string filename);
	bool WriteFile(const std::string filename = "");

	bool WasReadSuccesfully() {return m_bReadSuccesfull;}

	bool ReadBoolValue(std::string section, std::string key, bool defaultValue);
	std::string ReadStringValue(std::string section, std::string key, std::string defaultValue);
	int ReadIntValue(std::string section, std::string key, int defaultValue);
	double ReadDoubleValue(std::string section, std::string key, double defaultValue);

	void WriteBoolValue(std::string section, std::string key, bool value);
	void WriteStringValue(std::string section, std::string key, std::string value);
	void WriteIntValue(std::string section, std::string key, int value);
	void WriteDoubleValue(std::string section, std::string key, double value);

	bool DoesKeyExist(std::string section, std::string key);

private:
	struct SValue
	{
		std::string key;
		std::string value;

		SValue() {}
		SValue(std::string key, std::string value) : key(key), value(value) {}
	};

	struct SSection
	{
		std::vector<SValue> values;
		std::string name;

		SSection() {}
		SSection(std::string name) : name(name) {}
	};

	SValue *FindKey(std::string section, std::string key);
	void ParseLine(const std::string &line);
	void AddSection(const std::string name);
	void AddKey(SValue keyValue);

	bool m_bReadSuccesfull;
	int m_currentSectionIndex;

	std::vector<SSection> m_sections;

	std::string m_filename;
};