#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include "Component.h"

#include <vector>

class GameObject
{
public:
	GameObject() = default;
	GameObject(const std::string name, const GameObject& parent);
	~GameObject();

	void Update();
	Component* CreateComponent(const Component::ComponentType type);

public:
	std::string name = "";
	std::vector<Component*> components;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
};

#endif //_GAMEOBJECT_H_