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

#include "Level.h"
#include "OnTheEdge.h"
#include <fstream>
#include <sstream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLevel::CLevel(CTiledTerrain *terrain, CObjectManager *objectManager) : m_terrain(terrain),
	m_gravity(DEFAULT_GRAVITY), m_friction(0.01f), m_density(0.08f),
	m_bSinglePlayerMap(TRUE), m_lightDirection(D3DXVECTOR3(0.f, -1.f, 0.f)), 
	m_defaultDirectory("levels"), m_lightColor(D3DCOLOR_RGBA(150, 150, 150, 255)),
	m_om(objectManager), m_levelSkybox(NULL), m_levelLight(NULL), m_bShowPlayerShips(TRUE),
	m_bShowStartingPositions(FALSE), m_scnManager(NULL), m_bMultiplayer(FALSE), m_bLevelLoaded(FALSE)
{
	SObjectInstanceInfo p;
	::memset(&p, 0, sizeof(SObjectInstanceInfo));
	p.pos = D3DXVECTOR3(100.f, 7.f, 100.f);
	int i;
	for(i=0;i<2;i++)
	{
		m_playerPositions[i] = p;
		m_playerMesh[i] = NULL;
		m_playerObjects[i] = NULL;
	}
	for(i=0;i<3;++i)
		m_music[i] = NULL;
}

CLevel::~CLevel()
{
	RemoveDialogues();
	RemoveTerminals();
	RemoveWaypointSets();
}

void CLevel::RemoveDialogues()
{
	vector<CDialogue*>::iterator it = m_dialogues.begin();
	for(; it != m_dialogues.end(); ++it)
		if((*it) != NULL)
			delete (*it);
	m_dialogues.clear();
}

void CLevel::RemoveTerminals()
{
	vector<CTerminal*>::iterator it = m_terminals.begin();
	for(; it != m_terminals.end(); ++it)
		if((*it) != NULL)
			delete (*it);
	m_terminals.clear();
}

void CLevel::RemoveWaypointSets()
{
	vector<CWayPointSet*>::iterator it = m_waypointSets.begin();
	for(; it != m_waypointSets.end(); ++it)
		if((*it) != NULL)
			delete (*it);
	m_waypointSets.clear();
}

bool CLevel::LoadDefaultLevel()
{
	// Load tileset
	if(CTileManager::GetManager()->LoadTileset("data\\defaulttileset.dtb") == FALSE)
	{
		CPrinter::logError("Failed loading default level: failed loading tileset");
		return FALSE;
	}

	STile tiles[16*16] = {
		{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 3}, {1, 1, 3}, {0, 0, 1}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {3, 1, 3}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {3, 1, 3}, {3, 1, 2}, {3, 1, 2}, {3, 1, 2}, {2, 1, 3}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 3}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {0, 1, 4}, {0, 1, 1}, {3, 2, 3}, {2, 2, 3}, {2, 1, 2}, {0, 0, 1}, {3, 1, 3}, {2, 1, 3}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {0, 1, 3}, {1, 1, 2}, {0, 2, 3}, {1, 2, 3}, {2, 1, 2}, {0, 0, 1}, {0, 1, 2}, {2, 1, 2}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 3}, {1, 1, 2}, {1, 1, 3}, {0, 0, 1}, {0, 1, 2}, {2, 1, 2}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 2}, {2, 1, 4}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 0}, 
        {0, 0, 0}, {3, 1, 2}, {3, 1, 4}, {2, 1, 3}, {0, 0, 1}, {0, 0, 0}, {0, 0, 1}, {3, 1, 3}, {0, 1, 1}, {2, 1, 2}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
        {0, 0, 0}, {1, 1, 2}, {0, 1, 1}, {2, 1, 2}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 1, 4}, {0, 1, 1}, {0, 1, 1}, {2, 1, 3}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {3, 1, 3}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 0}, {0, 1, 2}, {2, 1, 2}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 1, 2}, {3, 2, 3}, {3, 2, 4}, {2, 2, 3}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 3}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 0}, {0, 1, 2}, {2, 1, 1}, {3, 1, 2}, {3, 1, 2}, {3, 1, 2}, {0, 1, 1}, {0, 2, 3}, {0, 2, 1}, {2, 2, 2}, {2, 1, 3}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 0}, {0, 1, 2}, {2, 1, 1}, {1, 1, 2}, {1, 1, 2}, {1, 1, 2}, {0, 1, 1}, {0, 1, 1}, {0, 2, 3}, {1, 2, 3}, {0, 1, 1}, {2, 1, 3}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, 
		{0, 0, 0}, {3, 1, 2}, {0, 1, 1}, {2, 1, 2}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 1, 3}, {0, 1, 1}, {0, 1, 1}, {1, 1, 2}, {1, 1, 2}, {1, 1, 3}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, 
		{0, 0, 0}, {1, 1, 2}, {1, 1, 4}, {1, 1, 3}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 1, 3}, {1, 1, 3}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
        };/**/
	/*STile tiles[16*16] = {
		{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 17}, {1, 1, 17}, {0, 0, 1}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {3, 1, 17}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {3, 1, 17}, {3, 1, 16}, {3, 1, 16}, {3, 1, 16}, {2, 1, 17}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 17}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {0, 1, 14}, {0, 1, 1}, {3, 2, 17}, {2, 2, 17}, {2, 1, 16}, {0, 0, 1}, {3, 1, 17}, {2, 1, 17}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {0, 1, 17}, {1, 1, 16}, {0, 2, 17}, {1, 2, 17}, {2, 1, 16}, {0, 0, 1}, {0, 1, 16}, {2, 1, 16}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 17}, {1, 1, 16}, {1, 1, 17}, {0, 0, 1}, {0, 1, 16}, {2, 1, 16}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
        {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 16}, {2, 1, 14}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 0}, 
        {0, 0, 0}, {3, 1, 16}, {3, 1, 14}, {2, 1, 17}, {0, 0, 1}, {0, 0, 0}, {0, 0, 1}, {3, 1, 17}, {0, 1, 1}, {2, 1, 16}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
        {0, 0, 0}, {1, 1, 16}, {0, 1, 1}, {2, 1, 16}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 1, 14}, {0, 1, 1}, {0, 1, 1}, {2, 1, 17}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {3, 1, 17}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 0}, {0, 1, 16}, {2, 1, 16}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 1, 16}, {3, 2, 17}, {3, 2, 14}, {2, 2, 17}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 17}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 0}, {0, 1, 16}, {2, 1, 1}, {3, 1, 16}, {3, 1, 16}, {3, 1, 16}, {0, 1, 1}, {0, 2, 17}, {0, 2, 1}, {2, 2, 16}, {2, 1, 17}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 0}, {0, 1, 16}, {2, 1, 1}, {1, 1, 16}, {1, 1, 16}, {1, 1, 16}, {0, 1, 1}, {0, 1, 1}, {0, 2, 17}, {1, 2, 17}, {0, 1, 1}, {2, 1, 17}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, 
		{0, 0, 0}, {3, 1, 16}, {0, 1, 1}, {2, 1, 16}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 1, 17}, {0, 1, 1}, {0, 1, 1}, {1, 1, 16}, {1, 1, 16}, {1, 1, 17}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}, 
		{0, 0, 0}, {1, 1, 16}, {1, 1, 14}, {1, 1, 17}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 1, 17}, {1, 1, 17}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}, 
		{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
        };/**/
	POINT d;
	d.x = d.y = 16;

	SLevelFileHeader h;
	h.FillDefaults();

	ApplyHeader(h);

	m_bLevelLoaded = TRUE;
	m_name = "Default level";
	m_filename = "";

	m_dialogues.clear();
	m_waypointSets.clear();

	int i;
	for(i=0;i<3;++i)
		m_music[i] = NULL;

	m_playerPositions[0].pos = D3DXVECTOR3(50.f, 50.f, 50.f);

	if(!m_terrain->Create(tiles, d))
		return false;

	// Remove all objects from the current level and resive the object manager grid
	GetOTE()->GetObjectManagerSceneNode()->SetYLimits(-10.f, 225.f);
	GetOTE()->GetObjectManagerSceneNode()->Create((DWORD)(16.f*TILE_DIMENSION), (DWORD)(16.f*TILE_DIMENSION), 100);

	return true;
}

