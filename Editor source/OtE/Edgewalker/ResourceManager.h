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

#ifndef __CRESOURCEMANAGER_H__
#define __CRESOURCEMANAGER_H__

#include <map>

using namespace std;

#include "DirectX9\\Texture.h"
#include "DirectX9\\Mesh.h"
class CFont;

class CResourceManager
{
public:
	CResourceManager();
	~CResourceManager();

	/**
	 * Returns specified texture. If the texture isn't
	 * loaded in the memory then the method automatically
	 * loads it and returns newly created pointer.
	 * NOTICE! It's not recommended to use this method
	 * everywhere as it's a bit slow. It's preferred
	 * that this is only called when first getting the
	 * texture pointer and the pointer is then stored
	 * into whatever called this.
	 **/
	CTexture *GetTexture(string filename);
	/**
	 * Returns specified mesh. If the mesh isn't
	 * loaded in the memory then the method automatically
	 * loads it and returns newly created pointer
	 * NOTICE! It's not recommended to use this method
	 * everywhere as it's a bit slow. It's preferred
	 * that this is only called when first getting the
	 * mesh pointer and the pointer is then stored
	 * into whatever called this.
	 **/
	CMesh *GetMesh(string filename);

	CFont *GetFont(string name, string filename = "", int size = 14, bool bold = false, bool italic = false);

	/**
	 * Adds externally created texture object
	 **/
	bool AddLoadedTexture(CTexture *texture, bool autoRemove = true);
	/**
	 * Adds externally created mesh object
	 **/
	bool AddLoadedMesh(CMesh *mesh, bool autoRemove = true);
	/**
	 * Adds externally create font object
	 * /name - The internal name of the font by which it
	 * is called (same fonts of different settings must be
	 * given different names
	 **/
	bool AddLoadedFont(CFont *font, string name, bool autoRemove = true);

	void ReleaseSpecifiedTextures(string partialFilename);
	void ReleaseSpecifiedMeshes(string partialFilename);
	void ReleaseAll();

	void SetDevice(IDirect3DDevice9 *device) {m_pD3DDevice = device;}

	void SetTextureColorkey(D3DCOLOR color) {m_textureColorkey = color;}

	void RestoreDefaultColorkey() {m_textureColorkey = m_defaultTextureColorkey;}
	void SetDefaultColorKey(D3DCOLOR color) {m_defaultTextureColorkey = color;}

	static CResourceManager &GetManager() {return *m_this;}

private:
	static CResourceManager *m_this;

	bool CheckPartialTextureFilename(const string &str, CTexture* tex);
	bool CheckPartialMeshFilename(const string &str, CMesh* mesh);

	string m_partialFilename;

	map<string, CTexture*> m_textures;
	map<string, CMesh*> m_meshes;
	map<string, CFont*> m_fonts;

	IDirect3DDevice9 *m_pD3DDevice;

	D3DCOLOR m_textureColorkey, m_defaultTextureColorkey;

	CTexture *LoadTexture(string &filename);
	CMesh *LoadMesh(string &filename);
	CFont *LoadFont(string &filename, int size, bool bold, bool italic);
};

#endif // #ifndef __CRESOURCEMANAGER_H__