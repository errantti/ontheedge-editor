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

#include "..\\OnTheEdge.h"
#include "FloatingText.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CObjectManager::CObjectManager() : m_terrain(NULL), m_bShowTriggers(FALSE), m_highestObjectID(100000), 
	m_enemyCount(0), m_highestEnemyID(10000), m_highestDynamicObjectID(2000), m_highestStaticObjectID(800000)
{
}

CObjectManager::~CObjectManager()
{
}

void CObjectManager::LoadDefaultTemplates()
{
	// default ship
	SShipTemplate s;
	s.FillDefaults();

	s.vt.thrust = 16000.f;
	s.boostForce = 30000.f;
	s.boostEnergyConsumption = 40.f;
	s.vt.peIDAccelerate = 3;
	s.vt.dt.onDestruction.particleEffectID = 2;
	s.vt.turnSpeed = 2.5f;

	s.vt.dt.hitPoints = 400;
	s.vt.dt.mass = 100.f;
	s.vt.dt.templateID = 1;
	strcpy(s.vt.dt.meshFilename, "meshes\\Ship.x");
	strcpy(s.vt.dt.name, "Basic ship");

	m_ships.push_back(s);

	// default enemy
	SEnemyTemplate e;
	e.FillDefaults();

	e.vt.dt.hitPoints = 400;
	e.vt.dt.mass = 100.f;
	e.vt.dt.templateID = 1;

	strcpy(e.vt.dt.meshFilename, "meshes\\E_Vanquisher1.x");
	strcpy(e.vt.dt.name, "Temporary enemy");

	e.gadgetID[0] = 1;
	e.vt.thrust = 15000.f;
	e.vt.turnSpeed = 2.f;

	m_enemies.push_back(e);
}

void CObjectManager::LoadDefaultDatabases(std::string folder)
{
	std::string file = folder + "dot.dtb";
	if(LoadDynamicObjectTemplates(file.c_str()) == FALSE)
		CPrinter::log("Failed loading default dynamic object templates", LOG_ERROR);

	file = folder + "sot.dtb";
	if(LoadStaticObjectTemplates(file.c_str()) == FALSE)
		CPrinter::log("Failed loading default static object templates", LOG_ERROR);

	file = folder + "ships.dtb";
	if(LoadShipTemplates(file.c_str()) == FALSE)
		CPrinter::log("Failed loading default ship templates", LOG_ERROR);

	file = folder + "enemies.dtb";
	if(LoadEnemyTemplates(file.c_str()) == FALSE)
		CPrinter::log("Failed loading default enemy templates", LOG_ERROR);
}

CShip *CObjectManager::DropShip(long templateID, D3DXVECTOR3 pos, long id)
{
	SShipTemplate *s = NULL;
	for(UINT i=0;i<m_ships.size();i++)
	{
		if(m_ships[i].vt.dt.templateID == templateID)
		{
			s = &m_ships[i];
			break;
		}
	}
	if(s == NULL)
		return NULL;

	ISceneManager *sm = GetEdgewalkerApp()->GetSceneManager();
	CTiledTerrain *t = sm->GetTiledTerrain();

	CShip *ship = new CShip(id);
	ship->ApplyTemplate(*s);

	if(id != -1)
	{
		ship->SetID(id);
		ship->SetTeamID(id); // Default team ID to ship ID
		if(id > m_highestObjectID)
			m_highestObjectID = id;
	}
	else
		ship->SetID(++m_highestObjectID);

	//CMesh *mesh = sm->GetMesh(s->vt.dt.meshFilename);
	CMesh *mesh = CResourceManager::GetManager().GetMesh(s->vt.dt.meshFilename);
	if(mesh == NULL)
	{
		delete ship;
		CPrinter::log("Failed loading ship mesh", LOG_ERROR);
		return NULL;
	}
	ship->SetMesh(mesh);

	pos.y = t->GetTerrainHeight(pos.x, pos.z) + ship->GetBoundingSphere().radius;

	ship->SetBeginningPosition(pos);

	m_scObManager->AddNewObject(ship);

	return ship;
}

