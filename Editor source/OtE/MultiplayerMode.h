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

#ifndef __CMULTIPLAYERMODE_H__
#define __CMULTIPLAYERMODE_H__

#include <string>
#include <vector>
#include "Edgewalker\\Scene\\MeshSceneNode.h"

#define MULTIPLAYER_LEVEL_END_DELAY		3.f
#pragma warning( disable : 4786 )

enum MULTIPLAYER_MODE
{
	MULTIPLAYER_MODE_QUICKMATCH,
	MULTIPLAYER_MODE_TOURNAMENT
};

class CMultiplayerMode
{
public:
	struct SMultiplayerSettings
	{
		bool bEnergyPods, bRepairPods, bEnemies;

		void FillDefaults()
		{
			bEnergyPods = false;
			bRepairPods = false;
			bEnemies = false;
		}
	};

	CMultiplayerMode();
	~CMultiplayerMode();

	bool StartQuickMatch();

	inline SMultiplayerSettings GetSettings() {return m_settings;}
	inline void SetSettings(SMultiplayerSettings settings) {m_settings = settings;}

	void Update(float frameTime);

	inline CCameraFollowSceneNode *GetCamera() {return m_camera;}

	inline void SetDefaultCameraDirectionVector(D3DXVECTOR3 vec) {m_defaultCameraVector = vec;}
	inline void SetDefaultShipDistance(float defDist) {m_defaultDistance = defDist;}
	inline void SetMinimumShipDistance(float minDist) {m_minDistance = minDist;}
	inline void SetMaximumShipDistance(float maxDist) {m_maxDistance = maxDist;}

	void EndMatch();

	inline float GetLevelEndCounter() {return m_levelEndCounter;}

	inline int GetNumMultiplayerMaps() {return (int)m_levelFilenames.size();}

private:
	void FindMultiplayerMapFilenames();
	bool InitializeQuickMatch();

	bool StartMatch();

	bool InitCamera();
	void RemoveCamera();
	void UpdateCamera();

	vector<string> m_levelFilenames;

	/***
	 * Multiplayer settings
	 ***/
	SMultiplayerSettings m_settings;

	/***
	 * 
	 ***/
	MULTIPLAYER_MODE m_mode;
	long m_shipID[2];
	int m_gadgets[2][2];
	string m_nextLevel;

	bool m_bMatchInitialized;

	float m_defaultDistance, m_minDistance, m_maxDistance;
	D3DXVECTOR3 m_defaultCameraVector;
	CMeshSceneNode *m_cameraTarget;
	CCameraFollowSceneNode *m_camera;

	float m_levelEndCounter;
	bool m_bCountingToLevelEnd;
};

#endif // #ifndef __CMULTIPLAYERMODE_H__
