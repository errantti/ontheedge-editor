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

#ifndef __CAMMODATABASE_H__
#define __CAMMODATABASE_H__

#include "Edgewalker\\Scene\\MeshSceneNode.h"
#include "Objects\\Ammo.h"
#include <vector>

#define AMMO_FILE_BEGIN_IDENTIFIER	0xB9738AC2

class CAmmoDatabase
{
public:
	CAmmoDatabase();
	virtual ~CAmmoDatabase();

	SAmmoTemplate *GetAmmoTemplate(long id);
	void AddAmmoTemplate(SAmmoTemplate temp) {m_ammoTemplates.push_back(temp);}
	//void AddAmmoTemplate(pair<SAmmoTemplate, SEffects> temp) {m_ammoTemplates.push_back(temp);}

	void ConstructReferences();
	bool LoadDatabase(const char *filename);
	bool SaveDatabase(const char *filename);

	void ClearDatabase();

	bool LoadDefaultDatabase();

	void LoadDefaultAmmos();

	bool ExportTemplate(long id, std::string fullPath);
	bool ImportTemplate(long id, std::string fullPath);

	long GetNewID();

	inline vector<SAmmoTemplate> *GetAmmoTemplates() {return &m_ammoTemplates;}

private:
	vector<SAmmoTemplate> m_ammoTemplates;
};

#endif // #ifndef __CAMMODATABASE_H__