CEnemy *CObjectManager::DropEnemy(long templateID, D3DXVECTOR3 pos, long id, CEnemyAI *ai, bool bNew)
{
	SEnemyTemplate *s = NULL;
	for(UINT i=0;i<m_enemies.size();i++)
	{
		if(m_enemies[i].vt.dt.templateID == templateID)
		{
			s = &m_enemies[i];
			break;
		}
	}
	if(s == NULL)
		return NULL;

	ISceneManager *sm = GetEdgewalkerApp()->GetSceneManager();
	CTiledTerrain *t = sm->GetTiledTerrain();

	CEnemy *enemy = new CEnemy;
	enemy->ApplyTemplate(*s);

	CMesh *mesh = CResourceManager::GetManager().GetMesh(s->vt.dt.meshFilename);
	if(mesh == NULL)
	{
		delete enemy;
		CPrinter::log("Failed loading enemy mesh", LOG_ERROR);
		return NULL;
	}
	enemy->SetMesh(mesh);
	++m_enemyCount;

	if(s->vt.dt.bDropWhenLoaded)
		pos.y = t->GetTerrainHeight(pos.x, pos.z) + enemy->GetBoundingSphere().radius;

	enemy->SetBeginningPosition(pos);
	if(ai)
		enemy->SetController(ai);
	if(id != -1)
	{
		enemy->SetID(id);
		enemy->SetTeamID(id);	// Default team ID to enemy ID
		if(id > m_highestEnemyID)
			m_highestEnemyID = id;
		if(id > m_highestObjectID)
			m_highestObjectID = id;
	}
	else if(bNew)
		enemy->SetID(GetNewEnemyID());

	if(bNew)
	{
		SObjectInstanceInfo *info = enemy->GetObjectInfo(true);
		info->particleEffectID = s->vt.dt.defaultParticleEffectID;
		info->effectPos = s->vt.dt.defaultEffectPos;
		enemy->ApplyObjectInfo(*info, false);
	}

	m_scObManager->AddNewObject(enemy);
	/*if(enemy->IsMobile() && !enemy->IsBoltedToGround())
	{
		CParticleSystemSceneNode *e = (CParticleSystemSceneNode*)(GetOTE()->GetParticleEffects()->StartEffect(enemy, 3));
		enemy->SetEngineParticleEmitter(e);
	}*/

	return enemy;
}

CDynamicObject *CObjectManager::DropDynamicObject(long templateID, D3DXVECTOR3 pos, long id, bool bNew)
{
	SDynamicObjectTemplate *s = NULL;
	for(UINT i=0;i<m_dynamicObjects.size();i++)
	{
		if(m_dynamicObjects[i].templateID == templateID)
		{
			s = &m_dynamicObjects[i];
			break;
		}
	}
	if(s == NULL)
		return NULL;

	ISceneManager *sm = GetEdgewalkerApp()->GetSceneManager();
	CTiledTerrain *t = sm->GetTiledTerrain();

	CDynamicObject *object = new CDynamicObject(NULL, -1);
	object->ApplyDynamicObjectTemplate(*s);

	CMesh *mesh = CResourceManager::GetManager().GetMesh(s->meshFilename);
	if(mesh == NULL)
	{
		delete object;
		CPrinter::log("Failed loading dynamic object mesh", LOG_ERROR);
		return NULL;
	}
	object->SetMesh(mesh);

	if(id != -1)
	{
		object->SetID(id);
		if(id > m_highestDynamicObjectID)
			m_highestDynamicObjectID = id;
		if(id > m_highestObjectID)
			m_highestObjectID = id;
	}
	else if(bNew)
		object->SetID(GetNewDynamicObjectID());

	if(s->bDropWhenLoaded || pos.y < 0.1f)
		pos.y = t->GetTerrainHeight(pos.x, pos.z) + object->GetBoundingSphere().radius;

	object->SetBeginningPosition(pos);

	if(bNew)
	{
		SObjectInstanceInfo *info = object->GetObjectInfo(true);
		info->particleEffectID = s->defaultParticleEffectID;
		info->effectPos = s->defaultEffectPos;
		object->ApplyObjectInfo(*info, false);
	}

	m_scObManager->AddNewObject(object);

	return object;
}

