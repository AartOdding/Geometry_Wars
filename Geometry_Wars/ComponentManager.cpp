#include "ComponentManager.h"

#include "HealthComponent.h"


ComponentManager::ComponentManager()
{

}


ComponentManager::~ComponentManager()
{

}

ComponentID getComponentID() {
}


Component* ComponentManager::createHealthComponent(GameObject * gameObject) {
	Component * component = new HealthComponent(gameObject->getEntity());
	std::unique_ptr<Component> upointComponent(component);
	components.emplace_back(upointComponent);
}

/*
*Ook hiervoor eens goed kijken naar templates
*
Component& createComponent(Entity * entity) {
	Component * component = new Component(entity);
	std::unique_ptr<Entity> upointEntety(entity);
	entities.emplace_back(upointEntety);
}
*/