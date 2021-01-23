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

#ifndef __CQUESTITEM_H__
#define __CQUESTITEM_H__

#include "IInventoryItem.h"

struct SQuestItemTemplate
{
	long id;
	char name[32];
	char iconTexture[128];
	char description[INVENTORY_ITEM_DESCRIPTION_LENGTH];

	void FillDefaults()
	{
		id = -1;
		name[0] = '\0';
		iconTexture[0] = '\0';
		description[0] = '\0';
	}
};

class CQuestItem : public IInventoryItem
{
public:
	CQuestItem(SQuestItemTemplate &t);

	INVENTORY_ITEM_TYPE GetItemType() {return INVENTORY_ITEM_QUESTITEM;}

	void ApplyTemplate(SQuestItemTemplate &t);

protected:
};

#endif // #ifndef __CQUESTITEM_H__