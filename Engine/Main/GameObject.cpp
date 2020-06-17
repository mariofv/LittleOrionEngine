#include "GameObject.h"
#include "Application.h"
#include "EditorUI/Panel/PanelHierarchy.h"
#include "Helper/Config.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleAudio.h"
#include "Module/ModuleEffects.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScriptManager.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleTexture.h"
#include "Module/ModuleUI.h"
#include "Module/ModulePhysics.h"
#include "ResourceManagement/Resources/Texture.h"
#include "ResourceManagement/Resources/Prefab.h"

#include "Component/Component.h"
#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentCanvasRenderer.h"
#include "Component/ComponentEventSystem.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentParticleSystem.h"
#include "Component/ComponentLight.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentSpriteMask.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentText.h"
#include "Component/ComponentTransform.h"

#include <Brofiler/Brofiler.h>
#include <pcg_basic.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <algorithm>

GameObject::GameObject() : aabb(this), UUID(pcg32_random())
{
	CreateTransforms();
}

GameObject::GameObject(unsigned int UUID) : aabb(this),  UUID(UUID)
{
	CreateTransforms();
}

GameObject::GameObject(const std::string name) :
	name(name),
	aabb(this),
	UUID(pcg32_random())
{
	CreateTransforms();
}

GameObject::GameObject(const GameObject& gameobject_to_copy) :
	aabb(gameobject_to_copy.aabb),
	transform(gameobject_to_copy.transform),
	transform_2d(gameobject_to_copy.transform_2d),
	UUID(pcg32_random())
{
	transform.owner = this;
	transform_2d.owner = this;
	aabb.owner = this;
	*this << gameobject_to_copy;
}

GameObject& GameObject::operator<<(const GameObject& gameobject_to_copy)
{

	if(!is_prefab_parent && gameobject_to_copy.transform.modified_by_user)
	{
		transform.SetTranslation(gameobject_to_copy.transform.GetTranslation());
		transform.SetRotation(gameobject_to_copy.transform.GetRotation());
		//gameobject_to_copy.transform.modified_by_user = false;
	}

	transform.SetScale(gameobject_to_copy.transform.GetScale());
	CopyComponentsPrefabs(gameobject_to_copy);
	this->name = gameobject_to_copy.name;
	this->active = gameobject_to_copy.active;
	this->SetStatic(gameobject_to_copy.is_static);
	this->hierarchy_depth = gameobject_to_copy.hierarchy_depth;
	this->hierarchy_branch = gameobject_to_copy.hierarchy_branch;
	this->original_UUID = gameobject_to_copy.original_UUID;
	return *this;
}

void GameObject::Delete(std::vector<GameObject*>& children_to_remove)
{
	children_to_remove.push_back(this);
	if (!is_static)
	{
		App->space_partitioning->RemoveAABBTree(this);
	}

	if (parent != nullptr)
	{
		parent->RemoveChild(this);
	}

	for (int i = (components.size() - 1); i >= 0; --i)
	{
		components[i]->Delete();
		components[i] = nullptr;
	}

	for (int i = (children.size() - 1); i >= 0; --i)
	{
		children[i]->parent = nullptr;
		children[i]->Delete(children_to_remove);
	}

	if (is_prefab_parent)
	{
		prefab_reference->RemoveInstance(this);
	}
}
void GameObject::Duplicate(const GameObject& gameobject_to_copy)
{
	if (!is_prefab_parent && gameobject_to_copy.transform.modified_by_user)
	{
		transform.SetTranslation(gameobject_to_copy.transform.GetTranslation());
		transform.SetRotation(gameobject_to_copy.transform.GetRotationRadiants());
		//gameobject_to_copy.transform.modified_by_user = false;
	}
	transform.SetScale(gameobject_to_copy.transform.GetScale());
	transform_2d = gameobject_to_copy.transform_2d;
	CopyComponents(gameobject_to_copy);
	this->name = gameobject_to_copy.name;
	this->active = gameobject_to_copy.active;
	this->transform_2d_enabled = gameobject_to_copy.transform_2d_enabled;
	this->SetStatic(gameobject_to_copy.is_static);
	this->hierarchy_depth = gameobject_to_copy.hierarchy_depth;
	this->hierarchy_branch = gameobject_to_copy.hierarchy_branch;
	this->original_UUID = gameobject_to_copy.original_UUID;
	this->tag = gameobject_to_copy.tag;
	if(gameobject_to_copy.prefab_reference != nullptr && !gameobject_to_copy.is_prefab_parent)
	{
		this->original_UUID = 0;
		this->prefab_reference = nullptr;
	}


	return;
}

