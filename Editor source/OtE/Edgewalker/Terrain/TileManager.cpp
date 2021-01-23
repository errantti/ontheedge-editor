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

#include "TileManager.h"
#include "..\\Scene\\ISceneManager.h"
#include "..\\Utilities.h"

using namespace std;

CTileManager *CTileManager::m_mgr = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileManager::CTileManager() : m_bResourcesLoaded(false), m_texturePath("textures\\tilesets\\"),
m_meshPath("meshes\\tilesets\\"), m_bShowDummyTiles(false)
{
	m_mgr = this;
}

CTileManager::~CTileManager()
{
}

STileInfo *CTileManager::GetTileTemplate(DWORD id)
{
	for(int i=0;i<m_tileTemplates.size();i++)
		if(m_tileTemplates[i].id == id)
			return &m_tileTemplates[i];

	return NULL;
}

STileInfo *CTileManager::GetTileTemplate(TILE_TYPE type, UINT height, bool random)
{
	STileInfo *backup = NULL;
	UINT backupHeight = 0;

	vector<STileInfo>::iterator it = m_tileTemplates.begin();

	vector<STileInfo*> results;

	for(; it != m_tileTemplates.end(); ++it)
		if(it->tileType == type)
		{
			if(it->height == height)
			{
				if(random)
					results.push_back(&(*it));
				else
					return &(*it);
			}
			else
			{
				if(backup == NULL || (it->height > backupHeight && backupHeight < height))
				{
					backup = &(*it);
					backupHeight = it->height;
				}
			}
		}

	if(random && results.size() > 0)
		return results[GetDXApp()->RandInt(0, results.size()-1)];

	return backup;
}

// TEST
void CTileManager::FillDefaultTileData()
{
	static STileInfo s;
	m_tileTemplates.clear();
	s.FillDefaults();

	m_texturePath = "textures\\tilesets\\";
	m_meshPath = "meshes\\tilesets\\";
	m_name = "default tileset1";
	m_filename = "data\\defaulttileset.dtb";

	s.collisionType = TILE_COLLISION_NONE;
	s.id = 0;
	strcpy(s.meshFilename, "");
	strcpy(s.name, "Empty space");
	m_tileTemplates.push_back(s);

	s.collisionType = TILE_COLLISION_BOUNDING_BOX;
	s.id = 1;
	s.height = 0;
	strcpy(s.meshFilename, "T1_platform0.X");
	strcpy(s.name, "Platform");
	m_tileTemplates.push_back(s);

	s.collisionType = TILE_COLLISION_BOUNDING_BOX;
	s.id = 2;
	s.height = 1;
	strcpy(s.meshFilename, "T1_Cliff.x");
	strcpy(s.name, "Cliff");
	m_tileTemplates.push_back(s);

	s.collisionType = TILE_COLLISION_BOUNDING_BOX;
	s.id = 3;
	strcpy(s.meshFilename, "T1_CliffCorner.X");
	strcpy(s.name, "Cliff corner");
	m_tileTemplates.push_back(s);

	s.collisionType = TILE_COLLISION_RAMP;
	s.id = 4;
	strcpy(s.meshFilename, "T1_elevation.X");
	strcpy(s.name, "Ramp");
	m_tileTemplates.push_back(s);
}

