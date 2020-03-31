#include "StateMachineImporter.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Resources/StateMachine.h"
ImportResult StateMachineImporter::Import(const File & file, bool force) const
{
	ImportResult import_result;

	if (file.filename.empty() || !file.loaded_correctly)
	{
		APP_LOG_ERROR("Importing state machine error: Couldn't find the file to import.")
			return import_result;
	}

	ImportOptions already_imported = GetAlreadyImportedResource(file);
	if (already_imported.uuid != 0 && !force) {
		APP_LOG_INFO("state machine already imported.")
			import_result.success = true;
		import_result.exported_file = already_imported.exported_file;
		return import_result;
	}

	StateMachine state_machine(file.file_path);
	state_machine.Load(file);

	uint32_t num_clips = state_machine.clips.size();
	uint32_t num_states = state_machine.states.size();
	uint32_t num_transitions = state_machine.transitions.size();
	uint32_t ranges[3] = { num_clips, num_states, num_transitions };

	uint32_t size_of_clip= sizeof(uint64_t) + sizeof(uint32_t);
	uint32_t size_of_state = sizeof(uint64_t) * 2;
	uint32_t size_of_transitions = sizeof(uint64_t) * 3 + sizeof(long);
	uint32_t size = sizeof(ranges) + size_of_clip * num_clips + size_of_transitions * num_transitions + size_of_state * num_states;

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

		bytes = sizeof(uint32_t) ;
		uint32_t animation_uuid = clip->animation->GetUUID();
		memcpy(cursor, &animation_uuid, bytes);
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

		bytes = sizeof(long);
		memcpy(cursor, &transition->interpolation_time, bytes);
		cursor += bytes; 
	}

	std::string exported_file = SaveMetaFile(file.file_path, ResourceType::STATE_MACHINE);
	App->filesystem->Save(exported_file.c_str(), data, size);

	import_result.success = true;
	import_result.exported_file = exported_file;
	return import_result;
}
