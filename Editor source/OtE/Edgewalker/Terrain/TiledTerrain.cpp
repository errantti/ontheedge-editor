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

#include "TiledTerrain.h"
#include "..\\Scene\\CameraSceneNode.h"
#include <queue>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTiledTerrain::CTiledTerrain(ISceneNode *parent, ISceneManager *manager, DWORD id)
	: CTileQuadTreeNode(parent, manager), m_bMapLoaded(FALSE), m_width(TILE_DIMENSION),
	m_depth(TILE_DIMENSION), m_bNodeTableGenerated(FALSE), m_lastPathSearchIndex(0)
{
	m_ID = id;
	m_ID = 44;
	m_tileMgr = CTileManager::GetManager();
	m_bVisible = FALSE;
	m_size.x = m_size.y = 10;
}

CTiledTerrain::~CTiledTerrain()
{
	//Release();
}

void CTiledTerrain::Release()
{
	list<ISceneNode*>::iterator it = m_children.begin();
	for(; it != m_children.end(); it++)
	{
		if((*it) != NULL)
		{
			delete (*it);
			(*it) = NULL;
		}
	}
	m_children.clear();
	m_tiles.clear();
	m_terrainNodes.clear();
	m_bNodeTableGenerated = FALSE;
	m_bMapLoaded = FALSE;
}

/**
 * OnPreRender
 * Recurses through the quad-tree and registers the visible tiles
 * for rendering using frustum culling (aabb tests for speed's sake)
 **/
void CTiledTerrain::OnPreRender(bool isShadowPass)
{
	if(m_bMapLoaded && m_bVisible)
	{
		// Count how many tiles were rendered this frame
		CTile::TilesRendered = 0;
		CFrustum &f = m_manager->GetActiveCamera()->GetFrustum();
		CTileQuadTreeNode::OnPreRender(f, false, isShadowPass);
	}
}

void CTiledTerrain::Render(bool isShadowPass)
{
	// Should never happen
}

/**
 * Create
 * Creates a tile map of size x*y tiles (x and y are stored in parameter dimensions
 * /param tiles - an array of STile structures ordered row by row
 **/
bool CTiledTerrain::Create(STile *tiles, POINT dimensions)
{
	if(m_tileMgr->GetTileCount()<1)
	{
		CPrinter::log("Tried creating tile terrain while no sufficient tileset has been loaded.", LOG_ERROR);
		return FALSE;
	}

	if(tiles == NULL)
	{
		CPrinter::log("Failed creating tile terrain: null tile data pointer", LOG_ERROR);
		return FALSE;
	}

	if(dimensions.x < 1 || dimensions.y < 1)
	{
		CPrinter::log("Incorrect tile map dimensions", LOG_ERROR);
		return FALSE;
	}

	// Make sure the old tile map is removed
	Release();

	m_tiles.resize(dimensions.x * dimensions.y, NULL);
	m_size = dimensions;

	int x,y;
	CTile *tile;
	DWORD offset;
	POINT pos;

	// Write the tile data
	for(y=0;y<dimensions.y;y++)
	{
		offset = y*dimensions.x;
		pos.y = y;

		for(x=0;x<dimensions.x;x++)
		{
			pos.x = x;
			tile = new CTile(this, m_manager, pos, &tiles[offset+x]);
			m_tiles[offset+x] = tile;
		}
	}

	m_bMapLoaded = TRUE;
	m_bVisible = TRUE;

	m_width = dimensions.x * TILE_DIMENSION;
	m_depth = dimensions.y * TILE_DIMENSION;

	// Create the quad-tree structure
	rect<int> r(0, dimensions.x-1, 0, dimensions.y-1);
	CTileQuadTreeNode::Create(m_tiles, r, dimensions);

	// Update the bounding boxes
	CTileQuadTreeNode::UpdateBoundingBox();

	//if(!ConstructNodeTable(dimensions))
	//	CPrinter::logError("Failed constructing node table (pathfinding won't be functional)");
	ConstructNodeTable(dimensions);

	CPrinter::log("Tile terrain created succesfully");

	return TRUE;
}

STile *CTiledTerrain::GetTileInfoArray(POINT &dimensions)
{
	DWORD size = m_size.x * m_size.y;
	dimensions = m_size;

	STile *array = new STile[size];
	if(array == NULL)
		return NULL;

	for(DWORD i=0;i<m_tiles.size();i++)
		array[i] = m_tiles[i]->GetTileInfo();

	return array;
}

void CTiledTerrain::CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions)
{
	if(object == NULL)
		return;
	if(object->IsStatic())
		return;

	if(object->RequiresCollisionDetermination())
		CheckCollisionsSimple(object, collisions);
	else
		CheckIfCollides(object);
}

