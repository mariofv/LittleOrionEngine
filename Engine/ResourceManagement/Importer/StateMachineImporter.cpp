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
	delete[] state_machine_data.buffer;
	std::vector<Config> clips_config;
	state_machine_config.GetChildrenConfig("Clips", clips_config);

	std::vector<Config> states_config;
	state_machine_config.GetChildrenConfig("States", states_config);

	std::vector<Config> transitions_config;
	state_machine_config.GetChildrenConfig("Transitions", transitions_config);

	//Set float
	std::vector<Config> float_variables_config;
	state_machine_config.GetChildrenConfig("FloatVariables", float_variables_config);

	//Set int
	std::vector<Config> int_variables_config;
	state_machine_config.GetChildrenConfig("IntVariables", int_variables_config);

	//Set bool
	std::vector<Config> bool_variables_config;
	state_machine_config.GetChildrenConfig("BoolVariables", bool_variables_config);

	//GetConditions size
	uint32_t number_of_float_conditions = 0;
	for(auto& transition_config : transitions_config)
	{
		//Conditions
		std::vector<Config> conditions_config;
		transition_config.GetChildrenConfig("FloatConditions", conditions_config);

		number_of_float_conditions += conditions_config.size();
	}

	uint32_t number_of_int_conditions = 0;
	for (auto& transition_config : transitions_config)
	{
		//Conditions
		std::vector<Config> conditions_config;
		transition_config.GetChildrenConfig("IntConditions", conditions_config);

		number_of_int_conditions += conditions_config.size();
	}

	uint32_t number_of_bool_conditions = 0;
	for (auto& transition_config : transitions_config)
	{
		//Conditions
		std::vector<Config> conditions_config;
		transition_config.GetChildrenConfig("BoolConditions", conditions_config);

		number_of_int_conditions += conditions_config.size();
	}

	uint32_t num_clips = clips_config.size();
	uint32_t num_states = states_config.size();
	uint32_t num_transitions = transitions_config.size();
	uint32_t num_float_variables_config = float_variables_config.size();
	uint32_t num_int_variables_config = int_variables_config.size();
	uint32_t num_bool_variables_config = bool_variables_config.size();
	uint32_t ranges[6] = { num_clips, num_states, num_transitions, num_float_variables_config, num_int_variables_config, num_bool_variables_config };

	uint32_t size_of_clip = sizeof(uint64_t) + sizeof(uint32_t) + sizeof(bool);
	uint32_t size_of_state = sizeof(uint64_t) * 2 + sizeof(float);
	uint32_t size_of_transitions = sizeof(uint64_t) * 5 + sizeof(bool) + 3 * sizeof(uint32_t);// number of conditions
	
	//Conditions size
	uint32_t size_of_float_variables = sizeof(uint64_t) + sizeof(float);
	uint32_t size_of_int_variables = sizeof(uint64_t) + sizeof(int);
	uint32_t size_of_bool_variables = sizeof(uint64_t) + sizeof(bool);
	uint32_t size_of_float_condition = sizeof(uint64_t) * 2 + sizeof(float);
	uint32_t size_of_int_condition = sizeof(uint64_t) * 2 + sizeof(int);
	uint32_t size_of_bool_condition = sizeof(uint64_t) * 2 + sizeof(bool);
	uint32_t size = sizeof(ranges) + size_of_clip * num_clips + size_of_transitions * num_transitions + size_of_state * num_states + size_of_float_variables * num_float_variables_config + size_of_int_variables * num_int_variables_config + size_of_bool_variables * num_bool_variables_config +  number_of_float_conditions * size_of_float_condition + number_of_int_conditions * size_of_int_condition + number_of_bool_conditions * size_of_bool_condition + sizeof(uint64_t)/*Default state*/;

	char* data = new char[size]; // Allocate
	char* cursor = data;
	size_t bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	cursor += bytes; // Store Clips

	for (auto & clip : clips_config)
	{

		std::string name;
		clip.GetString("Name", name, "");
		uint64_t name_hash = std::hash<std::string>{}(name);

		uint32_t animation_uuid = clip.GetUInt32("AnimationUUID", 0);

		bool loop = clip.GetBool("Loop", false);

		bytes = sizeof(uint64_t);
		memcpy(cursor, &name_hash, bytes);
		cursor += bytes; // Store Clip

		bytes = sizeof(uint32_t);
		memcpy(cursor, &animation_uuid, bytes);
		cursor += bytes;

		bytes = sizeof(bool);
		memcpy(cursor, &loop, bytes);
		cursor += bytes;
	}

	for (auto & state : states_config)
	{
		std::string clip_name;
		std::string name;
		state.GetString("Name", name, "");
		state.GetString("ClipName", clip_name, "");
		uint64_t name_hash = std::hash<std::string>{}(name);
		uint64_t clip_hash = std::hash<std::string>{}(clip_name);
		//Adding speed
		float clip_speed = state.GetFloat("Speed", 1.0f);
		//
		bytes = sizeof(uint64_t);
		memcpy(cursor, &name_hash, bytes);
		cursor += bytes; // Store states
		bytes = sizeof(uint64_t);
		memcpy(cursor, &clip_hash, bytes);
		cursor += bytes; // Store states

		bytes = sizeof(float);//for speed
		memcpy(cursor, &clip_speed, bytes);
		cursor += bytes;
	}

	for (auto & transition : transitions_config)
	{
		std::string trigger;
		uint64_t source = transition.GetUInt("Source", 0);
		uint64_t target = transition.GetUInt("Target", 0);
		transition.GetString("Trigger", trigger, "");
		int64_t interpolation_time = transition.GetInt64("Interpolation", 0);
		uint64_t trigger_hash = std::hash<std::string>{}(trigger);
		uint64_t priority = transition.GetUInt("Priority", 0);
		bool automatic = transition.GetBool("Automatic", false);
		
		bytes = sizeof(uint64_t);
		memcpy(cursor, &source, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(cursor, &target, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(cursor, &trigger_hash, bytes);
		cursor += bytes;

		bytes = sizeof(uint64_t);
		memcpy(cursor, &interpolation_time, bytes);
		cursor += bytes; 

		bytes = sizeof(uint64_t);
		memcpy(cursor, &priority, bytes);
		cursor += bytes;

		bytes = sizeof(bool);
		memcpy(cursor, &automatic, bytes);
		cursor += bytes;


		//Float Conditions
		std::vector<Config> float_conditions_config;
		transition.GetChildrenConfig("FloatConditions", float_conditions_config);

		uint32_t number_of_float_conditions = float_conditions_config.size();

		bytes = sizeof(uint32_t);
		memcpy(cursor, &number_of_float_conditions, bytes);
		cursor += bytes;

		for(auto& condition : float_conditions_config)
		{
			uint64_t name_hash = condition.GetUInt("VariableNameHash", 0);
			uint64_t comparator = condition.GetUInt("Comparator", 0);
			float value = condition.GetFloat("Value", 0.f);

			bytes = sizeof(uint64_t);
			memcpy(cursor, &name_hash, bytes);
			cursor += bytes;

			bytes = sizeof(uint64_t);
			memcpy(cursor, &comparator, bytes);
			cursor += bytes;

			bytes = sizeof(float);
			memcpy(cursor, &value, bytes);
			cursor += bytes;
		}

		//Int Conditions
		std::vector<Config> int_conditions_config;
		transition.GetChildrenConfig("IntConditions", int_conditions_config);

		uint32_t number_of_int_conditions = int_conditions_config.size();

		bytes = sizeof(uint32_t);
		memcpy(cursor, &number_of_int_conditions, bytes);
		cursor += bytes;

		for (auto& condition : int_conditions_config)
		{
			uint64_t name_hash = condition.GetUInt("VariableNameHash", 0);
			uint64_t comparator = condition.GetUInt("Comparator", 0);
			int value = condition.GetInt("Value", 0);

			bytes = sizeof(uint64_t);
			memcpy(cursor, &name_hash, bytes);
			cursor += bytes;

			bytes = sizeof(uint64_t);
			memcpy(cursor, &comparator, bytes);
			cursor += bytes;

			bytes = sizeof(int);
			memcpy(cursor, &value, bytes);
			cursor += bytes;
		}

		//Bool Conditions
		std::vector<Config> bool_conditions_config;
		transition.GetChildrenConfig("BoolConditions", bool_conditions_config);

		uint32_t number_of_bool_conditions = bool_conditions_config.size();

		bytes = sizeof(uint32_t);
		memcpy(cursor, &number_of_bool_conditions, bytes);
		cursor += bytes;

		for (auto& condition : bool_conditions_config)
		{
			uint64_t name_hash = condition.GetUInt("VariableNameHash", 0);
			uint64_t comparator = condition.GetUInt("Comparator", 0);
			bool value = condition.GetBool("Value", false);

			bytes = sizeof(uint64_t);
			memcpy(cursor, &name_hash, bytes);
			cursor += bytes;

			bytes = sizeof(uint64_t);
			memcpy(cursor, &comparator, bytes);
			cursor += bytes;

			bytes = sizeof(bool);
			memcpy(cursor, &value, bytes);
			cursor += bytes;
		}

	}

	for(auto& float_variable : float_variables_config)
	{
		uint64_t name_hash = float_variable.GetUInt("FloatNameHash", 0);
		float value = float_variable.GetFloat("FloatValue", 0.f);

		bytes = sizeof(uint64_t);
		memcpy(cursor, &name_hash, bytes);
		cursor += bytes;

		bytes = sizeof(float);
		memcpy(cursor, &value, bytes);
		cursor += bytes;
	}

	for (auto& int_variable : int_variables_config)
	{
		uint64_t name_hash = int_variable.GetUInt("IntNameHash", 0);
		int value = int_variable.GetInt("IntValue", 0);

		bytes = sizeof(uint64_t);
		memcpy(cursor, &name_hash, bytes);
		cursor += bytes;

		bytes = sizeof(int);
		memcpy(cursor, &value, bytes);
		cursor += bytes;
	}

	for (auto& bool_variable : bool_variables_config)
	{
		uint64_t name_hash = bool_variable.GetUInt("BoolNameHash", 0);
		bool value = bool_variable.GetBool("BoolValue", false);

		bytes = sizeof(uint64_t);
		memcpy(cursor, &name_hash, bytes);
		cursor += bytes;

		bytes = sizeof(bool);
		memcpy(cursor, &value, bytes);
		cursor += bytes;
	}

	uint64_t default_state = state_machine_config.GetUInt("Default", 0);
	bytes = sizeof(uint64_t);
	memcpy(cursor, &default_state, bytes);
	cursor += bytes;

	return FileData{data, size};
}
