#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#define ENGINE_EXPORTS
#include "Globals.h"
#include "Component/Component.h"
#include "Component/ComponentAABB.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentTransform2D.h"
#include "Component/ComponentUI.h"


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

	GameObject& operator=(const GameObject& gameobject_to_copy) = default;
	GameObject& operator<<(const GameObject& gameobject_to_copy);
	GameObject& operator=(GameObject&& gameobject_to_move) = default;

	void Duplicate(const GameObject& gameobject_to_copy);

	void SetTransform(GameObject* game_object);

	ENGINE_API bool IsEnabled() const;
	ENGINE_API void SetEnabled(bool able);

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
	ENGINE_API Component* CreateComponent(const ComponentUI::UIType ui_type);
	ENGINE_API Component* CreateComponent(const ComponentCollider::ColliderType collider_type);
	void RemoveComponent(Component* component);
	ENGINE_API Component* GetComponent(const Component::ComponentType type) const;
	ENGINE_API ComponentScript* GetComponentScript(const char* name) const;
	ENGINE_API Component* GetComponent(const ComponentUI::UIType ui_type) const;
	ENGINE_API Component* GetComponent(const ComponentCollider::ColliderType collider_type) const;

	void MoveUpInHierarchy() const;
	void MoveDownInHierarchy() const;
	bool IsAboveInHierarchy(const GameObject &potential_child) const;
	void UpdateHierarchyDepth();
	void UpdateHierarchyBranch();

	int GetHierarchyDepth() const;
	void SetHierarchyDepth(int value);

	//Prefabs
	GameObject * GetPrefabParent();
	void UnpackPrefab();

private:
	void SetHierarchyStatic(bool is_static);
	Config SaveTransform() const;
	Config SaveTransform2D() const;
	void LoadTransforms(Config config);
	void CreateTransforms();
	void CopyComponentsPrefabs(const GameObject & gameobject_to_copy);
	void CopyComponents(const GameObject& gameobject_to_copy);
	void RemoveComponentsCopying(const GameObject& gameobject_to_copy);


public:
	std::vector<Component*> components;
	std::string name = "";

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	uint64_t UUID = -1;
	ComponentAABB aabb;
	ComponentTransform transform;
	ComponentTransform2D transform_2d;

	//TODO: Maybe move this to a component editor?
	// This should not be public. Public for now while implementing prefab.
	uint64_t original_UUID = 0; 
	bool is_prefab_parent = false;
	std::shared_ptr<Prefab> prefab_reference = nullptr;
	bool original_prefab = false;
	bool modified_by_user = false;

private:
	bool active = true;
	bool is_static = false;
	int hierarchy_depth = 0;
	int hierarchy_branch = 0;

	friend class PanelGameObject;
};

#endif //_GAMEOBJECT_H_