void CTiledTerrain::CheckCollisionsQuadtree(IGeometricObject *object, vector<SCollision> &collisions)
{
}

void CTiledTerrain::CheckIfCollides(IGeometricObject *object)
{
	bsphere3d& bs = object->GetTransformedSphere();
	if(bs.center.x<0.f || bs.center.z<0.f)
		return;
	int x = (int)(bs.center.x*ONE_PER_TILE_DIMENSION), z = (int)(bs.center.z*ONE_PER_TILE_DIMENSION);

	if(x >= m_size.x || z >= m_size.y)
		return;

	int i = z * m_size.x + x;

	CTile *t = m_tiles[i];

	aabbox3d obox(D3DXVECTOR3(bs.center.x-bs.radius,bs.center.y-bs.radius,bs.center.z-bs.radius), 
		D3DXVECTOR3(bs.center.x+bs.radius,bs.center.y+bs.radius,bs.center.z+bs.radius));

	//box = t->GetTransformedBox();
	if(t->CheckIfVerticalOnlyCollision(object, obox))
	{
		object->RegisterCollision(this);
	}

	/**
	 * On the Edge: game specific optimization: don't check tiles other than the one
	 * the object is currently on (accurate enough for small and fast entities like
	 * ammos
	 **/
}

/**
 * simple, fast and very inaccurate collision detection
 **/
void CTiledTerrain::CheckCollisionsSimple(IGeometricObject *object, vector<SCollision> &collisions)
{
	bsphere3d& bs = object->GetTransformedSphere();
	if(bs.center.x<0.f || bs.center.z<0.f)
		return;
	int x = (int)(bs.center.x*ONE_PER_TILE_DIMENSION), z = (int)(bs.center.z*ONE_PER_TILE_DIMENSION);

	if(x >= m_size.x || z >= m_size.y)
		return;

	int i = z * m_size.x + x;

	CTile *t = m_tiles[i];

	SCollision c;
	c.o1 = this;
	c.o2 = object;
	c.v = D3DXVECTOR3(0.f, 0.f, 0.f);
	aabbox3d box,obox(D3DXVECTOR3(bs.center.x-bs.radius,bs.center.y-bs.radius,bs.center.z-bs.radius), 
		D3DXVECTOR3(bs.center.x+bs.radius,bs.center.y+bs.radius,bs.center.z+bs.radius));

	D3DXVECTOR3 &collisionVector = c.v;

	bool bX = false, bY = false, bZ = false;

	box = t->GetTransformedBox();
	t->CheckForVerticalOnlyCollision(object, c.v, obox, bY);

	//D3DXVECTOR3 &speed = object->GetSpeed();

	/*switch(t->GetCollisionType())
	{
	case TILE_COLLISION_NONE:
		break;
	case TILE_COLLISION_ACCURATE:
	case TILE_COLLISION_BOUNDING_BOX:
		//float h = t->GetTileHeight();
		if(box.IntersectsWithBox(obox))
		{
			if(speed.y>0.f)
			{
				if(bs.center.y+bs.radius > box.minCorner.y)
				{
					// register collision
					c.v = D3DXVECTOR3(0.f, bs.center.y+bs.radius-box.maxCorner.y, 0.f);
					bVerticalCollisionDetected = TRUE;
					collisions.push_back(c);
				}
			}
			else
			{
				if(bs.center.y-bs.radius < box.maxCorner.y)
				{
					// register collision
					c.v = D3DXVECTOR3(0.f, box.maxCorner.y-bs.center.y+bs.radius, 0.f);
					bVerticalCollisionDetected = TRUE;
					collisions.push_back(c);
				}
			}
		}
		break;
	case TILE_COLLISION_RAMP:
		if(box.IntersectsWithBox(obox))
		{
			t->CheckRampCollisions(object, collisions, obox, 1, TRUE);
		}
		break;
	}*/

	int hlvl = t->GetHeightLevel();
	int h[] = {hlvl, hlvl, hlvl, hlvl};

	if(t->GetCollisionType() == TILE_COLLISION_RAMP)
	{
		int rot = t->GetTileRotation();
		switch(rot)
		{
		case 0:
		case 2:
			h[1] = h[3] = hlvl-1; break;
		case 1:
		case 3:
			h[0] = h[2] = hlvl-1; break;
		}
	}

	bool skipCorners = false;

	// Check first the major directions
	if(x>0)
		m_tiles[i-1]->CheckForCollisionSimple(object, c.v, obox, h[2], bX, bY, bZ, 2);
	if(x<m_size.x-1)
		m_tiles[i+1]->CheckForCollisionSimple(object, c.v, obox, h[0], bX, bY, bZ, 0);
	if(z>0)
		m_tiles[i-m_size.x]->CheckForCollisionSimple(object, c.v, obox, h[1], bX, bY, bZ, 1);
	if(z<m_size.y-1)
		m_tiles[i+m_size.x]->CheckForCollisionSimple(object, c.v, obox, h[3], bX, bY, bZ, 3);

	/**
	 * When an object is sliding against a wall it always collides
	 * to the tile in the major direction. Speeding towards the wall
	 * can then cause the corner tile to individually report a
	 * collision in the other x/z axis and thus cause the object
	 * to crash into thin air. This optimization checks whether
	 * x/z collision has been detected in the any of the major
	 * neighbor tiles and if yes making the object's box just a little
	 * smaller to avoid these thin air crashes. This small a change
	 * shouldn't cause any noticeable error in the collision
	 * detection but it removes majority of thin air crashes
	 **/
	if(bX || bZ)
	{
		/*obox.minCorner += D3DXVECTOR3(1.1f, 1.1f, 1.1f);
		obox.maxCorner -= D3DXVECTOR3(1.1f, 1.1f, 1.1f);*/
		bX = bZ = true;
		if(bY)
			skipCorners = true;
	}

	if(!skipCorners)
	{
		// And then the minor
		if(z>0)
		{
			if(x>0)
				m_tiles[i-m_size.x-1]->CheckForCollisionSimple(object, c.v, obox, hlvl, bX, bY, bZ, -1);
			if(x<m_size.x-1)
				m_tiles[i-m_size.x+1]->CheckForCollisionSimple(object, c.v, obox, hlvl, bX, bY, bZ, -1);
		}
		if(z<m_size.y-1)
		{
			if(x>0)
				m_tiles[i+m_size.x-1]->CheckForCollisionSimple(object, c.v, obox, hlvl, bX, bY, bZ, -1);
			if(x<m_size.x-1)
				m_tiles[i+m_size.x+1]->CheckForCollisionSimple(object, c.v, obox, hlvl, bX, bY, bZ, -1);
		}
	}

	if(bX || bY || bZ)
	{
		collisions.push_back(c);
		object->RegisterCollision(this);
	}
}

