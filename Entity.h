#pragma once
#include "GraphicalObject.h"
#include "Window.h"

class Entity : public GraphicalObject
{
public:
	using::GraphicalObject::GraphicalObject;
	void UpdatePos(Window* wnd);
	void Collision(std::vector<GraphicalObject> Rectangles);
private:
	float VelX= 0.03;
	float VelY= 0.05;
};

