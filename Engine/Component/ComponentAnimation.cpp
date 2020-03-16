#include "ComponentAnimation.h"

#include "Animation/AnimController.h"
#include "Component/ComponentTransform.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleTime.h"

ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::ANIMATION)
{
	animation_controller = new AnimController();
}

ComponentAnimation::ComponentAnimation(GameObject * owner) : Component(owner, ComponentType::ANIMATION)
{
	animation_controller = new AnimController();
}

ComponentAnimation::~ComponentAnimation()
{
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
	config.AddString(animation_path, "AnimationResource");
}

void ComponentAnimation::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	config.GetString("AnimationResource", animation_path, "");
}

void ComponentAnimation::UpdateBone(GameObject* current_bone)
{
	float3 bone_position;
	Quat bone_rotation;
	if (animation_controller->GetTransform(current_bone->name, bone_position, bone_rotation))
	{
		current_bone->transform.SetTranslation(bone_position);
		current_bone->transform.SetRotation(bone_rotation.ToFloat3x3());
	}
	
	for (auto& children_bone : current_bone->children)
	{
		UpdateBone(children_bone);
	}
}