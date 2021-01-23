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

#ifndef __IDIRECTXRESOURCE_H__
#define __IDIRECTXRESOURCE_H__

#include <vector>
using namespace std;

class IDirectXResource
{
public:
	IDirectXResource() {m_resources.push_back(this);}
	virtual ~IDirectXResource()
	{
		vector<IDirectXResource*>::iterator it = m_resources.begin();
		for(; it != m_resources.end(); ++it)
			if(*it == this)
			{
				m_resources.erase(it);
				return;
			}
	}

	/* Iterate through the vector and update all instances */
	static void UpdateOnDeviceLost()
	{
		vector<IDirectXResource*>::iterator it = m_resources.begin();
		for(; it != m_resources.end(); ++it)
			(*it)->OnDeviceLost();
	}
	/* Iterate through the vector and update all instances */
	static void UpdateOnDeviceReset()
	{
		vector<IDirectXResource*>::iterator it = m_resources.begin();
		for(; it != m_resources.end(); ++it)
			(*it)->OnDeviceReset();
	}

	/**
	 * Pure virtual functions, usually it's enough to call 
	 * corresponding function to the directx interface
	 **/
	virtual void OnDeviceLost() = 0;
	virtual void OnDeviceReset() = 0;

private:
	/// pointers to all the instances of this class
	static vector<IDirectXResource*> m_resources;
};

#endif // #ifndef __IDIRECTXRESOURCE_H__
