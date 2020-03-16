#include "ComponentAnimation.h"

#include "Animation/AnimController.h"
#include "Main/Application.h"
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
	if (App->time->isGameRunning())
	{
		animation_controller->Update();
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

void ComponentAnimation::OnPlay()
{

}

void ComponentAnimation::OnStop()
{
}

void ComponentAnimation::OnUpdate()
{

}