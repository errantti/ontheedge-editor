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

#include "FloatingText.h"
#include "..\\Edgewalker\\Edgewalker.h"

std::vector<char> CFloatingText::m_charmap;
CTexture *CFloatingText::m_fontTex = NULL;
D3DXVECTOR2 CFloatingText::m_fontSize(1.f/16.f, 1.f/16.f);

CFloatingText::CFloatingText(ISceneNode *parent, ISceneManager *manager, std::string text, DWORD id,
			  D3DXVECTOR3 position, D3DXVECTOR3 scale) :
	ISceneNode(parent, manager, id, position, D3DXVECTOR3(0.f, 0.f, 0.f), scale),
	m_color(0xFFFFFFFF), m_text(text), m_timer(-1000.f), m_fadeTime(1.f), m_triggerTime(4.f)
{
	m_bAutomaticCulling = false;
	RebuildVertices();
}

CFloatingText::~CFloatingText()
{
}

void CFloatingText::SetCharmap(const char *chars, int length)
{
	m_charmap.clear();
	for(int i=0;i<length;++i)
		m_charmap.push_back(chars[i]);
}

void CFloatingText::SetText(std::string text)
{
	if(text == m_text)
		return;
	m_text = text;
	RebuildVertices();
}

void CFloatingText::RebuildVertices()
{
	m_vertices.clear();

	int len = (int)m_text.size();
	if(len < 1)
		return;

	float w = m_relativeScale.x;

	float center = m_relativeScale.x * (float)len * 0.5f;

	float x = center;
	float y1 = m_relativeScale.y * -0.5f;
	float y2 = -y1;

	FTVertex verts[] = {
		{x-w, y1, 0.f, 0.f, 1.f},
		{x, y1, 0.f, 1.f, 1.f},
		{x-w, y2, 0.f, 0.f, 0.f},
		{x, y2, 0.f, 1.f, 0.f}
	};

	int k;
	for(int i=0; i<len; ++i)
	{
		char ch = m_text[i];
		if(ch != ' ')
		{
			D3DXVECTOR2 texCorner = GetCharPosition(ch);

			verts[0].u = texCorner.x+m_fontSize.x;
			verts[0].v = texCorner.y+m_fontSize.y;
			verts[1].u = texCorner.x;
			verts[1].v = texCorner.y+m_fontSize.y;
			verts[2].u = texCorner.x+m_fontSize.x;
			verts[2].v = texCorner.y;
			verts[3].u = texCorner.x;
			verts[3].v = texCorner.y;

			m_vertices.push_back(FTQuad(verts[0], verts[1], verts[2], verts[3]));
		}

		for(k=0;k<4;++k)
			verts[k].x -= w;
	}
}

void CFloatingText::OnPreRender(bool isShadowPass)
{
	if (isShadowPass)
		return;

	if(m_bVisible && m_text.size() > 0 && m_fontTex != NULL)
	{
		m_manager->RegisterNodeForRendering(this, SNG_ALPHA_BLENDED);
	}
	ISceneNode::OnPreRender(isShadowPass);
}

void CFloatingText::Render(bool isShadowPass)
{
	if(m_fontTex == NULL)
		return;

	int len = (int)m_text.size();
	if(len < 1)
		return;

	if(m_timer < 0.f && m_timer > -100.f)
		return;
	if(m_timer > 0.f)
	{
		if(m_timer < m_fadeTime)
		{
			DWORD alpha;
			alpha = ((int)(m_timer * m_onePerFadeTime * 255.f))&0xFF;
			m_color = (m_color&0x00FFFFFF) | (alpha<<24);
		}
	}

	IDirect3DDevice9 *pd = GetDXApp()->GetDevice();

	D3DXMATRIX mat = ISceneManager::GetManager()->GetActiveCamera()->GetBillboardMatrix();
	mat._41 = m_absoluteTransformation._41;
	mat._42 = m_absoluteTransformation._42;
	mat._43 = m_absoluteTransformation._43;
	pd->SetTransform(D3DTS_WORLD, &mat);
	pd->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	GetDXApp()->SetTexture(m_fontTex);
	pd->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pd->SetTextureStageState(0, D3DTSS_CONSTANT, m_color);
	pd->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CONSTANT);
	pd->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	
	std::vector<FTQuad>::iterator it = m_vertices.begin();
	for(int i=0; i<len && it != m_vertices.end(); ++i)
	{
		if(m_text[i] != ' ')
		{
			pd->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, it->verts, sizeof(FTVertex));
			++it;
		}
	}
	pd->SetRenderState(D3DRS_ZENABLE, TRUE);
	pd->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
}

void CFloatingText::Update(float frameTime)
{
	if(m_timer > 0.f)
		m_timer -= frameTime;

	ISceneNode::Update(frameTime);
}

void CFloatingText::UpdateAbsolutePosition()
{
	//D3DXMatrixIdentity(&m_absoluteTransformation);
	if(m_parent)
	{
		m_absoluteTransformation = m_parent->GetAbsoluteTransformation();
		m_absoluteTransformation._41 += m_relativeTranslation.x;
		m_absoluteTransformation._42 += m_relativeTranslation.y;
		m_absoluteTransformation._43 += m_relativeTranslation.z;
	}
}

D3DXVECTOR2 CFloatingText::GetCharPosition(char ch)
{
	float tempx = 1.f - m_fontSize.x;

	std::vector<char>::iterator it = m_charmap.begin();
	float x = 0.f, y = 0.f;
	for(; it != m_charmap.end(); ++it)
	{
		if(*it == ch)
			return D3DXVECTOR2(x,y);

		x += m_fontSize.x;
		if(x >= tempx)
		{
			x = 0.f;
			y += m_fontSize.y;
		}
	}
	return D3DXVECTOR2(0.f, 0.f);
}

void CFloatingText::SetFadeTrigger(float time, float fadeTime)
{
	m_timer = m_triggerTime = time;
	m_fadeTime = fadeTime;
	m_onePerFadeTime = 1.f / m_fadeTime;
	m_color |= 0xFF000000;
}

void CFloatingText::DisableFadeTrigger()
{
	m_timer = -1000.f;
	m_color |= 0xFF000000;
}