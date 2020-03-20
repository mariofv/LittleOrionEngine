#include "ComponentAnimation.h"

#include "Animation/AnimController.h"
#include "Component/ComponentTransform.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAnimation.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleResourceManager.h"

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
	config.AddString(animation->exported_file, "AnimationResource");
}

void ComponentAnimation::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	std::string animation_path;
	config.GetString("AnimationResource", animation_path, "");

	animation = App->resources->Load<Animation>(animation_path);
}

void ComponentAnimation::UpdateBone(GameObject* current_bone)
{
	float4x4 bone_transform;
	if (animation_controller->GetTransformation(current_bone->name, bone_transform))
	{
		current_bone->transform.SetGlobalModelMatrix(bone_transform);
	}

	for (auto& children_bone : current_bone->children)
	{
		UpdateBone(children_bone);
	}
}