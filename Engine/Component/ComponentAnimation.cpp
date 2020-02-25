#include "ComponentAnimation.h"



ComponentAnimation::ComponentAnimation() : Component(nullptr, ComponentType::ANIMATION)
{

}

ComponentAnimation::ComponentAnimation(GameObject * owner) : Component(owner, ComponentType::ANIMATION)
{

}


ComponentAnimation::~ComponentAnimation()
{
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