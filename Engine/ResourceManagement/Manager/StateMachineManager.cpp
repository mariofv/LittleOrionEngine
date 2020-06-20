#include "StateMachineManager.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "ResourceManagement/Resources/StateMachine.h"

FileData StateMachineManager::Binarize(StateMachine* state_machine)
{
	Config state_machine_config;
	state_machine->Save(state_machine_config);

	std::string serialized_state_machine_string;
	state_machine_config.GetSerializedString(serialized_state_machine_string);

	char* state_machine_bytes = new char[serialized_state_machine_string.size() + 1];
	memcpy(state_machine_bytes, serialized_state_machine_string.c_str(), serialized_state_machine_string.size() + 1);

	FileData state_machine_data{ state_machine_bytes, serialized_state_machine_string.size() + 1 };
	return state_machine_data;
}

std::shared_ptr<StateMachine> StateMachineManager::Load(uint32_t uuid, const FileData& resource_data)
{
	char * data = (char*)resource_data.buffer;
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
		memcpy(&clip->name_hash, cursor, bytes);
		cursor += bytes; // Store Clip

		bytes = sizeof(uint32_t);
		uint32_t animation_uuid;
		memcpy(&animation_uuid, cursor, bytes);
		cursor += bytes;
		if (animation_uuid != 0)
		{
			auto & animation_resource = App->resources->Load<Animation>(animation_uuid);
			if (animation_resource)
			{
				clip->SetAnimation(App->resources->Load<Animation>(animation_uuid));
			}
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
		cursor += bytes; // Get state
		uint64_t clip_hash;

		bytes = sizeof(uint64_t);
		memcpy(&clip_hash, cursor, bytes);
		cursor += bytes; // Get clip

		bytes = sizeof(float);//for speed
		memcpy( &state->speed, cursor, bytes);
		cursor += bytes;

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
		memcpy(&transition->source_hash, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(&transition->target_hash, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(&transition->trigger_hash, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(&transition->interpolation_time, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(&transition->priority, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(bool);
		memcpy(&transition->automatic, cursor, bytes);
		cursor += bytes;
	}
	std::shared_ptr<StateMachine> new_state_machine = std::make_shared<StateMachine>(uuid, std::move(clips), std::move(states), std::move(transitions));
	bytes = sizeof(uint64_t);
	memcpy(&new_state_machine->default_state, cursor, bytes);
	cursor += bytes;

	return new_state_machine;
}

FileData StateMachineManager::Create()
{
	StateMachine created_state_machine;
	created_state_machine.states.push_back(std::make_shared<State>("Entry", nullptr));
	created_state_machine.states.push_back(std::make_shared<State>("End", nullptr));
	created_state_machine.default_state = std::hash<std::string>{}("Entry");

	return Binarize(&created_state_machine);

}