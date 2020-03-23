#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#define ENGINE_EXPORTS
#include "Globals.h"
#include "Component/Component.h"
#include "Component/ComponentAABB.h"
#include "Component/ComponentTransform.h"


#include <GL/glew.h>

class Prefab;
class ComponentCamera; 
class GameObject
{
public:
	GameObject();
	GameObject(unsigned int UUID);
	GameObject(const std::string name);
	~GameObject() = default;

	//Copy and move
	GameObject(const GameObject& gameobject_to_copy);
	GameObject(GameObject&& gameobject_to_move) = default;

	GameObject & operator=(const GameObject& gameobject_to_copy) = default;
	GameObject & operator<<(const GameObject& gameobject_to_copy);
	GameObject & operator=(GameObject&& gameobject_to_move) = default;

	bool IsEnabled() const;
	void SetEnabled(bool able);

	void SetStatic(bool is_static);
	bool IsStatic() const;

	bool IsVisible(const ComponentCamera& camera) const;
	ENGINE_API void Update();
	void Delete(std::vector<GameObject*>& children_to_remove);

	void Save(Config& config) const;
	void Load(const Config& config);

	void SetParent(GameObject* new_parent);
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);

	ENGINE_API Component* CreateComponent(const Component::ComponentType type);
	void RemoveComponent(Component* component);
	ENGINE_API Component* GetComponent(const Component::ComponentType type) const;
	ENGINE_API ComponentScript* GetComponentScript(std::string& name);

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
	void CopyComponents(const GameObject& gameobject_to_copy);

public:
	std::vector<Component*> components;
	std::string name = "";

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	uint64_t UUID = -1;
	ComponentAABB aabb;
	ComponentTransform transform;

	//TODO: Maybe move this to a component editor?
	// This should not be public. Public for now while implementing prefab.
	uint64_t original_UUID = 0; 
	bool is_prefab_parent = false;
	Prefab* prefab_reference = nullptr;
	bool original_prefab = false;
private:
	bool active = true;
	bool is_static = false;
	int hierarchy_depth = 0;
	int hierarchy_branch = 0;

	friend class PanelGameObject;
};

#endif //_GAMEOBJECT_H_