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

// CameraFollowSceneNode.cpp: implementation of the CCameraFollowSceneNode class.
//
//////////////////////////////////////////////////////////////////////

#include "CameraFollowSceneNode.h"
#include <d3dx9math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraFollowSceneNode::CCameraFollowSceneNode(ISceneNode *parent, ISceneManager *manager, long ID, D3DXVECTOR3 relPosition)
: CCameraSceneNode(parent, manager, ID), m_bRotateWithTarget(false)
{
	m_relativeTranslation = relPosition;
}

CCameraFollowSceneNode::CCameraFollowSceneNode(ISceneNode *parent, ISceneManager *manager, long ID, 
		float radius , float angle)
		: CCameraSceneNode(parent, manager, ID, D3DXVECTOR3(10.f, 10.f, 10.f))
{
}

CCameraFollowSceneNode::~CCameraFollowSceneNode()
{
}

void CCameraFollowSceneNode::OnPreRender(bool isShadowPass)
{
	CDirectX9 *dx = GetDXApp();

	if(m_manager->GetActiveCamera() == this && m_manager->IsSettingCameras())
	{
		// Activate this camera
		dx->SetTransform(D3DTS_PROJECTION, &m_projection);

		m_target = m_parent->GetAbsolutePosition();
		D3DXVECTOR3 pos(m_relativeTranslation + m_target);
		if(m_bRotateWithTarget)
		{
			pos.x = m_absoluteTransformation._41;
			pos.y = m_absoluteTransformation._42;
			pos.z = m_absoluteTransformation._43;
		}
		D3DXMatrixLookAtLH(&m_view, &pos, &m_target, &m_up);
		UpdateBillboardMatrix();

		m_manager->RegisterNodeForRendering(this, SNG_CAMERA);

		// Update frustum
		CalculateFrustum(&pos);
	}

	// Only recurse further if the node is visible and this is the 'real' pre-rendering phase
	if(m_bVisible && !m_manager->IsSettingCameras())
		ISceneNode::OnPreRender(isShadowPass);
}

void CCameraFollowSceneNode::Update(float frameTime)
{
	ISceneNode::Update(frameTime);
}

D3DXVECTOR3 CCameraFollowSceneNode::GetAbsolutePosition()
{
	D3DXVECTOR3 pos;
	if(m_bRotateWithTarget)
	{
		pos.x = m_absoluteTransformation._41;
		pos.y = m_absoluteTransformation._42;
		pos.z = m_absoluteTransformation._43;
	}
	else
		pos = m_parent->GetAbsolutePosition() + m_relativeTranslation;

	return pos;
}

D3DXVECTOR3 CCameraFollowSceneNode::GetAbsoluteRotation()
{
	D3DXVECTOR3 rot;
	float temp = -m_relativeTranslation.x/m_relativeTranslation.z;
	rot.y = atanf(temp);
	if(temp > 0.f && m_relativeTranslation.z < 0.f)
		rot.y += D3DX_PI;
	rot.x = atanf(m_relativeTranslation.y/(cosf(rot.y)*sqrtfast(m_relativeTranslation.x*m_relativeTranslation.x+
		m_relativeTranslation.z*m_relativeTranslation.z)));
	rot.z = 0.f;
	return rot;
}

D3DXVECTOR3 CCameraFollowSceneNode::GetRotation() const
{
	D3DXVECTOR3 rot;
	//rot.y = -atanf(-m_relativeTranslation.x/m_relativeTranslation.z);

	float temp = -m_relativeTranslation.x/m_relativeTranslation.z;
	rot.y = atanf(temp);
	if(m_relativeTranslation.x > 0.f)
		rot.y += D3DX_PI;

	//rot.x = atanf(m_relativeTranslation.y/(cosf(rot.y)*sqrtfast(m_relativeTranslation.x*m_relativeTranslation.x+
	//	m_relativeTranslation.z*m_relativeTranslation.z)));
	rot.x = asinf(m_relativeTranslation.y / sqrtfast(m_relativeTranslation.x * m_relativeTranslation.x +
		m_relativeTranslation.y * m_relativeTranslation.y + m_relativeTranslation.z * m_relativeTranslation.z));
	rot.z = 0.f;
	return rot;
}

void CCameraFollowSceneNode::UpdateBillboardMatrix()
{
	D3DXVECTOR3 n;
	D3DXVec3Normalize(&n, &m_relativeTranslation);
	//n = -n;
	D3DXVECTOR3 uo(0.f, 1.f, 0.f);
	D3DXVECTOR3 u, r;
	D3DXVec3Cross(&r, &uo, &n);
	D3DXVec3Normalize(&r, &r);
	D3DXVec3Cross(&u, &n, &r);
	D3DXVec3Normalize(&u, &u);

	D3DXMatrixIdentity(&m_billboardMatrix);
	m_billboardMatrix._11 = r.x;
	m_billboardMatrix._12 = r.y;
	m_billboardMatrix._13 = r.z;

	m_billboardMatrix._21 = u.x;
	m_billboardMatrix._22 = u.y;
	m_billboardMatrix._23 = u.z;

	m_billboardMatrix._31 = n.x;
	m_billboardMatrix._32 = n.y;
	m_billboardMatrix._33 = n.z;
}