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

#include "Tile.h"
#include "..\\SimpleStaticProfiler.h"

int CTile::TilesRendered = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTile::CTile(ISceneNode *parent, ISceneManager *manager, POINT position, STile *info)
:	IGeometricObject(NULL, manager, GENERIC_TILE_ID), m_tileID(info->tileID), m_collisionType(TILE_COLLISION_BOUNDING_BOX),
	m_heightLevel(info->heightLevel), m_mesh(NULL), m_texture(NULL)
{
	m_parent = parent;
	m_bStatic = TRUE;

	m_yRotation = info->yRotation;

	m_relativeTranslation = D3DXVECTOR3(position.x * TILE_DIMENSION, 
		m_heightLevel * TILE_LEVEL_HEIGHT, position.y * TILE_DIMENSION);

	m_relativeRotation = D3DXVECTOR3(0.f, (D3DX_PI*0.5f)*(float)m_yRotation, 0.f);

	m_bAutomaticCulling = FALSE;

	// Get the tile template
	STileInfo *tileTemplate = CTileManager::GetManager()->GetTileTemplate(m_tileID);
	if(tileTemplate == NULL)
	{
		// Shouldn't happen unless the database hasn't been loaded or the data is corrupted
		CPrinter::logs("Couldn't find tile template id " + m_tileID, LOG_ERROR);
		return;
	}

	m_collisionType = tileTemplate->collisionType;
	m_tileType = tileTemplate->tileType;

	m_tileHeight = tileTemplate->fHeight;
	if(tileTemplate->mesh != NULL)
	{
		//m_bbox = tileTemplate->mesh->GetBoundingBox();
		m_bbox.maxCorner = D3DXVECTOR3(50.f, 0.f, 50.f);
		m_bbox.minCorner = D3DXVECTOR3(0.f, -tileTemplate->fHeight, 0.f);

		// Should never happen but just to make sure..
		if(tileTemplate->tileType == TILE_TYPE_DUMMY)
			m_mesh = NULL;
		else
			m_mesh = tileTemplate->mesh;

		if(CUT_TILE_AABBOX_TOP)
			m_bbox.maxCorner.y = 0.f;
	}
	else
	{
		if(tileTemplate->tileType == TILE_TYPE_DUMMY)
		{
			m_bbox.maxCorner = D3DXVECTOR3(50.f, 0.f, 50.f);
			m_bbox.minCorner = D3DXVECTOR3(0.f, -tileTemplate->fHeight, 0.f);
		}
		else
			m_bbox = aabbox3d(0.f);
	}

	m_texture = tileTemplate->texture;

	if(m_collisionType == TILE_COLLISION_NONE)
		m_heightLevel = -1;
	//if(tileTemplate->tileType == TILE_TYPE_RAMP)
	//	--m_heightLevel;
}

CTile::~CTile()
{
}

STile CTile::GetTileInfo()
{
	STile t;
	t.tileID = m_tileID;
	t.heightLevel = m_heightLevel;
	t.yRotation = m_yRotation;
	return t;
}

void CTile::SetTileInfo(DWORD tileID)
{
	// Get the tile template
	STileInfo *tileTemplate = CTileManager::GetManager()->GetTileTemplate(tileID);
	if(tileTemplate == NULL)
	{
		// Shouldn't happen unless the database hasn't been loaded or the data is corrupted
		CPrinter::logs("Couldn't find tile template id " + m_tileID, LOG_ERROR);
		return;
	}

	SetTileInfo(*tileTemplate);
}