CTile *CTiledTerrain::GetTile(unsigned int x, unsigned int y)
{
	return m_tiles[y*m_size.x+x];
}

CTile *CTiledTerrain::GetTileByIndex(int x, int y)
{
	if(x<0 || y<0 || x>= m_size.x || y>= m_size.y)
		return NULL;
	return m_tiles[y*m_size.x+x];
}

CTile *CTiledTerrain::GetTile(float x, float z)
{
	if(x<0.f || z<0.f || x>m_width || z>m_depth)
		return NULL;

	return m_tiles[(unsigned int)(z*ONE_PER_TILE_DIMENSION) * m_size.x + (unsigned int)(x*ONE_PER_TILE_DIMENSION)];
}

float CTiledTerrain::GetTerrainHeight(float x, float z)
{
	if(x<0.f || z<0.f || x>m_width || z>m_depth)
		return -200.f;

	static unsigned int oz,ox;
	oz = (unsigned int)(z*ONE_PER_TILE_DIMENSION);
	ox = (unsigned int)(x*ONE_PER_TILE_DIMENSION);
	CTile *t = m_tiles[oz * m_size.x + ox];

	if(t->GetCollisionType() == TILE_COLLISION_RAMP)
		return t->GetTileHeight(x-(float)ox*TILE_DIMENSION, z-(float)oz*TILE_DIMENSION);
	else
		return t->GetTileHeight();
}

void CTiledTerrain::TrimTiles(CTile *center, bool randomize)
{
	D3DXVECTOR3 pos = center->GetTranslation();

	const float f1 = TILE_DIMENSION * (-0.5f), f2 = TILE_DIMENSION * 0.5f, f3 = TILE_DIMENSION * 1.5f;
	CTile *tiles[9] = {
		GetTile(pos.x+f1, pos.z+f3), GetTile(pos.x+f2, pos.z+f3), GetTile(pos.x+f3, pos.z+f3), 
		GetTile(pos.x+f1, pos.z+f2), center,					  GetTile(pos.x+f3, pos.z+f2), 
		GetTile(pos.x+f1, pos.z+f1), GetTile(pos.x+f2, pos.z+f1), GetTile(pos.x+f3, pos.z+f1)
	};

	for(int i=0; i<9; ++i)
		if(tiles[i] != NULL)
			TrimTile(tiles[i], randomize);
}

