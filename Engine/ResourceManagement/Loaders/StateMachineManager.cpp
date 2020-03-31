#include "StateMachineManager.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/StateMachine.h"
std::shared_ptr<StateMachine> StateMachineManager::Load(const std::string & file_path)
{
	if (!App->filesystem->Exists(file_path.c_str()))
	{
		APP_LOG_ERROR("Error loading Mesh %s.", file_path.c_str());
		return nullptr;
	}

	APP_LOG_INFO("Loading model %s.", file_path.c_str());
	size_t mesh_size;
	char * data = App->filesystem->Load(file_path.c_str(), mesh_size);
	char* cursor = data;

	uint32_t ranges[3];
	//Get ranges
	size_t bytes = sizeof(ranges); 
	memcpy(ranges, cursor, bytes);


	std::vector<std::shared_ptr<Clip>> clips;
	clips.resize(ranges[0]);
	cursor += bytes; // Get clips
	for (auto clip : clips)
	{

		bytes = sizeof(Clip);
		memcpy(&(*clip), cursor, bytes);
		cursor += bytes; 

		bytes = sizeof(uint32_t);
		uint32_t animation_uuid;
		memcpy(&animation_uuid, cursor, bytes);
		cursor += bytes; 

		std::string  animation_path = App->resources->resource_DB->GetEntry(animation_uuid)->exported_file;
		clip->animation = App->resources->Load<Animation>(animation_path);
	}

	std::vector< std::shared_ptr<State>> states;
	states.resize(ranges[1]);
	for (auto state : states)
	{
		bytes = sizeof(State);
		memcpy(&(*state), cursor, bytes);
		cursor += bytes;
	}

	std::vector<std::shared_ptr<Transition>> transitions;
	clips.resize(ranges[2]);
	for (auto transition : transitions)
	{
		bytes = sizeof(Transition);
		memcpy(&(*transition), cursor, bytes);
		cursor += bytes;
	}
	std::shared_ptr<StateMachine> new_state_machine = std::make_shared<StateMachine>(std::move(clips), std::move(states), std::move(transitions), file_path);
	free(data);

	return new_state_machine;
}
