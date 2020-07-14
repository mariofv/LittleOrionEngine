#include "StateMachine.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleResourceManager.h"
#include "Helper/Config.h"

//Structs

Clip::Clip(std::string& name, std::shared_ptr<Animation>& animation, bool loop) :
	name(name), name_hash(std::hash<std::string>{}(name)), animation(animation), loop(loop) {

	if (animation != nullptr)
	{
		animation_time = (animation->frames / animation->frames_per_second) * 1000;
	}
}

void Clip::SetAnimation(const std::shared_ptr<Animation>& animation)
{
	this->animation = animation;
	animation_time = (animation->frames / animation->frames_per_second) * 1000;
}


State::State(std::string name, std::shared_ptr<Clip> clip) :
	name(name), name_hash(std::hash<std::string>{}(name)),clip(clip) {
}

State::State(std::string name, std::shared_ptr<Clip> clip, float speed) :
	name(name), name_hash(std::hash<std::string>{}(name)), clip(clip), speed(speed) {
}

State::State(std::string name, std::shared_ptr<Clip> clip, float speed, float2& position) :
	name(name), name_hash(std::hash<std::string>{}(name)), clip(clip), speed(speed), position(position){
}
Transition::Transition(uint64_t source, uint64_t target, std::string & trigger, uint64_t interpolation) :
	source_hash(source), 
	trigger(trigger),
	target_hash(target),
	trigger_hash(std::hash<std::string>{}(trigger)),
	interpolation_time(interpolation) {};

StateMachine::StateMachine(uint32_t uuid, std::vector<std::shared_ptr<Clip>>&& clips, std::vector<std::shared_ptr<State>>&& states, std::vector<std::shared_ptr<Transition>>&& transitions)
	: clips(clips)
	,states(states)
	,transitions(transitions)
	,Resource(uuid)
{
}

StateMachine & StateMachine::operator=(const StateMachine & state_machine_to_copy)
{
	this->clips = state_machine_to_copy.clips;
	this->states = state_machine_to_copy.states;
	this->transitions = state_machine_to_copy.transitions;
	this->default_state = state_machine_to_copy.default_state;
	return *this;
}

