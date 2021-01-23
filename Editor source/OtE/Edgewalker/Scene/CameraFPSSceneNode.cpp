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

// CameraFPSSceneNode.cpp: implementation of the CCameraFPSSceneNode class.
//
//////////////////////////////////////////////////////////////////////

#include "CameraFPSSceneNode.h"
#include "..\\Edgewalker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCameraFPSSceneNode::CCameraFPSSceneNode(ISceneNode *parent, ISceneManager *manager, CInputMouse *mouse, 
		DWORD id,float movespeed, float rotatespeed, D3DXVECTOR3 position)
		:	CCameraSceneNode(parent, manager, id, position, position+D3DXVECTOR3(-1.f, -5.f, 0.f)), 
		m_speed(movespeed),	m_rotateSpeed(rotatespeed), m_mouse(mouse), m_rotationRequiresMouseRight(false),
		m_bActive(false)
{
	m_controls.FillDefaults();
}

CCameraFPSSceneNode::~CCameraFPSSceneNode()
{
}

/**
 * Update the camera FPS-style
 **/
void CCameraFPSSceneNode::Update(float frameTime)
{
	if(m_mouse == NULL || !m_bActive)
		return;

	CInputKeyboard *keyb = GetEdgewalkerApp()->GetKeyboard();

	if(!m_rotationRequiresMouseRight || m_mouse->GetMouseRightDown())
	{
		m_relativeRotation.x += m_rotateSpeed * m_mouse->GetRelativeY();
		m_relativeRotation.y += m_rotateSpeed * m_mouse->GetRelativeX();
	}

	if(m_relativeRotation.x < -D3DX_PI*0.5f+0.01f)
		m_relativeRotation.x = -D3DX_PI*0.5f+0.01f;
	if(m_relativeRotation.x > D3DX_PI*0.5f-0.01f)
		m_relativeRotation.x = D3DX_PI*0.5f-0.01f;

	D3DXVECTOR3 movedir;


	D3DXVECTOR3 pos(0.f, 0.f, 0.f);
	bool update = FALSE;

	m_target.x = sinf(m_relativeRotation.y) * cosf(m_relativeRotation.x);
	m_target.y = -sinf(m_relativeRotation.x);
	m_target.z = cosf(m_relativeRotation.y) * cosf(m_relativeRotation.x);
	
	if(keyb->GetButtonDown(m_controls.forward))
	{
		pos.x = sinf(m_relativeRotation.y) * cosf(m_relativeRotation.x);
		pos.y = -sinf(m_relativeRotation.x);
		pos.z = cosf(m_relativeRotation.y) * cosf(m_relativeRotation.x);
		update = TRUE;
	}
	else if(keyb->GetButtonDown(m_controls.backward))
	{
		pos.x = -sinf(m_relativeRotation.y) * cosf(m_relativeRotation.x);
		pos.y = sinf(m_relativeRotation.x);
		pos.z = -cosf(m_relativeRotation.y) * cosf(m_relativeRotation.x);
		update = TRUE;
	}

	if(keyb->GetButtonDown(m_controls.strifeleft))
	{
		pos.x -= cosf(m_relativeRotation.y);
		pos.z += sinf(m_relativeRotation.y);
		update = TRUE;
	}
	else if(keyb->GetButtonDown(m_controls.striferight))
	{
		pos.x += cosf(m_relativeRotation.y);
		pos.z -= sinf(m_relativeRotation.y);
		update = TRUE;
	}


	if(update)
	{
		m_relativeTranslation += (frameTime * m_speed) * pos;
	}

	m_target += m_relativeTranslation;

	CCameraSceneNode::Update(frameTime);
	UpdateBillboardMatrix();
}

void CCameraFPSSceneNode::UpdateBillboardMatrix()
{
	D3DXMATRIX matRotX, matRotY;

	D3DXMatrixRotationX(&matRotX, m_relativeRotation.x);
	D3DXMatrixRotationY(&matRotY, m_relativeRotation.y);

	D3DXMatrixMultiply(&m_billboardMatrix, &matRotX, &matRotY);/**/
}