void CTile::SetTileInfo(STileInfo &info)
{
	m_tileID = info.id;

	m_collisionType = info.collisionType;
	m_tileType = info.tileType;
	m_tileHeight = info.fHeight;
	if(info.mesh != NULL)
	{
		//m_bbox = info.mesh->GetBoundingBox();
		//m_mesh = info.mesh;

		m_bbox.maxCorner = D3DXVECTOR3(50.f, 0.f, 50.f);
		m_bbox.minCorner = D3DXVECTOR3(0.f, -info.fHeight, 0.f);

		// Should never happen but just to make sure..
		if(info.tileType == TILE_TYPE_DUMMY)
			m_mesh = NULL;
		else
			m_mesh = info.mesh;

		if(CUT_TILE_AABBOX_TOP)
			m_bbox.maxCorner.y = 0.f;
	}
	else
	{
		if(info.tileType == TILE_TYPE_DUMMY)
		{
			m_bbox.maxCorner = D3DXVECTOR3(50.f, 0.f, 50.f);
			m_bbox.minCorner = D3DXVECTOR3(0.f, -info.fHeight, 0.f);
		}
		m_mesh = NULL;
	}

	m_texture = info.texture;

	if(m_collisionType == TILE_COLLISION_NONE)
		m_heightLevel = -1;
	else if(m_heightLevel<0)
	{
		m_relativeTranslation.y = 0.f;
		m_heightLevel = 0;
	}

	UpdateBoundingBox();
}

void CTile::RotateClockwise()
{
	 ++m_yRotation;
	 if(m_yRotation > 3) 
		 m_yRotation = 0;

	 m_relativeRotation.y = (D3DX_PI*0.5f)*(float)m_yRotation;
}

void CTile::SetTileRotation(int yRotation)
{
	if(yRotation < 0)
		yRotation = 0;
	else if(yRotation > 3)
		yRotation = 3;

	m_yRotation = yRotation;

	m_relativeRotation.y = (D3DX_PI*0.5f)*(float)m_yRotation;
}

void CTile::Elevate()
{
	if(m_collisionType == TILE_COLLISION_NONE)
		return;

	if(m_heightLevel == MAX_HEIGHT_LEVEL)
		return;

	++m_heightLevel;

	m_relativeTranslation.y += TILE_LEVEL_HEIGHT;
	UpdateBoundingBox();
}

void CTile::Lower()
{
	if(m_collisionType == TILE_COLLISION_NONE)
		return;

	if(m_heightLevel == 0)
		return;

	--m_heightLevel;

	m_relativeTranslation.y -= TILE_LEVEL_HEIGHT;
	UpdateBoundingBox();
}

void CTile::OnPreRender(bool isShadowPass)
{
	// It's assumed that if this function is called the frustum
	// check has already been made and this tile is visible
	if(m_mesh != NULL)
	{
		++TilesRendered;
		m_manager->RegisterNodeForRendering(this, SNG_DEFAULT);
	}
	else if(CTileManager::GetManager()->GetShowDummyTiles() && m_tileType == TILE_TYPE_DUMMY)
	{
		//CPrinter::log("Dummy ren");
		m_manager->RegisterNodeForRendering(this, SNG_DEFAULT);
	}
}

void CTile::Render(bool isShadowPass)
{
	//static CSimpleStaticProfiler profiler("results.txt");

	CDirectX9 *dx = GetDXApp();
	if(m_mesh)
	{
		dx->SetTransform(D3DTS_WORLD, &m_absoluteTransformation);

		if (isShadowPass)
			m_mesh->RenderWithoutTexture();
		else
		{
			if(m_texture)
				m_mesh->RenderWithCustomTexture(m_texture);
			else
			{
		//profiler.StartTiming();
				m_mesh->Render();
		//profiler.StopTiming();
			}
		}
	}
	else if(m_tileType == TILE_TYPE_DUMMY)
	{
		if (!isShadowPass)
			dx->RenderWireframeAABB(m_bbox, false, 0.f, false, D3DXCOLOR(0.f, 0.f, 5.5f, 2.5f));
	}
}

void CTile::RenderSubset(bool isShadowPass)
{
}

void CTile::SetSubset()
{
}

int CTile::GetSubsetCount()
{
	if(m_mesh)
		return 0;
	else
		return 0;
}

