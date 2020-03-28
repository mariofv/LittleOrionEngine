#include "ComponentAnimation.h"

#include "Animation/AnimController.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentTransform.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleResourceManager.h"

ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::ANIMATION)
{
	animation_controller = new AnimController();
	SetAnimation(App->resources->Load<Animation>("Library/Metadata/38/3800295065"));
	
}

ComponentAnimation::ComponentAnimation(GameObject* owner) : Component(owner, ComponentType::ANIMATION)
{
	animation_controller = new AnimController();
	GetChildrenMeshes(owner);
}

Component* ComponentAnimation::Clone(bool original_prefab) const
{
	ComponentAnimation * created_component;
	if (original_prefab)
	{
		created_component = new ComponentAnimation();
	}
	else
	{
		created_component = App->animations->CreateComponentAnimation();
	}
	*created_component = *this;
	return created_component;
};

void ComponentAnimation::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentAnimation*>(component_to_copy) = *this;
}



void ComponentAnimation::Update()
{
	animation_controller->Update();

	if (animation_controller->playing)
	{
		UpdateBone(owner);
	}
}

void ComponentAnimation::Delete()
{
	App->animations->RemoveComponentAnimation(this);
}

void ComponentAnimation::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddUInt((uint64_t)type, "ComponentType");
	config.AddBool(active, "Active");

	config.AddString(animation_controller->animation->exported_file, "AnimationResource");
}

void ComponentAnimation::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	std::string animation_path;
	config.GetString("AnimationResource", animation_path, "");
	SetAnimation(App->resources->Load<Animation>(animation_path));
}

void ComponentAnimation::SetAnimation(std::shared_ptr<Animation>& animation)
{
	animation_controller->animation = animation;
	animation_controller->Init();
}


void ComponentAnimation::UpdateBone(GameObject* current_bone)
{
	float3 bone_position;
	if (animation_controller->GetTranslation(current_bone->name, bone_position))
	{
		current_bone->transform.SetTranslation(bone_position);
	}

	Quat bone_rotation;
	if (animation_controller->GetRotation(current_bone->name, bone_rotation))
	{
		current_bone->transform.SetRotation(bone_rotation.ToFloat3x3());
	}
	for (auto& mesh : skinned_meshes) 
	{
		mesh->UpdatePalette(*current_bone);
	}
	for (auto& children_bone : current_bone->children)
	{
		UpdateBone(children_bone);
	}

}

void ComponentAnimation::GetChildrenMeshes(GameObject* current_mesh_gameobject)
{
	ComponentMeshRenderer* mesh_renderer = static_cast<ComponentMeshRenderer*>(current_mesh_gameobject->GetComponent(ComponentType::MESH_RENDERER));
	if (mesh_renderer)
	{
		skinned_meshes.push_back(mesh_renderer);
	}
	
	for (auto& child_gameobject : current_mesh_gameobject->children)
	{
		GetChildrenMeshes(child_gameobject);
	}
}