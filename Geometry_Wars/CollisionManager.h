#pragma once

#include <map>
#include <vector>
#include <algorithm>
#include "CollideComponent.h"

constexpr auto TREESLICE = 100;

//
//PLAYER,ENEMY,BULLET,ENEMYBULLET,POWERUP
const std::vector<std::vector<int>> collisionTable = { {1,3},{/*0,*/2},{1},{0},{0} };


struct CollisionEvent {
	CollideComponent * entity1;
	CollideComponent * entity2;
};

class CollisionManager{
public:
	CollisionManager();
	~CollisionManager();
	void RegisterEntity(CollideComponent * entity);
	void Update();
	bool can_collide(CollideMask c1, CollideMask c2);

private:
	
	void CleanUp();
	void ResolveCollisions();
	void RemoveDupplicateCollisions(); // is deze niet overbodig?
	
	std::vector<CollideComponent*> entities;

	//std::map<int, std::map<int, CollideComponent *>> collideTree;
	std::vector<std::vector<CollideComponent *>> sortedCollideComponents;
	std::vector<CollisionEvent> collisions;
	
};