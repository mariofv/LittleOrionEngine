#ifndef _MODULEANIMATION_H_
#define _MODULEANIMATION_H_

#include "Module.h"

#include "ModuleDebugDraw.h"

class ComponentAnimation;
class GameObject;

class ModuleAnimation : public Module
{
public:
	ModuleAnimation() = default;
	~ModuleAnimation() = default;
	
	bool Init() override;
	bool CleanUp() override;
	
	ComponentAnimation* CreateComponentAnimation();
	void RemoveComponentAnimation(ComponentAnimation* animation_to_remove);
	void UpdateAnimationMeshes();
	void PlayAnimations() const;

private:
	std::vector<ComponentAnimation*> animations;
	friend ModuleDebugDraw;
};

#endif //_MODULEANIMATION_H_