void CTiledTerrain::TrimTile(CTile *tile, bool randomize)
{
	// Skip empty tiles, that would mess up the layout of the level
	if(tile->GetTileType() == TILE_TYPE_EMPTY)
		return;

	D3DXVECTOR3 pos = tile->GetTranslation();

	const float f1 = TILE_DIMENSION * (-0.5f), f2 = TILE_DIMENSION * 0.5f, f3 = TILE_DIMENSION * 1.5f;
	CTile *tiles[9] = {
		GetTile(pos.x+f1, pos.z+f3), GetTile(pos.x+f2, pos.z+f3), GetTile(pos.x+f3, pos.z+f3), 
		GetTile(pos.x+f1, pos.z+f2), tile,						  GetTile(pos.x+f3, pos.z+f2), 
		GetTile(pos.x+f1, pos.z+f1), GetTile(pos.x+f2, pos.z+f1), GetTile(pos.x+f3, pos.z+f1)
	};

	/*
	 This describes a boolean 3x3 matrix that described the height differences
	 of the tiles around the tile that we're trimming. If a cell is true then
	 it means that the tile in the corresponding position relative to the tile
	 is higher or at equal height whereas false means that tile is lower than
	 the tile in question.
	   ^ 0 1 2
	 Z | 3 4 5
	   | 6 7 8
	   ------>
	       X
     Thus index number four in the center represents the tile in question
	 */
	  
	bool matrix[9] = {true, true, true, true, true, true, true, true, true};

	int i, maxHeight = 0;
	int heightLevel = tile->GetHeightLevel();
	int temp;
	int yRot = 0;
	TILE_TYPE type = TILE_TYPE_PLATFORM;

	for(i=0; i<9; ++i)
	{
		if(tiles[i] != NULL)
		{
			if((temp = tiles[i]->GetHeightLevel()) < heightLevel)
			{
				matrix[i] = false;
				if(heightLevel - temp > maxHeight && temp >= 0)
					maxHeight = heightLevel - temp;
			}
			else
				matrix[i] = true;
		}
		else
			matrix[i] = false;
	}

	if(maxHeight > 1)
		maxHeight = 2;

	/*
	 If all the four main neighbours are equal or higher height then
	 this tile is an inner corner(s). Otherwise it is an outer corner,
	 wall, platform, catwalk, catwalk end or block
	 */
	if(matrix[1] == true && matrix[3] == true && matrix[5] == true && matrix[7] == true)
	{
		/*
		 Calculate the correct kind of inner corner
		 */
		bool cormat[4] = {matrix[0], matrix[2], matrix[6], matrix[8]};

		int sum = 0;
		for(i=0;i<4;++i)
			if(cormat[i])
				++sum;

		if(sum == 3)
		{
			// 1 corner
			type = TILE_TYPE_PLATFORM_CORNER;
			if(!cormat[0]) yRot = 0;
			if(!cormat[1]) yRot = 1;
			if(!cormat[2]) yRot = 3;
			if(!cormat[3]) yRot = 2;
		}
		else if(sum == 2)
		{
			// 2 corner, cross or normal
			if(cormat[0] == cormat[3])
			{
				// Cross
				type = TILE_TYPE_PLATFORM_DOUBLECORNERCROSS;

				if(cormat[0])
					yRot = 1;
				else
					yRot = 0;
			}
			else
			{
				// normal
				type = TILE_TYPE_PLATFORM_DOUBLECORNER;
				if(cormat[0])
				{
					if(cormat[1])
						yRot = 0;
					else
						yRot = 3;
				}
				else
				{
					if(cormat[1])
						yRot = 1;
					else
						yRot = 2;
				}
			}
		}
		else if(sum == 1)
		{
			type = TILE_TYPE_PLATFORM_TRIPLECORNER;
			if(cormat[0]) yRot = 3;
			if(cormat[1]) yRot = 0;
			if(cormat[2]) yRot = 2;
			if(cormat[3]) yRot = 1;
		}
		else if(sum == 0)
		{
			type = TILE_TYPE_PLATFORM_FOURCORNER;
		}
		else
		{
			type = TILE_TYPE_PLATFORM;
		}
	}
	else
	{
		/* Find out the tile type
		 The only tiles that affect this are the close neighbors that share
		 edges like follows
		 . 1 .       0
		 3 . 5  => 1   2
		 . 7 .       3
		 Here we make another matrix that includes only the relevant values
		 to simplify the process of selecting the correct tile type and
		 rotation
		 */
		bool edgemat[4] = {matrix[1], matrix[3], matrix[5], matrix[7]};

		int sum = 0;
		for(i=0;i<4;++i)
			if(edgemat[i])
				++sum;
		sum=sum;

		if(sum == 0)
		{
			// Tile type block, correct height required
			type = TILE_TYPE_WALL_BLOCK;
		}
		else if(sum == 1)
		{
			// Tile type catwalk end, correct height and rotation required
			type = TILE_TYPE_CATWALK_END;
			if(edgemat[0]) yRot = 2;
			if(edgemat[1]) yRot = 1;
			if(edgemat[2]) yRot = 3;
			if(edgemat[3]) yRot = 0;
		}
		else if(sum == 2)
		{
			// Tile type catwalk or wall corner, correct height and rotation required
			if(edgemat[0] == edgemat[3])
			{
				// Catwalk
				type = TILE_TYPE_CATWALK;
				if(edgemat[0])
					yRot = 0;
				else
					yRot = 1;
			}
			else
			{
				// Corner
				type = TILE_TYPE_WALL_CORNER;
				if(edgemat[1])
				{
					if(edgemat[0])
						yRot = 2;
					else
						yRot = 1;
				}
				else if(edgemat[2])
				{
					if(edgemat[0])
						yRot = 3;
					else
						yRot = 0;
				}
			}
		}
		else if(sum == 3)
		{
			// Tile type wall, corret height and rotation required
			type = TILE_TYPE_WALL;
			if(!edgemat[0]) yRot = 1;
			if(!edgemat[1]) yRot = 0;
			if(!edgemat[2]) yRot = 2;
			if(!edgemat[3]) yRot = 3;
		}
		else
		{
			// Tile type platform, nothing required
			type = TILE_TYPE_PLATFORM;
		}
	}

	// Set new tile settings
	STileInfo *info = CTileManager::GetManager()->GetTileTemplate(type, (UINT)maxHeight, randomize);

	if(info == NULL)
		return;
	tile->SetTileInfo(*info);
	tile->SetTileRotation(yRot);
}