CStaticObject *CObjectManager::DropStaticObject(long templateID, D3DXVECTOR3 pos, long id, bool bEditor, bool bNew)
{
	SStaticObjectTemplate *s = NULL;
	for(UINT i=0;i<m_staticObjects.size();i++)
	{
		if(m_staticObjects[i].templateID == templateID)
		{
			s = &m_staticObjects[i];
			break;
		}
	}
	if(s == NULL)
		return NULL;

	ISceneManager *sm = GetEdgewalkerApp()->GetSceneManager();
	CTiledTerrain *t = sm->GetTiledTerrain();

	CStaticObject *object = new CStaticObject;
	object->ApplyStaticObjectTemplate(*s);

	object->SetID(id);

	STATIC_OBJECT_TYPE type = object->GetStaticObjectType();
	if((type != STATIC_OBJECT_GHOST_OBJECT && type != STATIC_OBJECT_PLAYER_STARTPOS) || bEditor)
	{
		CMesh *mesh = CResourceManager::GetManager().GetMesh(s->meshFilename);
		if(mesh == NULL)
		{
			delete object;
			CPrinter::log("Failed loading static object mesh", LOG_ERROR);
			return NULL;
		}
		object->SetMesh(mesh);
	}

	if(s->bDropWhenLoaded || pos.y < 0.1f)
		pos.y = t->GetTerrainHeight(pos.x, pos.z) + object->GetBoundingSphere().radius + object->GetBoundingSphere().center.y;

	object->SetBeginningPosition(pos);

	if(id != -1)
	{
		object->SetID(id);
		if(id > m_highestStaticObjectID)
			m_highestStaticObjectID = id;
		if(id > m_highestObjectID)
			m_highestObjectID = id;
	}
	else if(bNew)
	{
		object->SetID(GetNewStaticObjectID());
	}

	if(bNew)
	{
		SObjectInstanceInfo *info = object->GetObjectInfo(true);
		info->particleEffectID = s->defaultParticleEffectID;
		info->effectPos = s->defaultEffectPos;
		object->ApplyObjectInfo(*info, false);
	}

	m_scObManager->AddNewObject(object);

	return object;
}

CTrigger *CObjectManager::PlaceTrigger(STriggerInfo info)
{
	CTrigger *t = new CTrigger;
	t->ApplyTriggerInfo(info);
	m_scObManager->AddNewObject(t);
	return t;
}

bool CObjectManager::GetObjectList(vector<SObjectInstanceInfo> &objects, vector<STriggerInfo> &triggers)
{
	if(m_scObManager == NULL)
		return FALSE;

	objects.clear();
	triggers.clear();

	IGameObject *go = NULL;

	SObjectInstanceInfo o, *op;
	STriggerInfo t;
	int type;

	list<ISceneNode*>::iterator it = m_scObManager->GetChildren().begin();
	for(; it != m_scObManager->GetChildren().end(); it++)
	{
		go = (IGameObject*)(*it);
		type = go->GetType();
		if(type == ENEMY_OBJECT_ID)
		{
			op = ((CDynamicObject*)go)->GetObjectInfo(true);
			if(op != NULL)
				objects.push_back(*op);
			else
				CPrinter::logError("GetObjectList: object skipped becouse of instance info null reference");
		}
		else if(type == DYNAMIC_OBJECT_ID)
		{
			op = ((CDynamicObject*)go)->GetObjectInfo(true);
			if(op != NULL)
				objects.push_back(*op);
			else
				CPrinter::logError("GetObjectList: object skipped becouse of instance info null reference");
		}
		else if(type == STATIC_OBJECT_ID)
		{
			op = ((CStaticObject*)go)->GetObjectInfo(true);
			if(op != NULL)
				objects.push_back(*op);
			else
				CPrinter::logError("GetObjectList: object skipped becouse of instance info null reference");
		}
		else if(type == TRIGGER_OBJECT_ID)
		{
			t = ((CTrigger*)go)->GetTriggerInfo();
			triggers.push_back(t);
		}
		else
			continue;
	}

	return TRUE;
}

