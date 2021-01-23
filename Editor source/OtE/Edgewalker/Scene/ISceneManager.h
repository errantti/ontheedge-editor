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

#ifndef __ISCENEMANAGER_H__
#define __ISCENEMANAGER_H__

#include "..\\DirectX9\\DirectX9.h"

enum SCENE_NODE_GROUP {
	SNG_LIGHT,
	SNG_CAMERA,
	SNG_SKYBOX,
	SNG_DEFAULT,
	SNG_ALPHA_BLENDED,
	SNG_COUNT
};

class ISceneNode;
class CLightSceneNode;
class CCameraSceneNode;
class CCameraFPSSceneNode;
class CCameraFollowSceneNode;
class CSkyBoxSceneNode;
class CTiledTerrain;
class CParticleSystemSceneNode;
class CMeshSceneNode;
class CObjectManagerSceneNode;
class CParticleEngine;

struct EMIT_PARAMS;

class ISceneManager
{
public:
	ISceneManager() {m_mgr = this;}
	virtual ~ISceneManager() {}

	static ISceneManager *GetManager() {return m_mgr;}

	virtual CDirectX9 *GetDirectX9() {return GetDXApp();}
	virtual void Clear() = 0;
	virtual ISceneNode *GetRootSceneNode() = 0;

	virtual void RenderAll(bool isShadowPass = 0) = 0;

	virtual CCameraSceneNode *GetActiveCamera() = 0;
	virtual void SetActiveCamera(CCameraSceneNode *cam) = 0;

	virtual void RegisterNodeForRendering(ISceneNode *node, SCENE_NODE_GROUP group) = 0;

	virtual void AddNodeToRemoveList(ISceneNode* node) = 0;

	virtual void UpdateAll(float frameTime) = 0;

	virtual bool IsSettingCameras() = 0;

	virtual void RemoveNonSpecialObjects() = 0;

	virtual CParticleEngine &GetParticleEngine() = 0;

	// 
	virtual CTiledTerrain *GetTiledTerrain() = 0;
	virtual CObjectManagerSceneNode *GetObjectManager() = 0;

	///////////
	virtual CCameraSceneNode *AddCameraSceneNode(ISceneNode *parent = 0, long id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 lookat = D3DXVECTOR3(0.0f, 0.0f, 0.0f)) = 0;

	virtual CCameraFPSSceneNode *AddCameraFPSSceneNode(ISceneNode *parent = NULL, 
		long id = -1, D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		float movespeed = 100.f, float rotatespeed = 0.01f) = 0;

	virtual CCameraFollowSceneNode *AddCameraFollowSceneNode(ISceneNode *parent, long id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(10.f, 10.f, 10.f)) = 0;

	virtual CLightSceneNode *AddLightSceneNode(ISceneNode *parent = 0, bool bStatic = TRUE,
		D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DCOLOR color = D3DCOLOR_RGBA(255, 255, 255, 255),
		float radius = 500.0f, long id = -1) = 0;

	virtual CMeshSceneNode *AddMeshSceneNode(CMesh *mesh, ISceneNode *parent = 0, long id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f)) = 0;

	virtual CSkyBoxSceneNode *AddSkyBox(ISceneNode *parent = 0, long id = -1,
		CTexture *top = NULL, CTexture *bottom = NULL, CTexture *front = NULL,
		CTexture *back = NULL, CTexture *left = NULL, CTexture *right = NULL) = 0;

	virtual CParticleSystemSceneNode *AddParticleSystemSceneNode(ISceneNode *parent = 0, 
		long id = -1, D3DXVECTOR3 pos = D3DXVECTOR3(0.f, 0.f, 0.f), bool autoDestruct = FALSE, float duration = 4.f) = 0;

	virtual CTiledTerrain *AddTiledTerrain(long id = -1) = 0;

	virtual CObjectManagerSceneNode *AddObjectManager(long id, DWORD width, DWORD height, int sectorSize) = 0;

protected:
	static ISceneManager *m_mgr;		// 
};

#endif // #ifndef __ISCENEMANAGER_H__