CTile *CTiledTerrain::GetTileByRay(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir)
{
	return GetTileFromRay(pos, dir);
}

bool CTiledTerrain::GetRayIntersectLocation(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir, D3DXVECTOR3 &outIntersection)
{
	CTile *tile = GetTileFromRay(pos, dir);

	if(tile == NULL)
		return false;

	// Find the intersection point
	D3DXVECTOR3 p = pos;
	float y = tile->GetTranslation().y;
	float c = (y-p.y) / dir.y;
	p.x += dir.x*c;
	p.z += dir.z*c;
	p.y = y;

	// Get the tile at that position
	/*tile = GetTile(p.x, p.z);
	if(tile == NULL)
		return false;

	p.y = (float)tile->GetHeightLevel() * TILE_LEVEL_HEIGHT;*/
	outIntersection = p;

	return true;
}

void CTiledTerrain::ConstructNodeTable(POINT dimensions)
{
	m_bNodeTableGenerated = FALSE;
	m_terrainNodes.resize(dimensions.x * dimensions.y);

	int x,y;
	int offset, height;
	CTile *tile;
	bool nodeEndReached = false;

	vector<STerrainPointNode>::iterator nIt = m_terrainNodes.begin();
	for(y=0;y<dimensions.y;++y)
	{
		offset = y*dimensions.x;

		for(x=0;x<dimensions.x;++x)
		{
			if(nodeEndReached)
			{
				CPrinter::logError("ConstructNodeTable failed: tile and node tables have different dimensions");
				return;
			}

			tile = m_tiles[offset+x];
			nIt->FillDefaults();

			if(CheckAccessFromTile(x, y, x+1, y))
			{
				nIt->east = offset+x+1;
			}
			if(CheckAccessFromTile(x, y, x-1, y))
			{
				nIt->west = offset+x-1;
			}
			if(CheckAccessFromTile(x, y, x, y+1))
			{
				nIt->north = (y+1)*dimensions.x + x;
			}
			if(CheckAccessFromTile(x, y, x, y-1))
			{
				nIt->south = (y-1)*dimensions.x + x;
			}

			height = tile->GetHeightLevel();

			if(nIt->north >= 0 && nIt->east >= 0 && !IsTileHigherThanOrRamp(height, x+1, y+1))
			{
				if(m_tiles[nIt->north]->GetHeightLevel() == height &&
					m_tiles[nIt->east]->GetHeightLevel() == height)
					nIt->northeast = (y+1)*dimensions.x + x+1;
			}
			if(nIt->north >= 0 && nIt->west >= 0 && !IsTileHigherThanOrRamp(height, x-1, y+1))
			{
				if(m_tiles[nIt->north]->GetHeightLevel() == height &&
					m_tiles[nIt->west]->GetHeightLevel() == height)
					nIt->northwest = (y+1)*dimensions.x + x-1;
			}
			if(nIt->south >= 0 && nIt->east >= 0 && !IsTileHigherThanOrRamp(height, x+1, y-1))
			{
				if(m_tiles[nIt->south]->GetHeightLevel() == height &&
					m_tiles[nIt->east]->GetHeightLevel() == height)
					nIt->southeast = (y-1)*dimensions.x + x+1;
			}
			if(nIt->south >= 0 && nIt->west >= 0 && !IsTileHigherThanOrRamp(height, x-1, y-1))
			{
				if(m_tiles[nIt->south]->GetHeightLevel() == height &&
					m_tiles[nIt->west]->GetHeightLevel() == height)
					nIt->southwest = (y-1)*dimensions.x + x-1;
			}

			nIt->height = height;
			nIt->x = x;
			nIt->z = y;

			if(tile->GetTileType() == TILE_TYPE_RAMP)
				nIt->bRamp = true;

			++nIt;
			if(nIt == m_terrainNodes.end())
			{
				nodeEndReached = true;
			}
		}
	}

	m_bNodeTableGenerated = TRUE;
}

