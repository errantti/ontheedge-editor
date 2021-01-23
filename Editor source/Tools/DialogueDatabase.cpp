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

#include "DialogueDatabase.h"
#include "DialogueWnd.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDialogueDatabase::CDialogueDatabase() : m_templates(NULL)
{
	m_filename = "data\\dialogues.dtb";
}

CDialogueDatabase::~CDialogueDatabase()
{
}

void CDialogueDatabase::AddNewTemplate(ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	CDialogue d;
	DWORD id = 1;
	std::vector<CDialogue*>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if((*it)->GetID() >= id)
			 id = (*it)->GetID()+1;
	CDialogueWnd w;
	if(w.OpenWindow(parent, d, TRUE) == TRUE)
	{
		d.SetID(id);
		m_templates->push_back(new CDialogue(d));
		UpdateSelectionList(update);
	}
}

void CDialogueDatabase::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	CDialogue d;
	CDialogueWnd w;

	for(int i=0;i<m_templates->size();i++)
		if((*m_templates)[i]->GetID() == id)
		{
			d = *(*m_templates)[i];
			if(w.OpenWindow(parent, d, FALSE))
			{
				*(*m_templates)[i] = d;
				UpdateSelectionList(update);
			}
			return;
		}

	::MessageBox(NULL, "Internal error: template id not found!", "OtE Editor error!", MB_OK);
}

void CDialogueDatabase::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<CDialogue*>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if((*it)->GetID() == id)
		{
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CDialogueDatabase::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	m_templates = m_manager->GetDialogues();

	return true;
}

bool CDialogueDatabase::SaveDatabase()
{
	return true;
}

void CDialogueDatabase::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	std::string s;

	vector<CDialogue*>::iterator it = m_templates->begin();
	for(; it != m_templates->end(); ++it)
	{
		s = (*it)->GetName();
		list->AddEntry(SSelectionEntry(s, (*it)->GetID()));
	}

	list->UpdateList();
}