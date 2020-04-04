#include "ComponentAnimation.h"

#include "Animation/AnimController.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentTransform.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Resources/StateMachine.h"
ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::ANIMATION)
{
	animation_controller = new AnimController();
	
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
		UpdateBone(owner, -1, -1, -1);
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

	std::string state_machine_path = state_machine ? state_machine->exported_file : "";
	config.AddString(state_machine_path, "StateMachineResource");
}

void ComponentAnimation::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	std::string animation_path;
	config.GetString("AnimationResource", animation_path, "");
	SetAnimation(App->resources->Load<Animation>(animation_path));

	std::string state_machine_path;
	config.GetString("StateMachineResource", state_machine_path, "");
	if (!state_machine_path.empty())
	{
		state_machine = App->resources->Load<StateMachine>(state_machine_path);
	}
}

void ComponentAnimation::SetAnimation(std::shared_ptr<Animation>& animation)
{
	animation_controller->animation = animation;
	animation_controller->Init();
}


void ComponentAnimation::UpdateBone(GameObject* current_bone, int first_keyframe, int second_keyframe, float lambda)
{
	if (first_keyframe < 0)
	{
		animation_controller->GetKeyframes(first_keyframe, second_keyframe, lambda);
	}

	float3 bone_position;
	Quat bone_rotation;
	if (animation_controller->GetTransform(current_bone->name, bone_position, bone_rotation, first_keyframe,
		second_keyframe, lambda))
	{
		current_bone->transform.SetTranslation(bone_position);
		current_bone->transform.SetRotation(bone_rotation.ToFloat3x3());
	}

	for (auto& mesh : skinned_meshes) 
	{
		mesh->UpdatePalette(*current_bone);
	}
	for (auto& children_bone : current_bone->children)
	{
		UpdateBone(children_bone, first_keyframe, second_keyframe, lambda);
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