bool CLevel::NewLevel(unsigned int x, unsigned int z, unsigned int height, const char *tilesetFilename, bool msgBoxReport)
{
	// First, load the tileset
	CTileManager *manager = CTileManager::GetManager();
	if(manager == NULL)
		return false;

	if(manager->LoadTileset(tilesetFilename) == FALSE)
	{
		CPrinter::logError("Failed making a new level: failed loading selected tileset");
		if(msgBoxReport)
			::MessageBox(NULL, "Failed making a new level: failed loading selected tileset", "Error!", MB_OK);
		return FALSE;
	}

	m_tilesetFilename = tilesetFilename;

	// Some hard coded map limits...
	if(x<8)
		x = 8;
	if(x>512)
		x = 512;
	if(z<8)
		z = 8;
	if(z>512)
		z = 512;

	if(height < 0)
		height = 1;
	if(height > 7)
		height = 7;

	// NOTE! Only square maps are supported at the moment
	z = x;

	POINT d;
	d.x = x;
	d.y = z;

	STile *tiles = new STile[x*x];
	if(tiles == NULL)
	{
		if(msgBoxReport)
			::MessageBox(NULL, "Failed to create new level: out of memory", "Error!", MB_OK);
		return FALSE;
	}

	STileInfo *spaceTemplate = manager->GetTileTemplate(TILE_TYPE_EMPTY, 0);
	STileInfo *platformTemplate = manager->GetTileTemplate(TILE_TYPE_PLATFORM, 0);

	if(spaceTemplate == NULL || platformTemplate == NULL)
	{
		delete [] tiles;
		if(msgBoxReport)
			::MessageBox(NULL, "Failed to create new level: space or platform tile missing", "Error!", MB_OK);
		CPrinter::logError("Failed to create new level: space or platform tile missing");
		return false;
	}

	STile platform;
	platform.heightLevel = height;
	platform.tileID = platformTemplate->id;
	platform.yRotation = 0;
	STile space;
	space.heightLevel = -1;
	space.tileID = spaceTemplate->id;
	space.yRotation = 0;

	int ix, iz;
	long index=0;
	// Set platforms
	for(iz=1;iz<z-1;iz++)
	{
		index = iz*x;
		tiles[index] = space;
		tiles[index+x-1] = space;
		for(ix=1;ix<x-1;ix++)
			tiles[index+ix] = platform;
	}

	// Set bordering space
	index = x*(z-1);
	for(ix=0;ix<x;ix++)
	{
		tiles[ix] = space;
		tiles[index+ix] = space;
	}

	if(!m_terrain->Create(tiles, d))
	{
		delete [] tiles;
		if(msgBoxReport)
			::MessageBox(NULL, "Failed creating new level: couldn't create terrain", "Error!", MB_OK);
		return false;
	}

	delete [] tiles;

	for(int i=0;i<3;++i)
		m_music[i] = NULL;

	SLevelFileHeader h;
	h.FillDefaults();

	ApplyHeader(h);

	m_bLevelLoaded = TRUE;
	m_name = "Untitled1";
	m_filename = "";

	m_dialogues.clear();
	m_waypointSets.clear();

	// Remove all objects from the current level and resize the object manager grid
	GetOTE()->GetObjectManagerSceneNode()->SetYLimits(-10.f, 225.f);
	GetOTE()->GetObjectManagerSceneNode()->Create((DWORD)((float)x*TILE_DIMENSION), (DWORD)((float)z*TILE_DIMENSION), 100);

	return TRUE;
}

