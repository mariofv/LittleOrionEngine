#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include "Component/Component.h"
#include "Component/ComponentTransform.h"

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
	void RemoveChild(const GameObject *child);
	Component* CreateComponent(const Component::ComponentType type);

	void ShowPropertiesWindow();
	void ShowGameObjectHierarchy();

private:
	const GLuint GetMaterialTexture(const int material_index);
	void ShowGameObjectActionsMenu(const std::string label);

public:
	std::string name = "";

	ComponentTransform *transform = nullptr;
	std::vector<Component*> components;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

private:
	bool active = true;
};

#endif //_GAMEOBJECT_H_