void GameObject::SetTransform(GameObject* game_object)
{
	transform.SetTranslation(game_object->transform.GetTranslation());
	transform.SetRotation(game_object->transform.GetRotationRadiants());
	transform.SetScale(game_object->transform.GetScale());
}

bool GameObject::IsEnabled() const
{
	return active;
}

void GameObject::SetEnabled(bool able)
{
	active = able;

	for(const auto& component : components)
	{
		(able) ? component->Enable() : component->Disable();
	}

	for(const auto& child : children)
	{
		child->SetEnabled(able);
	}
}

void GameObject::SetStatic(bool is_static)
{
	SetHierarchyStatic(is_static);
	App->space_partitioning->GenerateQuadTree(); // TODO: Check this. This could be called with ungenerated bounding boxes, resulting in a wrong quadtree.
	App->space_partitioning->GenerateOctTree();
}

void GameObject::SetHierarchyStatic(bool is_static)
{
	this->is_static = is_static;

	//AABBTree
	(is_static) ? App->space_partitioning->RemoveAABBTree(this) : App->space_partitioning->InsertAABBTree(this);

	for (const auto& child : children)
	{
		child->SetStatic(is_static);
	}
}

void GameObject::LoadTransforms(Config config)
{
	Config transform_config;
	config.GetChildConfig("Transform", transform_config);
	transform.owner = this;
	transform.Load(transform_config);

	Config transform_2d_config;
	config.GetChildConfig("Transform2D", transform_2d_config);
	transform_2d.owner = this;
	transform_2d.Load(transform_2d_config);
}

void GameObject::CreateTransforms()
{
	transform = ComponentTransform();
	transform.owner = this;

	transform_2d = ComponentTransform2D();
	transform_2d.owner = this;
}

bool GameObject::IsStatic() const
{
	return is_static;
}

bool GameObject::IsVisible(const ComponentCamera& camera) const
{
	ComponentMeshRenderer* mesh = static_cast<ComponentMeshRenderer*>(GetComponent(Component::ComponentType::MESH_RENDERER));
	if ((mesh != nullptr && !mesh->IsEnabled()) || !IsEnabled() || !camera.IsInsideFrustum(aabb.bounding_box))
	{
		return false;
	}
	return true;
}

void GameObject::PreUpdate()
{
	BROFILER_CATEGORY("GameObject PreUpdate", Profiler::Color::Green);

	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->type != Component::ComponentType::SCRIPT)
		{
			components[i]->PreUpdate();
		}

	}
}

ENGINE_API void GameObject::Update()
{
	BROFILER_CATEGORY("GameObject Update", Profiler::Color::Green);

	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->type != Component::ComponentType::SCRIPT)
		{
			components[i]->Update();
		}

	}
}

void GameObject::PostUpdate()
{
	BROFILER_CATEGORY("GameObject PostUpdate", Profiler::Color::Green);

	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->type != Component::ComponentType::SCRIPT)
		{
			components[i]->PostUpdate();
		}

	}
}

void GameObject::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	if (parent != nullptr)
	{
		config.AddUInt(parent->UUID, "ParentUUID");
	}
	config.AddString(name, "Name");
	config.AddString(tag, "Tag");

	config.AddBool(is_static, "IsStatic");
	config.AddBool(active, "Active");

	Config transform_config;
	transform.Save(transform_config);
	config.AddChildConfig(transform_config, "Transform");

	Config transform_2d_config;
	transform_2d.Save(transform_2d_config);
	config.AddChildConfig(transform_2d_config, "Transform2D");

	config.AddBool(transform_2d_enabled, "Transform2DEnabled");

	std::vector<Config> gameobject_components_config(components.size());
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		components[i]->Save(gameobject_components_config[i]);
	}
	config.AddChildrenConfig(gameobject_components_config, "Components");
}

