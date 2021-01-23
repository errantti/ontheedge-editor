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

#include "PossessGadget.h"
#include "..\\Objects\\Vessel.h"
#include "..\\OnTheEdge.h"

CPossessGadget::CPossessGadget(SPossessGadgetTemplate &t, SGadgetTemplate &gt) :
	m_bActive(false)
{
	m_bActivateable = false;

	ApplyTemplate(t, gt);
}

CPossessGadget::~CPossessGadget() {}

void CPossessGadget::ApplyTemplate(SPossessGadgetTemplate &t, SGadgetTemplate &gt)
{
	IGadget::ApplyTemplate(gt);
	m_template = t;
}

void CPossessGadget::Update(float frameTime)
{
	if(m_bActive && m_vessel)
	{
		// Check if the player is no more possessing
		if(m_vessel->GetController() != NULL)
		{
			DeActivate();
			return;
		}
		float newEnergy = *m_energy - frameTime * m_template.energyConsumption;
		if(newEnergy < 0.f)
		{
			GetOTE()->AddScreenMessage("Ship's energy could no longer sustain the takeover gadget");
			DeActivate();
			return;
		}
		*m_energy = newEnergy;

		// Check distance
		D3DXVECTOR3 enpos = GetOTE()->GetPlayer(0)->GetFocusPoint();
		D3DXVECTOR3 plpos = m_vessel->GetTranslation();
		float dx,dy,dz;
		dx = plpos.x - enpos.x;
		dy = plpos.y - enpos.y;
		dz = plpos.z - enpos.z;
		if(dx*dx + dy*dy + dz*dz > m_template.maintainDistance * m_template.maintainDistance)
		{
			DeActivate();
			GetOTE()->AddScreenMessage("The enemy exceeded the range at which the takeover gadget functions");
		}
	}
}

void CPossessGadget::DeActivate()
{
	GetOTE()->GetPlayer(0)->UnPosses();
	m_bActive = false;
}

bool CPossessGadget::Activate()
{
	if(!m_vessel)
		return false;

	// Mind control gadgets are only for player ships to use
	if(m_vessel->GetType() != SHIP_OBJECT_ID)
		return false;

	m_bActive = true;

	// Get target enemy
	std::vector<IGameObject*> objs;
	D3DXVECTOR3 pos = m_vessel->GetTranslation();
	rect<float> area(pos.x - m_template.effectRadius, 
		pos.x + m_template.effectRadius, 
		pos.z - m_template.effectRadius, 
		pos.z + m_template.effectRadius);
	GetOTE()->GetObjectManagerSceneNode()->GetObjectsFromSectorsInArea(area, objs);

	std::vector<IGameObject*>::iterator it = objs.begin();
	for(; it != objs.end(); ++it)
	{
		if((*it)->GetType() == ENEMY_OBJECT_ID)
		{
			if(((CEnemy*)(*it))->GetCanBePossessed())
			{
				GetOTE()->GetPlayer(0)->PossessEnemy((CEnemy*)(*it));
				return true;
			}
		}
	}

	GetOTE()->AddScreenMessage("There were no enemies within gadget radius that could be taken over");

	return false;
}

void CPossessGadget::SetDescription()
{
}