void CLevel::InitiateDialogue(int dialogueID)
{
	for(int i=0;i<m_dialogues.size();i++)
		if(m_dialogues[i]->GetID() == dialogueID)
		{
			m_dialogues[i]->OpenDialogue();
			return;
		}
}

CWayPointSet *CLevel::CreateNewWaypointSet(std::string name)
{
	CWayPointSet *wps = new CWayPointSet();
	wps->SetID(GetNewWaypointSetID());
	wps->SetName(name);
	m_waypointSets.push_back(wps);
	return wps;
}

CWayPointSet *CLevel::GetWayPointSet(int id)
{
	vector<CWayPointSet*>::iterator it = m_waypointSets.begin();
	for(; it != m_waypointSets.end(); ++it)
		if((*it)->GetID() == id)
			return (*it);
	return NULL;
}

void CLevel::OpenTerminal(long terminalID)
{
	vector<CTerminal*>::iterator it = m_terminals.begin();
	for(; it != m_terminals.end(); ++it)
		if((*it)->GetID() == terminalID)
		{
			if((*it)->IsFunctional())
			{
				(*it)->OpenTerminal();
			}
			else
				GetOTE()->AddScreenMessage((*it)->GetDisfunctionalMessage());
			return;
		}
}

CTerminal *CLevel::GetTerminal(long terminalID)
{
	vector<CTerminal*>::iterator it = m_terminals.begin();
	for(; it != m_terminals.end(); ++it)
		if((*it)->GetID() == terminalID)
			return (*it);
	return NULL;
}

void CLevel::ApplyPhysicalValues()
{
	CPhysicsEngine *pse = GetEdgewalkerApp()->GetPhysicsEngine();
	pse->SetGravity(m_gravity);
	pse->SetDensity(m_density);
	pse->SetFriction(m_friction);
}

