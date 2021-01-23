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

#ifndef __COBJECTMANAGER_H__
#define __COBJECTMANAGER_H__

#include "..\\Edgewalker\\Scene\\ObjectManagerSceneNode.h"
#include "..\\Edgewalker\\Utilities.h"
#include "..\\Controllers\\EnemyAI.h"
#include "Ship.h"
#include "StaticObject.h"
#include "Trigger.h"
#include "Ammo.h"

#define OBJECT_FILE_BEGIN_IDENTIFIER		0x3B09FAC2

#define EXPLOSION_DAMAGE_FORCE_COEFFICIENT		1000.f

class CObjectManager
{
public:
	CObjectManager();
	~CObjectManager();

	void SetObjectManagerSceneNode(CObjectManagerSceneNode *omsn) {m_scObManager = omsn;}
	void SetTerrain(CTiledTerrain *terrain) {m_terrain = terrain;}

	CShip *DropShip(long templateID, D3DXVECTOR3 pos, long id=-1);
	CEnemy *DropEnemy(long templateID, D3DXVECTOR3 pos, long id=-1, CEnemyAI *ai = NULL, bool bNew = FALSE);
	CDynamicObject *DropDynamicObject(long templateID, D3DXVECTOR3 pos, long id = -1, bool bNew = FALSE);
	CStaticObject *DropStaticObject(long templateID, D3DXVECTOR3 pos, long id = -1, bool bEditor = FALSE, bool bNew = FALSE);
	/*CDynamicObject *AddBullet(D3DXVECTOR3 position, D3DXVECTOR3 rotation, float originRadius, 
		float lifetime, int ammoID, float direction, float speed, SDamageEffect damageEffect,
		bool bFlyToObjectDirection=FALSE, long teamID=-1, IGameObjectController *controller = NULL);*/
	CAmmo *AddAmmo(D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 originSpeed, float originRadius, float direction,
		float speed, SAmmoTemplate &temp, SAmmoInstanceData &instData, bool bFlyToObjectDirection=FALSE);

	/*CShip *PlaceShip(long templateID, D3DXVECTOR3 pos);
	CEnemy *PlaceEnemy(long templateID, D3DXVECTOR3 pos);
	CDynamicObject *PlaceDynamicObject(long templateID, D3DXVECTOR3 pos);
	CStaticObject *PlaceStaticObject(long templateID, D3DXVECTOR3 pos);
	CProjectile *PlaceProjectile(long templateID, D3DXVECTOR3 pos);*/

	CTrigger *PlaceTrigger(STriggerInfo info);

	bool LoadDynamicObjectTemplates(const char *filename);
	bool SaveDynamicObjectTemplates(const char *filename);
	bool LoadStaticObjectTemplates(const char *filename);
	bool SaveStaticObjectTemplates(const char *filename);
	bool LoadShipTemplates(const char *filename);
	bool SaveShipTemplates(const char *filename);
	bool LoadEnemyTemplates(const char *filename);
	bool SaveEnemyTemplates(const char *filename);

	void LoadDefaultTemplates();
	void LoadDefaultDatabases(std::string folder);

	void ClearAllTemplateDatabases();

	void RepairObjectPositions();

	bool GetObjectList(vector<SObjectInstanceInfo> &objects, vector<STriggerInfo> &triggers);
	bool SetObjects(vector<SObjectInstanceInfo> &objects, vector<STriggerInfo> &triggers, CEnemyAI *ai = NULL,
		bool bAddEnemies = TRUE, bool bAddEnergyPods = TRUE, bool bAddRepairPods = TRUE, bool bAddLevelTeleports = TRUE, 
		bool bEditor = FALSE);

	inline vector<SShipTemplate> *GetShipTemplates() {return &m_ships;}
	inline vector<SEnemyTemplate> *GetEnemyTemplates() {return &m_enemies;}
	inline vector<SDynamicObjectTemplate> *GetDynamicObjectTemplates() {return &m_dynamicObjects;}
	inline vector<SStaticObjectTemplate> *GetStaticObjectTemplates() {return &m_staticObjects;}

	inline bool GetShowTriggers() {return m_bShowTriggers;}
	inline void SetShowTriggers(bool value) {m_bShowTriggers = value;}
	inline bool GetShowGhostObjects() {return m_bShowGhostObjects;}

	void AddExplosionForce(D3DXVECTOR3 position, SExplosion &exp, bool bEnemy = false, 
		CVessel *source = NULL, DAMAGE_TYPE damageType = DAMAGE_TYPE_PROJECTILE);
	void AddAreaImpulse(D3DXVECTOR3 position, float radius, float force, CVessel *aggroSource = NULL);
	void AddImpulseForce(D3DXVECTOR3 position, D3DXVECTOR3 direction, float radius, float force, float angle);
	void AddConeAreaDamage(D3DXVECTOR3 position, D3DXVECTOR3 direction, float radius, float damage, float angle, 
		DAMAGE_TYPE damageType, bool bConstantDamage, bool bEnemy = false, CVessel *source = NULL);

	void KillObjectsBelowSurface();

	inline int GetNumEnemies() {return m_enemyCount;}

	long GetNewStaticObjectID();
	long GetNewEnemyID();
	long GetNewDynamicObjectID();

	long GetNewStaticObjectTemplateID();
	long GetNewEnemyTemplateID();
	long GetNewDynamicObjectTemplateID();
	long GetNewShipTemplateID();

	bool ExportDynamicObjectTemplate(long id, std::string fullPath);
	bool ImportDynamicObjectTemplate(long id, std::string fullPath);

	bool ExportStaticObjectTemplate(long id, std::string fullPath);
	bool ImportStaticObjectTemplate(long id, std::string fullPath);

	bool ExportEnemyTemplate(long id, std::string fullPath);
	bool ImportEnemyTemplate(long id, std::string fullPath);

	bool ExportShipTemplate(long id, std::string fullPath);
	bool ImportShipTemplate(long id, std::string fullPath);

protected:
	CObjectManagerSceneNode *m_scObManager;

	CTiledTerrain *m_terrain;

	vector<SShipTemplate> m_ships;
	vector<SEnemyTemplate> m_enemies;
	vector<SDynamicObjectTemplate> m_dynamicObjects;
	vector<SStaticObjectTemplate> m_staticObjects;

	long m_highestEnemyID;
	long m_highestDynamicObjectID, m_highestStaticObjectID;
	long m_highestObjectID;
	int m_enemyCount;

	bool m_bShowTriggers, m_bShowGhostObjects;
};

#endif // #ifndef __COBJECTMANAGER_H__
