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

#ifndef __CCAMERASCENENODE_H__
#define __CCAMERASCENENODE_H__

#include "ISceneNode.h"
#include "..\\DirectX9\\DirectX9.h"
#include "Frustum.h"

class CCameraSceneNode : public ISceneNode
{
public:
	CCameraSceneNode(ISceneNode *parent, ISceneManager *manager, DWORD id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 lookat = D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	virtual ~CCameraSceneNode();

	virtual void Render(bool isShadowPass);
	virtual void OnPreRender(bool isShadowPass);
	virtual void Update(float frameTime);
	virtual aabbox3d &GetBoundingBox() {return m_bbox;}

	virtual void SetProjectionMatrix(D3DXMATRIX proj) {m_projection = proj;}
	virtual D3DXMATRIX GetProjectionMatrix() const {return m_projection;}
	virtual D3DXMATRIX GetViewMatrix() const {return m_view;}

	virtual D3DXMATRIX GetBillboardMatrix() {return m_billboardMatrix;}

	virtual void SetFieldOfView(float fov) {m_fov = fov;}
	virtual void SetAspectRatio(float aspect) {m_aspect = aspect;}
	virtual void SetNearValue(float nearz) {m_zn = nearz;}
	virtual void SetFarValue(float farz) {m_zf = farz;}

	virtual void SetUpVector(D3DXVECTOR3 up) {m_up = up;}
	virtual void SetLookatVector(D3DXVECTOR3 target) {m_target = target;}

	virtual float GetFieldOfView() const {return m_fov;}
	virtual float GetAspectRatio() const {return m_aspect;}
	virtual float GetNearValue() const {return m_zn;}
	virtual float GetFarValue() const {return m_zf;}

	virtual D3DXVECTOR3 GetUpVector() const {return m_up;}
	virtual D3DXVECTOR3 GetLookatVector() const {return m_target;}

	virtual CFrustum &GetFrustum() {return m_frustum;}

	void CalculateProjectionMatrix();

protected:
	void CalculateFrustum(D3DXVECTOR3 *position = NULL);

	aabbox3d m_bbox;

	D3DXMATRIX m_projection, m_view;
	D3DXMATRIX m_billboardMatrix;

	D3DXVECTOR3 m_up, m_target;	// Position and direction vectors

	float m_fov;	// Field of view
	float m_aspect;	// Aspect ratio
	float m_zn;		// Z near value
	float m_zf;		// Z far value

	CFrustum m_frustum;	// Viewing frustum of the camera
};

#endif // #ifndef __CCAMERASCENENODE_H__
