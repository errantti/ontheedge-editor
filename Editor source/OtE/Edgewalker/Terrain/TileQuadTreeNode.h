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

#ifndef __CTILEQUADTREENODE_H__
#define __CTILEQUADTREENODE_H__

#include "Tile.h"
#include "..\\Scene\\Frustum.h"

// These don't have any practical function, just a reminder of the order
// in which the 
enum {
	TQT_X1Y1=0,
	TQT_X2Y1,
	TQT_X1Y2,
	TQT_X2Y2
};

class CTileQuadTreeNode : public IGeometricObject
{
public:
	CTileQuadTreeNode(ISceneNode *parent, ISceneManager *manager);
	virtual ~CTileQuadTreeNode();

	virtual void Create(vector<CTile*> &tiles, rect<int> r, POINT &dimensions);

	virtual void Render(bool isShadowPass);

	// Recursive function that goes thorugh the quad-tree
	// updating every node's bounding box
	virtual aabbox3d UpdateBoundingBox();

	void OnPreRender(CFrustum &f, bool preCulled, bool isShadowPass);

	virtual bool IsLeafNode() {return m_bLeaf;}

	virtual void CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions) {}
	virtual void UpdateBoundingVolumes() {}
	virtual void RegisterCollision(IGeometricObject *object) {}

protected:
	CTile *GetTileFromRay(D3DXVECTOR3 &pos, D3DXVECTOR3 &dir);

	// bounding box of the node contains all the bounding
	// boxes of its children

	// True if this node if a leaf node (=only has CTile children)
	bool m_bLeaf;
};

#endif // #ifndef __CTILEQUADTREENODE_H__
