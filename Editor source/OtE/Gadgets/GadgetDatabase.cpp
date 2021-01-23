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

#include "GadgetDatabase.h"
#include "..\\Edgewalker\\Utilities.h"
#include "ShieldGadget.h"
#include "StealthGadget.h"
#include "WeaponGadget.h"
#include "HoverGadget.h"
#include "PossessGadget.h"
#include "KineticGadget.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGadgetDatabase::CGadgetDatabase()
{
}

CGadgetDatabase::~CGadgetDatabase()
{
	UnloadTemplates();
}

void CGadgetDatabase::UnloadTemplates()
{
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_gadgets.begin();
	for(;it!=m_gadgets.end();it++)
		if(it->second != NULL)
			delete it->second;
	m_gadgets.clear();
}

void CGadgetDatabase::LoadDefaultGadgets()
{
	//SGadgetTemplate t;
	pair<SGadgetTemplate, SCustomGadgetTemplate*> p;
	p.first.FillDefaults();
	p.first.ID = 1;
	p.first.gadgetType = GADGET_TYPE_WEAPON;
	strcpy(p.first.name, "Pyssy");
	p.first.capacityRequirement = 20.f;
	//p.second = 
	SWeaponGadgetTemplate *t = new SWeaponGadgetTemplate();
	t->FillDefaults();
	t->force = 150.f;
	t->ammoTemplateID = 1;
	t->damage = 40.f;
	p.second = t;
	AddGadgetTemplate(p);

	/*t.FillDefaultWeapon();
	t.ammoTemplateID = 1;
	t.e.amount = 150.f;
	t.force = 150.f;
	t.lifetime = 0.9f;
	t.firingPace = 2.f;
	t.e.bExplosive = FALSE;
	t.e.particleEffectID = 2;

	strcpy(t.name, "Weapon");
	t.bPassive = FALSE;
	t.bLoadable = FALSE;
	t.mass = 100.f;
	t.ID = 1;
	AddGadgetTemplate(t);

	strcpy(t.name, "Bossi pyssy");
	t.ID = 2;
	t.firingPace = 0.2f;
	t.e.amount = 50;
	t.energyConsumption = 4.0f;
	t.mode = WEAPON_MODE_NOVA;
	t.numBullets = 16;
	t.angle = 0.8f;
	AddGadgetTemplate(t);

	strcpy(t.name, "Pyssy");
	t.ID = 3;
	t.firingPace = 0.3f;
	t.e.amount = 30.f;
	t.mode = WEAPON_MODE_FORWARD_ALTERNATING;
	t.numBullets = 3;
	t.angle = 0.5f;
	AddGadgetTemplate(t);

	t.FillDefaultLightening();
	t.ID = 4;
	t.force = 6000.f;
	AddGadgetTemplate(t);

	t.FillDefaultEnergy();
	t.ID = 5;
	t.loadSpeed = 15.f;
	AddGadgetTemplate(t);

	t.FillDefaultShield();
	t.ID = 6;
	t.damageCoefficient = 0.5f;
	AddGadgetTemplate(t);

	t.FillDefaultEngine();
	t.ID = 7;
	t.boostForce = 25000.f;
	t.force = 18000.f;
	AddGadgetTemplate(t);

	t.FillDefaultPush();
	t.ID = 8;
	AddGadgetTemplate(t);

	t.FillDefaultWeapon();
	t.ID = 9;
	strcpy(t.name, "Kova pyssy");
	AddGadgetTemplate(t);*/
}

