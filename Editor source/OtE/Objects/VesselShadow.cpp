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

#include "VesselShadow.h"

CVesselShadow::CVesselShadow(CMesh *mesh, ISceneNode *parent, ISceneManager *manager) :
	CMeshSceneNode(mesh, parent, manager)
{
}

CVesselShadow::~CVesselShadow()
{
}

void CVesselShadow::OnPreRender(bool isShadowPass)
{
	if(m_bVisible && m_mesh && !isShadowPass)
	{
		m_manager->RegisterNodeForRendering(this, SNG_ALPHA_BLENDED);
	}
	ISceneNode::OnPreRender(isShadowPass);
}

void CVesselShadow::Render(bool isShadowPass)
{
	if(m_mesh == NULL || isShadowPass)
		return;

	// Render the mesh
	CDirectX9 *dx = GetDXApp();
	if(dx == NULL)
		return;

	// Set blend render states
	LPDIRECT3DDEVICE9 pDevice = dx->GetDevice();
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);

	dx->SetTransform(D3DTS_WORLD, &m_absoluteTransformation);
	m_mesh->Render();
}

void CVesselShadow::CheckForCollision(IGeometricObject *object, vector<SCollision> &collisions)
{
}