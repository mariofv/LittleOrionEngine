#ifndef _COMPONENTAABBCOLLIDER_H_
#define _COMPONENTAABBCOLLIDER_H_

#include "Component.h"
#include "MathGeoLib.h"

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

	void GenerateBoundingBox();

	void ShowComponentWindow(){}
	AABB bounding_box;

};

#endif //_COMPONENTAABBCOLLIDER_H_