bool CGadgetDatabase::LoadDatabase(const char *filename)
{
	if(filename == NULL)
		return false;

	fstream f;
	f.open(filename, ios::in | ios::binary);
	if(!f.is_open())
		return false;
	m_gadgets.clear();

	DWORD id = 0;

	f.read((char*)&id, sizeof(DWORD));
	if(id != GADGET_DATABASE_BEGIN_IDENTIFIER)
	{
		f.close();
		return false;
	}

	DWORD count=0;
	f.read((char*)&count, sizeof(DWORD));
	if(count > 1000)
	{
		CPrinter::logError("Erraneous amount of gadgets, file propably corrupted");
		return false;
	}

	pair<SGadgetTemplate, SCustomGadgetTemplate*> item;

	for(DWORD i=0;i<count;i++)
	{
		f.read((char*)&item.first, sizeof(SGadgetTemplate));
		switch(item.first.gadgetType)
		{
		case GADGET_TYPE_WEAPON:
			item.second = new SWeaponGadgetTemplate;
			f.read((char*)item.second, sizeof(SWeaponGadgetTemplate));
			break;
		case GADGET_TYPE_KINETIC:
			item.second = new SKineticGadgetTemplate;
			f.read((char*)item.second, sizeof(SKineticGadgetTemplate));
			break;
		case GADGET_TYPE_HOVER:
			item.second = new SHoverGadgetTemplate;
			f.read((char*)item.second, sizeof(SHoverGadgetTemplate));
			break;
		case GADGET_TYPE_SHIELD:
			item.second = new SShieldGadgetTemplate;
			f.read((char*)item.second, sizeof(SShieldGadgetTemplate));
			break;
		case GADGET_TYPE_STEALTH:
			item.second = new SStealthGadgetTemplate;
			f.read((char*)item.second, sizeof(SStealthGadgetTemplate));
			break;
		case GADGET_TYPE_POSSESS:
			item.second = new SPossessGadgetTemplate;
			f.read((char*)item.second, sizeof(SPossessGadgetTemplate));
			break;
		default:
			break;
		}
		m_gadgets.push_back(item);
	}

	f.close();
	return TRUE;
}

bool CGadgetDatabase::SaveDatabase(const char *filename)
{
	if(filename == NULL)
		return false;

	fstream f;
	f.open(filename, ios::out | ios::binary);
	if(!f.is_open())
		return false;

	DWORD identifier = GADGET_DATABASE_BEGIN_IDENTIFIER;
	f.write((const char*)&identifier, sizeof(DWORD));

	DWORD count = m_gadgets.size();
	f.write((const char*)&count, sizeof(DWORD));

	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_gadgets.begin();
	for(; it != m_gadgets.end(); ++it)
	{
		f.write((const char*)&it->first, sizeof(SGadgetTemplate));
		switch(it->first.gadgetType)
		{
		case GADGET_TYPE_WEAPON:
			f.write((const char*)it->second, sizeof(SWeaponGadgetTemplate));
			break;
		case GADGET_TYPE_KINETIC:
			f.write((const char*)it->second, sizeof(SKineticGadgetTemplate));
			break;
		case GADGET_TYPE_HOVER:
			f.write((const char*)it->second, sizeof(SHoverGadgetTemplate));
			break;
		case GADGET_TYPE_SHIELD:
			f.write((const char*)it->second, sizeof(SShieldGadgetTemplate));
			break;
		case GADGET_TYPE_STEALTH:
			f.write((const char*)it->second, sizeof(SStealthGadgetTemplate));
			break;
		case GADGET_TYPE_POSSESS:
			f.write((const char*)it->second, sizeof(SPossessGadgetTemplate));
			break;
		default:
			break;
		}
	}

	f.close();
	return TRUE;
}

void CGadgetDatabase::AddGadgetTemplate(pair<SGadgetTemplate, SCustomGadgetTemplate*> gadget)
{
	if(gadget.second == NULL)
		return;
	m_gadgets.push_back(gadget);
}

pair<SGadgetTemplate, SCustomGadgetTemplate*> *CGadgetDatabase::GetGadgetTemplate(int id)
{
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_gadgets.begin();
	for(;it!=m_gadgets.end();it++)
		if(it->first.ID == id)
			return &(*it);
	return NULL;
}

IGadget *CGadgetDatabase::CreateGadget(int id)
{
	IGadget *g = NULL;
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_gadgets.begin();
	for(;it!=m_gadgets.end();it++)
		if(it->first.ID == id)
		{
			switch(it->first.gadgetType)
			{
			case GADGET_TYPE_WEAPON:
				g = new CWeaponGadget(*((SWeaponGadgetTemplate*)it->second), it->first);
				break;
			case GADGET_TYPE_KINETIC:
				g = new CKineticGadget(*((SKineticGadgetTemplate*)it->second), it->first);
				break;
			case GADGET_TYPE_HOVER:
				g = new CHoverGadget(*((SHoverGadgetTemplate*)it->second), it->first);
				break;
			case GADGET_TYPE_STEALTH:
				g = new CStealthGadget(*((SStealthGadgetTemplate*)it->second), it->first);
				break;
			case GADGET_TYPE_SHIELD:
				g = new CShieldGadget(*((SShieldGadgetTemplate*)it->second), it->first);
				break;
			case GADGET_TYPE_POSSESS:
				g = new CPossessGadget(*((SPossessGadgetTemplate*)it->second), it->first);
				break;
			}
			return g;
		}
	return NULL;
}

