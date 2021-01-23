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

#include "TileQuadTreeNode.h"
#include "..\\Scene\\CameraSceneNode.h"
#include "..\\Math.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTileQuadTreeNode::CTileQuadTreeNode(ISceneNode *parent, ISceneManager *manager)
		: IGeometricObject(parent, manager), m_bLeaf(FALSE)
{
	m_bAutomaticCulling = FALSE;
}

CTileQuadTreeNode::~CTileQuadTreeNode()
{
}

void CTileQuadTreeNode::OnPreRender(CFrustum &f, bool preCulled, bool isShadowPass)
{
	// Loop through children and do aabb frustum check to each of them
	list<ISceneNode*>::iterator it = m_children.begin();
	aabbox3d box;
	int result;

	//CCameraSceneNode *cam = m_manager->GetActiveCamera();

	if(m_bLeaf)
	{
		for(;it!=m_children.end();it++)
			if((*it) != NULL)
			{
				/*box = (*it)->GetTransformedBox();
				result = f.DoesIntersectWithAABB(box);
				if(result != OUTSIDE)/**/
					(*it)->OnPreRender(isShadowPass);
			}
		return;
	}

	if(preCulled)
	{
		for(;it!=m_children.end();it++)
			if((*it) != NULL)
				((CTileQuadTreeNode*)(*it))->OnPreRender(f, true, isShadowPass);
		return;
	}

	for(;it!=m_children.end();it++)
	{
		if((*it) != NULL)
		{
			box = (*it)->GetTransformedBox();
			result = f.DoesIntersectWithAABB(box);
			if(result == INSIDE)
				((CTileQuadTreeNode*)(*it))->OnPreRender(f, true, isShadowPass);
			else if(result == INTERSECTING)
				((CTileQuadTreeNode*)(*it))->OnPreRender(f, false, isShadowPass);
			//if(box.IntersectsWithBox(cam->GetFrustum().GetBoundingBox()))
			//	(*it)->OnPreRender();
		}
	}
}

void CTileQuadTreeNode::Render(bool isShadowPass)
{
	// Should never happen
}

aabbox3d CTileQuadTreeNode::UpdateBoundingBox()
{
	list<ISceneNode*>::iterator it = m_children.begin();

	aabbox3d box(0.f), temp;
	bool bSet = FALSE;

	for(;it!=m_children.end();it++)
	{
		if((*it) == NULL)
			continue;

		if(!m_bLeaf)
			temp = ((CTileQuadTreeNode*)(*it))->UpdateBoundingBox();
		else
		{
			// Ignore empty tiles
			if(((CTile*)(*it))->GetCollisionType() == TILE_COLLISION_NONE)
				continue;
			temp = ((CTile*)(*it))->UpdateBoundingBox();
		}

		if(!bSet)
		{
			box = temp;
			bSet = TRUE;
		}
		else
			box.AddInternalBox(temp);
	}

	if(bSet)
		m_bbox = box;
	return box;
}

