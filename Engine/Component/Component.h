#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Helper/Config.h"

#include <pcg_basic.h>

class GameObject;

class Component
{
public:
	enum class ComponentType
	{
		AABB,
		CAMERA,
		MATERIAL,
		MESH,
		TRANSFORM,
		LIGHT
	};

	Component(GameObject * owner, ComponentType componentType) : owner(owner), type(componentType), UUID(pcg32_random()) {};
	virtual ~Component() = default;

	//Copy and move
	Component(const Component& component_to_copy)
	{
		*this = component_to_copy;
	}
	Component(Component&& component_to_move)
	{
		*this = std::move(component_to_move);
	}

	virtual Component & operator=(const Component & component_to_copy)
	{
		this->active = component_to_copy.active;
		this->UUID = component_to_copy.UUID;
		this->owner = component_to_copy.owner;
		this->type = component_to_copy.type;
		return *this;
	}
	virtual Component & operator=(Component && component_to_copy)
	{

		this->active = component_to_copy.active;
		this->UUID = component_to_copy.UUID;
		component_to_copy.UUID = 0;

		this->owner = component_to_copy.owner;
		component_to_copy.owner = nullptr;
		this->type = component_to_copy.type;
		return *this;
	}

	virtual void Enable() { active = true;};
	virtual void Disable() { active = false;};
	virtual bool IsEnabled() const { return active; };


	virtual void Update() {};
	virtual void Delete() = 0;
	virtual Component* Clone(bool original_prefab = false) const = 0;
	virtual void Copy(Component * component_to_copy) const = 0;

	virtual void Save(Config& config) const = 0;
	virtual void Load(const Config &config) = 0;

	virtual ComponentType GetType() const { return type; };

	static ComponentType GetComponentType(unsigned int component_type_uint)
	{
		switch (component_type_uint) 
		{
		case 0:
			return ComponentType::AABB;
		case 1:
			return ComponentType::CAMERA;
		case 2:
			return ComponentType::MATERIAL;
		case 3:
			return ComponentType::MESH;
		case 4:
			return ComponentType::TRANSFORM;
		case 5:
			return ComponentType::LIGHT;
		}
	}

public:
	uint64_t UUID = 0;

	GameObject *owner = nullptr;
	ComponentType type;

	bool modified_by_user = false; //This is only for prefab and UI
	bool added_by_user = false; //This is only for prefab and UI
protected:
	bool active = true;
};

#endif //_COMPONENT_H_

