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

#ifndef __CHEALTHBAR_H__
#define __CHEALTHBAR_H__

#include "Dialogue.h"

class CHealthBar
{
public:
	CHealthBar();
	~CHealthBar();

	void SetHealth(float hitPoints, float maxHitpoints);

	void Update(float frametime);
	void Render();

	inline void ForceDeactivate() {m_bActive = false;}

	inline bool IsActive() {return m_bActive;}

	static void LoadImages(char *background, char *hpbar);
	static void UnloadImages();

	static void EnableFading(bool value) {m_bFadingEnabled = value;}
	static bool IsFadingEnabled() {return m_bFadingEnabled;}

private:
	float m_targetPercentage;
	float m_healthPercentage;

	float m_fadeTimer;
	DIALOGUE_STATE m_state;

	int m_framesToDeath;

	bool m_bActive;

	static CTexture *m_background, *m_healthTexture;
	static bool m_bFadingEnabled;
	static int m_instCount;
};

#endif // #ifndef __CHEALTHBAR_H__