aabbox3d CTile::UpdateBoundingBox()
{
	ISceneNode::UpdateAbsolutePosition();
	if(m_mesh || m_tileType == TILE_TYPE_DUMMY)
	{
		//m_bbox = m_mesh->GetBoundingBox();
		m_bbox.maxCorner = D3DXVECTOR3(50.f, 0.f, 50.f);
		m_bbox.minCorner = D3DXVECTOR3(0.f, -m_tileHeight, 0.f);
	}
	CMath::TransformBoundingBox(m_absoluteTransformation, m_bbox);
	return m_bbox;
}

aabbox3d &CTile::GetBoundingBox()
{
	if(m_mesh)
		return m_mesh->GetBoundingBox();
	else
		return m_bbox;
}

aabbox3d CTile::GetTransformedBox()
{
	return m_bbox;
}

D3DXMATRIX CTile::GetRelativeTransformation()
{
	return CMath::ConstructTransformationMatrix(m_relativeTranslation, m_relativeRotation, 
		m_relativeScale, D3DXVECTOR3(m_bbox.GetWidth()*0.5f, m_bbox.GetHeight()*0.5f, m_bbox.GetDepth()*0.5f));
}

D3DXVECTOR3 CTile::GetAbsolutePosition()
{
	return m_relativeTranslation;
}

void CTile::CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions)
{
}

bool CTile::CheckForVerticalOnlyCollision(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &box, bool &bY)
{
	switch(m_collisionType)
	{
	case TILE_COLLISION_NONE:
		return false;
	case TILE_COLLISION_ACCURATE:
	case TILE_COLLISION_BOUNDING_BOX:
		if(box.minCorner.y > m_bbox.maxCorner.y || box.maxCorner.y < m_bbox.minCorner.y)
			return false;
		bY = true;
		if(box.maxCorner.y + box.minCorner.y < m_bbox.maxCorner.y + m_bbox.minCorner.y)
			collisionVec.y = m_bbox.minCorner.y - box.maxCorner.y;
		else
			collisionVec.y = m_bbox.maxCorner.y - box.minCorner.y;
		return true;
	case TILE_COLLISION_RAMP:
		return CheckCenterRampCollisions(object, collisionVec, box, bY);
	}

	return false;
}

bool CTile::CheckIfVerticalOnlyCollision(IGeometricObject *object, aabbox3d &box)
{
	switch(m_collisionType)
	{
	case TILE_COLLISION_NONE:
		return false;
	case TILE_COLLISION_ACCURATE:
	case TILE_COLLISION_BOUNDING_BOX:
		if(box.minCorner.y > m_bbox.maxCorner.y || box.maxCorner.y < m_bbox.minCorner.y)
			return false;
		return true;
	case TILE_COLLISION_RAMP:
		return CheckIfCenterRampCollision(object);
	}

	return false;
}

void CTile::CheckForHorizontalCollisions(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &box, int vlvl)
{
	/*if(m_collisionType != TILE_COLLISION_BOUNDING_BOX)
		return;

	if(m_heightLevel <= vlvl)
		return;

	if(!m_bbox.IntersectsWithBox(box))
		return;

	float dx=0.f,dz=0.f;
	D3DXVECTOR3 &speed = object->GetSpeed();

	if(speed.x<0.f)
		dx = m_bbox.maxCorner.x-box.minCorner.x;
	else
		dx = m_bbox.minCorner.x-box.maxCorner.x;
	if(speed.z<0.f)
		dz = m_bbox.maxCorner.z-box.minCorner.z;
	else
		dz = m_bbox.minCorner.z-box.maxCorner.z;

	SCollision c;
	c.o1 = this;
	c.o2 = object;
	c.v = D3DXVECTOR3(0.f,0.f,0.f);
	if(fabs(dx)*fabs(speed.z)<fabs(dz)*fabs(speed.x))
		c.v.x = dx;
	else
		c.v.z = dz;
	collisions.push_back(c);
	object->RegisterCollision(this);*/
}

