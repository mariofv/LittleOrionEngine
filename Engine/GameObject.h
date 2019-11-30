#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include "Component/Component.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentAABBCollider.h"
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

	void SetParent(GameObject *new_parent);
	GameObject* CreateChild(std::string name = "");
	void AddChild(GameObject *child);
	void RemoveChild(GameObject *child);

	Component* CreateComponent(const Component::ComponentType type);
	Component* GetComponent(const Component::ComponentType type) const;

	void MoveUpInHierarchy();
	void MoveDownInHierarchy();
	bool IsAboveInHierarchy(const GameObject &potential_child) const;
	void UpdateHierarchyDepth();
	void UpdateHierarchyBranch();

	void ShowPropertiesWindow();

private:
	const GLuint GetMaterialTexture(const int material_index) const;

public:
	std::string name = "";

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;


	ComponentTransform transform;
	ComponentAABBCollider aabb_collider;

	std::vector<Component*> components;

private:
	bool active = true;
	int hierarchy_depth = 0;
	int hierarchy_branch = 0;
};

#endif //_GAMEOBJECT_H_