bool CObjectManager::SetObjects(vector<SObjectInstanceInfo> &objects, vector<STriggerInfo> &triggers, CEnemyAI *ai,
								bool bAddEnemies, bool bAddEnergyPods, bool bAddRepairPods, bool bAddLevelTeleports,
								bool bEditor)
{
	UINT i;
	for(i=0;i<triggers.size();i++)
	{
		PlaceTrigger(triggers[i]);
	}

	CEnemy *e = NULL;
	CDynamicObject *dO = NULL;
	CStaticObject *sO = NULL;

	SObjectInstanceInfo *o = NULL, *o2;
	m_enemyCount = 0;

	for(i=0;i<objects.size();i++)
	{
		o = &objects[i];
		switch(o->type)
		{
		case ENEMY_OBJECT_ID:
			if(!bAddEnemies)
				break;
			e = DropEnemy(o->templateID, o->pos, o->id, ai);
			if(e == NULL)
				break;
			e->ApplyObjectInfo(*o, false);
			o2 = e->GetObjectInfo(true);
			*o2 = *o;
			break;
		case DYNAMIC_OBJECT_ID:
			dO = DropDynamicObject(o->templateID, o->pos);
			dO->ApplyObjectInfo(*o, false);
			o2 = dO->GetObjectInfo(true);
			*o2 = *o;
			break;
		case STATIC_OBJECT_ID:
			sO = DropStaticObject(o->templateID, o->pos, -1, bEditor);
			sO->ApplyObjectInfo(*o, false);
			o2 = sO->GetObjectInfo(true);
			*o2 = *o;
			break;
		}
	}

	return TRUE;
}

void CObjectManager::RepairObjectPositions()
{
	if(m_scObManager == NULL || m_terrain == NULL)
		return;

	D3DXVECTOR3 pos;
	list<ISceneNode*> &v = m_scObManager->GetChildren();
	list<ISceneNode*>::iterator it = v.begin();
	for(;it != v.end();it++)
	{
		pos = (*it)->GetTranslation();
		pos.y = m_terrain->GetTerrainHeight(pos.x, pos.z);
		(*it)->SetTranslation(pos);
	}
}

/**
 * AddBullet
 * Adds a single bullet specified by the parameters to the object manager
 *	PARAMETERS:
 * position - position of the object from which the bullet originates
 * rotation - rotation of the object from which the bullet originates
 * originSpeed - speed of the object from which the bullet originated, added to bullet's speed
 * originRadius - source object's bounging sphere radius or the distance of the bullet's starting point
 *		from the object's position
 * lifetime - lifetime of the ammo if it doesn't hit anything
 * ammoID - ammo template ID
 * direction - dictates the direction the ammo is sent flying relative to the rotation of the ship's
 *		y-rotation if bFlyToObjectDirection is true, if not the bullet is only positioned as if it was
 *		going to fly to that direction
 * speed - ammo's initial speed
 * damageEffect - damage effect structure that defines how much and what kind of damage the ammo does on hit
 * bFlyToObjectDirection - explained above, see explanation of direction
 * teamID - the ID of the team to which the ammo belongs (it doesn't hurt those targets with the same team ID)
 * controller - controller object for "intelligent" ammos (for example homing missile)
 */
/*CDynamicObject *CObjectManager::AddBullet(D3DXVECTOR3 position, D3DXVECTOR3 rotation, float originRadius, 
		float lifetime, int ammoID, float direction, float speed, SDamageEffect damageEffect, 
		bool bFlyToObjectDirection, long teamID, IGameObjectController *controller)/**/

CAmmo *CObjectManager::AddAmmo(D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 originSpeed, float originRadius, 
		float direction, float speed, SAmmoTemplate &temp, SAmmoInstanceData &instData, bool bFlyToObjectDirection)
{
	ISceneManager *sm = GetEdgewalkerApp()->GetSceneManager();
	//CAmmoDatabase *a = GetOTE()->GetAmmoDatabase();

	if(!sm)
		return NULL;

	SAmmoTemplate &t = temp;//a->GetAmmoTemplate(ammoID);

	CAmmo *ammo = new CAmmo(-1);
	ammo->SetMesh(temp.mesh);

	ammo->ApplyTemplate(temp);
	ammo->ApplyInstanceData(instData);

	// Set ammo position
	D3DXVECTOR3 pos = position;
	D3DXVECTOR3 rot = rotation;
	D3DXVECTOR3 impulse;
	rot.y += direction;
	// Optimization
	impulse.x = sinf(rot.y);
	impulse.z = cosf(rot.y);
	pos.x += impulse.x * originRadius * 1.2f;
	pos.z += impulse.z * originRadius * 1.2f;
	ammo->SetBeginningPosition(pos);
	ammo->SetRotation(rotation);
	ammo->UpdateAbsolutePosition();

	// Attach a particle emitter object to the ammo
	if(temp.pEffect)
		GetOTE()->GetParticleEffects()->StartEffect(ammo, *temp.pEffect);

	m_scObManager->AddNewObject(ammo);

	// Set the initial speed
	if(bFlyToObjectDirection)
	{
		impulse.x = sinf(rotation.y);
		impulse.z = cosf(rotation.y);
	}
	impulse.y = 0.f;

	impulse *= speed;

	originSpeed.y = 0.f;
	ammo->SetSpeed(impulse + originSpeed);

	return ammo;
}

