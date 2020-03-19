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
		LIGHT,
        CANVAS,
		TRANSFORM2D,
		UI,
		TEXT
	};

	Component(ComponentType componentType) : owner(owner), type(componentType), UUID(pcg32_random()) {};
	Component(GameObject * owner, ComponentType componentType) : owner(owner), type(componentType), UUID(pcg32_random()) {};
	virtual ~Component() = default;

	virtual void Enable() { active = true; };
	virtual void Disable() { active = false; };
	virtual bool IsEnabled() const { return active; };


	virtual void Update() {};
	virtual void Delete() = 0;

	virtual void Save(Config& config) const = 0;
	virtual void Load(const Config &config) = 0;

	virtual ComponentType GetType() const { return type; };

	static ComponentType GetComponentType(unsigned int component_type_uint)
	{
		return ComponentType(component_type_uint);
	}

public:
	uint64_t UUID = 0;

	GameObject *owner = nullptr;
	ComponentType type;

protected:
	bool active = true;
};

#endif //_COMPONENT_H_

