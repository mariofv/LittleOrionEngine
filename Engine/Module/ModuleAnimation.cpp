#include "ModuleAnimation.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleTime.h"
#include "Component/ComponentAnimation.h"

bool ModuleAnimation::Init()
{
	APP_LOG_SECTION("************ Module Animation Init ************");
	tweener = new LOTween();

	return true;
}

update_status ModuleAnimation::Update()
{
	if (App->time->isGameRunning())
	{
		tweener->Update(App->time->delta_time);
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleAnimation::CleanUp()
{
	for (auto& animation : animations)
	{
		animation->owner->RemoveComponent(animation);
	}
	animations.clear();

	delete(tweener);

	return true;
}

ComponentAnimation* ModuleAnimation::CreateComponentAnimation()
{
	ComponentAnimation * new_animation = new ComponentAnimation();
	animations.push_back(new_animation);
	return new_animation;
}

ComponentAnimation* ModuleAnimation::CreateComponentAnimation(GameObject* owner)
{
	ComponentAnimation* new_animation = new ComponentAnimation(owner);
	animations.push_back(new_animation);
	return new_animation;
}

void ModuleAnimation::RemoveComponentAnimation(ComponentAnimation* animation_to_remove)
{
	const auto it = std::find(animations.begin(), animations.end(), animation_to_remove);
	if (it != animations.end())
	{
		delete *it;
		animations.erase(it);
	}
}

void ModuleAnimation::UpdateAnimationMeshes()
{
	for (auto & animation : animations)
	{
		animation->Init();
	}
}

void ModuleAnimation::PlayAnimations() const
{
	for(const auto& anim : animations)
	{
		anim->Play();
	}
}

TweenSequence * ModuleAnimation::CreateTweenSequence()
{
	TweenSequence* sequence = tweener->CreateSequence();
	return sequence;
}

void ModuleAnimation::CleanTweens()
{
	tweener->Reset();
}
