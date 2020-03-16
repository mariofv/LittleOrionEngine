#include "ComponentAnimation.h"

#include "Main/Application.h"
#include "Module/ModuleAnimation.h"

ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::ANIMATION)
{

}

ComponentAnimation::ComponentAnimation(GameObject * owner) : Component(owner, ComponentType::ANIMATION)
{

}

ComponentAnimation::~ComponentAnimation()
{
}

void ComponentAnimation::Update()
{

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

void ComponentAnimation::OnPlay()
{

}

void ComponentAnimation::OnStop()
{
}

void ComponentAnimation::OnUpdate()
{

}