#include "StateMachineManager.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/StateMachine.h"
std::shared_ptr<StateMachine> StateMachineManager::Load(const std::string & file_path)
{
	if (!App->filesystem->Exists(file_path.c_str()))
	{
		APP_LOG_ERROR("Error loading State Machine %s.", file_path.c_str());
		return nullptr;
	}

	APP_LOG_INFO("Loading State Machine  %s.", file_path.c_str());
	size_t size;
	char * data = App->filesystem->Load(file_path.c_str(), size);
	char* cursor = data;

	uint32_t ranges[3];
	//Get ranges
	size_t bytes = sizeof(ranges); 
	memcpy(ranges, cursor, bytes);
	cursor += bytes;


	std::vector<std::shared_ptr<Clip>> clips;
	clips.resize(ranges[0]);
	for (auto & clip : clips)
	{
		clip = std::make_shared<Clip>();

		bytes = sizeof(uint64_t);
		memcpy( &clip->name_hash, cursor, bytes);
		cursor += bytes; // Store Clip

		bytes = sizeof(uint32_t);
		uint32_t animation_uuid;
		memcpy(&animation_uuid, cursor, bytes);
		cursor += bytes; 
		if (animation_uuid != 0)
		{
			std::string  animation_path = App->resources->resource_DB->GetEntry(animation_uuid)->exported_file;
			clip->animation = App->resources->Load<Animation>(animation_path);
		}

		bytes = sizeof(bool);
		memcpy(&clip->loop, cursor, bytes);
		cursor += bytes;
	}

	std::vector< std::shared_ptr<State>> states;
	states.resize(ranges[1]);
	for (auto & state : states)
	{
		state = std::make_shared<State>();

		bytes = sizeof(uint64_t);
		memcpy(&state->name_hash, cursor, bytes);
		cursor += bytes; // Store states
		uint64_t clip_hash;
		bytes = sizeof(uint64_t);
		memcpy(&clip_hash, cursor, bytes);
		cursor += bytes; // Store states

		for (auto & clip : clips)
		{
			if (clip->name_hash == clip_hash)
			{
				state->clip = clip;
				break;
			}
		}
	}

	std::vector<std::shared_ptr<Transition>> transitions;
	transitions.resize(ranges[2]);
	for (auto & transition : transitions)
	{
		transition = std::make_shared<Transition>();
		bytes = sizeof(uint64_t);
		memcpy( &transition->source_hash, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(&transition->target_hash, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(&transition->trigger_hash, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy( &transition->interpolation_time, cursor, bytes);
		cursor += bytes;
	}
	std::shared_ptr<StateMachine> new_state_machine = std::make_shared<StateMachine>(std::move(clips), std::move(states), std::move(transitions), file_path);
	bytes = sizeof(uint64_t);
	memcpy(&new_state_machine->default_state, cursor, bytes);
	cursor += bytes;

	free(data);

	return new_state_machine;
}