bool CLevel::SaveLevel(const char *filename)
{
	if(filename == NULL && m_filename == "")
		return FALSE;

	if(m_bLevelLoaded == FALSE)
		return FALSE;

	string strFilename;

	if(filename)
		m_filename = filename;
	strFilename = m_defaultDirectory + "\\" + m_filename;

	/**
	 * Save level specific script file
	 **/
	std::string scriptFile = m_defaultDirectory + "\\";
	scriptFile += m_filename;
	int strl = scriptFile.size();
	scriptFile[strl-3] = 'o';
	scriptFile[strl-2] = 'l';
	scriptFile[strl-1] = 's';

	if(CScriptManager::GetManager().GetScriptEngine().SaveScriptCollection(scriptFile, false, true))
	{
		CPrinter::log("Succesfully saved level specific scripts");
	}
	else
		CPrinter::logError("Failed saving level specific scripts");

	/**
	 * Fill the level header structure
	 **/
	SLevelFileHeader h = GetLevelFileHeader();

	/**
	 * Get map data
	 **/

	POINT dimensions;
	STile *tiles = m_terrain->GetTileInfoArray(dimensions);

	if(tiles == NULL)
		return false;

	h.mapSize = dimensions;

	h.dwMapSize = sizeof(STile) * dimensions.x * dimensions.y;

	/**
	 * Get object data
	 **/

	vector<SObjectInstanceInfo> objects;
	vector<STriggerInfo> triggers;

	if(m_om->GetObjectList(objects, triggers) == FALSE)
	{
		delete [] tiles;
		CPrinter::log("Failed getting object list", LOG_ERROR);
		::MessageBox(NULL, "Failed getting object list", "OtE Editor error!", MB_OK);
		return FALSE;
	}

	h.numObjects = objects.size();
	h.numTriggers = triggers.size();
	h.dwObjectsSize = sizeof(SObjectInstanceInfo) * objects.size() + sizeof(STriggerInfo) * triggers.size();

	/**
	 * Get dialogue data
	 **/

	// TODO! Get dialogue data
	h.numDialogues = m_dialogues.size();

	/**
	 * Get waypoint set data
	 **/
	h.dwWaypointSetSize = 0;
	h.numWaypointSets = m_waypointSets.size();

	/**
	 * Get terminal data
	 **/
	h.dwTerminalsSize = 0;
	h.numTerminals = m_terminals.size();

	/**
	 * Write the level file
	 **/

	fstream f;
	f.open(strFilename.c_str(), ios::out | ios::binary);
	if(!f.is_open())
	{
		delete [] tiles;
		CPrinter::log("Failed opening level file for writing", LOG_ERROR);
		::MessageBox(NULL, "Failed opening level file for writing", "OtE Editor error!", MB_OK);
		return FALSE;
	}

	// Write the level file header
	f.write((const char*)&h, sizeof(SLevelFileHeader));

	// Write map data
	f.write((const char*)tiles, sizeof(STile) * dimensions.x * dimensions.y);
	delete [] tiles;

	// Write object data
	if(objects.size() > 0)
		f.write((const char*)&objects[0], sizeof(SObjectInstanceInfo) * objects.size());
	if(triggers.size() > 0)
		f.write((const char*)&triggers[0], sizeof(STriggerInfo) * triggers.size());

	// Write dialogue data
	vector<CDialogue*>::iterator dIt = m_dialogues.begin();
	for(; dIt != m_dialogues.end(); ++dIt)
	{
		(*dIt)->WriteDialogueInfoIntoStream(f);
	}

	// Write terminal data
	vector<CTerminal*>::iterator teIt = m_terminals.begin();
	for(; teIt != m_terminals.end(); ++teIt)
		(*teIt)->WriteTerminalDataIntoStream(f);

	// Write waypoint set data
	int i,j, count, id;
	char szTemp[32];
	vector<D3DXVECTOR3> *v = NULL;
	D3DXVECTOR3 waypoint;

	for(i=0;i<m_waypointSets.size();i++)
	{
		v = m_waypointSets[i]->GetWaypoints();
		count = v->size();
		id = m_waypointSets[i]->GetID();
		if(m_waypointSets[i]->GetName().size() > 31)
		{
			strncpy(szTemp, m_waypointSets[i]->GetName().c_str(), 31);
			szTemp[31] = '\0';
		}
		else
			strcpy(szTemp, m_waypointSets[i]->GetName().c_str());

		f.write((const char*)&count, sizeof(int));
		f.write((const char*)&id, sizeof(int));
		f.write((const char*)szTemp, sizeof(char)*32);
		for(j=0;j<count;j++)
		{
			waypoint = (*v)[j];
			f.write((const char*)&waypoint, sizeof(D3DXVECTOR3));
		}
	}

	// Write script names and filenames (in case the level specific script collection
	// gets corrupted and the scripts need to be quickly reloaded from text files)
	WriteScriptFilenames(f);

	// Write assistent messages
	vector<pair<int,string> >::iterator it = m_assistentMessages.begin();
	char buffer512[512];
	for(; it != m_assistentMessages.end(); ++it)
	{
		strncpy(buffer512, it->second.c_str(), 511);
		id = it->first;
		f.write((const char*)&id, sizeof(int));
		f.write(buffer512, 512);
	}

	f.close();

	return TRUE;
}

