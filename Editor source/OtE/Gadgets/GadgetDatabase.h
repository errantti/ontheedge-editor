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

#ifndef __CGADGETDATABASE_H__
#define __CGADGETDATABASE_H__

#include "IGadget.h"
#include <vector>

#define GADGET_DATABASE_BEGIN_IDENTIFIER	0x958B2C86

/**
 * CGadgetDatabase stores all gadgets, reads and writes information
 * about gadgets to file
 */
class CGadgetDatabase
{
public:
	CGadgetDatabase();
	~CGadgetDatabase();

	IGadget *CreateGadget(int id);
	pair<SGadgetTemplate, SCustomGadgetTemplate*> *GetGadgetTemplate(int id);
	void AddGadgetTemplate(pair<SGadgetTemplate, SCustomGadgetTemplate*> gadget);

	bool GetGadgetName(long id, std::string &outName);

	bool LoadDatabase(const char *filename);
	bool SaveDatabase(const char *filename);

	bool ExportTemplate(long id, std::string fullPath);
	bool ImportTemplate(long id, std::string fullPath);

	long GetNewID();

	bool LoadDefaultDatabase();

	void LoadDefaultGadgets();

	inline vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> > *GetGadgetTemplates() {return &m_gadgets;}

	void UnloadTemplates();

	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> > GetMultiplayerGadgets();

private:
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> > m_gadgets;

	//vector<CTexture*> m_iconTextures;
};

#endif // #ifndef __CGADGETDATABASE_H__
