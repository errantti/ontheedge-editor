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

#include "DirectX9.h"
#include "..\\ResourceManager.h"
#include "..\\SimpleStaticProfiler.h"

string CMesh::m_textureFolder = "textures\\";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMesh::CMesh()
: m_mesh(NULL)//, m_scalingCoefficient(1.f)
{
}

CMesh::~CMesh()
{
	Release();
}

void CMesh::Release()
{
	/*DWORD i,j;
	// Make sure that if there are multiple references to the same texture
	// the following material deletion doesn't try to delete the same texture
	// multiple times
	for(i=0;i<m_dwNumMaterials;i++)
	{
		for(j=i+1;j<m_dwNumMaterials;j++)
			if(m_materials[i].m_pTexture == m_materials[j].m_pTexture)
				m_materials[j].m_pTexture = NULL;
	}
	if(m_materials)
	{
		delete [] m_materials;
		m_materials = NULL;
	}*/
	if(m_mesh)
	{
		m_mesh->Release();
		m_mesh = NULL;
	}
}

bool CMesh::LoadMesh(const char *filename, IDirect3DDevice9 *pD3DDevice)
{
	ID3DXBuffer *materials;
	D3DXMATERIAL *pMaterials = NULL;

	if(pD3DDevice==NULL || filename==NULL)
	{
		CPrinter::log("Failed loading mesh: incorrect driver or filename", LOG_ERROR);
		return FALSE;
	}

	if(m_mesh != NULL)
	{
		m_mesh->Release();
		m_mesh = NULL;
	}

	m_pD3DDevice = pD3DDevice;
	m_filename = filename;

	std::string strFilename;

	// Load the actual mesh
	//if(FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM, m_pD3DDevice, NULL, &materials, NULL, &m_dwNumMaterials, &m_mesh)))
	if(FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_MANAGED, m_pD3DDevice, NULL, &materials, NULL, &m_dwNumSubsets, &m_mesh)))
	{
		CPrinter::log("Failed loading mesh from file", LOG_ERROR);
		return FALSE;
	}

	if(m_dwNumSubsets > 1)
	{
		/*CPrinter::log("There were more than one materials in a mesh, only one material was loaded");
		CPrinter::log(filename);*/
		strFilename = "Only one of several materials was loaded from mesh ";
		strFilename += filename;
		CPrinter::logs(strFilename);
	}

	// Query for material and texture info and load the textues if need be
	pMaterials = (D3DXMATERIAL*)materials->GetBufferPointer();

	//char fname[256];
	// TODO: possible memory leak. Do I have to release material buffer?

	CResourceManager &res = CResourceManager::GetManager();

	if(pMaterials != NULL)
	{
		m_material.m_material = pMaterials[0].MatD3D;

		// Set the ambient color to the same as diffuse
		m_material.m_material.Ambient = m_material.m_material.Diffuse;

		// Load the texture
		if(pMaterials[0].pTextureFilename)
		{
			strFilename = m_textureFolder;
			strFilename += pMaterials[0].pTextureFilename;
			m_material.m_pTexture = res.GetTexture(strFilename);
		}
	}
	else
	{
		//Create default material
		m_material.SetDefaultMaterial();
		m_material.m_pTexture = NULL;
	}

	//delete [] materials;

	ComputeBoundingVolumes();

	return TRUE;
}

void CMesh::ComputeBoundingVolumes()
{
	D3DXVECTOR3 *ptr;
	if(FAILED(m_mesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&ptr)))
	{
		CPrinter::log("Failed computing mesh's bounding box: failed locking vertex buffer", LOG_ERROR);
		m_bbox = aabbox3d(0.f);
		return;
	}

	if(FAILED(D3DXComputeBoundingBox(ptr, m_mesh->GetNumVertices(), D3DXGetFVFVertexSize(m_mesh->GetFVF()), 
		&m_bbox.minCorner, &m_bbox.maxCorner)))
	{
		CPrinter::log("Failed computing mesh's bounding box", LOG_ERROR);
		m_bbox = aabbox3d(0.f);
	}

	// compute bounding sphere for the mesh at the same
	if(FAILED(D3DXComputeBoundingSphere(ptr, m_mesh->GetNumVertices(), D3DXGetFVFVertexSize(m_mesh->GetFVF()), 
		&m_bsphere.center, &m_bsphere.radius)))
	{
		CPrinter::log("Failed computing mesh's bounding sphere", LOG_ERROR);
		m_bsphere = bsphere3d();
	}

	m_mesh->UnlockVertexBuffer();
}

void CMesh::Render() // TODO: optimize
{
	//static CSimpleStaticProfiler profiler("results.txt");

	if (!ShaderModePreparation(m_material.m_pTexture))
	{
		m_pD3DDevice->SetMaterial(&m_material.m_material);
		if(m_material.m_pTexture)
			m_pD3DDevice->SetTexture(0, m_material.m_pTexture->GetTexture());
		else
			m_pD3DDevice->SetTexture(0, NULL);
	}

	//profiler.StartTiming();
	//for(DWORD i=0;i<m_dwNumSubsets;i++)
	{
		//m_mesh->DrawSubset(i);
		m_mesh->DrawSubset(0);
	}
	//profiler.StopTiming();
}

void CMesh::RenderWithCustomTexture(CTexture *tex)
{
	if (!ShaderModePreparation(tex))
	{
		m_pD3DDevice->SetMaterial(&m_material.m_material);
		m_pD3DDevice->SetTexture(0, tex->GetTexture());
	}

	/*for(DWORD i=0;i<m_dwNumSubsets;i++)
	{
		m_mesh->DrawSubset(i);
	}*/
	m_mesh->DrawSubset(0);
}

void CMesh::RenderWithoutTexture()
{
	//m_pD3DDevice->SetMaterial(&m_material.m_material);
	//for(DWORD i=0;i<m_dwNumSubsets;i++)
	//	m_mesh->DrawSubset(i);
	ShaderModePreparation(NULL);

	if(m_mesh)
		m_mesh->DrawSubset(0);
}

/*
void CMesh::SetScalingCoefficient(float coef)
{
	m_scalingCoefficient = coef;

	m_bbox.maxCorner *= coef;
	m_bsphere.radius *= coef;
}*/

bool CMesh::ShaderModePreparation(CTexture *tex)
{
	CDirectX9 *dx = (CDirectX9*)CDirectX9::GetApp();

	if (!dx->IsUsingShader())
		return false;

	ID3DXEffect *effect = dx->GetEffect();
	if (effect == NULL)
		return false;

	D3DXMATRIX mWorldView;
	D3DXMATRIX mView;

	m_pD3DDevice->GetTransform(D3DTS_VIEW, &mView);
	m_pD3DDevice->GetTransform(D3DTS_WORLD, &mWorldView);
	D3DXMatrixMultiply( &mWorldView, &mWorldView, &mView );

	effect->SetMatrix( "g_mWorldView", &mWorldView );
	// Only use diffuse color from material
	effect->SetVector( "g_vMaterial", (D3DXVECTOR4*)&m_material.m_material );

	if (tex)
		effect->SetTexture( "g_txScene", tex->GetTexture() );
	else
		effect->SetTexture("g_txScene", NULL);

	effect->CommitChanges();

	return true;
}