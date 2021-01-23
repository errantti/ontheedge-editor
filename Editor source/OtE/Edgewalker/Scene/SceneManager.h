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

#ifndef __CSCENEMANAGER_H__
#define __CSCENEMANAGER_H__

#include "ISceneNode.h"
#include "..\\ParticleSystem\\ParticleEngine.h"
#include <vector>

enum eSCENEOBJECT
{
	eSCENEOBJECT_CAMERA,
	eSCENEOBJECT_LIGHT,
	eSCENEOBJECT_MESH,
	eSCENEOBJECT_SKYBOX,
	eSCENEOBJECT_TILEDTERRAIN,
	eSCENEOBJECT_OBJECTMANAGER,
	eSCENEOBJECT_CUSTOM,
	eSCENEOBJECT_COUNT
};

class CSceneManager : public ISceneNode, public ISceneManager
{
public:
	CSceneManager();
	virtual ~CSceneManager();

	virtual void Clear();
	virtual void RemoveAll();
	virtual ISceneNode *GetRootSceneNode() {return this;}

	virtual CCameraSceneNode *GetActiveCamera() {return m_activeCamera;}
	virtual void SetActiveCamera(CCameraSceneNode *cam);

	virtual void Render(bool isShadowPass);
	virtual aabbox3d &GetBoundingBox();

	virtual void RenderAll(bool isShadowPass = 0);
	virtual void UpdateAll(float frameTime = 0.f);
	virtual void RenderShadow();

	virtual void RegisterNodeForRendering(ISceneNode *node, SCENE_NODE_GROUP group);

	virtual void AddNodeToRemoveList(ISceneNode* node) {m_removeList.push_back(node);}
	virtual void RemoveMarkedNodes();

	virtual bool IsSettingCameras() {return m_bSettingCameras;}

	CObjectManagerSceneNode *GetObjectManager() {return m_objectManager;}
	CTiledTerrain *GetTiledTerrain() {return m_tiledTerrain;}

	virtual CParticleEngine &GetParticleEngine() {return m_particleEngine;}

	///////////
	virtual CCameraSceneNode *AddCameraSceneNode(ISceneNode *parent = 0, long id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 10.0f, 0.0f),
		D3DXVECTOR3 lookat = D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	virtual CCameraFPSSceneNode *AddCameraFPSSceneNode(ISceneNode *parent = NULL, 
		long id = -1, D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		float movespeed = 100.f, float rotatespeed = 0.01f);

	virtual CCameraFollowSceneNode *AddCameraFollowSceneNode(ISceneNode *parent, long id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(10.f, 10.f, 10.f));

	virtual CLightSceneNode *AddLightSceneNode(ISceneNode *parent = 0, bool bStatic = TRUE,
		D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DCOLOR color = D3DCOLOR_RGBA(255, 255, 255, 255),
		float radius = 500.0f, long id = -1);

	virtual CMeshSceneNode *AddMeshSceneNode(CMesh *mesh, ISceneNode *parent = 0, long id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3 scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f));

	virtual CSkyBoxSceneNode *AddSkyBox(ISceneNode *parent = 0, long id = -1,
		CTexture *top = NULL, CTexture *bottom = NULL, CTexture *front = NULL,
		CTexture *back = NULL, CTexture *left = NULL, CTexture *right = NULL);

	virtual CParticleSystemSceneNode *AddParticleSystemSceneNode(ISceneNode *parent = 0, 
		long id = -1, D3DXVECTOR3 pos = D3DXVECTOR3(0.f, 0.f, 0.f), bool autoDestruct = FALSE, float duration = 4.f);

	virtual CTiledTerrain *AddTiledTerrain(long id = -1);

	virtual CObjectManagerSceneNode *AddObjectManager(long id, DWORD width, DWORD height, int sectorSize);

	// Removes all objects but terrains and object managers
	virtual void RemoveNonSpecialObjects();

	bool GetIsCulled(ISceneNode *node);

protected:
	struct SDefaultNode
	{
		ISceneNode *node;
		CTexture *texture;

		SDefaultNode(ISceneNode *n) : node(n)
		{
			if(n->GetMesh())
				texture = n->GetMesh()->GetTexture();
			else
				texture = NULL;
		}

		bool operator < (const SDefaultNode &other) const
		{
			return (texture < other.texture);
		}
	};
	CObjectManagerSceneNode *m_objectManager;
	CTiledTerrain *m_tiledTerrain;
	
	CCameraSceneNode *m_activeCamera;

	// Render queues
	vector<ISceneNode*> m_lights;
	vector<ISceneNode*> m_cams;
	vector<ISceneNode*> m_skyBoxes;
	vector<ISceneNode*> m_defaultNodes;
	vector<ISceneNode*> m_alphaBlendedNodes;

	vector<ISceneNode*> m_removeList;

	CParticleEngine m_particleEngine;	// Particle engine object for the particle rendering pipeline

	bool m_bSettingCameras;		// True before OnPreRender calls when the currently active
								// camera's OnPreRender is called

	bool m_bShadowPass;
};

#endif // #ifndef __CSCENEMANAGER_H__
