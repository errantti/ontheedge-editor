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

#include "SelectionMeshSceneNode.h"
#include "OtE\\OnTheEdge.h"

CSelectionMeshSceneNode::CSelectionMeshSceneNode(CSceneManager *manager, CMesh *mesh)
: CMeshSceneNode(mesh, manager, manager), m_bLighted(false), m_currentTarget(NULL), m_bUseZBuffer(false)
{
	m_bVisible = false;
}

CSelectionMeshSceneNode::~CSelectionMeshSceneNode()
{
}

void CSelectionMeshSceneNode::Render()
{
	if(m_mesh == NULL)
		return;

	// Render the mesh
	CDirectX9 *dx = GetDXApp();
	if(dx == NULL)
		return;

	LPDIRECT3DDEVICE9 d = dx->GetDevice();
	D3DCOLOR backupAmbient;

	if(!m_bLighted)
	{
		d->GetRenderState(D3DRS_AMBIENT, &backupAmbient);
		d->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(255, 255, 255));
	}
	if(!m_bUseZBuffer)
		dx->EnableZBuffer(false);

	dx->SetTransform(D3DTS_WORLD, &m_absoluteTransformation);
	m_mesh->Render();

	if(!m_bLighted)
		d->SetRenderState(D3DRS_AMBIENT, backupAmbient);
	if(!m_bUseZBuffer)
		dx->EnableZBuffer(true);
}

void CSelectionMeshSceneNode::Update(float frameTime)
{
	// Rotate the mesh if selected
	if(m_bVisible)
	{
		m_relativeRotation.y += frameTime * 0.05f;
		if(m_relativeRotation.y > 2*D3DX_PI)
			m_relativeRotation.y -= 2*D3DX_PI;
	}
}

void CSelectionMeshSceneNode::UpdateSelection(IGeometricObject *target)
{
	//if(target == m_currentTarget)
	//	return;

	m_currentTarget = target;

	if(target == NULL || m_mesh == NULL)
	{
		m_bVisible = false;
		return;
	}

	m_bVisible = true;

	m_relativeTranslation = target->GetTranslation();

	aabbox3d targetBox = target->GetBoundingBox();
	float max = targetBox.GetWidth();
	if(targetBox.GetDepth() > max)
		max = targetBox.GetDepth();

	m_relativeTranslation.x += (targetBox.minCorner.x + targetBox.maxCorner.x)*0.5f;
	m_relativeTranslation.z += (targetBox.minCorner.z + targetBox.maxCorner.z)*0.5f;

	// Asuming square mesh in x/z level, TODO: don't make assumptions
	float coef = max/m_bbox.GetWidth();
	coef *= 1.4f;

	m_relativeScale.x = coef;
	m_relativeScale.z = coef;
}