void CTileQuadTreeNode::Create(vector<CTile*> &tiles, rect<int> r, POINT &dimensions)
{
	DWORD numTiles = (r.getWidth()+1)*(r.getHeight()+1);

	// Moving tile terrain from position (0.f, 0.f, 0.f) is not supported
	//m_relativeTranslation = D3DXVECTOR3(

	if(numTiles <= MAX_TILES_PER_NODE)
	{
		// This is a leaf node, the children of this node are CTile objects
		m_bLeaf = TRUE;

		// Clear any existing children - this shouldn't be necessary
		RemoveAll();

		int x,y;
		for(y=r.top;y<=r.bottom;y++)
		{
			for(x=r.left;x<=r.right;x++)
			{
				m_children.push_back((ISceneNode*)tiles[y*dimensions.x+x]);
			}
		}
		return;
	}
	else
	{
		// This is a branch node, the children of this node are other
		// CTileQuadTreeNode objects
		rect<int> x1y1, x2y1, x1y2, x2y2;
		int xCenter = (int)((r.left+r.right)/2);
		int yCenter = (int)((r.top+r.bottom)/2);

		// Construct the node rectangles
		// BORDERS
		x1y1.left = x1y2.left = r.left;
		x2y1.right = x2y2.right = r.right;
		// REMARK: top < bottom
		x1y1.top = x2y1.top = r.top;
		x1y2.bottom = x2y2.bottom = r.bottom;

		// CENTER
		x1y1.right = x1y2.right = xCenter;
		x2y1.left = x2y2.left = xCenter+1;
		x1y1.bottom = x2y1.bottom = yCenter;
		x1y2.top = x2y2.top = yCenter+1;

		//list<ISceneNode*>::iterator it = m_children.begin();

		CTileQuadTreeNode *node;

		// Create the nodes if needed
		// X1Y1
		if(x1y1.getWidth()>=0 && x1y1.getHeight()>=0)
		{
			node = new CTileQuadTreeNode(this, m_manager);
			//m_children.push_back(node);
			node->Create(tiles, x1y1, dimensions);
		}
		else
			m_children.push_back(NULL);

		//++it;

		// X2Y2
		if(x2y1.getWidth()>=0 && x2y1.getHeight()>=0)
		{
			node = new CTileQuadTreeNode(this, m_manager);
			//m_children.push_back(node);
			node->Create(tiles, x2y1, dimensions);
		}
		else
			m_children.push_back(NULL);

		//++it;

		// X1Y2
		if(x1y2.getWidth()>=0 && x1y2.getHeight()>=0)
		{
			node = new CTileQuadTreeNode(this, m_manager);
			//m_children.push_back(node);
			node->Create(tiles, x1y2, dimensions);
		}
		else
			m_children.push_back(NULL);

		//++it;

		// X2Y2
		if(x2y2.getWidth()>=0 && x2y2.getHeight()>=0)
		{
			node = new CTileQuadTreeNode(this, m_manager);
			//m_children.push_back(node);
			node->Create(tiles, x2y2, dimensions);
		}
		else
			m_children.push_back(NULL);
	}
}

CTile *CTileQuadTreeNode::GetTileFromRay(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir)
{
	list<ISceneNode*>::iterator it = m_children.begin();

	float distSq = 10000000.f, dist2Sq;

	aabbox3d box;
	D3DXVECTOR3 tilePos;
	CTile *tile = NULL, *tempTile = NULL;

	for(;it!=m_children.end();it++)
	{
		if(!m_bLeaf)
			box = ((IGeometricObject*)(*it))->GetBoundingBox();
		else
			box = ((CTile*)(*it))->GetTransformedBox();
		box.minCorner.y = 0.f;
		if(CMath::DoesRayIntersectAABB(box, pos, dir, 1000.f))
		{
			if(m_bLeaf)
			{
				// Check the distance
				tilePos = (*it)->GetTranslation() - pos;
				// Only check xz-distance here to avoid selecting lower tiles behind higher ones
				//if((dist2Sq = tilePos.x*tilePos.x + tilePos.y*tilePos.y + tilePos.z*tilePos.z) < distSq)
				if((dist2Sq = tilePos.x*tilePos.x + tilePos.z*tilePos.z) < distSq)
				{
					tile = (CTile*)(*it);
					distSq = dist2Sq;
				}
			}
			else
			{
				// Check the distance
				tempTile = ((CTileQuadTreeNode*)(*it))->GetTileFromRay(pos, dir);
				if(tempTile == NULL)
					continue;
				tilePos = tempTile->GetTranslation() - pos;
				// Only check xz-distance here to avoid selecting lower tiles behind higher ones
				//if((dist2Sq = tilePos.x*tilePos.x + tilePos.y*tilePos.y + tilePos.z*tilePos.z) < distSq)
				if((dist2Sq = tilePos.x*tilePos.x + tilePos.z*tilePos.z) < distSq)
				{
					distSq = dist2Sq;
					tile = tempTile;
				}
			}
		}
	}

	return tile;
}