void CObjectManager::AddExplosionForce(D3DXVECTOR3 position, SExplosion &exp, bool bEnemy, CVessel *source, DAMAGE_TYPE damageType)
{
	if(exp.radius < 15.f || exp.damage < 15.f)
		return;

	float distSq, dist, temp;
	D3DXVECTOR3 trans, t;
	int type;
	float radius2 = exp.radius*exp.radius;
	rect<float> r(position.x-exp.radius, position.x+exp.radius, position.z-exp.radius, position.z+exp.radius);

	vector<IGameObject*> objects;
	m_scObManager->GetObjectsFromSectorsInArea(r, objects);
	
	vector<IGameObject*>::iterator it = objects.begin();
	IGameObject *go = NULL;

	// Inflict exp.damage to all the objects in the area
	// The amount of exp.damage is relavite to the distance from the center of the explosion
	for(; it != objects.end(); ++it)
	{
		go = (*it);
		trans = go->GetTranslation();
		t.x = trans.x-position.x;
		t.y = trans.y-position.y;
		t.z = trans.z-position.z;
		distSq = t.x*t.x+t.y*t.y+t.z*t.z;
		if(distSq<radius2)
		{
			type = go->GetType();
			if(type == DYNAMIC_OBJECT_ID || type == ENEMY_OBJECT_ID || type == SHIP_OBJECT_ID)
			{
				// Inflict exp.damage to the object and apply a force
				dist = sqrtfast(distSq);
				temp = (exp.radius - dist)/exp.radius;
				if(temp<0.f)
					temp=0.f;
				if(go->RequiresCollisionDetermination())
					go->AddImpulse(t * (temp * EXPLOSION_DAMAGE_FORCE_COEFFICIENT));
				((CDynamicObject*)go)->InflictDamage(temp*exp.damage, bEnemy, damageType, source);
			}
		}
	}
}

void CObjectManager::AddImpulseForce(D3DXVECTOR3 position, D3DXVECTOR3 direction, float radius, float force, float angle)
{
	if(radius < 10.f || angle < 0.1f)
		return;

	float distSq, dist, temp, realAngle;
	D3DXVECTOR3 trans, t;
	int type;
	float radius2 = radius*radius;
	rect<float> r(position.x-radius, position.x+radius, position.z-radius, position.z+radius);

	float onePerRadius = 1.f / radius;

	vector<IGameObject*> objects;
	m_scObManager->GetObjectsFromSectorsInArea(r, objects);
	
	vector<IGameObject*>::iterator it = objects.begin();
	IGameObject *go = NULL;

	// Formulate the direction vector ('direction' parameter contains direction as x,y,z rotation) of length one
	D3DXVECTOR3 dirVec = D3DXVECTOR3(sinf(direction.y), 0.f, cosf(direction.y));

	// Add an impulse force to all the objects in the area
	// The amount of force is relavite to the distance from the center of the explosion
	for(; it != objects.end(); ++it)
	{
		go = (*it);
		if(!go->RequiresCollisionDetermination())
			continue;
		trans = go->GetTranslation();
		t.x = trans.x-position.x;
		t.y = trans.y-position.y;
		t.z = trans.z-position.z;
		distSq = t.x*t.x+t.y*t.y+t.z*t.z;
		if(distSq<radius2)
		{
			type = go->GetType();
			if(type == DYNAMIC_OBJECT_ID || type == ENEMY_OBJECT_ID || type == SHIP_OBJECT_ID)
			{
				// Check the angle of the enemy relative to the y-rotation of the origin
				// The angle of two vectors is calculated with the formula
				// cos(alpha) = r.q / (|r||q|)
				// where q is the direction vector and its length is one and
				// r is the vector from the origin to the target (t)

				// r.q
				temp = t.x * dirVec.x + t.z * dirVec.z;	// No need to calculate y becouse we know it's zero
				// |r|
				dist = sqrtfast(distSq);
				realAngle = acosf(temp / dist);

				if(realAngle < angle)
				{
					// Apply force to the object and apply a force
					temp = (radius - dist) * onePerRadius;
					if(temp < 0.f)
						temp = 0.f;
					go->AddImpulse(t * (temp * force));
				}
			}
		}
	}
}

