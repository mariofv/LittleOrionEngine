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
	std::unordered_map<uint32_t,std::vector<size_t>> skeleton_channels_joints_map;
	int animation_time = 0;
};

struct State
{
	State() = default;
	State(std::string name, std::shared_ptr<Clip> clip);

	std::string name;
	uint64_t name_hash = 0;
	std::shared_ptr<Clip> clip = nullptr;
};

struct Transition
{
	Transition() = default;
	Transition(uint64_t source, uint64_t target, std::string & trigger, long interpolation);
	uint64_t source_hash = 0;
	uint64_t target_hash = 0;
	uint64_t trigger_hash = 0;
	std::string trigger;
	uint64_t interpolation_time = 100;
};

class StateMachine : public Resource
{
public:
	StateMachine() = default;
	StateMachine(Metafile* resource_metafile) : Resource(resource_metafile) {};
	StateMachine(Metafile* resource_metafile, std::vector<std::shared_ptr<Clip>> && clips, std::vector<std::shared_ptr<State>> && states, std::vector<std::shared_ptr<Transition>> && transitions);
	StateMachine& operator=(const StateMachine& state_machine_to_copy);

	~StateMachine() = default;

	std::shared_ptr<State> GetDefaultState() const;
	std::shared_ptr<State> GetState(uint64_t state_hash) const;
	std::shared_ptr<Transition> GetTransition(const std::string & trigger, uint64_t state_hash) const;

	void Save() const;
	void Load(const Config& config);

private:
	void RemoveState(const std::shared_ptr<State> & state);
	void RemoveClip(const std::shared_ptr<Clip> & state);
	void AddClipToState(std::shared_ptr<State> & state, uint32_t animation_uuid);

public:
	std::vector<std::shared_ptr<Clip>> clips;
	std::vector<std::shared_ptr<State>> states;
	std::vector<std::shared_ptr<Transition>> transitions;
	uint64_t default_state = 0;
	friend class PanelStateMachine;
};

namespace Loader
{
	template<>
	static std::shared_ptr<StateMachine> Load(Metafile* metafile, const FileData& resource_data)
	{
		return StateMachineManager::Load(metafile, resource_data);
	}
}
#endif // !_STATEMACHINE_H_


