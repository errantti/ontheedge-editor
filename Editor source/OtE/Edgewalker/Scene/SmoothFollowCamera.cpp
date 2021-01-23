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

#include "SmoothFollowCamera.h"
#include <d3dx9math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmoothFollowCamera::CSmoothFollowCamera(ISceneNode *parent, ISceneManager *manager, long ID, 
		float radius, float xRotation, float baseYRotationAngle)
		: CCameraSceneNode(parent, manager, ID, D3DXVECTOR3(10.f, 10.f, 10.f)),
		m_baseYRotationAngle(baseYRotationAngle),
		m_radiusSmoothingCoefficient(2.2f),
		m_angleSmoothingCoefficient(2.2f),
		m_targetRadius(radius),
		m_currentRadius(radius)
{
	m_relativeRotation.x = xRotation;
	ForceIntoCorrectPosition();
	UpdatePosition();
}

CSmoothFollowCamera::~CSmoothFollowCamera()
{
}

void CSmoothFollowCamera::OnPreRender(bool isShadowPass)
{
	CDirectX9 *dx = GetDXApp();

	if(m_manager->GetActiveCamera() == this && m_manager->IsSettingCameras())
	{
		// Activate this camera
		dx->SetTransform(D3DTS_PROJECTION, &m_projection);

		m_target = m_parent->GetAbsolutePosition();
		/*D3DXVECTOR3 pos = m_relativeTranslation + m_target;
		if(m_bRotateWithTarget)
		{
			pos.x = m_absoluteTransformation._41;
			pos.y = m_absoluteTransformation._42;
			pos.z = m_absoluteTransformation._43;
		}*/
		D3DXVECTOR3 pos(m_relativeTranslation + m_target);

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

void CSmoothFollowCamera::Update(float frameTime)
{
	ISceneNode::Update(frameTime);

	if(absf(m_currentRadius - m_targetRadius) > 0.3f)
		m_currentRadius -= frameTime * m_radiusSmoothingCoefficient * (m_currentRadius - m_targetRadius);

	float delta = m_parent->GetRotation().y - m_relativeRotation.y;
	static float twoPi = D3DX_PI * 2.f;
	static float onePerTwoPi = 1.f / twoPi;

	int temp = (int)(delta * onePerTwoPi);
	delta -= (float)temp* twoPi;
	if(delta < 0.f)
		delta += twoPi;
	if(delta > 0.1f)
	{
		if(delta >= D3DX_PI)
			m_relativeRotation.y -= frameTime * m_angleSmoothingCoefficient * (twoPi -delta);
		else
			m_relativeRotation.y += frameTime * m_angleSmoothingCoefficient * delta;
	}

	UpdatePosition();
}

void CSmoothFollowCamera::UpdatePosition()
{
	// Update relative translation
	m_relativeTranslation = D3DXVECTOR3(0.f, 0.f, 0.f);

	// TODO: optimize, I was very lazy! -.-
	D3DXMATRIX mat;
	D3DXVECTOR3 rot(m_relativeRotation);
	//rot.y = rot.y + D3DX_PI;
	mat = CMath::ConstructTransformationMatrix(m_relativeTranslation, rot, m_relativeScale);
	m_relativeTranslation = D3DXVECTOR3(0.f, 0.f, -m_currentRadius);
	m_relativeTranslation = CMath::MatrixVectorMultiply(mat, m_relativeTranslation);
}

void CSmoothFollowCamera::OnPostRender(float frameTime)
{
}

void CSmoothFollowCamera::SetAngleSmoothingCoefficient(float coefficient)
{
	m_angleSmoothingCoefficient = coefficient;
}

void CSmoothFollowCamera::SetRadiusSmoothingCoefficient(float coefficient)
{
	m_radiusSmoothingCoefficient = coefficient;
}

D3DXVECTOR3 CSmoothFollowCamera::GetAbsolutePosition()
{
	D3DXVECTOR3 pos;
	/*if(m_bRotateWithTarget)
	{
		pos.x = m_absoluteTransformation._41;
		pos.y = m_absoluteTransformation._42;
		pos.z = m_absoluteTransformation._43;
	}
	else*/
		pos = m_parent->GetAbsolutePosition() + m_relativeTranslation;

	return pos;
}

void CSmoothFollowCamera::ForceIntoCorrectPosition()
{
	m_currentRadius = m_targetRadius;
	m_relativeRotation.y = m_parent->GetRotation().y;
}

D3DXVECTOR3 CSmoothFollowCamera::GetAbsoluteRotation()
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

D3DXVECTOR3 CSmoothFollowCamera::GetRotation() const
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

void CSmoothFollowCamera::SetBaseYRotationAngle(float angle)
{
}

void CSmoothFollowCamera::UpdateBillboardMatrix()
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