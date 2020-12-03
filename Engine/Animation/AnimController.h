#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"
#include "EditorUI/Panel/PanelStateMachine.h"

#include <vector>

struct Clip;
class GameObject;
class Skeleton;
class StateMachine;
struct State;
struct Transition;

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

typedef std::pair<size_t, GameObject*> AttachedBone;
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
	void UpdateAttachedBones(const std::shared_ptr<Skeleton>& skeleton, const std::vector<math::float4x4>& palette);
	void StartNextState(const std::string& trigger);
	bool IsOnState(const std::string& state);
	void SetSpeed(float speed);
	void SetFloat(uint64_t name_hash, float value);
	void SetInt(uint64_t name_hash, int value);
	void SetBool(uint64_t name_hash, bool value);
	void SetActiveState(const std::string& state, float interpolation_time);
	void SetIgnoreTransitions(bool enable);
private:
	void SetActiveState(std::shared_ptr<State>& state);
	void FinishActiveState();
	void AdjustInterpolationTimes();
	void ApplyAutomaticTransitionIfNeeded();
	//Variables
	void CheckConditions();
public:
	std::shared_ptr<StateMachine> state_machine = nullptr;
	std::vector<PlayingClip> playing_clips;
	std::vector<AttachedBone> attached_bones;

private:
	std::shared_ptr<Transition> active_transition;
	bool apply_transition = false;
	bool applying_automatic_transition = false;
	bool ignore_transition = false;
	std::shared_ptr<State> active_state = nullptr;
	friend class PanelComponent;
	friend class PanelStateMachine;
};

#endif //_ANIMCONTROLLER_H_