void CObjectManager::AddConeAreaDamage(D3DXVECTOR3 position, D3DXVECTOR3 direction, float radius, float damage,
						float angle, DAMAGE_TYPE damageType, bool bConstantDamage, bool bEnemy, CVessel *source)
{
	if(radius < 10.f || angle < 0.1f)
		return;

	float distSq, dist, temp, realAngle;
	D3DXVECTOR3 trans, t;
	int type;
	float radius2 = radius*radius;
	rect<float> r(position.x-radius, position.x+radius, position.z-radius, position.z+radius);

	float onePerRadius = 1.f / radius;

	vector<IGameObject*> objects;
	m_scObManager->GetObjectsFromSectorsInArea(r, objects);
	
	vector<IGameObject*>::iterator it = objects.begin();
	IGameObject *go = NULL;
	CDynamicObject *ob;

	// Formulate the direction vector ('direction' parameter contains direction as x,y,z rotation) of length one
	D3DXVECTOR3 dirVec = D3DXVECTOR3(sinf(direction.y), 0.f, cosf(direction.y));

	// Add an impulse force to all the objects in the area
	// The amount of force is relavite to the distance from the center of the explosion
	for(; it != objects.end(); ++it)
	{
		go = (*it);
		trans = go->GetTranslation();
		t.x = trans.x-position.x;
		t.y = trans.y-position.y;
		t.z = trans.z-position.z;
		distSq = t.x*t.x+t.y*t.y+t.z*t.z;
		if(distSq<radius2)
		{
			type = go->GetType();
			if(type == DYNAMIC_OBJECT_ID || type == ENEMY_OBJECT_ID || type == SHIP_OBJECT_ID)
			{
				// Check the angle of the enemy relative to the y-rotation of the origin
				// The angle of two vectors is calculated with the formula
				// cos(alpha) = r.q / (|r||q|)
				// where q is the direction vector and its length is one and
				// r is the vector from the origin to the target (t)

				// r.q
				temp = t.x * dirVec.x + t.z * dirVec.z;	// No need to calculate y becouse we know it's zero
				// |r|
				dist = sqrtfast(distSq);
				realAngle = acosf(temp / dist);

				ob = (CDynamicObject*)go;

				if(realAngle < angle)
				{
					// Apply force to the object and apply a force
					if(bConstantDamage)
						ob->InflictDamage(damage, bEnemy, damageType, source);
					else
					{
						temp = (radius - dist) * onePerRadius;
						ob->InflictDamage(damage * temp, bEnemy, damageType, source);
					}
				}
			}
		}
	}
}

bool CObjectManager::LoadDynamicObjectTemplates(const char *filename)
{
	return LoadVectorFromBinaryFile<SDynamicObjectTemplate>(m_dynamicObjects, filename, OBJECT_FILE_BEGIN_IDENTIFIER);
}

bool CObjectManager::SaveDynamicObjectTemplates(const char *filename)
{
	return SaveVectorToBinaryFile<SDynamicObjectTemplate>(m_dynamicObjects, filename, OBJECT_FILE_BEGIN_IDENTIFIER);
}

bool CObjectManager::LoadStaticObjectTemplates(const char *filename)
{
	return LoadVectorFromBinaryFile<SStaticObjectTemplate>(m_staticObjects, filename, OBJECT_FILE_BEGIN_IDENTIFIER);
}

bool CObjectManager::SaveStaticObjectTemplates(const char *filename)
{
	return SaveVectorToBinaryFile<SStaticObjectTemplate>(m_staticObjects, filename, OBJECT_FILE_BEGIN_IDENTIFIER);
}

