#ifndef _UTILS_H_
#define _UTILS_H_

#include <MathGeoLib.h>
#include <vector>

class Utils
{
public:

	Utils();
	~Utils();

	static Quat GenerateQuatFromDegFloat3(const float3 &rotation);
	static float3 GenerateDegFloat3FromQuat(const Quat &rotation);
	static float3 Float3RadToDeg(const float3 &rad_float3);
	static float3 Float3DegToRad(const float3 &deg_float3);

	static std::vector<float> GetVertices(const AABB &box);
	static std::vector<float> GetVertices(const AABB2D &box);
	size_t CStrlastIndexOfChar(const char* str, char find_char);
	bool PatchFileName(char* filename);
	std::string LoadFileContent(const std::string& path);
	void SaveFileContent(const std::string & source, std::string & destination);

	void ReplaceStringInPlace(std::string & subject, const std::string & search, const std::string & replace);

	static float3 Interpolate(const float3 & first, const float3 & second, float lambda);
	static Quat Interpolate(const Quat & first, const Quat & second, float lambda);
	static float4x4 Interpolate(const float4x4& first, const float4x4& second, float lambda);
};

#endif //_UTILS_H_

