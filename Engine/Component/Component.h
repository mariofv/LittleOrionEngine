#ifndef _COMPONENT_H_
#define _COMPONENT_H_

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
		TRANSFORM
	};

	Component(GameObject * owner, ComponentType componentType) : owner(owner), type(componentType), UUID(pcg32_random()) {};
	virtual ~Component() = default;

	virtual void Enable() = 0;
	virtual void Disable() = 0;
	virtual bool IsEnabled() const { return active; };

	virtual void Update() = 0;

	virtual ComponentType GetType() const { return type; };

	virtual void Delete() = 0;

	virtual void ShowComponentWindow() = 0;

public:
	unsigned int UUID = -1;

	GameObject *owner = nullptr;
	ComponentType type;

protected:
	bool active = true;
};

#endif //_COMPONENT_H_