bool CObjectManager::LoadShipTemplates(const char *filename)
{
	return LoadVectorFromBinaryFile<SShipTemplate>(m_ships, filename, OBJECT_FILE_BEGIN_IDENTIFIER);
}

bool CObjectManager::SaveShipTemplates(const char *filename)
{
	return SaveVectorToBinaryFile<SShipTemplate>(m_ships, filename, OBJECT_FILE_BEGIN_IDENTIFIER);
}

bool CObjectManager::LoadEnemyTemplates(const char *filename)
{
	return LoadVectorFromBinaryFile<SEnemyTemplate>(m_enemies, filename, OBJECT_FILE_BEGIN_IDENTIFIER);
}

bool CObjectManager::SaveEnemyTemplates(const char *filename)
{
	return SaveVectorToBinaryFile<SEnemyTemplate>(m_enemies, filename, OBJECT_FILE_BEGIN_IDENTIFIER);
}

void CObjectManager::ClearAllTemplateDatabases()
{
	m_enemies.clear();
	m_ships.clear();
	m_dynamicObjects.clear();
	m_staticObjects.clear();
}

void CObjectManager::KillObjectsBelowSurface()
{
	list<ISceneNode*> &l = m_scObManager->GetChildren();
	list<ISceneNode*>::iterator it = l.begin();
	vector<ISceneNode*> vec;
	for(; it!=l.end(); ++it)
		if((*it)->GetTranslation().y < -5.f)
			vec.push_back(*it);
	vector<ISceneNode*>::iterator vit = vec.begin();
	IGameObject* o = NULL;
	for(; vit!=vec.end(); ++vit)
	{
		o = (IGameObject*)(*vit);
		if(o->GetType() == ENEMY_OBJECT_ID || o->GetType() == SHIP_OBJECT_ID || o->GetType() == DYNAMIC_OBJECT_ID)
			((CDynamicObject*)o)->Kill();
		else
			m_scObManager->RemoveObject(o);
	}
}

long CObjectManager::GetNewStaticObjectID()
{
	// Should never be used runtime
	++m_highestStaticObjectID;
	while(m_scObManager->GetObject(m_highestStaticObjectID) != NULL)
		++m_highestStaticObjectID;

	if(m_highestStaticObjectID > m_highestObjectID)
		m_highestObjectID = m_highestStaticObjectID;

	return m_highestStaticObjectID;
}

long CObjectManager::GetNewDynamicObjectID()
{
	// Should never be used runtime
	++m_highestDynamicObjectID;
	while(m_scObManager->GetObject(m_highestDynamicObjectID) != NULL)
		++m_highestDynamicObjectID;

	if(m_highestDynamicObjectID > m_highestObjectID)
		m_highestObjectID = m_highestDynamicObjectID;

	return m_highestDynamicObjectID;
}

long CObjectManager::GetNewEnemyID()
{
	// Should never be used runtime
	++m_highestEnemyID;
	while(m_scObManager->GetObject(m_highestEnemyID) != NULL)
		++m_highestEnemyID;

	if(m_highestEnemyID > m_highestObjectID)
		m_highestObjectID = m_highestEnemyID;

	return m_highestEnemyID;
}

bool CObjectManager::ExportDynamicObjectTemplate(long id, std::string fullPath)
{
	vector<SDynamicObjectTemplate>::iterator it = m_dynamicObjects.begin();
	for(;it != m_dynamicObjects.end(); it++)
		if(it->templateID == id)
		{
			return ExportStructToFile(fullPath, *it, 1110);
		}
	return false;
}

bool CObjectManager::ImportDynamicObjectTemplate(long id, std::string fullPath)
{
	SDynamicObjectTemplate t;
	if(!ImportStructFromFile(fullPath, t, 1110))
		return false;

	if(id == -1)
		t.templateID = GetNewDynamicObjectTemplateID();
	else
	{
		t.templateID = id;
		std::vector<SDynamicObjectTemplate>::iterator it = m_dynamicObjects.begin();
		for(; it != m_dynamicObjects.end(); ++it)
			if(it->templateID == id)
			{
				*it = t;
				return true;
			}
	}
	m_dynamicObjects.push_back(t);

	return true;
}

