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

#include "TileDatabase.h"
#include "..\\Windows\\TileEditWnd.h"
#include "..\\..\\OtE\\Edgewalker\\Scene\\ISceneManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileDatabase::CTileDatabase() : m_templates(NULL)
{
	m_filename = "data\\tileset.dtb";
}

CTileDatabase::~CTileDatabase()
{
}

void CTileDatabase::AddNewTemplate(ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	STileInfo d;
	DWORD id = 1;
	for(int i=0;i<m_templates->size();i++)
	{
		if((*m_templates)[i].id >= id)
			 id = (*m_templates)[i].id+1;
	}
	CTileEditWnd w;
	if(w.OpenWindow(parent, d, TRUE) == TRUE)
	{
		d.id = id;

		// Load the tile model
		/*if(strcmp(d.meshFilename, ""))
		{
			d.mesh = CResourceManager::GetManager().GetMesh(d.meshFilename);
			if(d.mesh == NULL)
				CPrinter::log("Failed loading tile mesh", LOG_ERROR);
		}*/
		d.mesh = 0;

		m_templates->push_back(d);
		UpdateSelectionList(update);
	}
}

void CTileDatabase::EditTemplate(long id, ISelectionList *update, HWND parent)
{
	if(m_templates == NULL)
		return;

	STileInfo d;
	CTileEditWnd w;
	for(int i=0;i<m_templates->size();i++)
		if((*m_templates)[i].id == id)
		{
			d = (*m_templates)[i];
			if(w.OpenWindow(parent, d, FALSE))
			{
				// Update mesh
				if(strcmp(d.meshFilename, (*m_templates)[i].meshFilename))
				{
					// Mesh has changed
					/*if(strcmp(d.meshFilename, ""))
					{
						// Load the new mesh
						d.mesh = CResourceManager::GetManager().GetMesh(d.meshFilename);
						if(d.mesh == NULL)
							CPrinter::log("Failed loading tile mesh", LOG_ERROR);
					}
					else*/
						d.mesh = NULL;
				}
				(*m_templates)[i] = d;
				UpdateSelectionList(update);
			}
			return;
		}

	::MessageBox(NULL, "Internal error: template id not found!", "OtE Editor error!", MB_OK);
}

void CTileDatabase::RemoveTemplate(long id, ISelectionList *update)
{
	if(m_templates == NULL)
		return;

	std::vector<STileInfo>::iterator it = m_templates->begin();
	for(;it != m_templates->end(); it++)
		if(it->id == id)
		{
			m_templates->erase(it);
			UpdateSelectionList(update);
			return;
		}
}

bool CTileDatabase::LoadDatabase()
{
	if(m_manager == NULL)
		return false;

	m_templates = m_manager->GetTileTemplates();

	return true;
}

bool CTileDatabase::SaveDatabase()
{
	if(m_manager == NULL)
		return false;

	if(m_manager->SaveTileset(NULL))
	{
		::MessageBox(NULL, "Tileset saved succesfully!", "OtE Editor", MB_OK);
		return true;
	}

	::MessageBox(NULL, "Failed saving tileset!", "OtE Editor error", MB_OK);

	return false;
}

void CTileDatabase::UpdateSelectionList(ISelectionList *list)
{
	if(m_templates == NULL || list == NULL)
		return;

	list->ClearEntries();

	for(int i=0;i<m_templates->size();i++)
		list->AddEntry(SSelectionEntry((*m_templates)[i].name, (*m_templates)[i].id, (*m_templates)[i].subcategory));

	list->UpdateList();
}