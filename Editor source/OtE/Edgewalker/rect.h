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

#ifndef ____RECT_H__
#define ____RECT_H__

template <class T>
class rect
{
public:
	rect() : left(0), right(0), top(0), bottom(0) {}

	rect(T Left, T Right, T Top, T Bottom)
		: left(Left), right(Right), top(Top), bottom(Bottom)
	{}

	rect(const rect<T> &r)
		: left(r.left), right(r.right), top(r.top), bottom(r.bottom)
	{}

	void repair()
	{
		T t;
		if(left>right)
		{
			t = left;
			left = right;
			right = t;
		}
		if(top>bottom)
		{
			t = bottom;
			bottom = top;
			top = t;
		}
	}

	inline bool IsPointInside(T x, T y)
	{
		if(x<left || x>right || y<top || y>bottom)
			return false;
		return true;
	}

	inline bool IntersectsWithRect(rect<T> r)
	{
		if(r.right < left || r.left > right)
			return false;
		if(r.top > bottom || r.bottom < top)
			return false;
		return true;
	}

	T getWidth() {return right-left;}
	T getHeight() {return bottom-top;}

	T left, right, top, bottom;
};

typedef rect<int> intRect;

#endif