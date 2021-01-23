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

#ifndef __FLOATINGTEXT_H__
#define __FLOATINGTEXT_H__

#include "..\\Edgewalker\\Scene\\ISceneNode.h"
#include "..\\Edgewalker\\DirectX9\\Texture.h"

#include <vector>

class CFloatingText : public ISceneNode
{
public:
	CFloatingText(ISceneNode *parent, ISceneManager *manager, std::string text, DWORD id = -1,
		D3DXVECTOR3 position = D3DXVECTOR3(0.0f, 10.0f, 0.0f),
		D3DXVECTOR3 scale = D3DXVECTOR3(5.0f, 5.0f, 5.0f));
	virtual ~CFloatingText();

	void SetText(std::string text);
	void SetColor(DWORD color) {m_color = color;}

	void SetFadeTrigger(float time, float fadeTime);
	void DisableFadeTrigger();

	virtual void Render(bool isShadowPass);
	virtual void Update(float frameTime);
	virtual void OnPreRender(bool isShadowPass);
	virtual void UpdateAbsolutePosition();
	virtual aabbox3d &GetBoundingBox() {return *(aabbox3d*)NULL;}

	static void SetFontSize(D3DXVECTOR2 size) {m_fontSize = size;}
	static void SetTexture(CTexture *tex) {m_fontTex = tex;}
	static void SetCharmap(const char *chars, int length);

protected:
	struct FTVertex
	{
		float x,y,z;
		float u,v;
	};
	struct FTQuad
	{
		FTVertex verts[4];

		FTQuad(FTVertex v1, FTVertex v2, FTVertex v3, FTVertex v4)
		{
			verts[0] = v1;
			verts[1] = v2;
			verts[2] = v3;
			verts[3] = v4;
		}
	};

	void RebuildVertices();

	std::vector<FTQuad> m_vertices;

	D3DXVECTOR2 GetCharPosition(char ch);

	DWORD m_color;
	std::string m_text;

	float m_timer, m_fadeTime, m_triggerTime;
	float m_onePerFadeTime;

	static D3DXVECTOR2 m_fontSize;
	static CTexture *m_fontTex;
	static std::vector<char> m_charmap;
};

#endif // #ifndef __FLOATINGTEXT_H__