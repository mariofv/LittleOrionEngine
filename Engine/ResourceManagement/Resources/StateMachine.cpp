#include "StateMachine.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Helper/Config.h"

//Structs

Clip::Clip(std::string& name, std::shared_ptr<Animation>& animation, bool loop) :
	name(name), name_hash(std::hash<std::string>{}(name)), animation(animation), loop(loop), animation_time((animation->frames / animation->frames_per_second) * 1000) {}
void Clip::SetAnimation(const std::shared_ptr<Animation>& animation)
{
	this->animation = animation;
	animation_time = (animation->frames / animation->frames_per_second) * 1000;
}


State::State(std::string name, std::shared_ptr<Clip> clip) :
	name(name), name_hash(std::hash<std::string>{}(name)),clip(clip)  {
}

Transition::Transition(uint64_t source, uint64_t target, std::string & trigger, long interpolation) :
	source_hash(source), 
	trigger(trigger),
	target_hash(target),
	trigger_hash(std::hash<std::string>{}(trigger)),
	interpolation_time(interpolation) {};

StateMachine::StateMachine(std::vector<std::shared_ptr<Clip>>&& clips, std::vector<std::shared_ptr<State>>&& states, std::vector<std::shared_ptr<Transition>>&& transitions, const std::string & file_path)
	: clips(clips)
	,states(states)
	,transitions(transitions)
	,Resource(0, file_path)
{
}

StateMachine::StateMachine(const std::string& file_path):
Resource(0, file_path)
{

}

StateMachine & StateMachine::operator=(const StateMachine & state_machine_to_copy)
{
	this->clips = state_machine_to_copy.clips;
	this->states = state_machine_to_copy.states;
	this->transitions = state_machine_to_copy.transitions;
	return *this;
}

std::shared_ptr<State> StateMachine::GetDefaultState() const
{
	if (default_state == 0)
	{
		return states[0];
	}
	for (auto state : states)
	{
		if (state->name_hash == default_state)
		{
			return state;
		}
	}
	return nullptr;
}

std::shared_ptr<State> StateMachine::GetState(uint64_t state_hash) const
{
	for (auto & state : states)
	{
		if (state->name_hash == state_hash)
		{
			return state;
		}
	}
	return nullptr;
}

std::shared_ptr<Transition> StateMachine::GetTransition(const std::string & trigger, uint64_t state_hash) const
{
	uint64_t trigger_hash = std::hash<std::string>{}(trigger);
	for (auto & transition : transitions)
	{
		if (transition->source_hash == state_hash &&  transition->trigger_hash == trigger_hash)
		{
			return transition;
		}
	}
}


void StateMachine::RemoveState(const std::shared_ptr<State>& state)
{
	auto states_it = std::find(states.begin(), states.end(), state);
	if (states_it != states.end())
	{
		uint64_t transition_hash = (*states_it)->name_hash;
		auto transition_it = std::remove_if(transitions.begin(), transitions.end(), [&transition_hash](const auto & transition){
			return transition->source_hash == transition_hash || transition->target_hash == transition_hash;
		});
		if (transition_it != transitions.end())
		{
			transitions.erase(transition_it);
		}
		std::shared_ptr<Clip> clip = (*states_it)->clip;
		states.erase(states_it);
		if (clip != nullptr)
		{
			RemoveClip(clip);
		}
	}
}

void StateMachine::RemoveClip(const std::shared_ptr<Clip>& clip)
{
	bool is_being_use = false;
	for (auto & state : states)
	{
		is_being_use = state->clip && state->clip->name_hash == clip->name_hash;
		if (is_being_use)
		{
			break;
		}
	}
	if (!is_being_use)
	{
		uint64_t clip_hash = clip->name_hash;
		auto clip_it = std::remove_if(clips.begin(), clips.end(), [&clip_hash](const auto & clip) {
			return clip->name_hash == clip_hash;
		});
		clips.erase(clip_it);
	}
}

