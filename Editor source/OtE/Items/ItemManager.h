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

#ifndef __CITEMMANAGER_H__
#define __CITEMMANAGER_H__

#include "ShipPatch.h"
#include "QuestItem.h"
#include "GadgetComponent.h"

#include <vector>

#define ITEM_FILE_BEGIN_IDENTIFIER		0x2938DB10

/**
 * Stores the item templates (except for gadgets) and handles
 * reading and writing the item database
 */
class CItemManager
{
public:
	CItemManager();
	~CItemManager();

	CShipPatch *CreateShipPatch(long id);
	CQuestItem *CreateQuestItem(long id);
	CGadgetComponent *CreateGadgetComponent(long id);

	SShipPatchTemplate *GetPatchTemplate(long id);
	SQuestItemTemplate *GetQuestItemTemplate(long id);
	SGadgetComponentTemplate *GetComponentTemplate(long id);

	bool LoadDatabase(const char *filename);
	bool SaveDatabase(const char *filename);

	void ClearDatabase();

	std::vector<SShipPatchTemplate> &GetPatchTemplates() {return m_patchTemplates;}
	std::vector<SQuestItemTemplate> &GetQuestItemTemplates() {return m_questTemplates;}
	std::vector<SGadgetComponentTemplate> &GetComponentTemplates() {return m_componentTemplates;}

	bool ExportQuestItemTemplate(long id, std::string fullPath);
	bool ImportQuestItemTemplate(long id, std::string fullPath);

	bool ExportShipPatchTemplate(long id, std::string fullPath);
	bool ImportShipPatchTemplate(long id, std::string fullPath);

	bool ExportGadgetComponentTemplate(long id, std::string fullPath);
	bool ImportGadgetComponentTemplate(long id, std::string fullPath);

	long GetNewQuestItemID();
	long GetNewShipPatchID();
	long GetNewGadgetComponentID();

	static CItemManager &GetManager() {return *m_this;}

protected:
	std::vector<SShipPatchTemplate> m_patchTemplates;
	std::vector<SQuestItemTemplate> m_questTemplates;
	std::vector<SGadgetComponentTemplate> m_componentTemplates;

	static CItemManager *m_this;
};

#endif // #ifndef __CITEMMANAGER_H__