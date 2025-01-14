#pragma once
#include <vector>
#include <stdio.h>

#include "engine/Entity.h"
//#include <glm/glm.h>

class Component;
class GameObject 
{
public:
	GameObject() = delete;
	GameObject(Entity& entity);
	~GameObject();

	virtual void init() = 0;
	void update();
	Entity& getEntity();

private:
	Entity& entity;
	std::vector<Component> componentList;

};

