#include "ModuleAnimation.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Component/ComponentAnimation.h"

bool ModuleAnimation::Init()
{
	APP_LOG_SECTION("************ Module Animation Init ************");
	

	return true;
}

// Called every draw update
update_status ModuleAnimation::Update()
{

	return update_status::UPDATE_CONTINUE;
}

bool ModuleAnimation::CleanUp()
{
	for (auto& animation : animations)
	{
		animation->owner->RemoveComponent(animation);
	}
	animations.clear();

	return true;
}

ComponentAnimation* ModuleAnimation::CreateComponentAnimation()
{
	ComponentAnimation * new_animation = new ComponentAnimation();
	animations.push_back(new_animation);
	return new_animation;
}

void ModuleAnimation::RemoveComponentAnimation(ComponentAnimation* animation_to_remove)
{
	auto it = std::find(animations.begin(), animations.end(), animation_to_remove);
	if (it != animations.end())
	{
		delete *it;
		animations.erase(it);
	}
}