std::shared_ptr<State> StateMachine::GetDefaultState() const
{
	if (states.size() == 0)
	{
		return nullptr;
	}
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
	 return states[0];
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

std::shared_ptr<Transition> StateMachine::GetTriggerTransition(const std::string & trigger, uint64_t state_hash) const
{
	uint64_t trigger_hash = std::hash<std::string>{}(trigger);
	for (auto & transition : transitions)
	{
		if (transition->source_hash == state_hash &&  transition->trigger_hash == trigger_hash)
		{
			return transition;
		}
	}
	return nullptr;
}

std::shared_ptr<Transition> StateMachine::GetAutomaticTransition(uint64_t state_hash) const
{
	std::shared_ptr<Transition> automatic_transition = nullptr;
	for (auto & transition : transitions)
	{
		if (transition->source_hash == state_hash && transition->automatic)
		{
			automatic_transition = !automatic_transition || automatic_transition->priority < transition->priority ?  transition : automatic_transition;
		}
	}
	return automatic_transition;
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
	if (!is_being_use)
	{
		uint64_t clip_hash = clip->name_hash;
		auto clip_it = std::remove_if(clips.begin(), clips.end(), [&clip_hash](const auto & clip) {
			return clip->name_hash == clip_hash;
		});
		clips.erase(clip_it);
	}
}

void StateMachine::AddClipToState(std::shared_ptr<State>& state, uint32_t animation_uuid)
{
	std::shared_ptr<Animation> animation = App->resources->Load<Animation>(animation_uuid);
	std::shared_ptr<Clip> new_clip = std::make_shared<Clip>(App->resources->resource_DB->GetEntry(animation_uuid)->resource_name, animation, false);
	if (state->clip && new_clip->animation == state->clip->animation)
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

void StateMachine::Save(Config& config) const
{
	std::vector<Config> clips_config;
	for (auto& clip : clips)
	{
		Config clip_config;
		clip_config.AddUInt(clip->animation->GetUUID(), "AnimationUUID");
		clip_config.AddString(clip->name, "Name");
		clip_config.AddBool(clip->loop, "Loop");
		clips_config.push_back(clip_config);
	}
	config.AddChildrenConfig(clips_config, "Clips");

	std::vector<Config> states_config;
	for (auto& state : states)
	{
		Config state_config;
		state_config.AddString(state->name, "Name");
		if (state->clip != nullptr)
		{
			state_config.AddString(state->clip->name, "ClipName");
		}
		state_config.AddFloat(state->speed, "Speed");//Saving speed
		state_config.AddFloat(state->position.x, "XPosition");
		state_config.AddFloat(state->position.y, "YPosition");
		states_config.push_back(state_config);
	}
	config.AddChildrenConfig(states_config, "States");

	std::vector<Config> transitions_config;
	for (auto& transition : transitions)
	{
		Config transition_config;
		transition_config.AddUInt(transition->source_hash, "Source");
		transition_config.AddUInt(transition->target_hash, "Target");
		transition_config.AddString(transition->trigger, "Trigger");
		transition_config.AddUInt(transition->interpolation_time, "Interpolation");
		transition_config.AddBool(transition->automatic, "Automatic");
		transition_config.AddUInt(transition->priority, "Priority");
		transitions_config.push_back(transition_config);
	}
	config.AddChildrenConfig(transitions_config, "Transitions");

	config.AddUInt(default_state, "Default");
}

void StateMachine::Load(const Config& config)
{
	this->clips.clear();
	this->transitions.clear();
	this->states.clear();
	std::vector<Config> clips_config;
	config.GetChildrenConfig("Clips", clips_config);
	for (auto& clip_config : clips_config)
	{
		std::string name;
		clip_config.GetString("Name", name, "");

		uint32_t animation_uuid = clip_config.GetUInt("AnimationUUID", 0);
		std::shared_ptr<Animation> anim = nullptr;
		if (animation_uuid != 0)
		{
			anim = App->resources->Load<Animation>(animation_uuid);
		}

		bool loop = clip_config.GetBool("Loop", false);
		this->clips.push_back(std::make_shared<Clip>(name, anim, loop));
	}

	std::vector<Config> states_config;
	config.GetChildrenConfig("States", states_config);
	for (auto& state_config : states_config)
	{
		std::string clip_name;
		std::string name;
		state_config.GetString("Name", name, "");
		state_config.GetString("ClipName", clip_name, "");
		float clip_speed = state_config.GetFloat("Speed", 1.0f);
		std::shared_ptr<Clip> state_clip;
		for (auto& clip : clips)
		{
			if (clip->name == clip_name)
			{
				state_clip = clip;
			}
		}
		float2 position(10.0f, 10.0f);
		position.x = state_config.GetFloat("XPosition", 10.0f);
		position.y = state_config.GetFloat("YPosition", 10.0f);
		this->states.push_back(std::make_shared<State>(name, state_clip, clip_speed, position));
	}

	std::vector<Config> transitions_config;
	config.GetChildrenConfig("Transitions", transitions_config);
	for (auto& transition_config : transitions_config)
	{
		std::string trigger;
		uint64_t source = transition_config.GetUInt("Source", 0);
		uint64_t target = transition_config.GetUInt("Target", 0);
		transition_config.GetString("Trigger", trigger, "");
		uint64_t interpolation_time = transition_config.GetUInt("Interpolation", 0);
		this->transitions.push_back(std::make_shared<Transition>(source, target, trigger, interpolation_time));
		this->transitions.back()->automatic = transition_config.GetBool("Automatic", false);
		this->transitions.back()->priority = transition_config.GetUInt("Priority", 0);
	}

	default_state = config.GetUInt("Default", 0);
}

void StateMachine::LoadNames(const Config & config)
{

	std::vector<Config> clips_config;
	config.GetChildrenConfig("Clips", clips_config);
	for (auto& clip_config : clips_config)
	{
		std::string name;
		clip_config.GetString("Name", name, "");

		uint64_t name_hash = (std::hash<std::string>{}(name));
		for (auto & clip : clips)
		{
			if (clip->name_hash == name_hash)
			{
				clip->name = name;
			}
		}
	}

	std::vector<Config> states_config;
	config.GetChildrenConfig("States", states_config);
	for (auto& state_config : states_config)
	{
		std::string name;
		state_config.GetString("Name", name, "");
		uint64_t name_hash = (std::hash<std::string>{}(name));

		float2 position(10.0f, 10.0f);
		position.x = state_config.GetFloat("XPosition", 10.0f);
		position.y = state_config.GetFloat("YPosition", 10.0f);
		for (auto & state : states)
		{
			if (state->name_hash == name_hash)
			{
				state->name = name;
				state->position = position;
			}
		}
	}

	std::vector<Config> transitions_config;
	config.GetChildrenConfig("Transitions", transitions_config);
	for (auto& transition_config : transitions_config)
	{
		std::string trigger;
		transition_config.GetString("Trigger", trigger, "");
		uint64_t trigger_hash = (std::hash<std::string>{}(trigger));
		for (auto & transition : transitions)
		{
			if (transition->trigger_hash == trigger_hash)
			{
				transition->trigger = trigger;
			}
		}
	}

}
