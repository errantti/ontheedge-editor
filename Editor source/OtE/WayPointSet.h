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

#ifndef __CWAYPOINTSET_H__
#define __CWAYPOINTSET_H__

#include "Edgewalker\\DirectX9\\DirectX9.h"
#include <vector>
#include <string>

#define WAYPOINT_RECOGNIZION_THRESHOLD	3.f

class CWayPointSet  
{
public:
	CWayPointSet();
	~CWayPointSet();

	D3DXVECTOR3 GetClosestWaypoint(D3DXVECTOR3 pos, int *outIndex = NULL);
	D3DXVECTOR3 GetNextWaypoint(int &currentIndex);
	// Gives the next waypoint by determining which is the closest waypoint
	// to the current position
	D3DXVECTOR3 GetNextWaypoint(D3DXVECTOR3 &pos);

	D3DXVECTOR3 GetCurrentWaypoint(int index);
	D3DXVECTOR3 GetFirstWaypoint();

	inline vector<D3DXVECTOR3> *GetWaypoints() {return &m_waypoints;}
	inline string GetName() {return m_name;}
	inline void SetName(string name) {m_name = name;}

	inline const int GetID() const {return m_ID;}
	inline void SetID(int id) {m_ID = id;}

	inline void AddWaypoint(D3DXVECTOR3 waypoint) {m_waypoints.push_back(waypoint);}
	inline int GetWaypointCount() {return (int)m_waypoints.size();}

	void RemoveLastWaypoint();

	void Render(bool renderWithZBuffer = false);

protected:
	vector<D3DXVECTOR3> m_waypoints;
	string m_name;
	int m_ID;
};

#endif // #ifndef __CWAYPOINTSET_H__
