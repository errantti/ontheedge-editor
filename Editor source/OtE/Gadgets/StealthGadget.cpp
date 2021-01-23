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

#include "StealthGadget.h"
#include "..\\Objects\\Vessel.h"

CStealthGadget::CStealthGadget(SStealthGadgetTemplate &t, SGadgetTemplate &gt) :
	m_bActive(false)
{
	m_bActivateable = false;

	ApplyTemplate(t, gt);
}

CStealthGadget::~CStealthGadget() {}

void CStealthGadget::ApplyTemplate(SStealthGadgetTemplate &t, SGadgetTemplate &gt)
{
	IGadget::ApplyTemplate(gt);
	m_template = t;
}

void CStealthGadget::UnEquip()
{
	DeActivate();
	IGadget::UnEquip();
}

void CStealthGadget::Update(float frameTime)
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
	}
}

void CStealthGadget::DeActivate()
{
	m_bActive = false;
	if(!m_vessel)
		return;
	m_vessel->SetStealthCoefficient(1.f);
}

bool CStealthGadget::Activate()
{
	if(m_bActive)
	{
		DeActivate();
		return false;
	}
	if(m_vessel == NULL)
		return false;
	m_vessel->SetStealthCoefficient(m_template.coefficient);
	m_bActive = true;
	return true;
}

void CStealthGadget::SetDescription()
{
}