#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include "Component/Component.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentAABB.h"
#include <GL/glew.h>

#include <vector>

class ComponentCamera;

class GameObject
{
public:
	GameObject();
	GameObject(const std::string name);
	~GameObject();

	bool IsEnabled() const;

	void Update();

	void SetParent(GameObject *new_parent);
	void AddChild(GameObject *child);
	void RemoveChild(GameObject *child);

	Component* CreateComponent(const Component::ComponentType type);
	Component* GetComponent(const Component::ComponentType type) const;

	void MoveUpInHierarchy();
	void MoveDownInHierarchy();
	bool IsAboveInHierarchy(const GameObject &potential_child) const;
	void UpdateHierarchyDepth();
	void UpdateHierarchyBranch();

	const GLuint GetMaterialTexture(const int material_index) const;

	void ShowPropertiesWindow();

public:
	std::string name = "";

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;


	ComponentTransform transform;
	ComponentAABB aabb;

	std::vector<Component*> components;

private:
	bool active = true;
	bool is_static = false;
	int hierarchy_depth = 0;
	int hierarchy_branch = 0;
};

#endif //_GAMEOBJECT_H_