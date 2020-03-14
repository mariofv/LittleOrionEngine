#include "Utils.h"

#include <fstream>

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

float3 Utils::Float3DegToRad(const float3& deg_float3)
{
	float3 rad_float3 = float3(
		math::DegToRad(deg_float3.x),
		math::DegToRad(deg_float3.y),
		math::DegToRad(deg_float3.z)
	);

	return rad_float3;
}

std::vector<float> Utils::GetVertices(const AABB &box)
{
	static const int num_of_vertices = 8;
	float3 tmp_vertices[num_of_vertices];
	box.GetCornerPoints(&tmp_vertices[0]);

	std::vector<float> vertices(num_of_vertices * 3);
	for (unsigned int i = 0; i < num_of_vertices; ++i)
	{
		vertices[i * 3] = tmp_vertices[i].x;
		vertices[i * 3 + 1] = tmp_vertices[i].y;
		vertices[i * 3 + 2] = tmp_vertices[i].z;
	}

	return vertices;
}

std::vector<float> Utils::GetVertices(const AABB2D &box)
{
	static const int num_of_vertices = 4;
	float3 tmp_vertices[num_of_vertices];
	float2 max_point = box.maxPoint;
	float2 min_point = box.minPoint;
	//ClockWise from Top left
	tmp_vertices[0] = float3(min_point.x, max_point.y, 0.0f); // 0
	tmp_vertices[1] = float3(max_point, 0.0f); // 1
	tmp_vertices[2] = float3(max_point.x, min_point.y, 0.0f); // 2
	tmp_vertices[3] = float3(min_point, 0.0f); // 3

	std::vector<float> vertices(num_of_vertices * 3);
	for (unsigned int i = 0; i < num_of_vertices; ++i)
	{
		vertices[i * 3] = tmp_vertices[i].x;
		vertices[i * 3 + 1] = tmp_vertices[i].z;
		vertices[i * 3 + 2] = tmp_vertices[i].y;
	}

	return vertices;
}

size_t Utils::CStrlastIndexOfChar(const char* str, char find_char)
{
	intptr_t i = strlen(str) - 1;
	while (i >= 0)
	{
		if (str[i] == find_char)
		{
			return i;
		}
		--i;
	}
	return (size_t)-1;
}

bool Utils::PatchFileName(char* filename)
{
	size_t	dot_idx = CStrlastIndexOfChar(filename, '.');
	if (dot_idx != (size_t)-1)
	{
		filename[dot_idx - 1] = '_';
		return true;
	}
	else
	{
		return false;
	}

}

std::string Utils::LoadFileContent(const std::string& path) {
	std::ifstream file(path);
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void Utils::SaveFileContent(const std::string& source, std::string& destination)
{
	std::ofstream file(destination);
	file << source;
}

void Utils::ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}