bool CObjectManager::ExportStaticObjectTemplate(long id, std::string fullPath)
{
	vector<SStaticObjectTemplate>::iterator it = m_staticObjects.begin();
	for(;it != m_staticObjects.end(); it++)
		if(it->templateID == id)
		{
			return ExportStructToFile(fullPath, *it, 1120);
		}
	return false;
}

bool CObjectManager::ImportStaticObjectTemplate(long id, std::string fullPath)
{
	SStaticObjectTemplate t;
	if(!ImportStructFromFile(fullPath, t, 1120))
		return false;

	if(id == -1)
		t.templateID = GetNewStaticObjectTemplateID();
	else
	{
		t.templateID = id;
		std::vector<SStaticObjectTemplate>::iterator it = m_staticObjects.begin();
		for(; it != m_staticObjects.end(); ++it)
			if(it->templateID == id)
			{
				*it = t;
				return true;
			}
	}

	m_staticObjects.push_back(t);

	return true;
}

bool CObjectManager::ExportEnemyTemplate(long id, std::string fullPath)
{
	vector<SEnemyTemplate>::iterator it = m_enemies.begin();
	for(;it != m_enemies.end(); it++)
		if(it->vt.dt.templateID == id)
		{
			return ExportStructToFile(fullPath, *it, 1130);
		}
	return false;
}

bool CObjectManager::ImportEnemyTemplate(long id, std::string fullPath)
{
	SEnemyTemplate t;
	if(!ImportStructFromFile(fullPath, t, 1130))
		return false;

	if(id == -1)
		t.vt.dt.templateID = GetNewEnemyTemplateID();
	else
	{
		t.vt.dt.templateID = id;
		std::vector<SEnemyTemplate>::iterator it = m_enemies.begin();
		for(; it != m_enemies.end(); ++it)
			if(it->vt.dt.templateID == id)
			{
				*it = t;
				return true;
			}
	}

	m_enemies.push_back(t);

	return true;
}

bool CObjectManager::ExportShipTemplate(long id, std::string fullPath)
{
	vector<SShipTemplate>::iterator it = m_ships.begin();
	for(;it != m_ships.end(); it++)
		if(it->vt.dt.templateID == id)
		{
			return ExportStructToFile(fullPath, *it, 1140);
		}
	return false;
}

bool CObjectManager::ImportShipTemplate(long id, std::string fullPath)
{
	SShipTemplate t;
	if(!ImportStructFromFile(fullPath, t, 1140))
		return false;

	if(id == -1)
		t.vt.dt.templateID = GetNewShipTemplateID();
	else
	{
		t.vt.dt.templateID = id;
		std::vector<SShipTemplate>::iterator it = m_ships.begin();
		for(; it != m_ships.end(); ++it)
			if(it->vt.dt.templateID == id)
			{
				*it = t;
				return true;
			}
	}

	m_ships.push_back(t);

	return true;
}

long CObjectManager::GetNewDynamicObjectTemplateID()
{
	long highestId = -1;
	std::vector<SDynamicObjectTemplate>::iterator it = m_dynamicObjects.begin();
	for(; it != m_dynamicObjects.end(); ++it)
		if(it->templateID > highestId)
			highestId = it->templateID;
	return ++highestId;
}

long CObjectManager::GetNewStaticObjectTemplateID()
{
	long highestId = -1;
	std::vector<SStaticObjectTemplate>::iterator it = m_staticObjects.begin();
	for(; it != m_staticObjects.end(); ++it)
		if(it->templateID > highestId)
			highestId = it->templateID;
	return ++highestId;
}

long CObjectManager::GetNewEnemyTemplateID()
{
	long highestId = -1;
	std::vector<SEnemyTemplate>::iterator it = m_enemies.begin();
	for(; it != m_enemies.end(); ++it)
		if(it->vt.dt.templateID > highestId)
			highestId = it->vt.dt.templateID;
	return ++highestId;
}

long CObjectManager::GetNewShipTemplateID()
{
	long highestId = -1;
	std::vector<SShipTemplate>::iterator it = m_ships.begin();
	for(; it != m_ships.end(); ++it)
		if(it->vt.dt.templateID > highestId)
			highestId = it->vt.dt.templateID;
	return ++highestId;
}