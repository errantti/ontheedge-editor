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

#ifndef __CUTILITIES_H__
#define __CUTILITIES_H__

#include <vector>
#include <fstream>
#include <string>

/*#include <string>
#include <sstream>

void AppendIntToString(string &str, int value)
{
	string temp;
	stringstream stream;
	stream << value;
	stream >> temp;
	str += temp;
}

void AppendFloatToString(string &str, float value)
{
	string temp;
	stringstream stream;
	stream << value;
	stream >> temp;
	str += temp;
}*/

/**
 * Saves an std::vector of type T to a binary file
 * /param identifier - 32 bit number that is written
 *		to the beginning of the file
 */
template <typename T>
bool SaveVectorToBinaryFile(std::vector<T> &v, const char *filename, DWORD identifier)
{
	if(filename == NULL)
		return false;

	std::fstream f;
	f.open(filename, std::ios::out | std::ios::binary);
	if(!f.is_open())
		return false;

	f.write((const char*)&identifier, sizeof(DWORD));

	DWORD count = (DWORD)v.size();
	f.write((const char*)&count, sizeof(DWORD));

	for(DWORD i=0;i<count;i++)
		f.write((const char*)&v[i], sizeof(T));

	f.close();
	return TRUE;
}

/**
 * Reads an std::vector of type T from a binary file
 * written with for example SaveVectorToBinaryFile
 * /param identifier - 32 bit number that must be
 *		in the beginning of the file or the file is
 *		deemed corrupted or wrong format
 *		If 0, the test is skipped
 */
template <typename T>
bool LoadVectorFromBinaryFile(std::vector<T> &v, const char *filename, DWORD identifier)
{
	if(filename == NULL)
		return false;

	std::fstream f;
	f.open(filename, std::ios::in | std::ios::binary);
	if(!f.is_open())
		return false;
	v.clear();

	DWORD id = 0;

	f.read((char*)&id, sizeof(DWORD));
	if(identifier != 0 && id != identifier)
	{
		f.close();
		return false;
	}

	DWORD count=0;
	f.read((char*)&count, sizeof(DWORD));

	T item;

	for(DWORD i=0;i<count;i++)
	{
		f.read((char*)&item, sizeof(T));
		v.push_back(item);
	}

	f.close();
	return TRUE;
}

/**
 * Saves an std::vector of type T to a binary (file)stream
 */
template <typename T>
void SaveVectorToBinaryStream(std::fstream &f, std::vector<T> &v)
{
	DWORD count = v.size();
	f.write((const char*)&count, sizeof(DWORD));

	for(DWORD i=0;i<count;i++)
		f.write((const char*)&v[i], sizeof(T));
}

/**
 * Reads an std::vector of type T from a binary file
 * written with for example SaveVectorToBinaryFile
 * /param identifier - 32 bit number that must be
 *		in the beginning of the file or the file is
 *		deemed corrupted or wrong format
 *		If 0, the test is skipped
 */
template <typename T>
bool LoadVectorFromBinaryStream(std::fstream &f, std::vector<T> &v, DWORD maxCount)
{
	v.clear();

	DWORD count=0;
	f.read((char*)&count, sizeof(DWORD));

	if(count > maxCount)
		return false;

	T item;

	for(DWORD i=0;i<count;i++)
	{
		f.read((char*)&item, sizeof(T));
		v.push_back(item);
	}

	return true;
}

/**
 * Saves an std::vector of type pair<T,U> to a binary file
 * Only the T-type components of the pair are saved to
 * the file. The caller is responsible for processing 
 * the second component
 * /param identifier - 32 bit number that is written
 *		to the beginning of the file
 */
template <typename T, typename U>
bool SavePairVectorToBinaryFile(std::vector<pair<T,U> > &v, const char *filename, DWORD identifier)
{
	if(filename == NULL)
		return false;

	std::fstream f;
	f.open(filename, std::ios::out | std::ios::binary);
	if(!f.is_open())
		return false;

	f.write((const char*)&identifier, sizeof(DWORD));

	DWORD count = v.size();
	f.write((const char*)&count, sizeof(DWORD));

	std::vector<pair<T,U> >::iterator it = v.begin();
	for(; it != v.end(); ++it)
		f.write((const char*)&it->first, sizeof(T));

	f.close();
	return TRUE;
}

/**
 * Reads an std::vector of type pair<T,U> from a binary file
 * written with for example SaveVectorToBinaryFile.
 * Only reads the first component T from the file,
 * the caller is responsible for processing the U-type
 * component
 * /param identifier - 32 bit number that must be
 *		in the beginning of the file or the file is
 *		deemed corrupted or wrong format
 *		If 0, the test is skipped
 */
template <typename T, typename U>
bool LoadPairVectorFromBinaryFile(std::vector<pair<T,U> > &v, const char *filename, DWORD identifier)
{
	if(filename == NULL)
		return false;

	std::fstream f;
	f.open(filename, std::ios::in | std::ios::binary);
	if(!f.is_open())
		return false;
	v.clear();

	DWORD id = 0;

	f.read((char*)&id, sizeof(DWORD));
	if(identifier != 0 && id != identifier)
	{
		f.close();
		return false;
	}

	DWORD count=0;
	f.read((char*)&count, sizeof(DWORD));

	pair<T,U> item;

	for(int i=0;i<count;i++)
	{
		f.read((char*)&item.second, sizeof(T));
		v.push_back(item);
	}

	f.close();
	return TRUE;
}

/**
 * Reads a single struct from a file into a reference
 */
template<class T>
bool ImportStructFromFile(std::string filename, T &outImport, DWORD beginID)
{
	std::fstream f;
	f.open(filename.c_str(), std::ios::in | std::ios::binary);
	if(!f.is_open())
		return false;

	if(f.peek() == EOF)
		return false;

	DWORD id;
	f.read((char*)&id, sizeof(DWORD));
	if(id != beginID)
		return false;
	f.read((char*)&outImport, sizeof(outImport));

	f.close();

	return true;
}

/**
 * Writes a single struct to a file from a reference
 */
template<class T>
bool ExportStructToFile(std::string filename, T &exportStruct, DWORD beginID)
{
	std::fstream f;
	f.open(filename.c_str(), std::ios::out | std::ios::binary);
	if(!f.is_open())
		return false;

	f.write((const char*)&beginID, sizeof(DWORD));
	f.write((const char*)&exportStruct, sizeof(exportStruct));
	f.close();

	return true;
}

#endif // #ifndef __CUTILITIES_H__