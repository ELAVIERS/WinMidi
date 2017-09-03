#pragma once

class mat4f
{
public:
	mat4f();
	~mat4f() {};

	void ortho(int w, int h);
	void translate(int x, int y);
	//void rotate();
	//void scale();
protected:
	float _data[4][4];
};

