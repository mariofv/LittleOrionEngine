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

#include <Brofiler/Brofiler.h>

ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::ANIMATION)
{
	animation_controller = new AnimController();
}

ComponentAnimation::ComponentAnimation(GameObject* owner) : Component(owner, ComponentType::ANIMATION)
{
	animation_controller = new AnimController();
	Init();
}

void ComponentAnimation::Init()
{
	skinned_meshes.clear();
	GetChildrenMeshes(owner);
	if (animation_controller->state_machine)
	{
		GenerateJointChannelMaps();
	}
}

ComponentAnimation & ComponentAnimation::operator=(const ComponentAnimation & component_to_copy)
{
	Component::operator = (component_to_copy);
	*this->animation_controller = *component_to_copy.animation_controller;
	Init();
	return *this;
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

void ComponentAnimation::Disable()
{
	active = false;
	Stop();
}

void ComponentAnimation::SetStateMachine(uint32_t state_machine_uuid)
{
	animation_controller->SetStateMachine(state_machine_uuid);
	GenerateJointChannelMaps();
}

void ComponentAnimation::Play()
{
	PlayingClip * playing_clip = &animation_controller->playing_clips[0];
	if (!playing_clip->clip)
	{
		return;
	}
	playing_clip->current_time = 0;
	playing_clip->playing = true;
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
	if (!active)
	{
		return;
	}

	playing = animation_controller->Update();
	if (playing)
	{
		UpdateMeshes();
	}
}

void ComponentAnimation::UpdateMeshes()
{
	BROFILER_CATEGORY("Animation", Profiler::Color::PaleGoldenRod);
	if (!animation_controller->state_machine)
	{
		return;
	}
	for (auto & mesh : skinned_meshes)
	{
		pose.resize(mesh->skeleton->skeleton.size());
		auto & skeleton = mesh->skeleton;
		animation_controller->GetClipTransform(skeleton->GetUUID(), pose);
		mesh->UpdatePalette(pose);
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

	uint32_t state_machine_uuid = animation_controller->state_machine ? animation_controller->state_machine->GetUUID() : 0;
	config.AddUInt(state_machine_uuid, "StateMachineResource");
}

void ComponentAnimation::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	uint32_t state_machine_uuid = config.GetUInt("StateMachineResource", 0);
	if (state_machine_uuid != 0)
	{
		SetStateMachine(state_machine_uuid);
	}
}

void ComponentAnimation::GetChildrenMeshes(GameObject* current_mesh_gameobject)
{
	ComponentMeshRenderer* mesh_renderer = static_cast<ComponentMeshRenderer*>(current_mesh_gameobject->GetComponent(ComponentType::MESH_RENDERER));
	if (mesh_renderer && mesh_renderer->skeleton)
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
				break;
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
