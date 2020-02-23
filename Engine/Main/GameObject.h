#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include "Component/Component.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentAABB.h"

#include <GL/glew.h>

class ComponentCamera; 
class GameObject
{
public:
	GameObject();
	GameObject(unsigned int UUID);
	GameObject(const std::string name);
	~GameObject() = default;

	bool IsEnabled() const;
	void SetEnabled(bool able);

	void SetStatic(bool is_static);
	bool IsStatic() const;

	bool IsVisible(const ComponentCamera & camera) const;
	void Update();
	void Delete(std::vector<GameObject*> & children_to_remove);

	void Save(Config& config) const;
	void Load(const Config& config);

	void SetParent(GameObject *new_parent);
	void AddChild(GameObject *child);
	void RemoveChild(GameObject *child);

	Component* CreateComponent(const Component::ComponentType type);
	void RemoveComponent(Component * component);
	Component* GetComponent(const Component::ComponentType type) const;

	void MoveUpInHierarchy() const;
	void MoveDownInHierarchy() const;
	bool IsAboveInHierarchy(const GameObject &potential_child) const;
	void UpdateHierarchyDepth();
	void UpdateHierarchyBranch();

	void RenderMaterialTexture(unsigned int shader_program) const;

	int GetHierarchyDepth() const;
	void SetHierarchyDepth(int value);

private:
	void SetHierarchyStatic(bool is_static);

public:
	std::vector<Component*> components;
	std::string name = "";

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	uint64_t UUID = -1;
	ComponentAABB aabb;
	ComponentTransform transform;


private:
	bool active = true;
	bool is_static = false;
	int hierarchy_depth = 0;
	int hierarchy_branch = 0;

	friend class PanelGameObject;
};

#endif //_GAMEOBJECT_H_