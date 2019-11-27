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
	void Render() const;

	void AddChild(GameObject *child);
	void RemoveChild(GameObject *child);
	Component* CreateComponent(const Component::ComponentType type);

	void ShowPropertiesWindow();
	void ShowGameObjectHierarchy();

private:
	const GLuint GetMaterialTexture(const int material_index) const;
	void DragAndDrop(GameObject *game_object);
	void ShowGameObjectActionsMenu(const std::string label);

public:
	std::string name = "";

	ComponentTransform *transform = nullptr;
	std::vector<Component*> components;

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	int depth_in_hierarchy = 0;

private:
	bool active = true;
};

#endif //_GAMEOBJECT_H_