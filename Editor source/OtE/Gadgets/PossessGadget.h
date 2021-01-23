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

#ifndef __CPOSSESSGADGET_H__
#define __CPOSSESSGADGET_H__

#include "IGadget.h"

struct SPossessGadgetTemplate : public SCustomGadgetTemplate
{
	float energyConsumption, effectRadius, maintainDistance;

	void FillDefaults()
	{
		energyConsumption = 10.f;
		effectRadius = 50.f;
		maintainDistance = 250.f;
	}
};

/**
 * Gadget type that possesses a nearby enemy
 * giving control of it to player for a certain
 * time (the amount of time depends on how long
 * player's energy lasts)
 */
class CPossessGadget : public IGadget
{
public:
	CPossessGadget(SPossessGadgetTemplate &t, SGadgetTemplate &gt);
	~CPossessGadget();

	void ApplyTemplate(SPossessGadgetTemplate &t, SGadgetTemplate &gt);

	//bool Equip(CVessel *vessel, float &energy);
	//void UnEquip();
	void Update(float frameTime);

	bool Activate();

	bool IsActive() {return m_bActive;}

protected:
	void DeActivate();
	void SetDescription();

	SPossessGadgetTemplate m_template;

	bool m_bActive;
};

#endif // #ifndef __CPOSSESSGADGET_H__