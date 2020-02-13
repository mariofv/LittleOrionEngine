#include "SkeletonLoader.h"

#include "Brofiler/Brofiler.h"
#include <vector>
#include <Main/Application.h>
#include <Module/ModuleFileSystem.h>
#include <ResourceManagement/Resources/Skeleton.h>

std::shared_ptr<Skeleton> SkeletonLoader::Load(const std::string& file_path)
{
	BROFILER_CATEGORY("Load Skeleton", Profiler::Color::Brown);


	APP_LOG_INFO("Loading Skeleton %s.", file_path.c_str());
	size_t skeleton_size;
	char * data = App->filesystem->Load(file_path.c_str(), skeleton_size);
	char* cursor = data;

	uint32_t num_bones;
	//Get number of bones
	size_t bytes = sizeof(uint32_t);
	memcpy(&num_bones, cursor, bytes);

	std::vector<Skeleton::Joint> bones;

	bones.resize(num_bones);

	cursor += bytes; // Get bones
	bytes = sizeof(Skeleton::Joint) * num_bones;
	memcpy(&bones.front(), cursor, bytes);


	std::shared_ptr<Skeleton> new_skeleton = std::make_shared<Skeleton>(std::move(bones), file_path);
	free(data);

	return new_skeleton;
}