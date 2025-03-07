#pragma once

#include <bitset>
#include <array>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>
#include <typeinfo>


/*! \file Entity.h
	\brief The Entity in the entity component system. The Entity contains components and manages them after creation
*/

class Component;

/// def maxComponents
/**
* 	type constant to limit the amount of components that can be used by the game per entity
*/
constexpr auto maxComponents = 10;

//Array[Components] for indexation
//Vector<Components> for update
//Dubbele component managements, maar moet wel want anders is het allemaal kut
//Sterker nog 3 dubbelle check want ook bitset

/// def EntityID
/**
*	 type alias that ensures Entity ID's are always a type of std::size_t
*/
using EntityID = std::size_t;

/// def ComponentBitSet
/**
*	 type alias to keep a list of maxComponents bits. This is used to see if an entity contains a component
*/
using ComponentBitSet = std::bitset<maxComponents>;

///	def ComponentID
/**
*	type alias for an array that can contain all the possible componentTypes that exists
*/
using ComponentArray = std::array <Component *, maxComponents >;

class Entity
{
private:
	EntityID id;
	ComponentBitSet containsComponents;
	ComponentArray componentArray;
	bool changedComponentFlag;
	std::vector<std::unique_ptr<Component>> componentList;
	bool alive = true;
	bool removeAll = false;
public:

	Entity() = delete;
	/// Entity(EntityID id)
	/**
	*	Constructor of Entity
	*	@param id unique id generated by the EntityManager
	*/
	Entity(EntityID i);



	///  update()
	/**
	*	 Go over all components and call the execute function
	*/
	void update();

	void draw();

	/// ~EntityManager()
	/**
	*	 Destructor of Entity
	*/
	virtual ~Entity();

	///	setComponentFlag(Component *)
	/**
	*	sets the Componentflag of given component
	*
	*	@param Component that should be flagged.
	*/
	void setComponentFlag(Component *);


	///  setComponents(T, ...Args)
	/**
	* Template function that asserts a given type the function should run has Component as a base class. Creates a new component of given type with passes through parameters and adds it to the corresponding Entity variables
	*/
	template<typename T, typename... Args>
	void setComponent(Args&&... values) {
		static_assert(std::is_base_of<Component, T>::value);
		T * component = new T(std::forward<Args>(values)...);
		component->entity = this;
		auto componentID = EntityManager::template getLastComponentID<T>();
		component->id = componentID;
		componentArray[componentID] = component;
		containsComponents[componentID] = true;
		std::unique_ptr<Component> uniqueComponent(component);
		componentList.emplace_back(std::move(uniqueComponent));
	}

	/// getComponent<T>()
/**
*	 returns the component of Type T the entity contains
*
*	@param T *
*/
	template<typename T>
	T * getComponent() {
		static_assert(std::is_base_of<Component, T>::value);
		auto componentID = EntityManager::template getLastComponentID<T>();
		T * component = static_cast<T*>(componentArray[componentID]);
		return component;
	}

	///  hasComponent<T>()
	/** returns if the current entity contains a component of Type T
	*
	*	@param bool
	*/

	template<typename T>
	bool hasComponent() {
		static_assert(std::is_base_of<Component, T>::value);
		auto componentID = EntityManager::template getLastComponentID<T>();
		return containsComponents[componentID];
	}


	///removeComponent<T>()
	/**
	* template function that marks a component of Type T to be removed in the next clean up. Components cannot be removed directly because they may still be relevant in the current update loop
	*/
	template<typename T>
	void removeComponent() {
		static_assert(std::is_base_of<Component, T>::value);
		auto componentID = EntityManager::template getLastComponentID<T>();
		if (containsComponents[componentID]) {
			auto component = componentArray[componentID];
			component->remove = true;
			component->id = componentID;
			this->changedComponentFlag = true;
		}
	}

	template<typename T>
	bool removeFlaggedComponent() {
		static_assert(std::is_base_of<Component, T>::value);
		auto componentID = EntityManager::template getLastComponentID<T>();
		containsComponents[componentID] = false;
	}

	void removeAllComponents();
	/// setChangedComponent()
	/**
	*	 sets flag that one or more components are changed
	*/
	void setChangedComponent();
	bool changedComponents();
	void cleanUp();

	/// isActive()
	/**
	*	request if entity is still active
	*
	*	@param bool alive
	*/
	bool isActive();

};