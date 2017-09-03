#pragma once
class Note
{
public:
	Note();
	~Note();

	void render() {};

	float start, end;
	unsigned char pitch;
};