void StateMachine::AddClipToState(std::shared_ptr<State>& state, File& clip_file)
{
	std::string meta_path = Importer::GetMetaFilePath(clip_file.file_path);
	ImportOptions meta;
	Importer::GetOptionsFromMeta(meta_path, meta);
	std::shared_ptr<Animation> animation = App->resources->Load<Animation>(meta.exported_file);
	std::shared_ptr<Clip> new_clip = std::make_shared<Clip>(clip_file.filename_no_extension, animation, false);
	if (new_clip->animation == state->clip->animation)
	{
		return;
	}
	std::shared_ptr<Clip> old_clip = state->clip;
	state->clip = new_clip;
	if (old_clip != nullptr)
	{
		RemoveClip(old_clip);
	}
	clips.push_back(new_clip);
}

//TODO: Do it like a binary
void StateMachine::Save() const
{
	Config state_machine_config;
	std::vector<Config> clips_config;
	for (auto& clip : clips)
	{
		Config clip_config;
		clip_config.AddString(clip->animation->exported_file, "AnimationUUID");
		clip_config.AddString(clip->name, "Name");
		clip_config.AddBool(clip->loop, "Loop");
		clips_config.push_back(clip_config);
	}
	state_machine_config.AddChildrenConfig(clips_config, "Clips");

	std::vector<Config> states_config;
	for (auto& state : states)
	{
		Config state_config;
		state_config.AddString(state->name, "Name");
		if (state->clip != nullptr)
		{
			state_config.AddString(state->clip->name, "ClipName");
		}
		states_config.push_back(state_config);
	}
	state_machine_config.AddChildrenConfig(states_config, "States");

	std::vector<Config> transitions_config;
	for (auto& transition : transitions)
	{
		Config transition_config;
		transition_config.AddUInt(transition->source_hash, "Source");
		transition_config.AddUInt(transition->target_hash, "Target");
		transition_config.AddString(transition->trigger, "Trigger");
		transition_config.AddInt64(transition->interpolation_time, "Interpolation");
		transitions_config.push_back(transition_config);
	}
	state_machine_config.AddChildrenConfig(transitions_config, "Transitions");

	state_machine_config.AddUInt(default_state, "Default");
	std::string serialized_state_machine_string;
	state_machine_config.GetSerializedString(serialized_state_machine_string);

	App->filesystem->Save(exported_file.c_str(), serialized_state_machine_string.c_str(), serialized_state_machine_string.size());
}

void StateMachine::Load(const File& file)
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

	std::vector<Config> clips_config;
	state_machine_config.GetChildrenConfig("Clips", clips_config);
	for (auto& clip_config : clips_config)
	{
		std::string animation;
		std::string name;
		clip_config.GetString("AnimationUUID", animation, "");
		clip_config.GetString( "Name", name, "");
		bool loop = clip_config.GetBool("Loop", false);
		std::shared_ptr<Animation> anim = nullptr;
		if (!animation.empty())
		{
			anim = App->resources->Load<Animation>(animation);
		}
		this->clips.push_back(std::make_shared<Clip>(name, anim,loop));
	}

	std::vector<Config> states_config;
	state_machine_config.GetChildrenConfig( "States", states_config);
	for (auto& state_config : states_config)
	{
		std::string clip_name;
		std::string name;
		state_config.GetString("Name", name, "");
		state_config.GetString("ClipName", clip_name, "");
		std::shared_ptr<Clip> state_clip;
		for (auto& clip : clips)
		{
			if (clip->name == clip_name)
			{
				state_clip = clip;
			}
		}
		this->states.push_back(std::make_shared<State>(name, state_clip));
	}

	std::vector<Config> transitions_config;
	state_machine_config.GetChildrenConfig("Transitions", transitions_config);
	for (auto& transition_config : transitions_config)
	{
		std::string trigger;
		uint64_t source = transition_config.GetUInt("Source", 0);
		uint64_t target = transition_config.GetUInt("Target", 0);
		transition_config.GetString("Trigger", trigger, "");
		int64_t interpolation_time = transition_config.GetInt64("Interpolation", 0);
		this->transitions.push_back(std::make_shared<Transition>(source, target, trigger, interpolation_time));
	}

	default_state = state_machine_config.GetUInt("Default", 0);
}
