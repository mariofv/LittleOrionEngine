#include "MeshLoader.h"
#include "Brofiler/Brofiler.h"
#include <vector>
#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>
#include <ResourceManagement/Resources/Mesh.h>

std::shared_ptr<Mesh> MeshLoader::Load(const std::string& file_path)
{
	BROFILER_CATEGORY("Load Mesh", Profiler::Color::Brown);


	APP_LOG_INFO("Loading model %s.", file_path.c_str());
	size_t mesh_size;
	char * data = App->filesystem->Load(file_path.c_str(), mesh_size);
	char* cursor = data;

	uint32_t ranges[3];
	//Get ranges
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(ranges, cursor, bytes);

	std::vector<uint32_t> indices;
	std::vector<Mesh::Vertex> vertices;
	std::vector<uint32_t> meshes_materials_size;
	std::vector<std::string> meshes_textures_path;

	indices.resize(ranges[0]);

	cursor += bytes; // Get indices
	bytes = sizeof(uint32_t) * ranges[0];
	memcpy(&indices.front(), cursor, bytes);

	vertices.resize(ranges[1]);

	cursor += bytes; // Get vertices
	bytes = sizeof(Mesh::Vertex) * ranges[1];
	memcpy(&vertices.front(), cursor, bytes);


	cursor += bytes; // Get sizes
	bytes = sizeof(uint32_t) * ranges[2];
	meshes_materials_size.resize(ranges[2]);
	if (bytes != 0)
	{
		memcpy(&meshes_materials_size.front(), cursor, bytes);
	}

	meshes_textures_path.resize(ranges[2]);
	for (size_t i = 0; i < ranges[2]; i++)
	{
		cursor += bytes; // Get materials
		bytes = meshes_materials_size[i];
		std::vector<char> path;
		path.resize(bytes);
		memcpy(&path.front(), cursor, bytes);
		meshes_textures_path[i] = std::string(path.begin(), path.end());
	}

	std::shared_ptr<Mesh> new_mesh = std::make_shared<Mesh>(std::move(vertices), std::move(indices), std::move(meshes_textures_path), file_path);
	free(data);

	return new_mesh;
}
