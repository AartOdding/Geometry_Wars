#include "engine/Entity.h"
#include "engine/Component.h"

Entity::Entity(std::size_t _id) :id(_id), changedComponentFlag(true), alive(true)
{
}

Entity::~Entity()
{

}

void Entity::update() {
	for (auto& c : componentList) {
		c->execute();
	}
}
void Entity::draw() {}

void Entity::setChangedComponent() {}
bool Entity::changedComponents() {return true;}

//Make sure this is called after update. Else undefined behaviour may occur
//If flag of changedComponent is not set, the entity does not have to update
//Willen we het op deze manier doen? Nu kunnen we components niet tijdelijk inactief maken
void Entity::cleanUp() {
	if (!changedComponentFlag) {
		return;
	}
	else if (removeAll) {
		containsComponents.reset();

		componentList.clear();
		removeAll = false;
	}
	for (int i = 0; i < componentList.size() && !componentList.empty();) {
		if (componentList.at(i)->remove) {
			containsComponents[componentList.at(i)->id] = false;
			componentList.at(i).swap(componentList.at(componentList.size() - 1));
			componentList.pop_back();
		}
		else {
			i++;
		}
	}
	changedComponentFlag = false;
}

void Entity::removeAllComponents()
{
	removeAll = true;
	changedComponentFlag = true;
}

bool Entity::isActive() {
	return alive;
}

/*
void removeComponent(Component * component) {
	//if()
}

Ga hier ff uitzoeken hoe templates nu precies werken

/*
// template gedoe
void Entity::addComponent<T>(Component * component {

	// static assert check: std::is_base_of<>

	ComponentArray<staticCast<T> component, 1>;
}

void Entity::removeComponent() {
	//check if type
	//check if excists
	//set flag to false;
	//remove component
}*/