#include "mat4f.h"

mat4f::mat4f()
{
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			if (c == r) _data[c][r] == 1;
			else _data[c][r] = 0;
}

void mat4f::ortho(int w, int h)
{
	//scaling
	_data[0][0] = 2.f / w; //2/r-l
	_data[1][1] = 2.f / h; //2/t-b
	_data[2][2] = 0.f; //no z: should be 2/f-n
	_data[3][3] = 1.f; //Don't scale w

	//translation
	_data[3][0] = -w / 2.f; //-(r+l)/r-l
	_data[3][1] = -h / 2.f; //-(t+b)/t-b
	_data[3][2] = 0; //no z: -(f+n)/f-n
}

void mat4f::translate(int x, int y)
{
	_data[3][0] += x;
	_data[3][1] += y;
}


