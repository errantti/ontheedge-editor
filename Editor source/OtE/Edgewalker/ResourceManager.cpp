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

#include "ResourceManager.h"
#include "DirectX9\\Font.h"
#include <algorithm>
#include <iostream>

CResourceManager *CResourceManager::m_this = 0;

CResourceManager::CResourceManager() : m_pD3DDevice(0), m_textureColorkey(0), m_defaultTextureColorkey(0)
{
	if(m_this == 0)
		m_this = this;
}

CResourceManager::~CResourceManager()
{
	if(m_this == this)
		m_this = 0;
	ReleaseAll();
}

void CResourceManager::ReleaseAll()
{
	map<string, CTexture*>::iterator tIt;
	map<string, CMesh*>::iterator mIt;
	map<string, CFont*>::iterator fIt;
	for(tIt = m_textures.begin(); tIt != m_textures.end(); ++tIt)
		if(tIt->second != 0)
			delete tIt->second;
	for(mIt = m_meshes.begin(); mIt != m_meshes.end(); ++mIt)
		if(mIt->second != 0)
			delete mIt->second;
	for(fIt = m_fonts.begin(); fIt != m_fonts.end(); ++fIt)
		if(fIt->second != 0)
			delete fIt->second;
	m_textures.clear();
	m_meshes.clear();
	m_fonts.clear();
}

bool CResourceManager::CheckPartialTextureFilename(const string &str, CTexture* tex)
{
	/*if(tex != 0)
	{
		if(str.find(m_partialFilename) != -1)
		{
			// If the texture filename matches the partial filename
			// delete the texture resource and mark this unit to be
			// erased from map
			delete tex;
			return true;
		}
	}*/
	return false;
}

bool CResourceManager::CheckPartialMeshFilename(const string &str, CMesh* mesh)
{
	/*if(mesh != 0)
	{
		if(str.find(m_partialFilename) != -1)
		{
			// If the mesh filename matches the partial filename
			// delete the mesh resource and mark this unit to be
			// erased from map
			delete mesh;
			return true;
		}
	}*/
	return false;
}

void CResourceManager::ReleaseSpecifiedTextures(string partialFilename)
{
	//m_partialFilename = partialFilename;
	//m_textures.erase(remove_if(m_textures.begin(), m_textures.end(), CheckPartialTextureFilename), m_textures.end());
	map<string, CTexture*>::iterator it = m_textures.begin();
	for(; it != m_textures.end();)
	{
		//if(CheckPartialTextureFilename(it->first, it->second))
		if(it->first.find(partialFilename) != -1)
		{
			map<string, CTexture*>::iterator temp = it;
			delete it->second;
			++it;
			m_textures.erase(temp);
		}
		else
			++it;
	}
}

void CResourceManager::ReleaseSpecifiedMeshes(string partialFilename)
{
	//m_partialFilename = partialFilename;
	//partialFilename = "möh";
	//m_meshes.erase(remove_if(m_meshes.begin(), m_meshes.end(), CheckPartialMeshFilename), m_meshes.end());
	map<string, CMesh*>::iterator it = m_meshes.begin();
	for(; it != m_meshes.end();)
	{
		if(it->first.find(partialFilename) != -1)
		//if(CheckPartialMeshFilename(it->first, it->second))
		{
			map<string, CMesh*>::iterator temp = it;
			delete it->second;
			++it;
			m_meshes.erase(temp);
		}
		else
			++it;
	}
}

CTexture *CResourceManager::GetTexture(string filename)
{
	// Check whether the the texture already exists in the map
	map<string, CTexture*>::iterator it = m_textures.find(filename);
	if(it != m_textures.end())
		return it->second;

	// If it isn't there already, load it
	CTexture *tex = LoadTexture(filename);
	if(tex == 0)
		return 0;

	// ...and insert it into the map checking whether the insertion operation
	// was succesfull
	pair<map<string, CTexture*>::iterator, bool> pr;
	pr = m_textures.insert(pair<string, CTexture*>(filename, tex));
	if(!pr.second)
	{
		delete tex;
		CPrinter::log("CResourceManager::GetTexture error: insertion to map failed");
		return 0;
	}

	return tex;
}

