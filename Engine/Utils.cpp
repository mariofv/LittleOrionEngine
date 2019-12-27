#include "Utils.h"



Utils::Utils()
{
}


Utils::~Utils()
{
}

Quat Utils::GenerateQuatFromDegFloat3(const float3 &rotation)
{
	return Quat::FromEulerXYZ(
		math::DegToRad(rotation.x),
		math::DegToRad(rotation.y),
		math::DegToRad(rotation.z)
	);
}

float3 Utils::GenerateDegFloat3FromQuat(const Quat &rotation)
{
	float3 deg_rotation = rotation.ToEulerXYZ();
	deg_rotation.x = math::RadToDeg(deg_rotation.x);
	deg_rotation.y = math::RadToDeg(deg_rotation.y);
	deg_rotation.z = math::RadToDeg(deg_rotation.z);

	return deg_rotation;
}

float3 Utils::Float3RadToDeg(const float3& rad_float3)
{
	float3 deg_float3 = float3(
		math::RadToDeg(rad_float3.x),
		math::RadToDeg(rad_float3.y),
		math::RadToDeg(rad_float3.z)
	);

	return deg_float3;
}