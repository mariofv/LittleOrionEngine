#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "Globals.h"
#include "Component/Component.h"
#include "Component/ComponentAABB.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentTransform2D.h"

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
	void CopyParameters(const GameObject & gameobject_to_copy);
	GameObject& operator=(GameObject&& gameobject_to_move) = default;

	void Duplicate(const GameObject& gameobject_to_copy, GameObject* parent);

	void SetTransform(GameObject* game_object);

	ENGINE_API bool IsEnabled() const;
	ENGINE_API void SetEnabled(bool able);

	void SetStatic(bool is_static);
	bool IsStatic() const;

	ENGINE_API void PreUpdate();
	ENGINE_API void Update();
	ENGINE_API void PostUpdate();

	void Save(Config& config) const;
	void Load(const Config& config);
	void Delete(std::vector<GameObject*>& children_to_remove);

	void SetParent(GameObject* new_parent);
	void AddChild(GameObject* child);
	void RemoveChild(GameObject* child);

	Component::ComponentType GetTransformType() const;
	void SetTransform2DStatus(bool enabled);

	ENGINE_API Component* CreateComponent(const Component::ComponentType type);
	ENGINE_API Component* CreateComponent(const ComponentCollider::ColliderType collider_type);
	void RemoveComponent(Component* component);
	void RemoveComponent(uint64_t UUID);
	ENGINE_API Component* GetComponent(const Component::ComponentType type) const;
	ENGINE_API Component * GetComponent(uint64_t UUID) const;
	ENGINE_API ComponentScript* GetComponentScript(const char* name) const;
	ENGINE_API Component* GetComponent(const ComponentCollider::ColliderType collider_type) const;

	void MoveUpInHierarchy() const;
	void MoveDownInHierarchy() const;
	bool IsAboveInHierarchy(const GameObject &potential_child) const;
	void UpdateHierarchyDepth();
	void UpdateHierarchyBranch();

	// Returns the first GO with specified tag that is under the hierarchy of this GO
	ENGINE_API GameObject* GetChildrenWithTag(const std::string& tag);

	// Returns the first GO with specified name that is under the hierarchy of this GO
	ENGINE_API GameObject* GetChildrenWithName(const std::string& name);

	int GetHierarchyDepth() const;
	void SetHierarchyDepth(int value);

	bool IsVisible(const ComponentCamera& camera) const;
	bool IsVisible(const Frustum& frustum) const;

	//Prefabs
	GameObject * GetPrefabParent();
	void UnpackPrefab();
	void Reassign();

private:
	void SetHierarchyStatic(bool is_static);

	void LoadTransforms(Config config);
	void CreateTransforms();
	void CopyComponents(const GameObject& gameobject_to_copy);
	void RemoveComponentsCopying(const GameObject& gameobject_to_copy);

public:
	std::string name = "";
	uint64_t UUID = -1;
	std::string tag = ""; // Please don't modify this parameter in a script, always use the editor!
	
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;

	std::vector<Component*> components;

	ComponentTransform transform;
	ComponentTransform2D transform_2d;

	ComponentAABB aabb;

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

	bool transform_2d_enabled = false;

	int hierarchy_depth = 0;
	int hierarchy_branch = 0;

	friend class PanelGameObject;
};

#endif //_GAMEOBJECT_H_
