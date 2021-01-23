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

#include "KineticGadget.h"
#include "..\\OnTheEdge.h"

CKineticGadget::CKineticGadget(SKineticGadgetTemplate &t, SGadgetTemplate &gt) :
	m_activationTimer(0.f), m_activateEffect(0)
{
	m_bActivateable = true;

	ApplyTemplate(t, gt);
}

CKineticGadget::~CKineticGadget() {}

void CKineticGadget::ApplyTemplate(SKineticGadgetTemplate &t, SGadgetTemplate &gt)
{
	IGadget::ApplyTemplate(gt);
	m_template = t;

	m_activateEffect = GetOTE()->GetParticleEffects()->GetEffectData(t.particleEffectID);
}

void CKineticGadget::Update(float frameTime)
{
	if(m_activationTimer > 0.f)
		m_activationTimer -= frameTime;
}

bool CKineticGadget::Activate()
{
	if(m_activationTimer <= 0.f && *m_energy-m_template.energyConsumption > 0.f)
	{
		m_activationTimer = m_template.firingPace;
		*m_energy -= m_template.energyConsumption;

		GetOTE()->GetObjectManager()->AddImpulseForce(m_vessel->GetTranslation(), 
			m_vessel->GetRotation(), m_template.radius, m_template.force, m_template.angle);

		if(m_activateEffect)
			GetOTE()->GetParticleEffects()->StartEffect(m_vessel->GetTranslation(), 
				m_vessel->GetRotation(), *m_activateEffect);

		if(m_activationSound)
			m_activationSound->Play();
		return true;
	}

	return false;
}

void CKineticGadget::SetDescription()
{
}