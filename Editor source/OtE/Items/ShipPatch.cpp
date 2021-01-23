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

#include "ShipPatch.h"
#include "..\\Objects\\Ship.h"

CShipPatch::CShipPatch(SShipPatchTemplate &t) : m_bAttached(false)
{
	ApplyTemplate(t);
}

void CShipPatch::ApplyTemplate(SShipPatchTemplate &t)
{
	m_type = t.type;
	m_param1 = t.params[0];
	m_param2 = t.params[1];
	m_name = t.name;
	m_ID = t.id;
	m_description = t.description;
	m_iconTexture = CResourceManager::GetManager().GetTexture(t.iconTexture);
}

bool CShipPatch::Attach(CShip &ship)
{
	vector<CShipPatch>::iterator it = ship.m_patches.begin();
	for(; it != ship.m_patches.end(); ++it)
		if(it->m_ID == m_ID)
			return false;

	switch(m_type)
	{
	case PATCH_TYPE_ENERGY:
		ship.m_maxEnergy += m_param1;
		ship.m_energy += m_param1;
		break;
	case PATCH_TYPE_HEALTH:
		ship.m_maxHitPoints += (DWORD)m_param1;
		ship.m_hitPoints += (DWORD)m_param1;
		break;
	case PATCH_TYPE_ENERGY_REGENERATION:
		ship.m_energyRegeneration += m_param1;
		break;
	case PATCH_TYPE_CARRY_CAPACITY:
		ship.m_maxLoad += m_param1;
		break;
	case PATCH_TYPE_CONTROL_SYSTEM:
		break;
	case PATCH_TYPE_SPEED:
		ship.m_thrust += m_param1;
		break;
	case PATCH_TYPE_BOOST_SPEED:
		ship.m_boostedThrust += m_param1;
		ship.m_boostEnergyConsumption += m_param2;
		break;
	case PATCH_TYPE_TURN_SPEED:
		ship.m_turnSpeed -= m_param1;
		break;
	}
	m_bAttached = true;

	return true;
}

bool CShipPatch::Detach(CShip &ship)
{
	bool found = false;
	vector<CShipPatch>::iterator it = ship.m_patches.begin();
	for(; it != ship.m_patches.end(); ++it)
		if(it->m_ID == m_ID)
		{
			found = true;
			break;
		}

	if(!found)
		return false;

	switch(m_type)
	{
	case PATCH_TYPE_ENERGY:
		ship.m_maxEnergy -= m_param1;
		ship.m_energy -= m_param1;
		break;
	case PATCH_TYPE_HEALTH:
		ship.m_maxHitPoints -= (DWORD)m_param1;
		ship.m_hitPoints -= (DWORD)m_param1;
		break;
	case PATCH_TYPE_ENERGY_REGENERATION:
		ship.m_energyRegeneration -= m_param1;
		break;
	case PATCH_TYPE_CARRY_CAPACITY:
		ship.m_maxLoad -= m_param1;
		break;
	case PATCH_TYPE_CONTROL_SYSTEM:
		break;
	case PATCH_TYPE_SPEED:
		ship.m_thrust -= m_param1;
		break;
	case PATCH_TYPE_BOOST_SPEED:
		ship.m_boostedThrust -= m_param1;
		ship.m_boostEnergyConsumption -= m_param2;
		break;
	case PATCH_TYPE_TURN_SPEED:
		ship.m_turnSpeed += m_param1;
		break;
	}

	m_bAttached = false;

	return true;
}