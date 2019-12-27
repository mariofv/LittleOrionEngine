#ifndef _UTILS_H_
#define _UTILS_H_

#include "MathGeoLib.h"


class Utils
{
public:

	Utils();
	~Utils();

	static Quat GenerateQuatFromDegFloat3(const float3 &rotation);
	static float3 GenerateDegFloat3FromQuat(const Quat &rotation);
	static float3 Float3RadToDeg(const float3 &rad_float3);
	static float3 Float3DegToRad(const float3 &deg_float3);
};

#endif //_UTILS_H_

