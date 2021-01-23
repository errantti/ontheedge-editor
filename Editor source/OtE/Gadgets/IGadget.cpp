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

#include "IGadget.h"
#include "..\\OnTheEdge.h"
#include "..\\Objects\\Vessel.h"
//#include <sstream>

IGadget::IGadget() : m_bActivateable(false), m_energy(NULL), m_vessel(NULL)
{
}

bool IGadget::Equip(CVessel *vessel)
{
	if(vessel == NULL)
		return false;
	m_vessel = vessel;
	m_energy = &(m_vessel->GetEnergyReference());
	//m_energy = &energy;
	return true;
}

void IGadget::UnEquip()
{
	m_vessel = NULL;
	m_energy = NULL;
}

void IGadget::ApplyTemplate(SGadgetTemplate &t)
{
	m_name = t.name;
	m_capacityRequirement = t.capacityRequirement;
	m_activationScript = CScriptManager::GetManager().GetScript(t.activationScript);
	m_activationSound = CSoundManager::GetManager().GetSound(t.activationSoundID);
	m_bEnabledInMultiplayer = t.bMultiplayer;
	m_gadgetType = t.gadgetType;
	m_iconTexture = CResourceManager::GetManager().GetTexture(t.icon);
	m_ID = t.ID;
	m_useFlags = t.useFlags;
	m_description = t.description;
}

/***
 * GetDetailInfo
 * Returns some essential information about the gadget
 ***
string IGadget::GetDetailInfo()
{
	string str = m_t.name, strTemp;
	str += "\nMass: ";
	stringstream temp, temp2;
	temp << (int)m_t.mass;
	temp >> strTemp;
	str += strTemp;
	switch(m_t.gadgetType)
	{
	case GADGET_TYPE_WEAPON:
		str += " Type: weapon\n";
		if(m_t.firingPace > 1.2f)
			str+="Slow ";
		else if(m_t.firingPace > 0.5f)
			str+="Quick ";
		else
			str+="Fast ";

		if(m_t.e.amount > 90)
			str+="and destructive ";
		else if(m_t.e.amount > 40)
			str+="and moderately powerful ";
		else
			str+="and rather weak ";

		if(m_t.bLoadable)
			str+="loadable ";

		if(m_t.mode == WEAPON_MODE_NOVA || m_t.mode == WEAPON_MODE_NOVA_ALTERNATING)
			str+="nova weapon.";
		else if(m_t.mode == WEAPON_MODE_BACKWARD)
			str+="backward launching weapon.";
		else
			str+="cannon.";
		break;
	case GADGET_TYPE_KINETIC:
		str += " Type: kinetic gun\nSends a ";
		if(m_t.force > 4000.f)
			str+="powerful ";
		else if(m_t.force > 2500.f)
			str+="strong ";
		else
			str+="weak ";

		if(m_t.angle > 1.87f)
			str+="and very wide ";
		else if(m_t.angle > 1.1f)
			str+="and wide ";
		else
			str+="and narrow ";

		str+="blast wave at a ";
		if(m_t.firingPace > 1.2f)
			str+="slow ";
		else if(m_t.firingPace > 0.5f)
			str+="moderate ";
		else
			str+="rapid ";
		str+="pace.";
		break;
	case GADGET_TYPE_LIGHTEN:
		str += " Type: hover engine\n";
		if(m_t.zeroForce > 1400.f)
			str+="Adds a considerable extra hover force";
		else if(m_t.zeroForce > 500.f)
			str+="Adds weak extra hover force";
		else
			str+="Adds no hover force";

		if(m_t.force > 8000.f)
			str+=" and gives a strong vertical boost.";
		else if(m_t.force > 5500.f)
			str+=" and gives a moderate vertical boost.";
		else if(m_t.force > 3000.f)
			str+=" and gives a minor vertical boost.";
		else
			str+=".";

		str+=" Extra capacity: ";
		temp2 << (int)m_t.maxLoadIncrease;
		temp2 >> strTemp;
		str+=strTemp;
		break;
	case GADGET_TYPE_ENGINE:
		str += " Type: engine\n";
		if(m_t.force > 6000.f)
			str+="Gives a strong speed increase";
		else if(m_t.force > 3500.f)
			str+="Gives a noticeable speed increase";
		else if(m_t.force > 1500.f)
			str+="Gives a minor speed increase";
		else
			str+="Gives next to no speed increase";

		if(m_t.boostForce > 10000.f)
			str+=" and great boost increase.";
		else if(m_t.boostForce > 7000.f)
			str+=" and moderate boost increase.";
		else if(m_t.boostForce > 4000.f)
			str+=" and minor boost increase.";
		else
			str+=".";
		break;
	case GADGET_TYPE_SHIELD:
		str += " Type: shield generator\n";
		if(m_t.damageCoefficient > 0.9f)
			str+="Generates a weak defensive shield.";
		else if(m_t.damageCoefficient > 0.74f)
			str+="Generates an effective defensive shield.";
		else if(m_t.damageCoefficient > 0.6f)
			str+="Generates a strong defensive shield.";
		else if(m_t.damageCoefficient > 0.4f)
			str+="Generates an extremely strong defensive shield.h";
		else if(m_t.damageCoefficient > 0.2f)
			str+="Makes you almost invulnerable.";
		else
			str+="Generates an immunity shield.";
		break;
	case GADGET_TYPE_ENERGY:
		str += " Type: energy reactor\n";
		if(m_t.loadSpeed > 50.f)
			str+="Increases your energy regeneration speed immensely";
		else if(m_t.loadSpeed > 30.f)
			str+="Increases your energy regeneration speed greatly";
		else if(m_t.loadSpeed > 15.f)
			str+="Increases your energy regeneration speed moderately";
		else if(m_t.loadSpeed > 8.f)
			str+="Increases your energy regeneration speed a little";
		else
			str+="Has almost no effect on your energy renegeration speed";

		if(m_t.extraEnergy > 100.f)
			str+=" and has high energy storage potential.";
		else if(m_t.extraEnergy > 50.f)
			str+=" and has moderate energy storage potential.";
		else if(m_t.extraEnergy > 20.f)
			str+=" and has low energy storage potential.";
		break;
	}

	if(m_t.bPassive)
		str+=" Passive device.";
	else
	{
		if(m_t.energyConsumption > 50.f)
			str+=" Heavy energy consumption.";
		else if(m_t.energyConsumption > 15.f)
			str+=" Moderate energy consumption.";
		else
			str+=" Light energy consumption.";
	}

	return str;
}*/