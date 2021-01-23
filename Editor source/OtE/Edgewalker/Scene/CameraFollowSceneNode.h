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

#ifndef __CCAMERAFOLLOWSCENENODE_H__
#define __CCAMERAFOLLOWSCENENODE_H__

#include "CameraSceneNode.h"

class CCameraFollowSceneNode : public CCameraSceneNode  
{
public:
	CCameraFollowSceneNode(ISceneNode *parent, ISceneManager *manager, long ID = -1, 
		D3DXVECTOR3 relPosition = D3DXVECTOR3(10.f, 10.f, 10.f));
	CCameraFollowSceneNode(ISceneNode *parent, ISceneManager *manager, long ID = -1, 
		float radius = 100.f, float angle = 0.f);
	virtual ~CCameraFollowSceneNode();

	virtual void OnPreRender(bool isShadowPass);

	virtual void Update(float frameTime);

	D3DXVECTOR3 GetAbsolutePosition();
	D3DXVECTOR3 GetAbsoluteRotation();
	virtual D3DXVECTOR3 GetRotation() const;

	bool IsRotatingWithTarget() {return m_bRotateWithTarget;}
	void SetRotateWithTarget(bool value) {m_bRotateWithTarget = value;}

private:
	void UpdateBillboardMatrix();

	bool m_bRotateWithTarget;
};

#endif // #ifndef __CCAMERAFOLLOWSCENENODE_H__