void GameObject::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	assert(UUID != 0);

	config.GetString("Name", name, "GameObject");
	config.GetString("Tag", tag, "");

	uint64_t parent_UUID = config.GetUInt("ParentUUID", 0);
	GameObject* game_object_parent = App->scene->GetGameObject(parent_UUID);
	//assert(game_object_parent != nullptr);
	if (game_object_parent != nullptr && parent_UUID != 0)
	{
		game_object_parent->AddChild(this); //TODO: This should be in scene. Probably D:
	}

	SetStatic(config.GetBool("IsStatic", false));
	active = config.GetBool("Active", true);

	LoadTransforms(config);

	transform_2d_enabled = config.GetBool("Transform2DEnabled", false);

	std::vector<Config> gameobject_components_config;
	config.GetChildrenConfig("Components", gameobject_components_config);
	for (unsigned int i = 0; i < gameobject_components_config.size(); ++i)
	{
		uint64_t component_type_uint = gameobject_components_config[i].GetUInt("ComponentType", 0);
		assert(component_type_uint != 0);

		Component::ComponentType component_type = static_cast<Component::ComponentType>(component_type_uint);

		Component* created_component = nullptr;
		if (component_type == Component::ComponentType::COLLIDER) {
			ComponentCollider::ColliderType collider_type = static_cast<ComponentCollider::ColliderType>(gameobject_components_config[i].GetUInt("ColliderType", 0));
			created_component = CreateComponent(collider_type);
		}
		else
		{
			created_component = CreateComponent(component_type);
		}
		created_component->Load(gameobject_components_config[i]);
	}
}

void GameObject::SetParent(GameObject* new_parent)
{
	if (new_parent == parent)
	{
		return;
	}

	if (parent != nullptr)
	{
		parent->RemoveChild(this);
	}

	new_parent->AddChild(this);
}

void GameObject::AddChild(GameObject* child)
{
	if (child->parent != nullptr)
	{
		child->parent->RemoveChild(child);
	}

	child->parent = this;
	child->UpdateHierarchyDepth();
	child->UpdateHierarchyBranch();

	child->transform.ChangeLocalSpace(transform.GetGlobalModelMatrix());
	child->transform_2d.ChangeLocalSpace(transform_2d.GetGlobalModelMatrix());
	children.push_back(child);
}

void GameObject::RemoveChild(GameObject* child)
{
	std::vector<GameObject*>::iterator found = std::find(children.begin(), children.end(), child);
	if (found == children.end())
	{
		APP_LOG_ERROR("Inconsistent GameObject Tree.")
		return;
	}
	children.erase(found);
	child->parent = nullptr;
	child->hierarchy_depth = 0;
}

Component::ComponentType GameObject::GetTransformType() const
{
	if (transform_2d_enabled)
	{
		return Component::ComponentType::TRANSFORM2D;
	}
	else
	{
		return Component::ComponentType::TRANSFORM;
	}
}

void GameObject::SetTransform2DStatus(bool enabled)
{
	transform_2d_enabled = enabled;
}

