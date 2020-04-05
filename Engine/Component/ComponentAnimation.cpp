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

void ComponentAnimation::Init()
{
	GetChildrenMeshes(owner);
	GenerateJointChannelMaps();
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

void ComponentAnimation::SetStateMachine(std::shared_ptr<StateMachine>& state_machine)
{
	animation_controller->state_machine = state_machine;
	animation_controller->SetActiveAnimation();
	GenerateJointChannelMaps();
}

void ComponentAnimation::Play()
{
	current_time = 0;
	playing = true;
}

void ComponentAnimation::Stop()
{
	playing = false;
}


void ComponentAnimation::Update()
{
	if (!playing)
	{
		return;
	}

	current_time = current_time + static_cast<int>(App->time->delta_time);
	if (current_time >= animation_controller->animation_time)
	{
		if (animation_controller->loop)
		{

			current_time = current_time % animation_controller->animation_time;
		}
		else
		{
			playing = false;
		}
	}

	if (playing)
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

	std::string state_machine_path = animation_controller->state_machine ? animation_controller->state_machine->exported_file : "";
	config.AddString(state_machine_path, "StateMachineResource");
}

void ComponentAnimation::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	std::string state_machine_path;
	config.GetString("StateMachineResource", state_machine_path, "");
	if (!state_machine_path.empty())
	{
		SetStateMachine(App->resources->Load<StateMachine>(state_machine_path));
	}
}

void ComponentAnimation::UpdateBone(GameObject* current_bone)
{
	float3 bone_position;
	Quat bone_rotation;
	if (animation_controller->GetTransform(current_time,current_bone->name, bone_position, bone_rotation))
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

void ComponentAnimation::GenerateJointChannelMaps()
{
	meshes_channels_joints_map.resize(skinned_meshes.size());
	if (animation_controller->GetCurrentAnimation() == nullptr)
	{
		return;
	}
	auto & channels = animation_controller->GetCurrentAnimation()->keyframes[0].channels;
	for (size_t i = 0; i < skinned_meshes.size(); ++i)
	{
		auto & skeleton = skinned_meshes[i]->skeleton;
		meshes_channels_joints_map[i].resize(channels.size());
		for (size_t j = 0; j < channels.size() ; ++j)
		{
			auto & channel = channels[j];
			auto it = std::find_if(skeleton->skeleton.begin(), skeleton->skeleton.end(), [&channel](const Skeleton::Joint & joint) {
				return channel.name == joint.name;
			});

			meshes_channels_joints_map[i][j] = (it - skeleton->skeleton.begin());
		}
	}
}
