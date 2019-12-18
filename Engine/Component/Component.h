#ifndef _COMPONENT_H_
#define _COMPONENT_H_

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

	Component(GameObject * owner, ComponentType componentType) : owner(owner), type(componentType) {};
	virtual ~Component() = default;

	virtual void Enable() { active = true; };
	virtual void Disable() { active = false; };
	virtual bool IsEnabled() const { return active; };

	virtual void Update() {};

	virtual ComponentType GetType() const { return type; };

	virtual void Delete() = 0;

	virtual void ShowComponentWindow() = 0;

public:
	GameObject *owner = nullptr;
	ComponentType type;

protected:
	bool active = true;
};

#endif //_COMPONENT_H_