bool CLevel::LoadLevel(const char *filename, bool bAddPlayer, bool bAddEnemies, bool bAddEnergyPods,
		bool bAddRepairPods, bool bAddLevelTeleports, bool bEditor)
{
	if(filename == NULL)
	{
		CPrinter::log("Erraneous filename", LOG_ERROR);
		return FALSE;
	}

	if(m_bLevelLoaded)
	{
		CloseLevel();
	}

	int i;
	string strFilename = m_defaultDirectory + "\\" + filename;

	fstream f;
	f.open(strFilename.c_str(), ios::in | ios::binary);
	if(!f.is_open())
	{
		CPrinter::log("Failed opening level file. File is being used by another program or doesn't exist.", LOG_ERROR);
		::MessageBox(NULL, "Failed opening level file. File is being used by another program or doesn't exist.", "OtE Editor error!", MB_OK);
		return FALSE;
	}

	SLevelFileHeader h;

	// Read the header
	f.read((char*)&h, sizeof(SLevelFileHeader));

	if(h.levelFileBeginIdentifier != LEVEL_FILE_BEGIN_IDENTIFIER)
	{
		CPrinter::log("Failed opening level file: level file begin identifier mismatch", LOG_ERROR);
		::MessageBox(NULL, "Failed opening level file: level file begin identifier mismatch", "OtE Editor error!", MB_OK);
		f.close();
		return FALSE;
	}

	// Read the tile info
	STile *tiles = new STile[h.mapSize.x * h.mapSize.y];
	if(tiles == NULL)
	{
		CPrinter::log("Internal error: run out of memory", LOG_ERROR);
		::MessageBox(NULL, "Internal error: run out of memory", "OtE Editor error!", MB_OK);
		f.close();
		return FALSE;
	}

	f.read((char*)tiles, h.dwMapSize);

	// Read object info
	vector<SObjectInstanceInfo> objects;
	vector<STriggerInfo> triggers;

	SObjectInstanceInfo o;
	STriggerInfo t;

	if(h.numObjects > 2000 || h.numTriggers > 2000)
	{
		CPrinter::logError("Level file corrupted or object/trigger limit exceeded");
		delete [] tiles;
		return FALSE;
	}

	for(i=0;i<h.numObjects;i++)
	{
		f.read((char*)&o, sizeof(SObjectInstanceInfo));
		objects.push_back(o);
	}
	for(i=0;i<h.numTriggers;i++)
	{
		f.read((char*)&t, sizeof(STriggerInfo));
		triggers.push_back(t);
	}

	// Read dialogue info
	int j;
	int count,id;

	//CDialogue::SComment comment;
	//CDialogue dialogue;
	char szTemp[32];

	CDialogue *dl;
	for(i=0;i<h.numDialogues;i++)
	{
		dl = new CDialogue();
		if(dl->ReadDialogueInfoFromStream(f) == false)
		{
			delete dl;
			CPrinter::logError("Failed reading level file: error reading dialogue data");
			return false;
		}
		m_dialogues.push_back(dl);
	}

	CTerminal *terminal;
	for(i=0;i<h.numTerminals;++i)
	{
		terminal = new CTerminal();
		// Don't update commands yet because level script haven't been loaded
		if(terminal->ReadTerminalDataFromStream(f, false) == false)
		{
			delete terminal;
			CPrinter::logError("Failed reading terminal file: error reading terminal data");
			return false;
		}
		m_terminals.push_back(terminal);
	}

	// Read waypoint set data
	D3DXVECTOR3 waypoint;
	CWayPointSet *set;

	for(i=0;i<h.numWaypointSets;i++)
	{
		// Clear previous information
		//set.GetWaypoints()->clear();
		set = new CWayPointSet();

		f.read((char*)&count, sizeof(int));
		f.read((char*)&id, sizeof(int));
		f.read((char*)szTemp, sizeof(char)*32);

		set->SetName(szTemp);
		set->SetID(id);
		for(j=0;j<count;j++)
		{
			f.read((char*)&waypoint, sizeof(D3DXVECTOR3));
			set->GetWaypoints()->push_back(waypoint);
		}

		m_waypointSets.push_back(set);
	}

	SkipScriptFilenames(f);

	// Read assistent messages
	char buffer512[512];
	m_assistentMessages.clear();
	for(i=0;i<h.numAssistantMessages;++i)
	{
		f.read((char*)&id, sizeof(int));
		f.read(buffer512, 512);
		m_assistentMessages.push_back(pair<int,string>(id, buffer512));
	}

	f.close();

	/**
	 * Read level specific script file
	 **/
	std::string scriptFile = m_defaultDirectory + "\\";
	scriptFile += filename;
	int strl = scriptFile.size();
	scriptFile[strl-3] = 'o';
	scriptFile[strl-2] = 'l';
	scriptFile[strl-1] = 's';

	CScriptManager::GetManager().GetScriptEngine().UnloadTemporaryScripts();
	if(CScriptManager::GetManager().GetScriptEngine().LoadScriptCollection(scriptFile, false, true, true))
	{
		CPrinter::log("Succesfully loaded level specific scripts");
	}
	else
		CPrinter::logError("Failed loading level specific scripts");

	/**
	 * Process read data
	 **/

	// Load tileset
	if(CTileManager::GetManager()->LoadTileset(h.tilesetFilename) == FALSE)
	{
		delete [] tiles;
		CPrinter::logError("Failed loading level: failed loading tileset");
		return FALSE;
	}

	m_tilesetFilename = h.tilesetFilename;

	// Create terrain
	if(m_terrain->Create(tiles, h.mapSize) == FALSE)
	{
		delete [] tiles;
		CPrinter::log("Failed loading level: failed creating terrain", LOG_ERROR);
		//::MessageBox(NULL, "Failed loading level: failed creating terrain", "OtE Editor error!", MB_OK);
		return FALSE;
	}

	delete [] tiles;

	// Apply header info
	ApplyHeader(h);
	m_bLevelLoaded = TRUE;
	m_filename = filename;
	
	// Remove all objects from the current level and resize the object manager grid
	GetOTE()->GetObjectManagerSceneNode()->SetYLimits(-10.f, 225.f);
	GetOTE()->GetObjectManagerSceneNode()->Create(h.mapSize.x*TILE_DIMENSION, h.mapSize.y*TILE_DIMENSION, 100);

	if(m_om->SetObjects(objects, triggers, GetOTE()->GetEnemyAI(), bAddEnemies, 
		bAddEnergyPods, bAddRepairPods, bAddLevelTeleports, bEditor) == FALSE)
	{
		CPrinter::log("Failed loading level: failed creating objects", LOG_ERROR);
		::MessageBox(NULL, "Failed loading level: failed creating objects", "OtE Editor error!", MB_OK);
		return FALSE;
	}
	m_enemyCount = m_om->GetNumEnemies();

	// Update terminal commands
	vector<CTerminal*>::iterator termIt = m_terminals.begin();
	for(; termIt != m_terminals.end(); ++termIt)
		(*termIt)->UpdateCommands();

	// Update dialog on close scripts if needed
	vector<CDialogue*>::iterator diagIt = m_dialogues.begin();
	for(; diagIt != m_dialogues.end(); ++diagIt)
		(*diagIt)->UpdateOnEndScriptReference();

	// Load background music
	for(i=0;i<3;++i)
	{
		CSoundManager *m = GetOTE()->GetSoundManager();
		m_music[i] = m->GetSound(h.musicID[i]);
	}

	PlayRandomBackgroundMusic();

	return TRUE;
}

