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

#ifndef __CTILEDTERRAIN_H__
#define __CTILEDTERRAIN_H__

#include "TileQuadTreeNode.h"

class CTiledTerrain : public CTileQuadTreeNode
{
public:
	CTiledTerrain(ISceneNode *parent, ISceneManager *manager, DWORD id = -1);
	virtual ~CTiledTerrain();

	virtual void OnPreRender(bool isShadowPass);
	virtual void Render(bool isShadowPass);
	virtual void Update(float frameTime) {}

	// Create
	// Creates a tiled terrain object from input
	// /param tiles - tiles of the map as a STile-array
	bool Create(STile *tiles, POINT dimensions);
	vector<CTile*> *GetTileArray() {return &m_tiles;}
	void Release();

	inline CTile *GetTile(unsigned int x, unsigned int y);
	CTile *GetTileByIndex(int x, int y);
	CTile *GetTile(float x, float z);

	/**
	 * Iterates through the given tile and all eight tiles
	 * surrounding it and changes their type and rotation
	 * to fit together
	 * /param randomize - if true then instead of choosing
	 *   the first tile that fits the requirements the
	 *	 algorithm goes through all fitting tiles and chooses
	 *	 one of the at random resulting in more varied but
	 *	 still valid ground formations
	 */
	void TrimTiles(CTile *center, bool randomize = false);

	STile *GetTileInfoArray(POINT &dimensions);

	POINT GetMapSize() {return m_size;}

	inline float GetWidth() {return m_width;}
	inline float GetDepth() {return m_depth;}

	bool GetMapLoaded() {return m_bMapLoaded;}

	virtual void CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions);
	virtual void CheckCollisionsQuadtree(IGeometricObject *object, vector<SCollision> &collisions);
	virtual void CheckCollisionsSimple(IGeometricObject *object, vector<SCollision> &collisions);

	/**
	 * Checks whether given objects collides with terrain and reports
	 * the collision to the object. Doesn't calculate collision vector
	 * or send collision notification to physics engine
	 */
	void CheckIfCollides(IGeometricObject *object);

	float GetTerrainHeight(float x, float z);

	CTile *GetTileByRay(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir);
	bool GetRayIntersectLocation(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir, D3DXVECTOR3 &outIntersection);

	/**
	 * Finds the shortest path from location start to location target. If the path doesn't exist or it is longer
	 * than the maximum length specified by maxPathLength the method fails. If this happens then if acceptIncomplete
	 * is true, outPath is set to include the path that comes closest to the target location. Otherwise outPath
	 * will include the complete path from start to target. The path is also optimized to remove waypoints which are
	 * in line and thus useless or even performance hogs.
	 * The algorithm used is A* which uses the m_terrainNodes vector which is pregenerated when the tile map is
	 * loaded. Thus dynamic tree generation is not required which makes the algorithm rather efficient.
	 */
	bool FindPath(D3DXVECTOR3 start, D3DXVECTOR3 target, bool acceptIncomplete, vector<D3DXVECTOR3> &outPath, int iterationLimit = 50);

	/**
	 * Finds out whether the way between locations start and target is clear of terrain obstacles or drops and can be
	 * walked safely. This is not a path find method, this simply checks the straight line between the two given points.
	 *
	 * source - coordinates of the start location
	 * target - coordinates of the target location
	 * outIsPit - if the method fails, this value is set to true if the reason was a pit on the
	 *  way and false if it was a wall
	 * outLastGoodTile - if the method fails, this is coordinates to the last 'good' location (the last tile before wall or pit)
	 */
	bool IsWayClear(D3DXVECTOR3 start, D3DXVECTOR3 target, bool &outIsPit, D3DXVECTOR3 &outLastGoodTile);

protected:
	struct STerrainPointNode
	{
		// static terrain variables
		union {
			struct {
				int north,
					south,
					east,
					west;
				int northeast,
					northwest,
					southwest,
					southeast;
			};
			int neighbors[8];
		};
		int height;
		int x,z;

		// variables to be used by the route finding algorithm
		int pathFindIndex;	// an ascending number which is set when this node is used. One path index per path search
		float pathCost;		// Cost of the shortest path to this node
		float costEstimate;	// Estimation of the remaining path length
		int prevIndex;		// Index of the previous node in the current path
		bool bRamp;			// Tells whether this node is a ramp node

		void FillDefaults()
		{
			north = south = east = west = -1;
			northeast = northwest = southeast = southwest = -1;
			height = 0;

			pathCost = 10000;
			pathFindIndex = -1;
			prevIndex = -1;
			costEstimate = 0;
			bRamp = false;
		}
	};

	struct SNodeIndex
	{
		int index;
		float cost;

		SNodeIndex() : index(-1), cost(10000) {}

		SNodeIndex(int index, float cost) : cost(cost), index(index) {}

		bool operator<( const SNodeIndex &a ) const {
			return cost > a.cost;
		}
	};

	void CheckVCTile(int x, int y);
	void CheckVCTile(int i);

	void TrimTile(CTile *tile, bool randomize);

	void ConstructNodeTable(POINT dimensions);
	bool CheckAccessFromTile(int x1, int y1, int x2, int y2);
	bool IsTileHigherThanOrRamp(int height, int x, int y);
	bool GetTileCoordAt(float x, float z, int &outx, int &outz);
	D3DXVECTOR3 GetCenterOfTile(int x, int z, float defaultY = -10000.f);
	D3DXVECTOR3 GetCenterOfTile(int index, float defaultY = -10000.f);
	bool FillNode(STerrainPointNode &node, int targetX, int targetZ, float parentCost, float costAdd, int prevIndex);

	CTileManager *m_tileMgr;

	// Array containing all the tiles of the map in 
	// a centered storage
	vector<CTile*> m_tiles;

	vector<STerrainPointNode> m_terrainNodes;
	bool m_bNodeTableGenerated;

	int m_lastPathSearchIndex;

	float m_width, m_depth;

	// Size of the tile map (in tiles)
	POINT m_size;
	bool m_bMapLoaded;
};

#endif // #ifndef __CTILEDTERRAIN_H__
