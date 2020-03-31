#include "StateMachineImporter.h"
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

	uint32_t size = sizeof(ranges) + sizeof(Clip) * num_clips + sizeof(uint32_t) * num_clips  +sizeof(Transition) * num_transitions + sizeof(State) * num_states;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store Clips

	for (auto & clip : state_machine.clips)
	{
		bytes = sizeof(Clip);
		memcpy(cursor, &(*clip), bytes);
		cursor += bytes; // Store animations hashes

		bytes = sizeof(uint32_t) ;
		uint32_t animation_uuid = clip->animation->GetUUID();
		memcpy(cursor, &animation_uuid, bytes);
		cursor += bytes;
	}

	for (auto & state : state_machine.states)
	{
		bytes = sizeof(State);
		memcpy(cursor, &(*state), bytes);
		cursor += bytes; // Store states
	}

	for (auto & transition : state_machine.transitions)
	{
		bytes = sizeof(Transition);
		memcpy(cursor, &(*transition), bytes);
		cursor += bytes; // Store transitions
	}

	std::string exported_file = SaveMetaFile(file.file_path, ResourceType::STATE_MACHINE);

	import_result.success = true;
	import_result.exported_file = exported_file;
	return import_result;
}
