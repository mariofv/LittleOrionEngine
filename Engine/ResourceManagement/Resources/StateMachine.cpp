#include "StateMachine.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Helper/Config.h"

StateMachine::StateMachine(const std::string & mesh_file_path):
Resource(0, mesh_file_path)
{
	//states.push_back(std::make_shared<State>("Entry"));
}


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
		Config clip_config;
		clip_config.AddString(state->name, "Name");
		clip_config.AddString(state->clip->name, "clipName");
		states_config.push_back(clip_config);
	}
	state_machine_config.AddChildrenConfig(states_config, "States");

	std::vector<Config> transitions_config(transitions.size());
	for (auto & transition : transitions)
	{
		Config clip_config;
		clip_config.AddString(transition->source, "Source");
		clip_config.AddString(transition->target, "Target");
		clip_config.AddString(transition->trigger, "Trigger");
		clip_config.AddInt64(transition->interpolation_time, "Interpolation");
		transitions_config.push_back(clip_config);
	}
	state_machine_config.AddChildrenConfig(transitions_config, "Transitions");

	std::string serialized_state_machine_string;
	state_machine_config.GetSerializedString(serialized_state_machine_string);

	App->filesystem->Save(exported_file.c_str(), serialized_state_machine_string.c_str(), serialized_state_machine_string.size());
}