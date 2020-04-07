#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include <vector>

class StateMachine;
struct State;
struct Clip;
struct Transition;

struct PlayingClip
{
	std::shared_ptr<Clip> clip;
	int current_time = 0;
	bool playing = false;
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

	bool Update();
	void SetActiveAnimation();

	void GetPose(uint32_t skeleton_uuid, std::vector<float4x4> &pose);
	void StartNextState(const std::string & trigger);
private:
	void FinishActiveState();
	std::vector<float4x4> InterpolatePoses(const std::vector<float4x4> & first_pose, const std::vector<float4x4> & second_pose, float weight) const;
	void GetClipTransform(float current_time, uint32_t skeleton_uuid, const std::shared_ptr<Clip> &clip, std::vector<math::float4x4> & pose) const;
public:
	std::shared_ptr<StateMachine> state_machine;
	std::vector<PlayingClip> playing_clips;

private:
	std::shared_ptr<State> active_state;
	std::shared_ptr<Transition> active_transition;
	bool apply_transition = false;
	friend class PanelComponent;
};

#endif //_ANIMCONTROLLER_H_

