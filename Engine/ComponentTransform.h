#ifndef _COMPONENTTRANSFORM_H_
#define _COMPONENTTRANSFORM_H_

#include "Component.h"

#include "MathGeoLib.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform();
	~ComponentTransform();

private:
	float3 translation = float3::zero;
	float3 rotation = float3::zero;
	float3 scale = float3::one;
};

#endif //_COMPONENTTRANSFORM_H_