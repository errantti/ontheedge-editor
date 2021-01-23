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

#include "ShieldGadget.h"
#include "..\\OnTheEdge.h"

CShieldGadget::CShieldGadget(SShieldGadgetTemplate &t, SGadgetTemplate &gt) :
	m_bActive(false), m_hitEffect(0), m_node(NULL)
{
	m_bActivateable = false;

	ApplyTemplate(t, gt);
}

CShieldGadget::~CShieldGadget() 
{
	/*if(m_node)
	{
		m_node->Remove();
		m_node = NULL;
	}*/
}

void CShieldGadget::ApplyTemplate(SShieldGadgetTemplate &t, SGadgetTemplate &gt)
{
	IGadget::ApplyTemplate(gt);
	m_template = t;
	m_hitEffect = GetOTE()->GetParticleEffects()->GetEffectData(t.particleEffectID);
}

void CShieldGadget::UnEquip()
{
	DeActivate();
	IGadget::UnEquip();
	if(m_node)
	{
		m_node->Remove();
		m_node = NULL;
	}
}

void CShieldGadget::Update(float frameTime)
{
	if(m_bActive && m_vessel)
	{
		float newEnergy = *m_energy - frameTime * m_template.energyConsumption;
		if(newEnergy < 0.f)
		{
			DeActivate();
			return;
		}
		*m_energy = newEnergy;
		if(m_node)
			m_node->Emit();
	}
}

void CShieldGadget::DeActivate()
{
	m_bActive = false;
	if(!m_vessel)
		return;
	m_vessel->SetAlteredDamageType(DAMAGE_TYPE_NONE);
	m_vessel->SetAlteredDamageTypeCoefficient(1.f);
}

bool CShieldGadget::Activate()
{
	if(!m_vessel)
		return false;

	if(m_bActive)
	{
		DeActivate();
	}
	else
	{
		m_vessel->SetAlteredDamageType(m_template.type);
		m_vessel->SetAlteredDamageTypeCoefficient(m_template.coefficient);
		if(!m_node && m_hitEffect)
			m_node = GetOTE()->GetParticleEffects()->StartEffect(m_vessel, *m_hitEffect);
		m_bActive = true;
		return true;
	}
	return false;
}

void CShieldGadget::SetDescription()
{
}