void CTile::CheckForCollisionSimple(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &box, int hlvl, bool &bX, bool &bY, bool &bZ, int relPos)
{
	if(m_collisionType == TILE_COLLISION_NONE)
		return;
	else if(m_collisionType == TILE_COLLISION_RAMP)
		if(m_yRotation == relPos)
			++hlvl;

	if(m_heightLevel <= hlvl)
		return;

	if(!m_bbox.IntersectsWithBox(box))
		return;

	// Calculate the area that the aabboxes overlap and deduce the
	// direction of collision by finding minimum overlap axis
	float l,temp;
	int axis = 0;
	D3DXVECTOR3 &omi = box.minCorner, &oma = box.maxCorner, &mi = m_bbox.minCorner, &ma = m_bbox.maxCorner;
	if(oma.x > ma.x) {
		if(omi.x < mi.x)
			l = ma.x - mi.x;
		else
			l = ma.x - omi.x;
	} else {
		if(omi.x < mi.x)
			l = oma.x - mi.x;
		else
			l = oma.x - omi.x;
	}

	if(oma.y > ma.y) {
		if(omi.y < mi.y)
			temp = ma.y - mi.y;
		else
			temp = ma.y - omi.y;
	} else {
		if(omi.y < mi.y)
			temp = oma.y - mi.y;
		else
			temp = oma.y - omi.y;
	}
	if(temp < l)
	{
		l = temp;
		axis = 1;
	}

	if(oma.z > ma.z) {
		if(omi.z < mi.z)
			temp = ma.z - mi.z;
		else
			temp = ma.z - omi.z;
	} else {
		if(omi.z < mi.z)
			temp = oma.z - mi.z;
		else
			temp = oma.z - omi.z;
	}
	if(temp < l)
	{
		l = temp;
		axis = 2;
	}

	// Now that we know the collision depth and axis on which it occurred
	// get the collision direction, check whether that component was
	// previously occupied and if not, update the collision
	if(axis == 0)
	{
		if(bX)
			return;
		if(omi.x + oma.x < mi.x + ma.x)
			collisionVec.x = -l;
		else
			collisionVec.x = l;
		bX = true;
		return;
	}
	else if(axis == 1)
	{
		if(bY)
			return;
		if(omi.y + oma.y < mi.y + ma.y)
			collisionVec.y = -l;
		else
			collisionVec.y = l;
		bY = true;
		return;
	}
	else if(axis == 2)
	{
		if(bZ)
			return;
		if(omi.z + oma.z < mi.z + ma.z)
			collisionVec.z = -l;
		else
			collisionVec.z = l;
		bZ = true;
		return;
	}

	/*if(m_collisionType == TILE_COLLISION_RAMP)
	{
		CheckRampCollisions(object, collisions, box, hlvl);
		return;
	}

	float dx,dz,dy;
	D3DXVECTOR3 &speed = object->GetSpeed();

	if(speed.x<0.f)
		dx = m_bbox.maxCorner.x-box.minCorner.x;
	else
		dx = m_bbox.minCorner.x-box.maxCorner.x;
	if(speed.z<0.f)
		dz = m_bbox.maxCorner.z-box.minCorner.z;
	else
		dz = m_bbox.minCorner.z-box.maxCorner.z;
	if(speed.y<0.f)
		dy = m_bbox.maxCorner.y-box.minCorner.y;
	else
		//dy = m_bbox.minCorner.y-box.maxCorner.y;
		dy = 1000.f;

	SCollision c;
	c.o1 = this;
	c.o2 = object;
	c.v = D3DXVECTOR3(0.f,0.f,0.f);

	// See if the object collides to tile's top or bottom surface
	if(absf(dy*speed.z)<absf(dz*speed.y) && absf(dy*speed.x)<absf(dx*speed.y))
	{
		if(verticalCollisionDetected)
		{
			// A vertical collision has already been registered for this object
			return;
		}
		c.v.y = dy;
		collisions.push_back(c);
		object->RegisterCollision(this);
		verticalCollisionDetected = TRUE;
		return;
	}
	if(absf(dx*speed.z)<absf(dz*speed.x))
		c.v.x = dx;
	else
		c.v.z = dz;
	collisions.push_back(c);
	object->RegisterCollision(this);*/
}

