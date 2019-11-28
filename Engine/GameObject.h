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

	void ChangeParent(GameObject *new_parent);
	GameObject* CreateChild(std::string name = "");
	void AddChild(GameObject *child);
	void RemoveChild(GameObject *child);

	Component* CreateComponent(const Component::ComponentType type);

	bool IsAboveInHierarchy(const GameObject &potential_child) const;
	void UpdateHierarchyDepth();
	void UpdateHierarchyBranch();

	void GenerateBoundingBox();

	void ShowPropertiesWindow();

private:
	const GLuint GetMaterialTexture(const int material_index) const;

public:
	std::string name = "";

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	
	AABB bounding_box;

	ComponentTransform *transform = nullptr;

	std::vector<Component*> components;

private:
	bool active = true;
	int hierarchy_depth = 0;
	int hierarchy_branch = 0;
};

#endif //_GAMEOBJECT_H_