void CLevel::WriteScriptFilenames(fstream &f)
{
	vector<CScript*> &v = CScriptManager::GetManager().GetScriptEngine().GetCurrentScriptCollection();
	DWORD count = 0;
	vector<CScript*>::iterator it = v.begin();
	for(; it != v.end(); ++it)
		if((*it)->IsTemporary())
			++count;
	f.write((const char*)&count, sizeof(DWORD));

	char buffer[SCRIPT_FILENAME_LENGTH];
	for(it = v.begin(); it != v.end(); ++it)
		if((*it)->IsTemporary())
		{
			strncpy(buffer, (*it)->GetFilename().c_str(), SCRIPT_FILENAME_LENGTH-1);
			f.write(buffer, SCRIPT_FILENAME_LENGTH);
			strncpy(buffer, (*it)->GetName().c_str(), SCRIPT_NAME_LENGTH-1);
			f.write(buffer, SCRIPT_NAME_LENGTH);
		}
}

bool CLevel::SkipScriptFilenames(fstream &f)
{
	DWORD count;
	f.read((char*)&count, sizeof(DWORD));
	if(count > 100)
	{
		CPrinter::logError("Failed reading script filename data (scriptcount over bounds), level file may be corrupt");
		return false;
	}
	f.ignore(count * sizeof(char) * (SCRIPT_FILENAME_LENGTH + SCRIPT_NAME_LENGTH));

	return true;
}

bool CLevel::ApplyHeader(SLevelFileHeader &h)
{
	int i;

	m_bSinglePlayerMap = h.bSinglePlayerMap;
	m_density = h.density;
	m_description = h.description;
	m_friction = h.friction;
	m_gravity = h.gravity;
	m_lightColor = h.lightColor;
	m_lightDirection = h.lightDirection;
	m_ambientColor = h.ambientColor;
	m_name = h.name;
	for(i=0;i<2;i++)
		m_playerPositions[i] = h.player[i];
	for(i=0;i<6;i++)
		m_skyboxTextures[i] = h.skyboxTexture[i];

	m_repairPodRegeneration = h.repairPodRegeneration;
	m_energyPodRegeneration = h.energyPodRegeneration;

	if(m_terrain)
		m_terrain->SetVisible(TRUE);
	for(i=0;i<2;i++)
		if(m_playerMesh[i])
			m_playerMesh[i]->SetVisible(TRUE);

	for(i=0;i<3;++i)
		m_musicID[i] = h.musicID[i];
	m_bLoopMusic = h.bLoopMusic;
	m_bHasBossMusic = h.bHasBossMusicTheme;

	m_cameraParam = h.cameraParam;
	m_cameraType = h.cameraType;
	m_bForceLevelCamera = h.bForceLevelCamera;

	ApplyPhysicalValues();

	SetLevelLighting();
	SetLevelSkybox();
	//SetPlayerMeshes();

	return TRUE;
}

CLevel::SLevelFileHeader CLevel::GetLevelFileHeader()
{
	int i;
	SLevelFileHeader h;
	h.FillDefaults();

	h.bSinglePlayerMap = m_bSinglePlayerMap;
	h.density = m_density;

	strncpy(h.tilesetFilename, m_tilesetFilename.c_str(), 127);

	if(m_description.size() > 1023)
		strncpy(h.description, m_description.c_str(), 1023);
	else
		strcpy(h.description, m_description.c_str());

	if(m_name.size() > 31)
		strncpy(h.name, m_name.c_str(), 31);
	else
		strcpy(h.name, m_name.c_str());

	for(i=0;i<6;i++)
	{
		if(m_skyboxTextures[i].size() < 128)
			strcpy(h.skyboxTexture[i], m_skyboxTextures[i].c_str());
	}
	h.friction = m_friction;
	h.gravity = m_gravity;
	h.levelFileBeginIdentifier = LEVEL_FILE_BEGIN_IDENTIFIER;
	h.lightColor = m_lightColor;
	h.lightDirection = m_lightDirection;
	h.ambientColor = m_ambientColor;

	h.energyPodRegeneration = m_energyPodRegeneration;
	h.repairPodRegeneration = m_repairPodRegeneration;

	for(i=0;i<2;i++)
		h.player[i] = m_playerPositions[i];

	for(i=0;i<3;++i)
		h.musicID[i] = m_musicID[i];
	h.bHasBossMusicTheme = m_bHasBossMusic;
	h.bLoopMusic = m_bLoopMusic;

	h.cameraParam = m_cameraParam;
	h.cameraType = m_cameraType;
	h.bForceLevelCamera = m_bForceLevelCamera;

	h.mapSize = m_terrain->GetMapSize();
	h.numAssistantMessages = (int)m_assistentMessages.size();

	return h;
}

/**
 * Sets level lighting (directional light). If directional
 * light object hasn't already been added to the scene this
 * function does it
 **/