bool CTileManager::LoadTileResources()
{
	int i;
	bool succeeded = TRUE;
	CMesh *mesh;
	CTexture *tex;
	CDirectX9 *dx = GetDXApp();

	std::string filename, temp = CMesh::GetCurrentTextureFolder();
	CMesh::SetTextureFolder(m_texturePath);

	CResourceManager &rm = CResourceManager::GetManager();

	for(i=0;i<m_tileTemplates.size();i++)
	{
		if(m_tileTemplates[i].tileType == TILE_TYPE_DUMMY)
			continue;

		// Don't try to load mesh that's not supposed to be loaded
		if(strcmp(m_tileTemplates[i].meshFilename, ""))
		{
			filename = m_meshPath;
			filename += m_tileTemplates[i].meshFilename;
			// skip unloaded meshes
			if((mesh = rm.GetMesh(filename)) != NULL)
			{
				// Modify the mesh's bounding box. The rule for creating tiles is
				// that the 'walking' level should be at y=0 and the colliding part
				// below it. If the artist wants to add extra mesh data into the tile
				// that he doesn't want objects to collide with he can place it
				// above the zero-y-level. Here the bounding box is manually edited
				// to make that possible
				// Do this in CTile from now on
				/*aabbox3d &box = mesh->GetBoundingBox();

				// Set correct tile bounding box size
				box.maxCorner.y = 0.f;
				box.maxCorner.z = 50.f;
				box.maxCorner.x = 50.f;
				box.minCorner.x = 0.f;
				box.minCorner.z = 0.f;
				box.minCorner.y = -m_tileTemplates[i].fHeight;*/
				
				//if(box.minCorner.y >= 0.f)
				//	box.minCorner.y = -0.01f;

				m_tileTemplates[i].mesh = mesh;
			}
			else
				succeeded = FALSE;
		}
		else
			m_tileTemplates[i].mesh = NULL;

		if(strcmp(m_tileTemplates[i].overrideTexture, ""))
		{
			if((tex = rm.GetTexture(m_tileTemplates[i].overrideTexture)) != NULL)
				m_tileTemplates[i].texture = tex;
			else
				succeeded = FALSE;
		}
		else
			m_tileTemplates[i].texture = NULL;
	}

	if(succeeded)
		CPrinter::log("All tile models succesfully loaded.");
	else
		CPrinter::log("Not all tile models could be loaded");

	// restore the default texture folder
	CMesh::SetTextureFolder(temp);

	m_bResourcesLoaded = true;

	return succeeded;
}

bool CTileManager::LoadTileset(const char *filename, bool autoLoadResources)
{
	if(filename == 0)
		return false;

	if(!m_filename.compare(filename))
	{
		// The requested tileset is already loaded
		if(autoLoadResources && !m_bResourcesLoaded)
			LoadTileResources();
		return true;
	}
	else
	{
		// Unload the previous tileset models and textures
		UnloadTilesetResources();
	}

	fstream f;
	f.open(filename, ios::in | ios::binary);
	if(!f.is_open())
	{
		CPrinter::logError("Couldn't open tileset file");
		return false;
	}

	m_filename = filename;

	DWORD id = TILE_FILE_BEGIN_IDENTIFIER;
	char buffer[128];
	try {
		f.read((char*)&id, sizeof(DWORD));
		if(id != TILE_FILE_BEGIN_IDENTIFIER)
		{
			f.close();
			CPrinter::logError("CTileManager::LoadTileset: File begin identifier mismatch: corrupt file or uncompatible version");
			return false;
		}

		f.read(buffer, 64);
		m_name = buffer;
		
		f.read(buffer, 128);
		m_texturePath = buffer;

		f.read(buffer, 128);
		m_meshPath = buffer;

		// Load the tile data
		if(LoadVectorFromBinaryStream<STileInfo>(f, m_tileTemplates, 1000) == FALSE)
			throw exception("Corrupt tileset file");
	} catch (exception) {
		return false;
	}

	// Load the models and textures
	if(autoLoadResources)
		LoadTileResources();

	return true;
}

bool CTileManager::SaveTileset(const char *filename)
{
	fstream f;
	if(filename == NULL)
		f.open(m_filename.c_str(), ios::out | ios::binary);
	else
		f.open(filename, ios::out | ios::binary);

	if(!f.is_open())
	{
		CPrinter::logError("Couldn't open tileset file for saving");
		return false;
	}

	DWORD identifier = TILE_FILE_BEGIN_IDENTIFIER;
	char buffer[128];

	try {
		f.write((const char*)&identifier, sizeof(DWORD));

		strncpy(buffer, m_name.c_str(), 63);
		f.write(buffer, 64);

		strncpy(buffer, m_texturePath.c_str(), 127);
		f.write(buffer, 128);

		strncpy(buffer, m_meshPath.c_str(), 127);
		f.write(buffer, 128);

		SaveVectorToBinaryStream<STileInfo>(f, m_tileTemplates);
	} catch (exception) {
		return false;
	}

	return true;
}

void CTileManager::ResetTileset()
{
	m_name = "";
	m_filename = "";
	m_texturePath = "textures\\tilesets\\";
	m_meshPath = "meshes\\tilesets\\";
	m_tileTemplates.clear();
	UnloadTilesetResources();
}

bool CTileManager::LoadDefaultTileset()
{
	return LoadTileset("data\\tileset.dtb");
}

void CTileManager::UnloadTilesetResources()
{
	CResourceManager::GetManager().ReleaseSpecifiedMeshes(m_meshPath);
	CResourceManager::GetManager().ReleaseSpecifiedTextures(m_texturePath);
	m_bResourcesLoaded = false;
}