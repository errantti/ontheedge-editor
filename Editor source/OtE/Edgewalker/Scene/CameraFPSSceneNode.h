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

#ifndef __CCAMERAFPSSCENENODE_H__
#define __CCAMERAFPSSCENENODE_H__

#include "CameraSceneNode.h"
#include "..\\Input\\InputMouse.h"

struct SFPSCameraControlSet
{
	BYTE forward, backward, strifeleft, striferight;

	void FillDefaults()
	{
		forward = DIK_W;
		backward = DIK_S;
		strifeleft = DIK_A;
		striferight = DIK_D;
	}
};

class CCameraFPSSceneNode : public CCameraSceneNode  
{
public:
	CCameraFPSSceneNode(ISceneNode *parent, ISceneManager *manager, CInputMouse *mouse, 
		DWORD id = -1, float movespeed = 1.f, float rotatespeed = 0.02f,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	virtual ~CCameraFPSSceneNode();

	virtual void Update(float frameTime);

	void SetControls(SFPSCameraControlSet controls) {m_controls = controls;}
	void SetRotationRequiresMouseRight(bool value) {m_rotationRequiresMouseRight = value;}

	void ActivateInput(bool value) {m_bActive = value;}

protected:
	void UpdateBillboardMatrix();

	float m_speed, m_rotateSpeed;
	SFPSCameraControlSet m_controls;
	bool m_rotationRequiresMouseRight, m_bActive;

	CInputMouse *m_mouse;
};

#endif // #ifndef __CCAMERAFPSSCENENODE_H__
