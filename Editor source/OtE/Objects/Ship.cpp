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
#include "..\\Gadgets\\WeaponGadget.h"

unsigned int CShip::m_shipCount = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShip::CShip(long id) : CVessel(id), 
	m_boostedThrust(100.f), 
	m_maxLoad(150.f),
	m_boostEnergyConsumption(70.f), 
	m_gadgetChangeTimeCounter(0.f), 
	m_bChangingGadget(FALSE),
	m_load(0.f),
	m_bG2DownPrev(false),
	m_gadget2ActivationTimer(0.f)
{
	m_type = SHIP_OBJECT_ID;
	m_gadgets[0] = m_gadgets[1] = NULL;
	++m_shipCount;
	m_bIsEnemy = false;
	m_bVisiblyEnemy = false;
}

CShip::~CShip()
{
	CEnemy::ReportPotentialTargetDestruction(this);
	--m_shipCount;
}

void CShip::ApplyTemplate(SShipTemplate t, bool bInitiateEffects, bool bRemoveOldEffects)
{
	// Apply the dynamic object template
	ApplyVesselTemplate(t.vt, bInitiateEffects, bRemoveOldEffects);

	m_boostEnergyConsumption = t.boostEnergyConsumption;
	m_boostedThrust = t.boostForce;
	m_maxLoad = t.maxLoad;

	// Delete existing gadgets
	for(int i=0;i<2;++i)
		m_gadgets[i] = NULL;
}

void CShip::Update(float frameTime)
{
	CVessel::Update(frameTime);

	for(int i=0; i<NUM_GADGETS; ++i)
		if(m_gadgets[i])
			m_gadgets[i]->Update(frameTime);

	if(m_gadget2ActivationTimer > 0.f)
		m_gadget2ActivationTimer -= frameTime;
}

bool CShip::BoostedThrust(float frameTime)
{
	float energy = m_energy - m_boostEnergyConsumption * frameTime;
	if(energy < 0.f)
		return FALSE;

	m_energy = energy;
	D3DXVECTOR3 force;
	force.x = sinf(m_relativeRotation.y) * m_boostedThrust;
	force.z = cosf(m_relativeRotation.y) * m_boostedThrust;
	force.y = 0.f;
	AddForce(force);
	if(m_peAccelerate)
		m_peAccelerate->Emit();

	return TRUE;
}

void CShip::UpdateGadgets(bool g1Down, bool g2Down)
{
	bool bActivated = false;
	if(m_bG2DownPrev && !g2Down && m_gadget2ActivationTimer <= 0.f)
	{
		if(m_gadgets[1])
		{
			if(m_gadgets[1]->IsActive())
			{
				m_bG2DownPrev = true;
				m_gadgets[1]->Activate();
			}
		}
	}
	else if(!m_bG2DownPrev && g2Down)
	{
		m_bG2DownPrev = true;
		if(m_gadgets[1])
		{
			m_gadgets[1]->Activate();
			// Toggle gadget activation delay
			// If player keeps the gadget 2 activation button pushed long enough
			// then it functions as as primary gadget in that releasing the button
			// will deactivate the gadget
			m_gadget2ActivationTimer = 0.4f;
		}
	}

	if(g1Down && m_gadgets[0])
	{
		bActivated = m_gadgets[0]->Activate();
		if(m_gadgets[0]->GetGadgetType() == GADGET_TYPE_WEAPON && bActivated)
			if(((CWeaponGadget*)m_gadgets[0])->IsOneUseGadget())
			{
				if(m_controller != NULL)
				{
					((CPlayer*)m_controller)->RemoveGadget(m_gadgets[0]->GetID());
					GetOTE()->AddScreenMessage("Disposable gadget used and removed");
				}
			}
	}

	if(!g2Down)
		m_bG2DownPrev = false;
}

/*SObjectInstanceInfo CShip::GetObjectInfo()
{
	SObjectInstanceInfo s;
	s.id = m_ID;
	s.pos = m_relativeTranslation;
	s.rot = m_relativeRotation;
	s.templateID = m_templateID;
	s.type = SHIP_OBJECT_ID;

	return s;
}*/

void CShip::Kill()
{
	// Report that a player ship has been destroyed
	CLevel *lvl = GetOTE()->GetCurrentLevel();
	if(lvl)
		lvl->ReportShipRemoval(this);

	CDynamicObject::Kill();
}

void CShip::RemoveGadgets()
{
	if(m_gadgets[0])
	{
		//m_gadgets[0]->UnEquip();
		m_gadgets[0] = NULL;
	}
	if(m_gadgets[1])
	{
		//m_gadgets[1]->UnEquip();
		m_gadgets[1] = NULL;
	}
}

void CShip::ReAttachPatches()
{
	vector<CShipPatch>::iterator it = m_patches.begin();
	for(; it != m_patches.end(); ++it)
		it->Attach(*this);
}