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
		TRANSFORM
	};

	Component() = default;
	virtual ~Component() {};

	virtual void Enable() = 0;
	virtual void Disable() = 0;
	virtual void Update() = 0;

	virtual ComponentType GetType() const = 0;

public:
	GameObject *owner = nullptr;

private:
	bool active = true;
};

#endif //_COMPONENT_H_

