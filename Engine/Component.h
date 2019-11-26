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
	~Component() = default;;

	virtual void Enable() = 0;
	virtual void Disable() = 0;
	virtual void Update() = 0;

public:
	ComponentType ctype;
	bool active = false;
	GameObject *owner = nullptr;
};

#endif //_COMPONENT_H_

