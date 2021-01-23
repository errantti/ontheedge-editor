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

#ifndef __ILIGHT_H__
#define __ILIGHT_H__

#include <d3d9.h>
#include <vector>

/// Maximum number of lights active at once
#define MAXIMUM_LIGHT_COUNT		6

/**
 * Interface for light objects
 */
class ILight
{
public:
	ILight();
	virtual ~ILight();

	void SetLight(D3DLIGHT9 light) {m_light = light; m_bSet = TRUE;}
	D3DLIGHT9 GetLight() const {return m_light;}
	int GetIndex() const {return m_iIndex;}

	void Enable(bool bEnabled, IDirect3DDevice9 *pd3dd = NULL);
	bool GetEnabled() const {return m_bEnabled;}

	void SetStatic(bool value) {m_bStatic = value;}
	bool GetStatic() {return m_bStatic;}

	inline bool IsSet() {return m_bSet;}
	inline void SetIsSet(bool value) {m_bSet = value;}

	static void DisableDynamicLights(IDirect3DDevice9 *pd3dd);

protected:
	D3DLIGHT9 m_light;
	bool m_bEnabled, m_bSet;
	bool m_bStatic;				// True if the light is static (not updated on every frame)

private:
	void UpdateLightIndex(unsigned int newIndex) {m_iIndex = newIndex;}

	unsigned int m_iIndex;
	static ILight *m_lights[MAXIMUM_LIGHT_COUNT];
};

#endif // #ifndef __ILIGHT_H__
