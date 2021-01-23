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

#ifndef __CTILEMANAGER_H__
#define __CTILEMANAGER_H__

#include <vector>
#include <string>
#include "..\\DirectX9\\DirectX9.h"

enum TILE_COLLISION_TYPE {
	TILE_COLLISION_BOUNDING_BOX,
	TILE_COLLISION_ACCURATE,
	TILE_COLLISION_RAMP,
	TILE_COLLISION_CYLINDER,
	TILE_COLLISION_NONE
};

#define TILE_DIMENSION		50.f
#define TILE_LEVEL_HEIGHT	25.f
#define ONE_PER_TILE_DIMENSION	(1.f/TILE_DIMENSION)
#define MAX_HEIGHT_LEVEL	7
#define MAX_TILES_PER_NODE	8

#define TILE_FILE_BEGIN_IDENTIFIER 0x0F38BDF1

/**
 * If CUT_TILE_AABBOX_TOP is defined true, the tile mesh's
 * aabbox's will be restricted to negative y-axel to enable
 * tiles that have parts that don't affect collision
 * detection
 */
#define CUT_TILE_AABBOX_TOP			true

// For future features - to automatize elevation cornering
enum TILE_TYPE {
	TILE_TYPE_PLATFORM=0,
	TILE_TYPE_PLATFORM_CORNER,
	TILE_TYPE_PLATFORM_DOUBLECORNER,
	TILE_TYPE_PLATFORM_DOUBLECORNERCROSS,
	TILE_TYPE_PLATFORM_TRIPLECORNER,
	TILE_TYPE_PLATFORM_FOURCORNER,
	TILE_TYPE_WALL,
	TILE_TYPE_WALL_CORNER,
	TILE_TYPE_WALL_BLOCK,
	TILE_TYPE_CATWALK,
	TILE_TYPE_CATWALK_END,
	TILE_TYPE_RAMP,
	TILE_TYPE_HOLE,
	TILE_TYPE_EMPTY,
	TILE_TYPE_OTHER,

	/**
	 * Special tile type, dummy tiles don't have meshes or any other
	 * kind of visual representation by themselves, they are only used
	 * to create collision detection for neighboring tiles that have
	 * meshes larger than usual. For example there are four tiles
	 * numbered 1,2,3 and 4 like this:
	 * 1 2
	 * 3 4
	 * Let's say that tile 1 is given a mesh that is 100x50x100 instead
	 * of the usual 50x25x50. Tile one's mesh thus overlaps tiles' 2,3
	 * 4 meshes and looks bad. So tiles 2,3 and 4 are marked dummy tiles
	 * and positioned so that the collision detection looks good on the
	 * tile 1's whole mesh. This can be exploited to create large and
	 * unique areas
	 */
	TILE_TYPE_DUMMY = 5000,
	TILE_TYPE_FORCEDWORD=0xFFFFFFFF
};

struct STileInfo
{
	char meshFilename[128];
	char overrideTexture[128];
	/// Filename of the mesh that is used for collision detection if collisionType = accurate
	char collisionMeshFilename[128];

	/// Name of the tile
	char name[64];
	/// Name of the category of the tile (either by themes or tile types, has no meaning for gameplay)
	char subcategory[32];
	DWORD id;
	/// Height of the tile (height levels, ramp for example is always one, one height level = 25.f)
	UINT height;
	/// Exact height (for future use)
	float fHeight;

	/// Shape param, for example custom radius of the mesh for cylinder collision
	float shapeParam;
	TILE_COLLISION_TYPE collisionType;
	TILE_TYPE tileType;
	CMesh *mesh, *collisionMesh;
	CTexture *texture;

	STileInfo() : mesh(NULL), texture(NULL), collisionMesh(NULL)
	{
	}

	void FillDefaults()
	{
		collisionType = TILE_COLLISION_BOUNDING_BOX;
		tileType = TILE_TYPE_PLATFORM;
		id = 1;
		meshFilename[0] = '\0';
		collisionMeshFilename[0] = '\0';
		overrideTexture[0] = '\0';
		name[0] = '\0';
		subcategory[0] = '\0';
		height = 1;
		fHeight = 25.f;
		shapeParam = 25.f;
	}
};

class CTileManager
{
public:
	CTileManager();
	virtual ~CTileManager();

	static CTileManager *GetManager() {return m_mgr;}

	// Loads info about tile templates from a file
	bool LoadTileset(const char *filename, bool autoLoadResources = true);
	// Saves the current info about tile templates into a file
	bool SaveTileset(const char *filename);

	// Loads all the meshes of the tile templates in the 'database'
	bool LoadTileResources();
	void UnloadTilesetResources();

	void ResetTileset();

	void SetDefaultTilesetFilename(std::string filename) {m_filename = filename;}
	void SetTexturePath(std::string path) {m_texturePath = path;}
	void SetMeshPath(std::string path) {m_meshPath = path;}
	void SetName(std::string name) {m_name = name;}

	std::string GetFilename() {return m_filename;}
	std::string GetName() {return m_name;}
	std::string GetTexturePath() {return m_texturePath;}
	std::string GetMeshPath() {return m_meshPath;}

	// Add a new tile template to the 'database'
	void AddTileTemplate(STileInfo);

	// Returns how many tile templates there are in the 'database'
	int GetTileCount() {return (int)m_tileTemplates.size();}

	vector<STileInfo> *GetTileTemplates() {return &m_tileTemplates;}

	// Get tile template by id
	STileInfo *GetTileTemplate(DWORD id);
	// Get tile template by filename
	STileInfo *GetTileTemplate(char *filename);
	/**
	 * Get tile template by tile type (the first that mathes the type)
	 * /param random = if true the returned tile is chosen at random
	 *	from all the templates that satisfy the requirements instead
	 *	of the first one
	 */
	STileInfo *GetTileTemplate(TILE_TYPE type, UINT height = 1, bool random = false);

	inline vector<STileInfo> &GetTemplateVector() {return m_tileTemplates;}

	inline bool GetShowDummyTiles() {return m_bShowDummyTiles;}
	inline void SetShowDummyTiles(bool value) {m_bShowDummyTiles = value;}

	bool LoadDefaultTileset();

	// TEST
	void FillDefaultTileData();

protected:
	static CTileManager *m_mgr;

	bool m_bResourcesLoaded, m_bShowDummyTiles;

	/// Name of the currently loaded tileset
	std::string m_name;
	std::string m_filename;

	/// Texture file path
	std::string m_texturePath;
	/// Mesh file path
	std::string m_meshPath;

	/// Tiles of the currently loaded tileset
	std::vector<STileInfo> m_tileTemplates;
};

#endif // #ifndef __CTILEMANAGER_H__
