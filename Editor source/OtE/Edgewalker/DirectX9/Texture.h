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

#ifndef __CTEXTURE_H__
#define __CTEXTURE_H__

#include <d3d9.h>
#include <string>
using namespace std;

class CTexture
{
public:
	CTexture();
	~CTexture();

	inline LPDIRECT3DTEXTURE9 GetTexture() {return m_texture;}
	bool LoadTexture(const char *filename, D3DCOLOR colorkey = 0);

	inline DWORD GetOriginalWidth() const {return m_iWidth;}
	inline DWORD GetOriginalHeight() const {return m_iHeight;}

	inline string &GetFilename() {return m_filename;}

	void *Lock();
	void Unlock();

	void Release();

	/*bool operator < (const CTexture *other) const
	{
		return (this < other);
	}*/

protected:
	LPDIRECT3DTEXTURE9 m_texture;

	DWORD m_iWidth, m_iHeight;	// Original size of the texture image
	string m_filename;
};

typedef CTexture *PTEXTURE;

#endif // #ifndef __CTEXTURE_H__
