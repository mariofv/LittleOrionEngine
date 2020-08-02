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

	uint32_t ranges[6];
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

		//Float conditions
		uint32_t number_of_float_conditions = 0;
		bytes = sizeof(uint32_t);
		memcpy(&number_of_float_conditions, cursor, bytes);
		cursor += bytes;

		for(size_t i = 0; i < number_of_float_conditions; ++i)
		{
			Condition<float> condition;

			bytes = sizeof(uint64_t);
			memcpy(&condition.name_hash_variable, cursor, bytes);
			cursor += bytes;

			uint64_t comparator = 0;
			bytes = sizeof(uint64_t);
			memcpy(&comparator, cursor, bytes);
			cursor += bytes;

			condition.comparator = static_cast<Comparator>(comparator);

			switch (condition.comparator)
			{
				case Comparator::GREATER:
					condition.comparator_function = std::greater();
					break;
				case Comparator::LESSER:
					condition.comparator_function = std::less();
					break;
				case Comparator::EQUAL:
					condition.comparator_function = std::equal_to();
					break;
				case Comparator::NOT_EQUAL:
					condition.comparator_function = std::not_equal_to();
					break;
				default:
					break;
			}

			bytes = sizeof(float);
			memcpy(&condition.value, cursor, bytes);
			cursor += bytes;

			transition->float_conditions.push_back(condition);
		}

		//Int conditions
		uint32_t number_of_int_conditions = 0;
		bytes = sizeof(uint32_t);
		memcpy(&number_of_int_conditions, cursor, bytes);
		cursor += bytes;

		for (size_t i = 0; i < number_of_int_conditions; ++i)
		{
			Condition<int> condition;

			bytes = sizeof(uint64_t);
			memcpy(&condition.name_hash_variable, cursor, bytes);
			cursor += bytes;

			uint64_t comparator = 0;
			bytes = sizeof(uint64_t);
			memcpy(&comparator, cursor, bytes);
			cursor += bytes;

			condition.comparator = static_cast<Comparator>(comparator);

			switch (condition.comparator)
			{
			case Comparator::GREATER:
				condition.comparator_function = std::greater();
				break;
			case Comparator::LESSER:
				condition.comparator_function = std::less();
				break;
			case Comparator::EQUAL:
				condition.comparator_function = std::equal_to();
				break;
			case Comparator::NOT_EQUAL:
				condition.comparator_function = std::not_equal_to();
				break;
			default:
				break;
			}

			bytes = sizeof(int);
			memcpy(&condition.value, cursor, bytes);
			cursor += bytes;

			transition->int_conditions.push_back(condition);
		}

		//Bool conditions
		uint32_t number_of_bool_conditions = 0;
		bytes = sizeof(uint32_t);
		memcpy(&number_of_bool_conditions, cursor, bytes);
		cursor += bytes;

		for (size_t i = 0; i < number_of_bool_conditions; ++i)
		{
			Condition<bool> condition;

			bytes = sizeof(uint64_t);
			memcpy(&condition.name_hash_variable, cursor, bytes);
			cursor += bytes;

			uint64_t comparator = 2;
			bytes = sizeof(uint64_t);
			memcpy(&comparator, cursor, bytes);
			cursor += bytes;

			condition.comparator = static_cast<Comparator>(comparator);

			switch (condition.comparator)
			{
			case Comparator::EQUAL:
				condition.comparator_function = std::equal_to();
				break;
			case Comparator::NOT_EQUAL:
				condition.comparator_function = std::not_equal_to();
				break;
			default:
				break;
			}

			bytes = sizeof(bool);
			memcpy(&condition.value, cursor, bytes);
			cursor += bytes;

			transition->bool_conditions.push_back(condition);
		}
	}

	std::unordered_map<uint64_t, float> float_variables;
	for(size_t i = 0; i < ranges[3]; ++i)
	{
		uint64_t name_hash;
		bytes = sizeof(uint64_t);
		memcpy(&name_hash, cursor, bytes);
		cursor += bytes;

		float value_float = 0.f;
		bytes = sizeof(float);
		memcpy(&value_float, cursor, bytes);
		cursor += bytes;

		float_variables[name_hash] = value_float;
	}

	std::unordered_map<uint64_t, int> int_variables;
	for (size_t i = 0; i < ranges[4]; ++i)
	{
		uint64_t name_hash;
		bytes = sizeof(uint64_t);
		memcpy(&name_hash, cursor, bytes);
		cursor += bytes;

		int value_int = 0;
		bytes = sizeof(int);
		memcpy(&value_int, cursor, bytes);
		cursor += bytes;

		int_variables[name_hash] = value_int;
	}

	std::unordered_map<uint64_t, bool> bool_variables;
	for (size_t i = 0; i < ranges[5]; ++i)
	{
		uint64_t name_hash;
		bytes = sizeof(uint64_t);
		memcpy(&name_hash, cursor, bytes);
		cursor += bytes;

		bool value_bool = false;
		bytes = sizeof(bool);
		memcpy(&value_bool, cursor, bytes);
		cursor += bytes;

		bool_variables[name_hash] = value_bool;
	}

	std::shared_ptr<StateMachine> new_state_machine = std::make_shared<StateMachine>(uuid, std::move(clips), std::move(states), std::move(transitions));
	
	new_state_machine->SetFloatVariables(float_variables);
	new_state_machine->SetIntVariables(int_variables);
	new_state_machine->SetBoolVariables(bool_variables);
	
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