bool CTiledTerrain::CheckAccessFromTile(int x1, int y1, int x2, int y2)
{
	bool xAxis = true;
	if(x1 != x2)
	{
		if(y1 != y2)
			return false;
		if(abs(x1-x2) > 1)
			return false;
	}
	else
	{
		if(y1 == y2)
			return true;
		if(abs(y1-y2) > 1)
			return false;
		xAxis = false;
	}

	CTile *tile1 = GetTileByIndex(x1, y1);
	if(tile1 == NULL)
		return false;
	CTile *tile2 = GetTileByIndex(x2, y2);
	if(tile2 == NULL)
		return false;

	TILE_TYPE type1 = tile1->GetTileType();
	TILE_TYPE type2 = tile2->GetTileType();
	if(tile1->GetCollisionType() == TILE_COLLISION_NONE || tile2->GetCollisionType() == TILE_COLLISION_NONE)
		return false;

	if(type1 == TILE_TYPE_RAMP)
	{
		// Object on ramp can't go to tiles on ramp's either side
		int rot = tile1->GetTileRotation();
		if(rot % 2 == (int)xAxis)
			return false;
		// TODO: additional direction check
		return true;
	}
	else
	{
		int heightDifference = tile1->GetHeightLevel() - tile2->GetHeightLevel();
		if(heightDifference >= 0)
			return true;
		else if(heightDifference == -1)
		{
			if(type2 == TILE_TYPE_RAMP)
			{
				// TODO: additional direction check
				if(tile2->GetTileRotation() % 2 != (int)xAxis)
					return true;
			}
		}
	}

	return false;
}

bool CTiledTerrain::IsTileHigherThanOrRamp(int height, int x, int y)
{
	CTile *tile = GetTileByIndex(x, y);
	if(tile == NULL)
		return true;
	int tHeight = tile->GetHeightLevel();
	//if(tHeight > height || tHeight == -1)
	if(tHeight != height || tHeight == -1)
		return true;
	else if(tile->GetTileType() == TILE_TYPE_RAMP)
		return true;
	return false;
}

bool CTiledTerrain::GetTileCoordAt(float x, float z, int &outx, int &outz)
{
	if(x<0.f || z<0.f || x>m_width || z>m_depth)
		return false;

	outx = (int)(x*ONE_PER_TILE_DIMENSION);
	outz = (int)(z*ONE_PER_TILE_DIMENSION);
	return true;
}

