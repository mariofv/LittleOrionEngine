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
	if (animation_controller->state_machine)
	{
		GenerateJointChannelMaps();
	}
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
	
	auto & playing_clip = animation_controller->playing_clips[0];
	playing_clip.current_time = 0;
	playing_clip.playing = true;
	playing = true;
}

void ComponentAnimation::Stop()
{
	auto & playing_clip = animation_controller->playing_clips[0];
	playing_clip.playing = false;
	playing = false;
}

void ComponentAnimation::ActiveAnimation(const std::string & trigger)
{
	animation_controller->StartNextState(trigger);
}


void ComponentAnimation::Update()
{
	playing = animation_controller->Update();
	if (playing)
	{
		UpdateMeshes();
	}
}

void ComponentAnimation::UpdateMeshes()
{
	for (size_t i = 0; i < skinned_meshes.size(); i++)
	{
		auto & skeleton = skinned_meshes[i]->skeleton;
		std::vector<float4x4> pose(skeleton->skeleton.size());
		animation_controller->GetPose(skeleton->GetUUID(), pose);
		skinned_meshes[i]->UpdatePalette(pose);
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
	
	for (auto& clip : animation_controller->state_machine->clips)
	{
		for (auto& mesh : skinned_meshes)
		{
			auto & skeleton = mesh->skeleton;
			if (clip->animation && clip->skeleton_channels_joints_map.find(skeleton->GetUUID()) != clip->skeleton_channels_joints_map.end())
			{
				return;
			}
			auto & channels = clip->animation->keyframes[0].channels;
			std::vector<size_t> meshes_channels_joints_map(channels.size());
			for (size_t j = 0; j < channels.size(); ++j)
			{
				auto & channel = channels[j];
				auto it = std::find_if(skeleton->skeleton.begin(), skeleton->skeleton.end(), [&channel](const Skeleton::Joint & joint) {
					return channel.name == joint.name;
				});

				meshes_channels_joints_map[j] = (it - skeleton->skeleton.begin());
			}
			clip->skeleton_channels_joints_map[skeleton->GetUUID()] = std::move(meshes_channels_joints_map);
		}
	}
}