bool CTile::CheckCenterRampCollisions(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &box, bool &bY)
{
	D3DXVECTOR3 t = object->GetTranslation();
	float h = t.y-GetTileHeight(t.x-m_bbox.minCorner.x, t.z-m_bbox.minCorner.z);
	bool negative = false;
	if(h<0.f)
	{
		h=-h;
		negative = true;
	}
	h*=RAMP_VERT_DIST_COEFFICIENT;
	if((h -= object->GetBoundingSphere().radius) < 0.f)
	{
		// Collision
		collisionVec = DEFAULT_RAMP_COLLISION_UNIT_VECTOR;
		if(m_yRotation == 1)
		{
			collisionVec.z = -collisionVec.x;
			collisionVec.x = 0.f;
		}
		else if(m_yRotation == 2)
			collisionVec.x = -collisionVec.x;
		else if(m_yRotation == 3)
		{
			collisionVec.z = collisionVec.x;
			collisionVec.x = 0.f;
		}
		if(negative)
			collisionVec*=h;
		else
			collisionVec*=-h;

		bY = true;
		return true;
	}

	return false;
}

bool CTile::CheckIfCenterRampCollision(IGeometricObject *object)
{
	D3DXVECTOR3 t = object->GetTranslation();
	float h = t.y-GetTileHeight(t.x-m_bbox.minCorner.x, t.z-m_bbox.minCorner.z);
	//if(h<0.f)
	//	h=-h;
	h*=RAMP_VERT_DIST_COEFFICIENT;
	if(absf(h) - object->GetBoundingSphere().radius < 0.f)
		return true;
	else if(h<0.f)
		if(t.y > m_bbox.minCorner.y)
			return true;

	return false;
}

bool CTile::CheckTileIntersection(aabbox3d &box)
{
	return m_bbox.IntersectsWithBox(box);
}

float CTile::GetTileHeight(float x, float z)
{
	switch(m_collisionType)
	{
	case TILE_COLLISION_ACCURATE:
		return 0.f;
	case TILE_COLLISION_RAMP:
		if(m_yRotation & 1)
		{
			if(m_yRotation > 2)
				return TILE_LEVEL_HEIGHT * ((float)m_heightLevel - 1.f + z*ONE_PER_TILE_DIMENSION);
				//return TILE_LEVEL_HEIGHT * ((float)m_heightLevel + z*ONE_PER_TILE_DIMENSION);
			else
				return TILE_LEVEL_HEIGHT * ((float)m_heightLevel - z * ONE_PER_TILE_DIMENSION);
				//return TILE_LEVEL_HEIGHT * ((float)m_heightLevel +1.f - z * ONE_PER_TILE_DIMENSION);
		}
		else
		{
			if(m_yRotation == 0)
				return TILE_LEVEL_HEIGHT * ((float)m_heightLevel - 1.f + x*ONE_PER_TILE_DIMENSION);
				//return TILE_LEVEL_HEIGHT * ((float)m_heightLevel + x*ONE_PER_TILE_DIMENSION);
			else
				return TILE_LEVEL_HEIGHT * ((float)m_heightLevel - x * ONE_PER_TILE_DIMENSION);
				//return TILE_LEVEL_HEIGHT * ((float)m_heightLevel + 1.f - x * ONE_PER_TILE_DIMENSION);
		}
	case TILE_COLLISION_BOUNDING_BOX:
		return m_heightLevel * TILE_LEVEL_HEIGHT;
	case TILE_COLLISION_NONE:
	default:
		return -200.f;
	}
}