bool CTiledTerrain::FindPath(D3DXVECTOR3 start, D3DXVECTOR3 target, bool acceptIncomplete, vector<D3DXVECTOR3> &outPath, int iterationLimit)
{
	// TODO: improvement: what if target or start is on an empty tile?

	if(!m_bNodeTableGenerated)
	{
		// If node table hasn't been preconstructed there's nothing to do
		outPath.clear();
		return false;
	}

	int x0,y0,x1,y1;
	if(!GetTileCoordAt(start.x, start.z, x0, y0))
	{
		outPath.clear();
		return false;
	}
	if(!GetTileCoordAt(target.x, target.z, x1, y1))
	{
		outPath.clear();
		return false;
	}

	if(x1 == x0 && y1 == y0)
	{
		outPath.clear();
		return true;
	}

	++m_lastPathSearchIndex;
	if(m_lastPathSearchIndex > 1000000)
		m_lastPathSearchIndex = 0;

	int startIndex = y0 * m_size.x + x0;
	int targetIndex = y1 * m_size.x + x1;
	int currentIndex;
	int i;
	bool pathFound = false;

	FillNode(m_terrainNodes[startIndex], x1, y1, 0, 0, startIndex);

	float shortestEstimation = m_terrainNodes[startIndex].costEstimate;
	int closestTileIndex = startIndex;

	std::priority_queue<SNodeIndex> openList;
	openList.push(SNodeIndex(startIndex, m_terrainNodes[startIndex].costEstimate));

	int iterations = 0;
	while(!openList.empty() && iterations < iterationLimit)
	{
		currentIndex = openList.top().index;
		if(currentIndex == targetIndex)
		{
			pathFound = true;
			break;
		}

		openList.pop();
		STerrainPointNode &node = m_terrainNodes[currentIndex];

		for(i=0;i<8;++i)
		{
			int neighborIndex = node.neighbors[i];
			if(neighborIndex == -1)
				continue;

			STerrainPointNode &next = m_terrainNodes[neighborIndex];
			if(FillNode(next, x1, y1, node.pathCost, (i<4)?1.f:1.4f, currentIndex))
			{
				openList.push(SNodeIndex(neighborIndex, next.pathCost + next.costEstimate));
				if(next.costEstimate < shortestEstimation)
				{
					shortestEstimation = next.costEstimate;
					closestTileIndex = neighborIndex;
				}
			}
		}
		++iterations;
	}

	if(!pathFound && acceptIncomplete)
		targetIndex = closestTileIndex;

	if(pathFound || acceptIncomplete)
	{
		// Optimize and store the path that was found
		std::vector<int> backwardsPath;

		STerrainPointNode *lastNode = &m_terrainNodes[targetIndex];

		int lastX = lastNode->x, lastZ = lastNode->z;
		int dx = 0, dy = 0;
		int lastIndex = targetIndex, currentIndex;

		int i;
		for(i=0;i<iterationLimit;++i)	// Just a measure to avoid infinite loops in case of bugs in path creation
		{
			currentIndex = lastNode->prevIndex;
			STerrainPointNode &node = m_terrainNodes[currentIndex];
			if(lastNode == &node)
			{
				// Don't add the node that the object is already in
				//backwardsPath.push_back(node.prevIndex);
				break;
			}

			if(dx != lastX - node.x || dy != lastZ - node.z || lastNode->bRamp)
			{
				backwardsPath.push_back(lastIndex);
			}/**/

			dx = lastX - node.x;
			dy = lastZ - node.z;
			lastX = node.x;
			lastZ = node.z;

			lastIndex = currentIndex;
			lastNode = &node;
		}

		outPath.clear();
		int size = (int)backwardsPath.size();
		//if(size > 4)
		//	outPath.resize(size);

		//for(i=size-1; i>=0; --i)
		// Store the final coordinates in backwards order
		// TODO: bool param to select which way to store these
		i=0;
		if(pathFound)
		{
			// If path was found then the first (backwards order so that means target) location is the target
			i=1;
			D3DXVECTOR3 pos = GetCenterOfTile(backwardsPath[0]);
			pos.x = target.x;
			pos.z = target.z;
			outPath.push_back(pos);
		}
		for(; i<size; ++i)
			outPath.push_back(GetCenterOfTile(backwardsPath[i]));

		if(pathFound)
			return true;
	}

	return false;
}

bool CTiledTerrain::FillNode(STerrainPointNode &node, int targetX, int targetZ, float parentCost, float costAdd, int prevIndex)
{
	if(node.pathFindIndex == m_lastPathSearchIndex)
		return false;

	node.pathFindIndex = m_lastPathSearchIndex;

	float tempx = (float)(targetX-node.x);
	float tempz = (float)(targetZ-node.z);
	node.costEstimate = sqrt(tempx*tempx+tempz*tempz);

	node.pathCost = parentCost + costAdd;
	node.prevIndex = prevIndex;

	return true;
}

D3DXVECTOR3 CTiledTerrain::GetCenterOfTile(int x, int z, float defaultY)
{
	if(x<0 || z<0 || x>=m_size.x || z>=m_size.y)
		return D3DXVECTOR3(-50.f, defaultY, -50.f);

	D3DXVECTOR3 pos = m_tiles[z*m_size.x + x]->GetTranslation();
	pos.x += 25.f;
	pos.z += 25.f;
	if(defaultY != -10000.f)
		pos.y = defaultY;
	return pos;
}

D3DXVECTOR3 CTiledTerrain::GetCenterOfTile(int index, float defaultY)
{
	if(index < 0 || index>= (int)m_terrainNodes.size())
		return D3DXVECTOR3(-50.f, defaultY, -50.f);

	D3DXVECTOR3 pos = m_tiles[index]->GetTranslation();
	pos.x += 25.f;
	pos.z += 25.f;
	pos.y += 10.f;
	if(defaultY != -10000.f)
		pos.y = defaultY;
	return pos;
}