bool CGadgetDatabase::GetGadgetName(long id, std::string &outName)
{
	IGadget *g = NULL;
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_gadgets.begin();
	for(;it!=m_gadgets.end();it++)
		if(it->first.ID == id)
		{
			outName = it->first.name;
			return true;
		}
	return false;
}

vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> > CGadgetDatabase::GetMultiplayerGadgets()
{
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> > mp;
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_gadgets.begin();
	for(; it != m_gadgets.end(); ++it)
		if(it->first.bMultiplayer)
			mp.push_back(*it);
	return mp;
}


bool CGadgetDatabase::ExportTemplate(long id, std::string fullPath)
{
	bool found = false;
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_gadgets.begin();
	for(;it != m_gadgets.end(); it++)
		if(it->first.ID == id)
		{
			found = true;
			break;
		}
	if(!found)
		return false;

	fstream f;
	f.open(fullPath.c_str(), ios::out | ios::binary);
	if(!f.is_open())
		return false;

	DWORD identifier = 15343;
	f.write((const char*)&identifier, sizeof(DWORD));

	f.write((const char*)&it->first, sizeof(SGadgetTemplate));
	switch(it->first.gadgetType)
	{
	case GADGET_TYPE_WEAPON:
		f.write((const char*)it->second, sizeof(SWeaponGadgetTemplate));
		break;
	case GADGET_TYPE_KINETIC:
		f.write((const char*)it->second, sizeof(SKineticGadgetTemplate));
		break;
	case GADGET_TYPE_HOVER:
		f.write((const char*)it->second, sizeof(SHoverGadgetTemplate));
		break;
	case GADGET_TYPE_SHIELD:
		f.write((const char*)it->second, sizeof(SShieldGadgetTemplate));
		break;
	case GADGET_TYPE_STEALTH:
		f.write((const char*)it->second, sizeof(SStealthGadgetTemplate));
		break;
	case GADGET_TYPE_POSSESS:
		f.write((const char*)it->second, sizeof(SPossessGadgetTemplate));
		break;
	}

	f.close();
	return true;
}

bool CGadgetDatabase::ImportTemplate(long id, std::string fullPath)
{
	fstream f;
	f.open(fullPath.c_str(), ios::in | ios::binary);
	if(!f.is_open())
		return false;

	DWORD identifier = 0;

	f.read((char*)&identifier, sizeof(DWORD));
	if(identifier != 15343)
	{
		f.close();
		return false;
	}

	pair<SGadgetTemplate, SCustomGadgetTemplate*> item;

	f.read((char*)&item.first, sizeof(SGadgetTemplate));
	switch(item.first.gadgetType)
	{
	case GADGET_TYPE_WEAPON:
		item.second = new SWeaponGadgetTemplate;
		f.read((char*)item.second, sizeof(SWeaponGadgetTemplate));
		break;
	case GADGET_TYPE_KINETIC:
		item.second = new SKineticGadgetTemplate;
		f.read((char*)item.second, sizeof(SKineticGadgetTemplate));
		break;
	case GADGET_TYPE_HOVER:
		item.second = new SHoverGadgetTemplate;
		f.read((char*)item.second, sizeof(SHoverGadgetTemplate));
		break;
	case GADGET_TYPE_SHIELD:
		item.second = new SShieldGadgetTemplate;
		f.read((char*)item.second, sizeof(SShieldGadgetTemplate));
		break;
	case GADGET_TYPE_STEALTH:
		item.second = new SStealthGadgetTemplate;
		f.read((char*)item.second, sizeof(SStealthGadgetTemplate));
		break;
	case GADGET_TYPE_POSSESS:
		item.second = new SPossessGadgetTemplate;
		f.read((char*)item.second, sizeof(SPossessGadgetTemplate));
		break;
	}
	f.close();

	if(id == -1)
		item.first.ID = GetNewID();
	else
	{
		item.first.ID = id;
		vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_gadgets.begin();
		for(;it != m_gadgets.end(); it++)
			if(it->first.ID == id)
			{
				it->first = item.first;
				if(it->second)
					delete it->second;
				it->second = item.second;
				return true;
			}
	}
	m_gadgets.push_back(item);

	return true;
}

long CGadgetDatabase::GetNewID()
{
	long id = 1;
	vector<pair<SGadgetTemplate, SCustomGadgetTemplate*> >::iterator it = m_gadgets.begin();
	for(;it != m_gadgets.end(); it++)
		if(it->first.ID > id)
			id = it->first.ID;

	return ++id;
}