void CLevel::SetLevelLighting()
{
	if(m_levelLight == NULL)
	{
		ISceneManager *m = ISceneManager::GetManager();
		m_levelLight = m->AddLightSceneNode(NULL, TRUE);
	}
	if(m_levelLight == NULL)
		return;

	D3DLIGHT9 l = GetEdgewalkerApp()->SetupDirectionalLight(m_lightColor, m_lightDirection);

	// Shader can use the direction
	D3DXVec3Normalize(&m_lightDirection, &m_lightDirection);
	m_levelLight->SetRotation(m_lightDirection);

	// Calculate light position based on current tilemap and light's direction
	vector<CTile*> *tiles = m_terrain->GetTileArray();
	POINT mapsize = m_terrain->GetMapSize();
	long size = mapsize.x * mapsize.y;
	int totalHeight = 0;
	int maxHeight = 0;
	int h; 

	vector<CTile*>::iterator it = tiles->begin();
	for(; it != tiles->end(); ++it)
	{
		h = (*it)->GetHeightLevel();
		if(h > maxHeight)
			maxHeight = h;
		totalHeight += h;
	}

	float avgHeight = (float)totalHeight / (float)size * TILE_LEVEL_HEIGHT;
	D3DXVECTOR3 pos(m_terrain->GetWidth() * 0.5f, avgHeight, m_terrain->GetDepth() * 0.5f);
	pos -= m_terrain->GetWidth() * m_lightDirection;
	m_levelLight->SetTranslation(pos);

	m_levelLight->SetLight(l);
	m_levelLight->Enable(TRUE, GetDXApp()->GetDevice());

	// Set ambient light
	GetEdgewalkerApp()->SetAmbientLight(m_ambientColor);
}

/**
 * Sets level skybox and adds skybox object to scene if
 * there isn't one already
 **/
void CLevel::SetLevelSkybox()
{
	if(m_levelSkybox == NULL)
	{
		ISceneManager *m = ISceneManager::GetManager();
		m_levelSkybox = m->AddSkyBox(NULL);
	}
	if(m_levelSkybox == NULL)
		return;

	CTexture *textures[6];
	int j;
	bool bSet = FALSE;
	for(int i=0;i<6;i++)
	{
		if(m_skyboxTextures[i] != "")
		{
			bSet = FALSE;
			for(j=i-1;j>=0;j--)
			{
				if(m_skyboxTextures[i] == m_skyboxTextures[j])
				{
					bSet = TRUE;
					break;
				}
			}
			if(bSet)
			{
				textures[i] = textures[j];
				continue;
			}
			textures[i] = new CTexture;
			if(textures[i]->LoadTexture(m_skyboxTextures[i].c_str()) == FALSE)
			{
				delete textures[i];
				textures[i] = NULL;
			}
		}
		else
			textures[i] = NULL;
	}

	m_levelSkybox->SetTextures(textures[0], textures[1], textures[2], textures[3], textures[4], textures[5]);
}

/**
 * Sets player meshes to correct positions. Loads the meshes
 * and adds scene objects if they haven't been set already
 **/
void CLevel::SetPlayerMeshes()
{
	CMesh *mesh = NULL;
	for(int i=0;i<2;i++)
	{
		if(m_playerMesh[i] == NULL)
		{
			ISceneManager *m = ISceneManager::GetManager();
			if(i==0)
			{
				mesh = CResourceManager::GetManager().GetMesh("meshes\\BlueFlag.x");
				/*mesh = m->GetMesh("meshes\\BlueFlag.x");
				if(mesh == NULL)
					mesh = GetDXApp()->LoadMesh("meshes\\BlueFlag.x");*/
			}
			else if(i==1)
			{
				mesh = CResourceManager::GetManager().GetMesh("meshes\\RedFlag.x");
				/*mesh = m->GetMesh("meshes\\RedFlag.x");
				if(mesh == NULL)
					mesh = GetDXApp()->LoadMesh("meshes\\RedFlag.x");*/
			}
			m_playerMesh[i] = m->AddMeshSceneNode(mesh);

			if(m_playerMesh[i] == NULL)
				continue;
		}

		// repair player position
		if(m_terrain)
		{
			if(m_terrain->GetMapLoaded())
				m_playerPositions[i].pos.y = m_terrain->GetTerrainHeight(m_playerPositions[i].pos.x, m_playerPositions[i].pos.z) + 7.f;
		}

		// position the mesh
		m_playerMesh[i]->SetTranslation(m_playerPositions[i].pos);
		m_playerMesh[i]->SetRotation(m_playerPositions[i].rot);
	}
}

void CLevel::SetPlayerPosition(int player, D3DXVECTOR3 pos)
{
	if(player<0 || player>1)
		return;

	m_playerPositions[player].pos = pos;
	SetPlayerMeshes();
}

void CLevel::CloseLevel()
{
	if(m_terrain)
		m_terrain->Release();
	ISceneManager *m = ISceneManager::GetManager();
	m->GetRootSceneNode()->RemoveChild(m_levelSkybox);
	m_levelSkybox = NULL;
	int i;
	for(i=0;i<2;i++)
	{
		m_playerObjects[i] = NULL;
		if(m_playerMesh[i] == NULL)
			continue;
		m_playerMesh[i]->SetVisible(FALSE);
	}
	GetEdgewalkerApp()->GetSceneManager()->GetObjectManager()->Reset();

	// Clear instanced controllers
	CControllerManager::GetManager().RemoveInstancedControllers();

	m_bLevelLoaded = FALSE;
	m_filename = "";

	RemoveDialogues();
	RemoveTerminals();
	RemoveWaypointSets();

	// 
	/*string str;
	stringstream temp;
	//temp << CParticleSystemSceneNode::GetEmitterCount();
	temp >> str;
	str += " particle emitters left in the scene after level closed";
	CPrinter::logs(str);*/
}

