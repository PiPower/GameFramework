#pragma once
#include "GraphicalObject.h"

class Entity : public GraphicalObject
{
public:
	using::GraphicalObject::GraphicalObject;
	void Collision(std::vector<GraphicalObject> Rectangles);
};

