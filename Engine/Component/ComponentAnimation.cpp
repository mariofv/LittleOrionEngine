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

AnimController* ComponentAnimation::GetAnimController()
{
	return animation_controller;
}
ComponentAnimation::~ComponentAnimation()
{
	if (animation_controller)
	{
		delete animation_controller;
	}
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
	CloneBase(static_cast<Component*>(created_component));
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
	playing_clip->current_time = 0.0f;
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

ENGINE_API bool ComponentAnimation::IsOnState(const std::string& trigger)
{
	return animation_controller->IsOnState(trigger);
}

ENGINE_API float ComponentAnimation::GetCurrentClipPercentatge() const
{
	if (animation_controller->playing_clips[ClipType::ACTIVE].clip)
	{
		return math::Clamp01(float(animation_controller->playing_clips[ClipType::ACTIVE].current_time) / float(animation_controller->playing_clips[ClipType::ACTIVE].clip->animation_time));
	}

	return 0.0f;
}

ENGINE_API int ComponentAnimation::GetTotalAnimationTime() const
{
	for (auto& playing_clip : animation_controller->playing_clips)
	{
		if (!playing_clip.clip)
		{
			break;
		}

		return playing_clip.clip->animation_time;

	}
	return 0;
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
		animation_controller->GetClipTransform(skeleton, pose);
		mesh->UpdatePalette(pose);
		animation_controller->UpdateAttachedBones(skeleton->GetUUID(), pose);
	}
}

void ComponentAnimation::Delete()
{
	App->animations->RemoveComponentAnimation(this);
}

void ComponentAnimation::SpecializedSave(Config& config) const
{
	uint32_t state_machine_uuid = animation_controller->state_machine ? animation_controller->state_machine->GetUUID() : 0;
	config.AddUInt(state_machine_uuid, "StateMachineResource");
}

void ComponentAnimation::SpecializedLoad(const Config& config)
{
	uint32_t state_machine_uuid = config.GetUInt32("StateMachineResource", 0);
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

		if (!animation_controller->playing_clips[ClipType::ACTIVE].clip)
		{
			return;
		}
		std::shared_ptr<Animation>& animation = animation_controller->playing_clips[ClipType::ACTIVE].clip->animation;
		auto& skeleton_channels_joints_map = animation_controller->skeleton_channels_joints_map;
		for (auto& mesh : skinned_meshes)
		{
			auto & skeleton = mesh->skeleton->skeleton;
			size_t skeleton_uuid = mesh->skeleton->GetUUID();
			if (!animation || skeleton_channels_joints_map.find(skeleton_uuid) != skeleton_channels_joints_map.end())
			{
				break;
			}
			std::vector<std::pair<size_t, GameObject*>> meshes_joints_channels_map(skeleton.size());
			auto & channels = animation->keyframes[0].channels;
			for (size_t j = 0; j < skeleton.size(); ++j)
			{
				auto & joint = skeleton[j];
				auto it = std::find_if(channels.begin(), channels.end(), [&joint](const Animation::Channel& channel) {
					return channel.name == joint.name;
				});

				meshes_joints_channels_map[j].first = (it - channels.begin());
				GameObject* gameobject = nullptr;
				for (GameObject* child : mesh->owner->children)
				{
					if (child->name == joint.name && !gameobject)
					{
						assert(child->parent == mesh->owner);
						gameobject = child;
					}
				}
				meshes_joints_channels_map[j].second = gameobject;
			}
			skeleton_channels_joints_map[skeleton_uuid] = std::move(meshes_joints_channels_map);
	}
}
