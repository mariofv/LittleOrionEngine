#ifndef _MODULEANIMATION_H_
#define _MODULEANIMATION_H_
#define ENGINE_EXPORTS

#include "Animation/Tween/LOTween.h"
#include "Animation/Tween/TweenSequence.h"
#include "Module.h"

#include "ModuleDebugDraw.h"

class ComponentAnimation;
class GameObject;
class PanelStateMachine;

class ModuleAnimation : public Module
{
public:
	ModuleAnimation() = default;
	~ModuleAnimation() = default;
	
	bool Init() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	
	ComponentAnimation* CreateComponentAnimation();
	ComponentAnimation* CreateComponentAnimation(GameObject* owner);

	void RemoveComponentAnimation(ComponentAnimation* animation_to_remove);
	void UpdateAnimationMeshes();
	void PlayAnimations() const;

	ENGINE_API TweenSequence* CreateTweenSequence();
	void CleanTweens();

private:
	std::vector<ComponentAnimation*> animations;

	LOTween* tweener = nullptr;
	friend ModuleDebugDraw;
	friend PanelStateMachine;
};

#endif //_MODULEANIMATION_H_