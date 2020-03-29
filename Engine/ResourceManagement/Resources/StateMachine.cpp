#include "StateMachine.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Helper/Config.h"

StateMachine::StateMachine(const std::string & mesh_file_path):
Resource(0, mesh_file_path)
{
	//states.push_back(std::make_shared<State>("Entry"));
}

//TODO: Do it like a binary
void StateMachine::Save() const
{
	Config state_machine_config;
	std::vector<Config> clips_config(clips.size());
	for (auto & clip : clips)
	{
		Config clip_config;
		clip_config.AddString(clip->animation->exported_file, "AnimationUUID");
		clip_config.AddString(clip->name, "Name");
		clip_config.AddBool(clip->loop, "Loop");
		clips_config.push_back(clip_config);
	}
	state_machine_config.AddChildrenConfig(clips_config, "Clips");

	std::vector<Config> states_config(states.size());
	for (auto & state : states)
	{
		Config state_config;
		state_config.AddString(state->name, "Name");
		state_config.AddString(state->clip->name, "ClipName");
		states_config.push_back(state_config);
	}
	state_machine_config.AddChildrenConfig(states_config, "States");

	std::vector<Config> transitions_config(transitions.size());
	for (auto & transition : transitions)
	{
		Config transition_config;
		transition_config.AddString(transition->source, "Source");
		transition_config.AddString(transition->target, "Target");
		transition_config.AddString(transition->trigger, "Trigger");
		transition_config.AddInt64(transition->interpolation_time, "Interpolation");
		transitions_config.push_back(transition_config);
	}
	state_machine_config.AddChildrenConfig(transitions_config, "Transitions");

	std::string serialized_state_machine_string;
	state_machine_config.GetSerializedString(serialized_state_machine_string);

	App->filesystem->Save(exported_file.c_str(), serialized_state_machine_string.c_str(), serialized_state_machine_string.size());
}

void StateMachine::Load(const File & file)
{
	size_t size;
	char * state_machine_data =  App->filesystem->Load(file.file_path.c_str(), size);
	if (state_machine_data == nullptr)
	{
		return;
	}
	std::string serialized_machine_string = state_machine_data;
	free(state_machine_data);

	Config state_machine_config(serialized_machine_string);

	std::vector<Config> clips_config(clips.size());
	state_machine_config.GetChildrenConfig("States", clips_config);
	for (auto & clip_config : clips_config)
	{
		std::string animation;
		std::string name;
		clip_config.GetString("AnimationUUID", animation, "");
		clip_config.GetString( "Name", name, "");
		bool loop = clip_config.GetBool("Loop", false);

		this->clips.push_back(std::make_shared<Clip>(name,App->resources->Load<Animation>(animation),loop));
	}

	std::vector<Config> states_config(states.size());
	state_machine_config.GetChildrenConfig( "States", states_config);
	for (auto & state_config : states_config)
	{
		std::string clip_name;
		std::string name;
		state_config.GetString("Name", name, "");
		state_config.GetString("ClipName", clip_name, "");
		std::shared_ptr<Clip> state_clip;
		for (auto & clip : clips)
		{
			if (clip->name == name)
			{
				state_clip = clip;
			}
		}
		this->states.push_back(std::make_shared<State>(name, state_clip));
	}

	std::vector<Config> transitions_config(transitions.size());
	state_machine_config.GetChildrenConfig("Transitions", transitions_config);
	for (auto & transition_config : transitions_config)
	{
		std::string source;
		std::string target;
		std::string trigger;
		transition_config.GetString("Source", source, "");
		transition_config.GetString("Target", target, "");
		transition_config.GetString("Trigger", trigger, "");
		int64_t interpolation_time = transition_config.GetInt64("Interpolation", 0);
		this->transitions.push_back(std::make_shared<Transition>(source, target, trigger, interpolation_time));
	}
}
