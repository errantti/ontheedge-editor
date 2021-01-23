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

#include "CameraSceneNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraSceneNode::CCameraSceneNode(ISceneNode *parent, ISceneManager *manager, DWORD id,
	D3DXVECTOR3 position, D3DXVECTOR3 lookat)
	: ISceneNode(parent, manager, id, position), m_target(lookat)
{
	m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	m_fov = D3DX_PI / 4.0f;
	m_aspect = 4.0f / 3.0f;
	m_zn = 1.0f;
	m_zf = DEFAULT_DRAWDIST;

	CalculateProjectionMatrix();

	D3DXMatrixIdentity(&m_billboardMatrix);
}

CCameraSceneNode::~CCameraSceneNode()
{
	ISceneManager *m = ISceneManager::GetManager();
	if(m)
		if(m->GetActiveCamera() == this)
			m->SetActiveCamera(NULL);
}

//
void CCameraSceneNode::OnPreRender(bool isShadowPass)
{
	CDirectX9 *dx = GetDXApp();

	if(m_manager->GetActiveCamera() == this && m_manager->IsSettingCameras())
	{
		// Activate this camera
		dx->SetTransform(D3DTS_PROJECTION, &m_projection);

		D3DXVECTOR3 pos = GetAbsolutePosition();
		D3DXMatrixLookAtLH(&m_view, &pos, &m_target, &m_up);

		// Update billboard matrix
		D3DXVECTOR3 n = pos - m_target;
		D3DXVec3Normalize(&n, &n);
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

		m_manager->RegisterNodeForRendering(this, SNG_CAMERA);

		// Update frustum
		CalculateFrustum(&pos);
	}

	// Only recurse further if the node is visible and this is the 'real' pre-rendering phase
	if(m_bVisible && !m_manager->IsSettingCameras())
		ISceneNode::OnPreRender(isShadowPass);
}

void CCameraSceneNode::Render(bool isShadowPass)
{
	CDirectX9 *dx = GetDXApp();
	dx->SetTransform(D3DTS_VIEW, &m_view);
}

void CCameraSceneNode::Update(float frameTime)
{
	ISceneNode::Update(frameTime);
}

void CCameraSceneNode::CalculateProjectionMatrix()
{
	D3DXMatrixPerspectiveFovLH(&m_projection, m_fov, m_aspect, m_zn, m_zf);
}

void CCameraSceneNode::CalculateFrustum(D3DXVECTOR3 *position)
{
	D3DXVECTOR3 pos = (position) ? *position : GetAbsolutePosition();
	m_frustum.m_camPos = pos;
	m_frustum.Construct(m_view * m_projection);
	m_frustum.CalculateBoundingBox();
}