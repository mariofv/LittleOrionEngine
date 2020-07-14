#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "Resource.h"
#include "Animation.h"
#include "ResourceManagement/Manager/StateMachineManager.h"
#include "EditorUI/Panel/PanelStateMachine.h"
#include <unordered_map>

class File;
struct Clip
{
	Clip() = default;
	Clip(std::string& name, std::shared_ptr<Animation>& animation, bool loop);

	void SetAnimation(const std::shared_ptr<Animation>& animation);
	std::string name;
	uint64_t name_hash = 0;
	std::shared_ptr<Animation> animation = nullptr;
	bool loop = false;

	//RunTime only
	std::unordered_map<uint32_t, std::vector<size_t>> skeleton_channels_joints_map;
	float animation_time = 0.0f;
};

struct State
{
	State() = default;
	State(std::string name, std::shared_ptr<Clip> clip);
	State(std::string name, std::shared_ptr<Clip> clip, float speed);
	State(std::string name, std::shared_ptr<Clip> clip, float speed, float2& position);
	std::string name;
	uint64_t name_hash = 0;
	std::shared_ptr<Clip> clip = nullptr;
	//Adding speed parameter for accelerate/decelerate clip
	float speed = 1.0f;
	
	float2 position = { 10.0f, 10.0f };
};

template <typename T>
struct Parameter {
	std::string name;
	uint64_t hash;
	T value;
};

struct Transition
{
	Transition() = default;
	Transition(uint64_t source, uint64_t target, std::string & trigger, uint64_t interpolation);
	uint64_t source_hash = 0;
	uint64_t target_hash = 0;
	uint64_t trigger_hash = 0;

	//Conditions
	std::string trigger;
	uint64_t interpolation_time = 100;

	bool automatic = false;
	uint64_t priority = 0;

	std::vector<std::shared_ptr<Parameter<int>>> parameters;
};

class StateMachine : public Resource
{
public:
	StateMachine() = default;
	StateMachine(uint32_t uuid) : Resource(uuid) {};
	StateMachine(uint32_t uuid, std::vector<std::shared_ptr<Clip>> && clips, std::vector<std::shared_ptr<State>> && states, std::vector<std::shared_ptr<Transition>> && transitions);
	StateMachine& operator=(const StateMachine& state_machine_to_copy);

	~StateMachine() = default;

	std::shared_ptr<State> GetDefaultState() const;
	std::shared_ptr<State> GetState(uint64_t state_hash) const;
	std::shared_ptr<Transition> GetTriggerTransition(const std::string & trigger, uint64_t state_hash) const;
	std::shared_ptr<Transition> GetAutomaticTransition(uint64_t state_hash) const;

	void Save(Config& config) const;
	void Load(const Config& config);
	void LoadNames(const Config& config);

private:
	void RemoveState(const std::shared_ptr<State> & state);
	void RemoveClip(const std::shared_ptr<Clip> & state);
	void AddClipToState(std::shared_ptr<State> & state, uint32_t animation_uuid);

public:
	std::vector<std::shared_ptr<Clip>> clips;
	std::vector<std::shared_ptr<State>> states;
	std::vector<std::shared_ptr<Transition>> transitions;
	std::vector<std::shared_ptr<Parameter<int>>> parameters;
	uint64_t default_state = 0;
	friend class PanelStateMachine;
	
};

namespace ResourceManagement
{
	template<>
	static FileData Binarize<StateMachine>(Resource* state_machine)
	{
		return StateMachineManager::Binarize(static_cast<StateMachine*>(state_machine));
	};

	template<>
	static std::shared_ptr<StateMachine> Load(uint32_t uuid, const FileData& resource_data)
	{
		return StateMachineManager::Load(uuid, resource_data);
	}

	template<>
	static FileData Create<StateMachine>()
	{
		return StateMachineManager::Create();
	};
}
#endif // !_STATEMACHINE_H_