ENGINE_API Component* GameObject::CreateComponent(const Component::ComponentType type)
{
	Component* created_component;
	switch (type)
	{
	case Component::ComponentType::AABB:
		created_component = new ComponentAABB(this);
		break;
	case Component::ComponentType::ANIMATION:
		created_component = App->animations->CreateComponentAnimation(this);
		break;

	case Component::ComponentType::CANVAS:
		created_component = App->ui->CreateComponentCanvas();
		break;

	case Component::ComponentType::CANVAS_RENDERER:
		created_component = App->ui->CreateComponentCanvasRenderer();
		break;

	case Component::ComponentType::CAMERA:
		created_component = App->cameras->CreateComponentCamera();
		break;

	case Component::ComponentType::EVENT_SYSTEM:
		created_component = App->ui->CreateComponentEventSystem();
		break;

	case Component::ComponentType::MESH_RENDERER:
		created_component = App->renderer->CreateComponentMeshRenderer();
		break;

	case Component::ComponentType::LIGHT:
		created_component = App->lights->CreateComponentLight();
		break;

	case Component::ComponentType::SCRIPT:
		created_component = App->scripts->CreateComponentScript();
		break;

	case Component::ComponentType::UI_BUTTON:
		created_component = App->ui->CreateComponentUI<ComponentButton>();
		break;

	case Component::ComponentType::UI_IMAGE:
		created_component = App->ui->CreateComponentUI<ComponentImage>();
		break;

	case Component::ComponentType::UI_SPRITE_MASK:
		created_component = App->ui->CreateComponentUI<ComponentSpriteMask>();
		break;

	case Component::ComponentType::UI_TEXT:
		created_component = App->ui->CreateComponentUI<ComponentText>();
		break;

	case Component::ComponentType::BILLBOARD:
		created_component = App->effects->CreateComponentBillboard();
		break;

	case Component::ComponentType::PARTICLE_SYSTEM:
		created_component = App->effects->CreateComponentParticleSystem();
		break;

	case Component::ComponentType::AUDIO_SOURCE:
		created_component = App->audio->CreateComponentAudioSource();
		break;
		
	default:
		APP_LOG_ERROR("Error creating component. Incorrect component type.");
		return nullptr;
	}
	created_component->owner = this;

	created_component->Init();

	components.push_back(created_component);

	if (created_component->Is2DComponent())
	{
		transform_2d_enabled = true;
	}

	return created_component;
}


ENGINE_API Component* GameObject::CreateComponent(const ComponentCollider::ColliderType collider_type)
{
	Component* created_component = App->physics->CreateComponentCollider(collider_type, this);
	components.push_back(created_component);
	return created_component;
}

void GameObject::RemoveComponent(Component* component_to_remove)
{
	const auto it = std::find(components.begin(), components.end(), component_to_remove);
	if (it != components.end())
	{
		component_to_remove->Delete();
		components.erase(it);
	}
}
void GameObject::RemoveComponent(uint64_t UUID)
{
	Component * component = GetComponent(UUID);
	if (component)
	{
		RemoveComponent(component);
	}
}
ENGINE_API Component* GameObject::GetComponent(const Component::ComponentType type) const
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == type)
		{
			return components[i];
		}
	}
	return nullptr;
}

ENGINE_API Component* GameObject::GetComponent(uint64_t UUID) const
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->UUID == UUID)
		{
			return components[i];
		}
	}
	return nullptr;
}

ENGINE_API ComponentScript* GameObject::GetComponentScript(const char* name) const
{
	for (unsigned int i = 0; i < components.size(); ++i)
	{
		if (components[i]->type == Component::ComponentType::SCRIPT)
		{
			ComponentScript* script = (ComponentScript* )components[i];
			if (script->name == name)
			{
				return script;
			}
		}
	}

	return nullptr;
}

Component* GameObject::GetComponent(const ComponentCollider::ColliderType collider_type) const
{
	for (auto component : components)
	{
		if (component->GetType() == Component::ComponentType::COLLIDER)
		{
			ComponentCollider* collider = static_cast<ComponentCollider*>(component);
			if (collider->collider_type == collider_type)
			{
				return collider;
			}
		}
	}
	return nullptr;
}

void GameObject::MoveUpInHierarchy() const
{
	std::vector<GameObject*>::iterator silbings_position = std::find(parent->children.begin(), parent->children.end(), this);
	if (silbings_position == parent->children.end())
	{
		APP_LOG_ERROR("Inconsistent GameObject Tree.")
		return;
	}

	std::vector<GameObject*>::iterator swapped_silbing = max(silbings_position - 1, parent->children.begin());
	std::iter_swap(silbings_position, swapped_silbing);
}

void GameObject::MoveDownInHierarchy() const
{
	std::vector<GameObject*>::iterator silbings_position = std::find(parent->children.begin(), parent->children.end(), this);
	if (silbings_position == parent->children.end())
	{
		APP_LOG_ERROR("Inconsistent GameObject Tree.")
		return;
	}

	std::vector<GameObject*>::iterator swapped_silbing = min(silbings_position + 1, parent->children.end() - 1);
	std::iter_swap(silbings_position, swapped_silbing);
}

