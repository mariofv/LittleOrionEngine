#ifndef _COMPONENTAABBCOLLIDER_H_
#define _COMPONENTAABBCOLLIDER_H_

#include "Component.h"

class Texture;

class ComponentAABBCollider : public Component
{
public:
	ComponentAABBCollider();
	ComponentAABBCollider(GameObject * owner);
	~ComponentAABBCollider();

	void Enable() override;
	void Disable() override;
	void Update() override;

};

#endif //_COMPONENTAABBCOLLIDER_H_
