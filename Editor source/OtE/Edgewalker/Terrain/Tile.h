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

#ifndef __CTILE_H__
#define __CTILE_H__

#include "..\\IGeometricObject.h"
#include "TileManager.h"

#define GENERIC_TILE_ID	-356
#define MAX_HEIGHT_LEVEL	7

// RAMP_VERT_DIST_COEFFICIENT = TILE_DIMENSION / sqrt(TILE_DIMENSION^2 + TILE_HEIGHT^2)
//#define RAMP_VERT_DIST_COEFFICIENT	(50.f/55.901699f)
#define RAMP_VERT_DIST_COEFFICIENT		0.894427f
// using this coefficient can be calculated a point's distance
// from ramp when point's y-distance from the ramp is known
// Thus a = y-distance, x = distance, x = RAMP_VERT_DIST_COEFFICIENT * a

#define DEFAULT_RAMP_COLLISION_UNIT_VECTOR	D3DXVECTOR3(-0.44721f, 0.89443f, 0.f)

struct STile
{
	// Y-rotation as a coefficient of 90 degrees (possible values 0-3)
	unsigned int yRotation;
	// Height level. Value between 0-MAX_HEIGHT_LEVEL
	unsigned int heightLevel;
	// ID of the tile template used for this tile
	DWORD tileID;
};

class CTile : public IGeometricObject
{
public:
	CTile(ISceneNode *parent, ISceneManager *manager, POINT position, STile *info);
	virtual ~CTile();

	virtual void Render(bool isShadowPass);
	virtual void RenderSubset(bool isShadowPass);
	virtual void SetSubset();
	virtual int GetSubsetCount();

	virtual aabbox3d &GetBoundingBox();
	virtual aabbox3d GetTransformedBox();
	virtual void Update(float frameTime) {}

	void OnPreRender(bool isShadowPass);

	virtual float GetTileHeight(float x=0.f, float z=0.f);
	inline TILE_COLLISION_TYPE &GetCollisionType() {return m_collisionType;}
	inline int GetHeightLevel() {return m_heightLevel;}
	inline DWORD &GetTileID() {return m_tileID;}
	inline int &GetTileRotation() {return m_yRotation;}

	void SetTileRotation(int yRotation);
	void RotateClockwise();
	void Elevate();
	void Lower();
	void SetTileInfo(DWORD tileID);
	void SetTileInfo(STileInfo &info);
	STile GetTileInfo();

	TILE_TYPE GetTileType() {return m_tileType;}

	virtual aabbox3d UpdateBoundingBox();

	virtual D3DXMATRIX GetRelativeTransformation();
	virtual D3DXVECTOR3 GetAbsolutePosition();

	virtual void CheckForHorizontalCollisions(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &box, int vlvl);
	/**
	 * This method should only called when it is known that the object
	 * to be tested does intersect with the tile in x/z-plane. If collision
	 * in y-direction does happen the collision vector's y-component
	 * is overriden so this test should only be called once per collision vector
	 * and always before the other tests
	 */
	virtual bool CheckForVerticalOnlyCollision(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &box, bool &bY);
	virtual bool CheckIfVerticalOnlyCollision(IGeometricObject *object, aabbox3d &box);

	virtual void CheckForCollisionSimple(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &box, int vlvl, bool &bX, bool &bY, bool &bZ, int relPos);

	virtual void CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions);
	virtual void UpdateBoundingVolumes() {}
	virtual void RegisterCollision(IGeometricObject *object) {}

	virtual bool CheckTileIntersection(aabbox3d &box);

	//virtual void CheckRampCollisions(IGeometricObject *object, vector<SCollision> &collisions, aabbox3d &box, int vlvl, bool bCenter = FALSE);
	static int TilesRendered;

protected:
	bool CheckCenterRampCollisions(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &box, bool &bY);
	bool CheckIfCenterRampCollision(IGeometricObject *object);

	CMesh *m_mesh;
	CTexture *m_texture;
	aabbox3d m_bbox;

	// Tile information
	TILE_COLLISION_TYPE m_collisionType;
	TILE_TYPE m_tileType;
	int m_yRotation;
	int m_heightLevel;
	DWORD m_tileID;
	float m_tileHeight;
};

#endif // #ifndef __CTILE_H__