bool GameObject::IsAboveInHierarchy(const GameObject &potential_child) const
{
	return (
		this->hierarchy_branch == potential_child.hierarchy_branch
		&& this->hierarchy_depth < potential_child.hierarchy_depth
	);
}

void GameObject::UpdateHierarchyDepth()
{
	hierarchy_depth = parent->hierarchy_depth + 1;
	for (unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->UpdateHierarchyDepth();
	}
}

void GameObject::UpdateHierarchyBranch()
{
	if (parent->hierarchy_branch == 0) // PARENT IS ROOT GAMEOBJECT
	{
		hierarchy_branch = App->editor->hierarchy->GetNextBranch();
	}
	else
	{
		hierarchy_branch = parent->hierarchy_branch;
	}

	for (unsigned int i = 0; i < children.size(); ++i)
	{
		children[i]->UpdateHierarchyBranch();
	}
}

int GameObject::GetHierarchyDepth() const
{
	return hierarchy_depth;
}

void GameObject::SetHierarchyDepth(int value)
{
	hierarchy_depth = value;
}

GameObject * GameObject::GetPrefabParent()
{
	GameObject *to_reimport = this;
	bool prefab_parent = is_prefab_parent;
	while (to_reimport && !prefab_parent)
	{
		to_reimport = to_reimport->parent;
		prefab_parent = to_reimport->is_prefab_parent;
	}
	return to_reimport;
}

void GameObject::UnpackPrefab()
{
	assert(!original_prefab);
	if (is_prefab_parent)
	{
		prefab_reference->RemoveInstance(this);
		is_prefab_parent = false;
	}
	prefab_reference = nullptr;
	original_UUID = false;
	for (const auto& child : children)
	{
		child->UnpackPrefab();
	}
}

void GameObject::CopyComponentsPrefabs(const GameObject& gameobject_to_copy)
{
	this->components.reserve(gameobject_to_copy.components.size());
	for (const auto& component : gameobject_to_copy.components)
	{
		component->modified_by_user = false;
		Component * my_component = GetComponent(component->type); //TODO: This doesn't allow multiple components of the same type
		if (my_component != nullptr && !my_component->modified_by_user)
		{
			component->Copy(my_component);
			my_component->owner = this;
		}
		else if (my_component == nullptr)
		{
			Component *copy = component->Clone(this->original_prefab);
			copy->owner = this;
			this->components.push_back(copy);
		}
	}
	RemoveComponentsCopying(gameobject_to_copy);
}

void GameObject::CopyComponents(const GameObject& gameobject_to_copy)
{
	this->components.reserve(gameobject_to_copy.components.size());
	for (const auto& component : gameobject_to_copy.components)
	{
		component->modified_by_user = false;
		//Component * my_component = GetComponent(component->type); //TODO: This doesn't allow multiple components of the same type
		Component* copy = nullptr;
		if(component->type == Component::ComponentType::SCRIPT)
		{
			copy = new ComponentScript(this, static_cast<ComponentScript*>(component)->name);
			ComponentScript* copied_script = static_cast<ComponentScript*>(copy);
			copied_script->name = static_cast<ComponentScript*>(component)->name;
		}
		else if (component->type == Component::ComponentType::COLLIDER)
		{
			copy = component->Clone(this, this->original_prefab);
		}
		else
		{
			copy = component->Clone(this->original_prefab);
		}
		copy->owner = this;
		this->components.push_back(copy);
	}

	RemoveComponentsCopying(gameobject_to_copy);
}

void GameObject::RemoveComponentsCopying(const GameObject & gameobject_to_copy)
{
	std::vector<Component*> components_to_remove;
	std::copy_if(
		components.begin(),
		components.end(),
		std::back_inserter(components_to_remove),
		[&gameobject_to_copy](const auto& component)
	{
		return gameobject_to_copy.GetComponent(component->type) == nullptr && !component->added_by_user;
	}
	);
	for (const auto& component : components_to_remove)
	{
		RemoveComponent(component);
	}
	this->aabb.GenerateBoundingBox();
}
