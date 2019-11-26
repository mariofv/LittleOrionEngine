#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include "Component.h"
#include "ComponentTransform.h"

#include <GL/glew.h>

#include <vector>

class GameObject
{
public:
	GameObject();
	GameObject(const std::string name);
	~GameObject();

	void Update();

	void AddChild(GameObject *child);
	Component* CreateComponent(const Component::ComponentType type);

	void ShowGameObjectHierarchy();

private:
	const GLuint GetMaterialTexture(const int material_index);
	void ShowGameObjectActionsMenu(const std::string label);

public:
	std::string name = "";

	ComponentTransform *transform = nullptr;
	std::vector<Component*> components;// = std::vector<Component*>();

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;// = std::vector<GameObject*>();
};

#endif //_GAMEOBJECT_H_