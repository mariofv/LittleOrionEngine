#include "Utils.h"

#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentTransform.h"
#include "Main/GameObject.h"

#include <fstream>
#include <IL/il.h>

Quat Utils::GenerateQuatFromDegFloat3(const float3& rotation)
{
	return Quat::FromEulerXYZ(
		math::DegToRad(rotation.x),
		math::DegToRad(rotation.y),
		math::DegToRad(rotation.z)
	);
}

float3 Utils::GenerateDegFloat3FromQuat(const Quat& rotation)
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

std::vector<float> Utils::GetVertices(const AABB& box)
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

std::vector<float> Utils::GetVertices(const AABB2D& box)
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

void Utils::GetCurrentPath(std::string& path)//to change and move to filesystem 
{
	char current_path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, current_path);
	path = current_path;
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

float3 Utils::Interpolate(const float3& first, const float3& second, float lambda)
{
	return first * (1.0f - lambda) + second * lambda;
}

Quat Utils::Interpolate(const Quat& first, const Quat& second, float lambda)
{
	Quat result;
	float dot = first.Dot(second);
	if (dot >= 0.0f) // Interpolate through the shortest path
	{
		result.x = first.x*(1.0f - lambda) + second.x*lambda;
		result.y = first.y*(1.0f - lambda) + second.y*lambda;
		result.z = first.z*(1.0f - lambda) + second.z*lambda;
		result.w = first.w*(1.0f - lambda) + second.w*lambda;
	}
	else
	{
		result.x = first.x*(1.0f - lambda) - second.x*lambda;
		result.y = first.y*(1.0f - lambda) - second.y*lambda;
		result.z = first.z*(1.0f - lambda) - second.z*lambda;
		result.w = first.w*(1.0f - lambda) - second.w*lambda;
	}
	result.Normalize();
	return result;
}

float4x4 Utils::Interpolate(const float4x4& first, const float4x4& second, float lambda)
{
	float4x4 result;

	float3 first_translation;
	Quat first_rotation;
	float3 first_scale;
	first.Decompose(first_translation, first_rotation, first_scale);

	float3 second_translation;
	Quat second_rotation;
	float3 second_scale;
	second.Decompose(second_translation, second_rotation, second_scale);
	result = float4x4::FromTRS(Interpolate(first_translation, second_translation, lambda), Interpolate(first_rotation, second_rotation, lambda), second_scale);
	return result;
}

float4x4 Utils::GetTransform(const aiMatrix4x4& current_transform, float scale_factor)
{
	aiVector3t<float> pScaling, pPosition;
	aiQuaterniont<float> pRotation;

	aiMatrix4x4 scale_matrix = aiMatrix4x4() * scale_factor;
	scale_matrix[3][3] = 1;

	aiMatrix4x4 node_transformation = scale_matrix * current_transform * scale_matrix.Inverse();
	node_transformation.Decompose(pScaling, pRotation, pPosition);

	math::float3 scale(pScaling.x, pScaling.y, pScaling.z);
	math::Quat rotation(pRotation.x, pRotation.y, pRotation.z, pRotation.w);
	math::float3 translation(pPosition.x, pPosition.y, pPosition.z);
	return math::float4x4::FromTRS(translation, rotation, scale);
}

size_t Utils::GetImageType(const std::string& file_extension)
{
	size_t image_type;

	if (file_extension == "bmp")
	{
		image_type = IL_BMP;
	}
	else if (file_extension == "cut")
	{
		image_type = IL_CUT;
	}
	else if (file_extension == "dds")
	{
		image_type = IL_DDS;
	}
	else if (file_extension == "gif")
	{
		image_type = IL_GIF;
	}
	else if (file_extension == "ico")
	{
		image_type = IL_ICO;
	}
	else if (file_extension == "jpg")
	{
		image_type = IL_JPG;
	}
	else if (file_extension == "lif")
	{
		image_type = IL_LIF;
	}
	else if (file_extension == "mng")
	{
		image_type = IL_MNG;
	}
	else if (file_extension == "pcd")
	{
		image_type = IL_PCD;
	}
	else if (file_extension == "pcx")
	{
		image_type = IL_PCX;
	}
	else if (file_extension == "pic")
	{
		image_type = IL_PIC;
	}
	else if (file_extension == "png")
	{
		image_type = IL_PNG;
	}
	else if (file_extension == "pbm")
	{
		image_type = IL_PNM;
	}
	else if (file_extension == "pgm")
	{
		image_type = IL_PNM;
	}
	else if (file_extension == "ppm")
	{
		image_type = IL_PNM;
	}
	else if (file_extension == "psd")
	{
		image_type = IL_PSD;
	}
	else if (file_extension == "psp")
	{
		image_type = IL_PSP;
	}
	else if (file_extension == "bw")
	{
		image_type = IL_SGI;
	}
	else if (file_extension == "rgb")
	{
		image_type = IL_SGI;
	}
	else if (file_extension == "rgba")
	{
		image_type = IL_SGI;
	}
	else if (file_extension == "sgi")
	{
		image_type = IL_SGI;
	}
	else if (file_extension == "tga")
	{
		image_type = IL_TGA;
	}
	else if (file_extension == "tif")
	{
		image_type = IL_TIF;
	}
	else if (file_extension == "tiff")
	{
		image_type = IL_TIF;
	}
	else if (file_extension == "jasc")
	{
		image_type = IL_JASC_PAL;
	}
	else
	{
		image_type = IL_TYPE_UNKNOWN;
	}

	return image_type;
}

btVector3 Utils::Float3TobtVector3(const float3& vector)
{
	return btVector3(vector.x, vector.y, vector.z);
}

void Utils::SplitCulledMeshRenderers(
	const std::vector<ComponentMeshRenderer*>& culled_mesh_renderers, 
	float3 camera_position,
	std::vector<MeshRendererDistancePair>& opaque_mesh_renderers,
	std::vector<MeshRendererDistancePair>& transparent_mesh_renderers
)
{
	for (ComponentMeshRenderer* culled_mesh_renderer : culled_mesh_renderers)
	{
		if (culled_mesh_renderer->mesh_to_render == nullptr || culled_mesh_renderer->material_to_render == nullptr)
		{
			continue;
		}

		float3 mesh_renderer_position = culled_mesh_renderer->owner->transform.GetGlobalTranslation();;
		float distance = mesh_renderer_position.Distance(camera_position);

		if (
			culled_mesh_renderer->material_to_render->material_type == Material::MaterialType::MATERIAL_TRANSPARENT
			|| culled_mesh_renderer->material_to_render->material_type == Material::MaterialType::MATERIAL_LIQUID
			|| culled_mesh_renderer->material_to_render->material_type == Material::MaterialType::MATERIAL_DISSOLVING
			)
		{
			transparent_mesh_renderers.push_back(Utils::MeshRendererDistancePair{ distance, culled_mesh_renderer });
		}
		else if (culled_mesh_renderer->material_to_render->material_type == Material::MaterialType::MATERIAL_OPAQUE)
		{
			opaque_mesh_renderers.push_back(Utils::MeshRendererDistancePair{ distance, culled_mesh_renderer });
		}
	}
}

void Utils::SortMeshRendererVector(std::vector<MeshRendererDistancePair>& mesh_renderers)
{
	std::sort(mesh_renderers.begin(), mesh_renderers.end(), [](const MeshRendererDistancePair& a, const MeshRendererDistancePair& b) {
		return a.distance > b.distance; 
	});
}
