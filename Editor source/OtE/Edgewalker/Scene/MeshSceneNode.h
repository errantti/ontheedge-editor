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

#ifndef __CMESHSCENENODE_H__
#define __CMESHSCENENODE_H__

#include "..\\IGeometricObject.h"
#include "..\\DirectX9\\Mesh.h"

class CMeshSceneNode : public IGeometricObject
{
public:
	CMeshSceneNode(CMesh *mesh, ISceneNode *parent, ISceneManager *manager, DWORD id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	virtual ~CMeshSceneNode();

	virtual void SetMesh(CMesh *mesh);
	virtual void CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions);
	virtual void UpdateBoundingVolumes();
	virtual void RegisterCollision(IGeometricObject *object) {}

	virtual void Render(bool isShadowPass);
	virtual void Update(float frameTime);
	virtual void OnPreRender(bool isShadowPass);

	// Automatically scales both the rendered object and this scene node's bounding
	// box and sphere
	virtual void SetScalingCoefficient(float coef);

protected:
	void GetSimpleBoxCollisionVector(IGeometricObject *object, D3DXVECTOR3 &collisionVec, aabbox3d &myBox, aabbox3d &obBox);

	CMesh *m_mesh;

	// Mesh scaling coefficient
	float m_scalingCoefficient;
};

#endif // #ifndef __CMESHSCENENODE_H__
