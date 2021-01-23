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

#include "HoverGadget.h"
#include "..\\Objects\\Vessel.h"

CHoverGadget::CHoverGadget(SHoverGadgetTemplate &t, SGadgetTemplate &gt) : m_bHover(false)
{
	m_bActivateable = false;

	ApplyTemplate(t, gt);
}

CHoverGadget::~CHoverGadget() {}

void CHoverGadget::ApplyTemplate(SHoverGadgetTemplate &t, SGadgetTemplate &gt)
{
	IGadget::ApplyTemplate(gt);
	m_template = t;
}

void CHoverGadget::Update(float frameTime)
{
	if(m_bHover && m_vessel)
	{
		float newEnergy = *m_energy - m_template.energyConsumption * frameTime;
		if(newEnergy > 0.f)
		{
			*m_energy = newEnergy;
			m_vessel->AddForce(D3DXVECTOR3(0.f, m_template.force, 0.f));
		}
		else
			m_bHover = false;
	}
}

void CHoverGadget::SetDescription()
{
}

bool CHoverGadget::Activate()
{
	m_bHover = !m_bHover;
	return true;
}