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

#include "SkyBoxSceneNode.h"
#include "CameraSceneNode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkyBoxSceneNode::CSkyBoxSceneNode(ISceneManager *mgr, ISceneNode *parent, DWORD id,
		CTexture *top, CTexture *bottom, CTexture *front,
		CTexture *back, CTexture *left, CTexture *right)
	: ISceneNode(parent, mgr, id)
{
	m_textures[0] = top;
	m_textures[1] = bottom;
	m_textures[2] = left;
	m_textures[3] = right;
	m_textures[4] = front;
	m_textures[5] = back;

	// Initialize skybox vertex buffers
	SSkyboxVertex verts[] = {
		{ -700.01f,  700.f, -700.01f, 0.0f, 0.0f },  // Top
	    {  700.01f,  700.f, -700.01f, 1.0f, 0.0f },
	    { -700.01f,  700.f,  700.01f, 0.0f, 1.0f },
	    {  700.01f,  700.f,  700.01f, 1.0f, 1.0f },

	    { -700.01f, -700.f,  700.01f, 0.0f, 0.0f },  // Bottom
	    {  700.01f, -700.f,  700.01f, 1.0f, 0.0f },
	    { -700.01f, -700.f, -700.01f, 0.0f, 1.0f },
	    {  700.01f, -700.f, -700.01f, 1.0f, 1.0f },

	    { -700.f,  700.01f, -700.01f, 0.0f, 0.0f },  // Left
	    { -700.f,  700.01f,  700.01f, 1.0f, 0.0f },
	    { -700.f, -700.01f, -700.01f, 0.0f, 1.0f },
	    { -700.f, -700.01f,  700.01f, 1.0f, 1.0f },

	    {  700.f,  700.01f,  700.01f, 0.0f, 0.0f },  // Right
	    {  700.f,  700.01f, -700.01f, 1.0f, 0.0f },
	    {  700.f, -700.01f,  700.01f, 0.0f, 1.0f },
	    {  700.f, -700.01f, -700.01f, 1.0f, 1.0f },

	    { -700.01f,  700.01f,  700.f, 0.0f, 0.0f },  // Front
	    {  700.01f,  700.01f,  700.f, 1.0f, 0.0f },
	    { -700.01f, -700.01f,  700.f, 0.0f, 1.0f },
	    {  700.01f, -700.01f,  700.f, 1.0f, 1.0f },
	 
		{  700.01f,  700.01f, -700.f, 0.0f, 0.0f },  // Back
		{ -700.01f,  700.01f, -700.f, 1.0f, 0.0f },
		{  700.01f, -700.01f, -700.f, 0.0f, 1.0f },
	    { -700.01f, -700.01f, -700.f, 1.0f, 1.0f },
	};

	CDirectX9 *dx = GetDXApp();
	if(dx == NULL)
		return;

	if(FAILED(dx->GetDevice()->CreateVertexBuffer(sizeof(SSkyboxVertex) * 24, 0,
		SkyboxFVF, D3DPOOL_MANAGED, &m_vb, NULL)))
	{
		CPrinter::log("Failed creating skybox vertex buffer", LOG_ERROR);
		m_vb = NULL;
		return;
	}

	BYTE *ptr;

	if(FAILED(m_vb->Lock(0,0,(void**)&ptr,0)))
	{
		CPrinter::log("Failed creating skybox: failed locking vertex buffer", LOG_ERROR);
		m_vb->Release();
		m_vb = NULL;
		return;
	}

	memcpy(ptr, verts, sizeof(verts));

	m_vb->Unlock();

	m_material.SetDefaultMaterial();

	CPrinter::log("Skybox succesfully created");
}

CSkyBoxSceneNode::~CSkyBoxSceneNode()
{
	Release();
}

void CSkyBoxSceneNode::SetTextures(	CTexture *top, CTexture *bottom, CTexture *front,
						CTexture *back, CTexture *left, CTexture *right)
{
	int i,j;
	bool bDelRequired = TRUE;
	CTexture *textures[6];
	textures[0] = top;
	textures[1] = bottom;
	textures[2] = left;
	textures[3] = right;
	textures[4] = front;
	textures[5] = back;

	for(i=0;i<6;i++)
	{
		if(m_textures[i] != textures[i])
		{
			if(m_textures[i] != NULL)
			{
				for(j=i+1;j<6;j++)
				{
					if(m_textures[i] == m_textures[j])
					{
						bDelRequired = FALSE;
						break;
					}
				}
				if(bDelRequired)
				{
					delete m_textures[i];
					m_textures[i] = NULL;
				}
			}
			m_textures[i] = textures[i];
		}
	}
}

void CSkyBoxSceneNode::Release()
{
	int j;
	for(int i=0;i<6;i++)
		if(m_textures[i])
		{
			for(j=i+1;j<6;j++)
				if(m_textures[j] == m_textures[i])
					m_textures[j] = NULL;
			delete m_textures[i];
			m_textures[i] = NULL;
		}

	if(m_vb != NULL)
	{
		m_vb->Release();
		m_vb = NULL;
	}
}

/**
 * should never happen
 **/
aabbox3d &CSkyBoxSceneNode::GetBoundingBox()
{
	return *((aabbox3d*)0);
}

void CSkyBoxSceneNode::Render(bool isShadowPass)
{
	if(m_vb == NULL || isShadowPass)
		return;

	CDirectX9 *dx = GetDXApp();
	if(dx == NULL)
		return;

	CCameraSceneNode *cam = ISceneManager::GetManager()->GetActiveCamera();

	D3DXMATRIX matTrans;
	D3DXVECTOR3 vec = cam->GetAbsolutePosition();

	D3DXMatrixTranslation(&matTrans, vec.x, vec.y, vec.z);
	//D3DXMatrixTranslation(&matTrans, 0.f, 0.f, 0.f);
	dx->SetTransform(D3DTS_WORLD, &matTrans);

	//dx->GetDevice()->SetStreamSource(0, m_vb, D3DXGetFVFVertexSize(SkyboxFVF), sizeof(SSkyboxVertex));
	dx->GetDevice()->SetStreamSource(0, m_vb, 0, sizeof(SSkyboxVertex));
	dx->GetDevice()->SetFVF(SkyboxFVF);
	dx->SetMaterial(m_material);

	//dx->EnableZBuffer(FALSE);
	dx->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	dx->EnableLighting(FALSE);

	D3DCOLOR backupAmbient;
	dx->GetDevice()->GetRenderState(D3DRS_AMBIENT, &backupAmbient);
	dx->SetAmbientLight(D3DCOLOR_RGBA(100, 100, 100, 255));

	for(int i=0;i<6;i++)
	{
		if(m_textures[i])
		{
			dx->SetTexture(m_textures[i]);
			dx->GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, i*4, 2);
		}
	}

	dx->SetAmbientLight(backupAmbient);
	dx->EnableLighting(TRUE);
	dx->EnableZBuffer(TRUE);
	dx->GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

void CSkyBoxSceneNode::OnPreRender(bool isShadowPass)
{
	if(m_bVisible && !isShadowPass)
	{
		m_manager->RegisterNodeForRendering(this, SNG_SKYBOX);

		ISceneNode::OnPreRender(isShadowPass);
	}
}
