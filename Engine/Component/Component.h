#ifndef _COMPONENT_H_
#define _COMPONENT_H_

class GameObject;

class Component
{
public:
	enum class ComponentType
	{
		MATERIAL,
		MESH,
		TRANSFORM,
		AABBCOLLIDER
	};

	Component(GameObject * owner, ComponentType componentType) : owner(owner), type(componentType) {};
	virtual ~Component() = default;

	virtual void Enable() = 0;
	virtual void Disable() = 0;
	virtual void Update() = 0;

	virtual ComponentType GetType() const { return type; };

	virtual void ShowComponentWindow() = 0;

public:
	GameObject *owner = nullptr;
	ComponentType type;

protected:
	bool active = true;
};

#endif //_COMPONENT_H_

