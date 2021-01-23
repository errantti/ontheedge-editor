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

#ifndef __CSELECTIONMESHSCENENODE_H__
#define __CSELECTIONMESHSCENENODE_H__

#include "OtE\\Edgewalker\\Scene\\MeshSceneNode.h"

class CSceneManager;

/**
 * Class that specifically handles selection mesh (for example
 * a circle that surrounds a target). The scene node is not
 * supposed to be made a child of the target but rather given
 * the target's info on every frame. The object updates its size
 * and placement based on the target.
 */
class CSelectionMeshSceneNode : public CMeshSceneNode
{
public:
	CSelectionMeshSceneNode(CSceneManager *manager, CMesh *mesh);
	virtual ~CSelectionMeshSceneNode();

	void UpdateSelection(IGeometricObject *target);

	virtual void Render();
	virtual void Update(float frameTime);

	void EnableLighting(bool value) {m_bLighted = value;}
	void EnableZBuffer(bool value) {m_bUseZBuffer = value;}

private:
	bool m_bLighted, m_bUseZBuffer;

	IGeometricObject *m_currentTarget;
};

#endif // __CSELECTIONMESHSCENENODE_H__