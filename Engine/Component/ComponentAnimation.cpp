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
	if (!animation_controller->state_machine)
	{
		return;
	}
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
	return *this;
}

Component* ComponentAnimation::Clone(GameObject* owner, bool original_prefab)
{
	ComponentAnimation * created_component;
	if (original_prefab)
	{
		created_component = new ComponentAnimation(owner);
	}
	else
	{
		created_component = App->animations->CreateComponentAnimation();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));
	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
	created_component->Init();
	return created_component;
};

void ComponentAnimation::CopyTo(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentAnimation*>(component_to_copy) = *this;
	static_cast<ComponentAnimation*>(component_to_copy) ->Init();
}

void ComponentAnimation::Disable()
{
	active = false;
	Stop();
}

void ComponentAnimation::SetStateMachine(uint32_t state_machine_uuid)
{
	animation_controller->SetStateMachine(state_machine_uuid);
	if (animation_controller->state_machine)
	{
		GenerateJointChannelMaps();
	}
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

void ComponentAnimation::ActiveAnimation(const std::string& trigger)
{
	animation_controller->StartNextState(trigger);
}

void ComponentAnimation::SetActiveState(const std::string& state, float interpolation_time)
{
	animation_controller->SetActiveState(state, interpolation_time);
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

ENGINE_API float ComponentAnimation::GetTotalAnimationTime() const
{
	for (auto& playing_clip : animation_controller->playing_clips)
	{
		if (!playing_clip.clip)
		{
			break;
		}

		return playing_clip.clip->animation_time;

	}
	return 0.0f;
}

void ComponentAnimation::SetAnimationSpeed(float speed) const
{
	animation_controller->SetSpeed(speed);

	return;
}

ENGINE_API void ComponentAnimation::SetFloat(std::string name, float value)
{
	uint64_t name_hash = std::hash<std::string>{}(name);
	animation_controller->SetFloat(name_hash, value);
}

ENGINE_API void ComponentAnimation::SetInt(std::string name, int value)
{
	uint64_t name_hash = std::hash<std::string>{}(name);
	animation_controller->SetInt(name_hash, value);
}

ENGINE_API void ComponentAnimation::SetBool(std::string name, bool value)
{
	uint64_t name_hash = std::hash<std::string>{}(name);
	animation_controller->SetBool(name_hash, value);
}

ENGINE_API void ComponentAnimation::SetIgnoreTransitions(bool enable)
{
	return animation_controller->SetIgnoreTransitions(enable);
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
		auto & skeleton = mesh->skeleton;
		animation_controller->GetClipTransform(skeleton, pose);
		mesh->UpdatePalette(pose);
		animation_controller->UpdateAttachedBones(skeleton, pose);
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

	for (auto& clip : animation_controller->state_machine->clips)
	{
		for (auto& mesh : skinned_meshes)
		{
			auto & skeleton = mesh->skeleton->skeleton;
			size_t skeleton_uuid = mesh->skeleton->GetUUID();
			GenerateAttachedBones(mesh->owner, skeleton);
			if (clip->animation == nullptr || clip->skeleton_channels_joints_map.find(skeleton_uuid) != clip->skeleton_channels_joints_map.end())
			{
				break;
			}
			std::vector<size_t> meshes_channels_joints_map(skeleton.size());
			auto & channels = clip->animation->keyframes[0].channels;
			for (size_t j = 0; j < skeleton.size(); ++j)
			{
				auto & joint = skeleton[j];
				auto it = std::find_if(channels.begin(), channels.end(), [&joint](const Animation::Channel & channel) {
					return channel.name == joint.name;
				});

				meshes_channels_joints_map[j] = (it - channels.begin());

			}
			clip->skeleton_channels_joints_map[skeleton_uuid] = std::move(meshes_channels_joints_map);


		}
	}
}

void ComponentAnimation::GenerateAttachedBones(GameObject* mesh, std::vector<Skeleton::Joint>& skeleton)
{
	if (mesh->children.empty())
	{
		return;
	}
	animation_controller->attached_bones.clear();

	for (size_t j = 0; j < skeleton.size(); ++j)
	{
		for (GameObject* child : mesh->children)
		{
			if (skeleton[j].name == child->name)
			{
				animation_controller->attached_bones.push_back({ j, child });
			}
		}
	}
}

