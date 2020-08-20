#pragma once
#include "GraphicalObject.h"
#include "Window.h"

class Entity : public GraphicalObject
{
	enum LastDirection
	{
		Right,
		Standing,
		Left
	};
public:
	using::GraphicalObject::GraphicalObject;
	void UpdatePos(Window* wnd,float Time);
	void Collision();
private:
	float VelX= 0.2;
	float VelY= 0.2;
	float Timer = 0;
	float FrameChange = 0.05;
	LastDirection  LastDir;
};

