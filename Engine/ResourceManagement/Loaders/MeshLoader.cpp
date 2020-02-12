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

	uint32_t ranges[2];
	//Get ranges
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(ranges, cursor, bytes);

	std::vector<uint32_t> indices;
	std::vector<Mesh::Vertex> vertices;

	indices.resize(ranges[0]);

	cursor += bytes; // Get indices
	bytes = sizeof(uint32_t) * ranges[0];
	memcpy(&indices.front(), cursor, bytes);

	vertices.resize(ranges[1]);

	cursor += bytes; // Get vertices
	bytes = sizeof(Mesh::Vertex) * ranges[1];
	memcpy(&vertices.front(), cursor, bytes);

	std::shared_ptr<Mesh> new_mesh = std::make_shared<Mesh>(std::move(vertices), std::move(indices), file_path);
	free(data);

	return new_mesh;
}