bool CTiledTerrain::IsWayClear(D3DXVECTOR3 start, D3DXVECTOR3 target, bool &outIsPit, D3DXVECTOR3 &outLastGoodTile)
{
	if(!m_bNodeTableGenerated)
	{
		// If node table hasn't been preconstructed there's nothing to do
		outLastGoodTile = start;
		return false;
	}

	// Tile coordinates of start and target
	int x0, y0, x1, y1;
	if(!GetTileCoordAt(start.x, start.z, x0, y0))
	{
		outIsPit = false;
		return false;
	}
	if(!GetTileCoordAt(target.x, target.z, x1, y1))
	{
		outIsPit = false;
		return false;
	}

	if(x1 == x0 && y1 == y0)
		return true;

	// 
	int lastGoodX=-1, lastGoodY=-1;
	int currentIndex;
	int currentHeight = -10000, lastHeight = 20;
	bool first = true;
	int actualX, actualY;
	int tilesFromDrop = 0;
	int lastX, lastY;

	// Loop through the tiles between the two points using a modified Bresenham's line algorithm
	// The modification is to make sure that the 'line' is handled from start to finish, not the
	// other way around. This is imperative for the rest of the algorithm to function
	bool steep = abs(y1-y0) > abs(x1 - x0);
	if(steep)
	{
		// Swap (x0,y0) (x1,y1), this doesn't affect the 
		int temp = y0;
		y0 = x0;
		x0 = temp;
		temp = y1;
		y1 = x1;
		x1 = temp;
	}
	// Bresenham's algorithm specific variables
	int deltax = abs(x1-x0);
	int deltay = abs(y1-y0);
	int error = -(deltax >> 1);
	int ystep;
	int y = y0;
	if(y0<y1)
		ystep = 1;
	else
		ystep = -1;

	// 
	int x = x0;
	while(true)
	{
		if(steep)
		{
			currentIndex = m_size.x * x + y;
			actualX = y;
			actualY = x;
		}
		else
		{
			currentIndex = m_size.x * y + x;
			actualX = x;
			actualY = y;
		}

		//// Actual 'is way clear' part of the algorithm
		// TODO: improve the check, for example to take into account that most objects could possible fly over
		// short pits it the other side is considerably lower than the current one
		// Also, take into account the node table. And possibly the object's current y-position as given
		// in the parameter 'start'
		STerrainPointNode &current = m_terrainNodes[currentIndex];
		if(first)
		{
			currentHeight = current.height;
			lastHeight = currentHeight;
			first = false;
			lastGoodX = actualX;
			lastGoodY = actualY;
		}
		else
		{
			currentHeight = current.height;
			if(currentHeight < 0)
			{
				// Encountered 'bottomless' pit
				outIsPit = true;
				outLastGoodTile = GetCenterOfTile(lastGoodX, lastGoodY);
				return false;
			}

			if(currentHeight > lastHeight)
			{
				if(tilesFromDrop == 0)
				{
					// Encountered elevation
					outIsPit = false;
					outLastGoodTile = GetCenterOfTile(lastX, lastY);
					return false;
				}
				else if(tilesFromDrop > 2)
				{
					// Encountered elevation after a fall and flat of at least three tiles (which is no longer considered pit)
					outIsPit = false;
					outLastGoodTile = GetCenterOfTile(lastX, lastY);
					return false;
				}
				else
				{
					// Encountered a pit
					outIsPit = true;
					outLastGoodTile = GetCenterOfTile(lastGoodX, lastGoodY);
					return false;
				}
			}
			else if(currentHeight < lastHeight)
			{
				if(tilesFromDrop == 0)
				{
					lastGoodX = lastX;
					lastGoodY = lastY;
				}
				tilesFromDrop = 1;
			}
			else
			{
				if(tilesFromDrop != 0)
				{
					++tilesFromDrop;
					if(tilesFromDrop > 2)
					{
						lastGoodX = actualX;
						lastGoodY = actualY;
					}
				}
			}
		}

		lastX = actualX;
		lastY = actualY;
		lastHeight = currentHeight;

		//// Bresenham continues
		error += deltay;
		if(error > 0)
		{
			y += ystep;
			error += -deltax;
		}

		if(x1<x0)
		{
			--x;
			if(x < x1)
				break;
		}
		else
		{
			++x;
			if(x > x1)
				break;
		}
	}

	// If no pits or elevations were encountered, return success, the way is clear
	return true;
}