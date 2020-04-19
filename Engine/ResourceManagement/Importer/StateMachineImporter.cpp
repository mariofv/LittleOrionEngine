#include "StateMachineImporter.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Resources/StateMachine.h"

FileData StateMachineImporter::ExtractData(Path& assets_file_path, const Metafile& metafile) const
{
	FileData state_machine_data = assets_file_path.GetFile()->Load();

	char* state_machine_data_buffer = (char*)state_machine_data.buffer;
	std::string serialized_state_machine_string = std::string(state_machine_data_buffer, state_machine_data.size);

	Config state_machine_config(serialized_state_machine_string);
	StateMachine state_machine;
	state_machine.Load(state_machine_config);

	uint32_t num_clips = state_machine.clips.size();
	uint32_t num_states = state_machine.states.size();
	uint32_t num_transitions = state_machine.transitions.size();
	uint32_t ranges[3] = { num_clips, num_states, num_transitions };

	uint32_t size_of_clip = sizeof(uint64_t) + sizeof(uint32_t) + sizeof(bool);
	uint32_t size_of_state = sizeof(uint64_t) * 2;
	uint32_t size_of_transitions = sizeof(uint64_t) * 4 + sizeof(bool);
	uint32_t size = sizeof(ranges) + size_of_clip * num_clips + size_of_transitions * num_transitions + size_of_state * num_states + sizeof(uint64_t)/*Default state*/;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store Clips

	for (auto & clip : state_machine.clips)
	{
		bytes = sizeof(uint64_t);
		memcpy(cursor, &clip->name_hash, bytes);
		cursor += bytes; // Store Clip

		bytes = sizeof(uint32_t);
		uint32_t animation_uuid = clip->animation->GetUUID();
		memcpy(cursor, &animation_uuid, bytes);
		cursor += bytes;

		bytes = sizeof(bool);
		memcpy(cursor, &clip->loop, bytes);
		cursor += bytes;
	}

	for (auto & state : state_machine.states)
	{
		bytes = sizeof(uint64_t);
		memcpy(cursor, &state->name_hash, bytes);
		cursor += bytes; // Store states
		uint64_t clip_hash = state->clip ? state->clip->name_hash : 0;
		bytes = sizeof(uint64_t);
		memcpy(cursor, &clip_hash, bytes);
		cursor += bytes; // Store states
	}

	for (auto & transition : state_machine.transitions)
	{
		bytes = sizeof(uint64_t);
		memcpy(cursor, &transition->source_hash, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(cursor, &transition->target_hash, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(cursor, &transition->trigger_hash, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(cursor, &transition->interpolation_time, bytes);
		cursor += bytes; 

		bytes = sizeof(bool);
		memcpy(cursor, &transition->automatic, bytes);

		cursor += bytes;
	}

	bytes = sizeof(uint64_t);
	memcpy(cursor, &state_machine.default_state, bytes);
	cursor += bytes;

	return FileData{data, size};
}
