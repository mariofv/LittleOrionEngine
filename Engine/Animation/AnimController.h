#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"
#include "EditorUI/Panel/PanelStateMachine.h"

#include <vector>
#include <unordered_map>

class StateMachine;
struct State;
struct Clip;
struct Transition;
class Skeleton;
class GameObject;

struct PlayingClip
{
	std::shared_ptr<Clip> clip;
	float speed = 0.0f;
	float current_time = 0.0f;
	bool playing = false;
	float interpolation_time = 0.0f;
	void Update();
};

enum ClipType
{
	ACTIVE = 0,
	NEXT
};
class AnimController
{
public:
	AnimController() { playing_clips.resize(2); };
	~AnimController() = default;

	AnimController(const AnimController& controller_to_copy) = default;
	AnimController(AnimController&& controller_to_move) = default;

	AnimController & operator=(const AnimController& controller_to_copy) = default;
	AnimController & operator=(AnimController&& controller_to_move) = default;

	bool Update();
	void SetStateMachine(uint32_t state_machine_uuid);
	void GetClipTransform(const std::shared_ptr<Skeleton>& skeleton, std::vector<math::float4x4>& pose);
	void UpdateAttachedBones(uint32_t skeleton_uuid, const std::vector<math::float4x4>& pose);
	void StartNextState(const std::string& trigger);
	bool IsOnState(const std::string& state);

private:
	void SetActiveState(std::shared_ptr<State> & state);
	void FinishActiveState();
	void AdjustInterpolationTimes();
	void ApplyAutomaticTransitionIfNeeded();
public:
	std::shared_ptr<StateMachine> state_machine = nullptr;
	std::vector<PlayingClip> playing_clips;
	std::unordered_map<uint32_t, std::vector<std::pair<size_t, GameObject*>>> skeleton_channels_joints_map;

private:
	std::shared_ptr<Transition> active_transition;
	bool apply_transition = false;
	std::shared_ptr<State> active_state = nullptr;
	friend class PanelComponent;
	friend class PanelStateMachine;
};

#endif //_ANIMCONTROLLER_H_

