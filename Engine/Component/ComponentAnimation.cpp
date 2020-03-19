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

void ComponentAnimation::Copy(Component * component_to_copy) const
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
	if (current_bone->name == "Hips")
	{
		int x = 0;
	}
	if (animation_controller->GetTranslation(current_bone->name, bone_position))
	{
		current_bone->transform.SetTranslation(bone_position);
	}

	Quat bone_rotation;
	if (animation_controller->GetRotation(current_bone->name, bone_rotation))
	{
		current_bone->transform.SetRotation(bone_rotation.ToFloat3x3());
	}

	for (auto& children_bone : current_bone->children)
	{
		UpdateBone(children_bone);
	}
}