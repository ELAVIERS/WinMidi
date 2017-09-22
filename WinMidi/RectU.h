#pragma once

class RectU
{
public:
	RectU(unsigned short left = 0, unsigned short top = 0, unsigned short right = 0, unsigned short bottom = 0) :
		left(left), top(top), right(right), bottom(bottom) {}

	~RectU();

	unsigned short top, right, bottom, left;

	inline bool	Overlaps(unsigned short x, unsigned short y)
	{
		return x >= left && x <= right && y >= top && y <= bottom;
	};

	inline void Set(unsigned short left, unsigned short top, unsigned short right, unsigned short bottom)
	{
		this->left = left; this->top = top; this->right = right; this->bottom = bottom;
	};
};