CShip *CLevel::CreatePlayer(int index, long templateID, CPlayer *controller, long id)
{
	if(index<0||index>1)
		return NULL;

	m_scnManager = GetEdgewalkerApp()->GetSceneManager()->GetObjectManager();

	// Causes a bug and is meaningless
	/*if(m_playerObjects[index] != NULL)
	{
	ship->BoostedThrust(0.f);
		if(m_scnManager)
			m_scnManager->RemoveObject(m_playerObjects[index]);
		else
		{
			CPrinter::log("Failed creating player object: scene manager pointter not set to CLevel", LOG_ERROR);
			return NULL;
		}
	ship->BoostedThrust(0.f);
	}*/

	CShip *ship;

	long startpos = GetOTE()->GetCampaign()->GetPlayerStartposID();

	if(startpos == -1)
	{
		ship = m_om->DropShip(templateID, m_playerPositions[index].pos, id);
	}
	else
	{
		IGameObject *pos = GetOTE()->GetObjectManagerSceneNode()->GetObject(startpos);
		if(pos == NULL)
			ship = m_om->DropShip(templateID, m_playerPositions[index].pos, id);
		else
			ship = m_om->DropShip(templateID, pos->GetTranslation(), id);
	}

	if(ship == NULL)
		return NULL;

	ship->SetController(controller);
	ship->SetRotation(m_playerPositions[index].rot);

	m_playerObjects[index] = ship;
	if(controller)
		controller->SetShip(ship);

	return ship;
}

void CLevel::ReportShipRemoval(CShip *ship)
{
	if(ship == NULL)
		return;

	for(int i=0;i<2;++i)
		if(ship == m_playerObjects[i])
		{
			m_playerObjects[i] = NULL;
			GetOTE()->GetPlayer(i)->SetShip(NULL);
			CCameraSceneNode *c = ISceneManager::GetManager()->GetActiveCamera();
			if(c)
			{
				if(c->GetParent() == ship)
				{
					// Activate secondary camera
					CCameraSceneNode *cs = GetOTE()->GetSecondaryCamera();
					if(c == NULL)
						return;
					D3DXVECTOR3 v = c->GetAbsolutePosition();
					cs->SetTranslation(v);
					//v = c->GetRotation();
					//cs->SetRotation(v);
					cs->SetLookatVector(ship->GetAbsolutePosition());
					ISceneManager::GetManager()->SetActiveCamera(cs);
				}
			}
		}
}

void CLevel::ReportEnemyRemoval(CEnemy *enemy)
{
	if(enemy == NULL)
		return;

	--m_enemyCount;
	if(CEnemy::GetEnemyCount()<=1 || m_enemyCount <= 0)	// double check to make sure simultaneous deaths don't jam the system
	{
		m_enemyCount = 0;
		GetOTE()->GetCampaign()->EndLevel(LEVEL_END_ENEMIES_KILLED);
	}

	GetOTE()->GetCampaign()->EndLevel(LEVEL_END_TARGET_DESTROYED, enemy->GetID());
}

int CLevel::GetNewWaypointSetID()
{
	int id = 0;
	vector<CWayPointSet*>::iterator it = m_waypointSets.begin();
	for(; it != m_waypointSets.end(); ++it)
		if((*it)->GetID() > id)
			id = (*it)->GetID();
	return ++id;
}

void CLevel::PlayRandomBackgroundMusic()
{
	// The third theme is boss music
	// Randomize between first two
	// TODO: improve
	CSoundManager *m = GetOTE()->GetSoundManager();
	if(GetOTE()->RandInt(0,1) == 1)
	{
		if(m_music[1])
			m->PlayMusic(m_music[1], false, true);
		else if(m_music[0])
			m->PlayMusic(m_music[0], false, true);
	}
	else
	{
		if(m_music[0])
			m->PlayMusic(m_music[0], false, true);
	}
}

bool CLevel::DisplayAssistentMessage(int id)
{
	vector<pair<int,string> >::iterator it = m_assistentMessages.begin();
	for(; it != m_assistentMessages.end(); ++it)
		if(it->first == id)
		{
			CAssistantMessages::GetIntance()->AddMessage(new string(it->second));
			return true;
		}
	return false;
}

void CLevel::RestrictPlayersToGameArea()
{
	for(int i=0;i<2;++i)
	{
		CShip *ship = m_playerObjects[i];
		if(ship == NULL)
			continue;

		D3DXVECTOR3 pos = ship->GetTranslation();
		float width = GetOTE()->GetSceneManager()->GetTiledTerrain()->GetWidth();
		float depth = GetOTE()->GetSceneManager()->GetTiledTerrain()->GetDepth();
		if(pos.x < 10.f)
			pos.x = 10.f;
		if(pos.x > width-10.f)
			pos.x = width-10.f;
		if(pos.z < 10.f)
			pos.z = 10.f;
		if(pos.z > depth-10.f)
			pos.z = depth-10.f;
		ship->SetTranslation(pos);
	}
}

void CLevel::OnDeviceLost()
{
}

void CLevel::OnDeviceReset()
{
	// Reset the lighting
	GetEdgewalkerApp()->SetAmbientLight(m_ambientColor);
}