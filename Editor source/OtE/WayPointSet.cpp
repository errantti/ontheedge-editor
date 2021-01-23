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

#include "WayPointSet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWayPointSet::CWayPointSet()
{
}

CWayPointSet::~CWayPointSet()
{
}

D3DXVECTOR3 CWayPointSet::GetClosestWaypoint(D3DXVECTOR3 pos, int *outIndex)
{
	float square = 10000000.f, current;
	float dx,dz;
	int index = -1;
	for(int i=0;i<m_waypoints.size();i++)
	{
		dx = m_waypoints[i].x-pos.x;
		dz = m_waypoints[i].z-pos.z;
		if((current = dx*dx + dz*dz) < square)
		{
			square = current;
			index = i;
		}
	}

	if(index == -1)
		return pos;
	else
	{
		if(outIndex != NULL)
			*outIndex = index;
		return m_waypoints[index];
	}
}

D3DXVECTOR3 CWayPointSet::GetNextWaypoint(int &currentIndex)
{
	if(currentIndex >= m_waypoints.size()-1)
		currentIndex = 0;
	else
		++currentIndex;

	if(m_waypoints.size()>0)
		return m_waypoints[currentIndex];
	else
		return D3DXVECTOR3(0.f,0.f,0.f);
}

D3DXVECTOR3 CWayPointSet::GetFirstWaypoint()
{
	if(m_waypoints.size() != 0)
		return m_waypoints[0];
	return D3DXVECTOR3(100.f, 100.f, 100.f);
}

D3DXVECTOR3 CWayPointSet::GetCurrentWaypoint(int index)
{
	if(m_waypoints.size() == 0)
		return D3DXVECTOR3(100.f, 100.f, 100.f);

	if(index >= m_waypoints.size()-1)
		index = 0;
	if(index < 0)
		index = 0;

	return m_waypoints[index];
}

void CWayPointSet::RemoveLastWaypoint()
{
	if(m_waypoints.size() > 0)
	{
		vector<D3DXVECTOR3>::iterator it = m_waypoints.begin();
		int size = (int)m_waypoints.size();
		for(int i=0; i<size-1; ++i, ++it);
		m_waypoints.erase(it);
	}
}

void CWayPointSet::Render(bool renderWithZBuffer)
{
	IDirect3DDevice9 *pd = GetDXApp()->GetDevice();

	if(m_waypoints.size() < 2)
	{
		// Nothing to render
		return;
	}
	
	// Render the waypoints as line leading from each waypoint to 
	// the next waypoint in the set

	DWORD fvfBack;
	pd->GetFVF(&fvfBack);
	pd->SetFVF(D3DFVF_XYZ);

	GetDXApp()->EnableLighting(FALSE);
	if(!renderWithZBuffer)
		GetDXApp()->EnableZBuffer(FALSE);
	pd->SetTexture(0, NULL);

	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	pd->SetTransform(D3DTS_WORLD, &m);

	pd->DrawPrimitiveUP(D3DPT_LINESTRIP, (int)m_waypoints.size()-1, 
		m_waypoints[0], sizeof(D3DXVECTOR3));

	pd->SetFVF(fvfBack);
	GetDXApp()->EnableLighting(TRUE);
	if(!renderWithZBuffer)
		GetDXApp()->EnableZBuffer(TRUE);
}