CMesh *CResourceManager::GetMesh(string filename)
{
	// Check whether the the texture already exists in the map
	map<string, CMesh*>::iterator it = m_meshes.find(filename);
	if(it != m_meshes.end())
		return it->second;

	// If it isn't there already, load it
	CMesh *mesh = LoadMesh(filename);
	if(mesh == 0)
		return 0;

	// ...and insert it into the map checking whether the insertion operation
	// was succesfull
	pair<map<string, CMesh*>::iterator, bool> pr;
	pr = m_meshes.insert(pair<string, CMesh*>(filename, mesh));
	if(!pr.second)
	{
		delete mesh;
		CPrinter::log("CResourceManager::GetMesh error: insertion to map failed");
		return 0;
	}

	return mesh;
}

CFont *CResourceManager::GetFont(string name, string filename, int size, bool bold, bool italic)
{
	// Check whether the font already exists in the map
	map<string, CFont*>::iterator it = m_fonts.find(name);
	if(it != m_fonts.end())
		return it->second;

	// If it isn't there already, load it
	CFont *font = LoadFont(filename, size, bold, italic);
	if(font == 0)
		return 0;

	// ...and insert it into the map checking whether the insertion operation
	// was succesfull
	pair<map<string, CFont*>::iterator, bool> pr;
	pr = m_fonts.insert(pair<string, CFont*>(name, font));
	if(!pr.second)
	{
		delete font;
		CPrinter::log("CResourceManager::GetFont error: insertion to map failed");
		return 0;
	}

	return font;
}

bool CResourceManager::AddLoadedTexture(CTexture *texture, bool autoRemove)
{
	if(texture == 0)
		return false;

	pair<map<string, CTexture*>::iterator, bool> pr;

	pr = m_textures.insert(pair<string, CTexture*>(texture->GetFilename(), texture));
	if(!pr.second)
	{
		if(autoRemove)
			delete texture;
		return false;
	}

	return true;
}

bool CResourceManager::AddLoadedMesh(CMesh *mesh, bool autoRemove)
{
	if(mesh == 0)
		return false;

	pair<map<string, CMesh*>::iterator, bool> pr;

	pr = m_meshes.insert(pair<string, CMesh*>(mesh->GetFilename(), mesh));
	if(!pr.second)
	{
		if(autoRemove)
			delete mesh;
		return false;
	}

	return true;
}

bool CResourceManager::AddLoadedFont(CFont *font, string name, bool autoRemove)
{
	if(font == 0)
		return false;

	pair<map<string, CFont*>::iterator, bool> pr;

	pr = m_fonts.insert(pair<string, CFont*>(name, font));
	if(!pr.second)
	{
		if(autoRemove)
			delete font;
		return false;
	}

	return true;
}

CTexture *CResourceManager::LoadTexture(string &filename)
{
	if(!filename.compare(""))
		return 0;
	CTexture *tex = new CTexture();
	if(!tex->LoadTexture(filename.c_str(), m_textureColorkey))
	{
		delete tex;
		/*CPrinter::log("CResourceManager::LoadTexture: Failed loading texture", LOG_ERROR);
		CPrinter::logError(filename);*/
		CPrinter::logs("CResourceManager::LoadTexture: Failed loading texture " + filename, LOG_ERROR);
		return 0;
	}
	return tex;
}

CMesh *CResourceManager::LoadMesh(string &filename)
{
	if(!filename.compare(""))
		return 0;
	CMesh *mesh = new CMesh();
	if(!mesh->LoadMesh(filename.c_str(), m_pD3DDevice))
	{
		delete mesh;
		string error = "CResourceManager::LoadMesh: Failed loading mesh ";
		error += filename;
		CPrinter::logError(error);
		return 0;
	}
	return mesh;
}

CFont *CResourceManager::LoadFont(string &filename, int size, bool bold, bool italic)
{
	CFont *font = new CFont();
	if(!font->Create(filename.c_str(), size, GetDXApp()->GetDevice(), bold, italic, false, false))
	{
		delete font;
		CPrinter::logError("CResourceManager::LoadFont: Failed loading font");
		return 0;
	}
	return font;
}