#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include <vector>

class StateMachine;
struct State;
struct Clip;
struct Transition;
class AnimController
{
public:
	AnimController() = default;
	~AnimController() = default;
	void SetActiveAnimation();

	void GetPose(float current_time, uint32_t skeleton_uuid, std::vector<float4x4> &pose) const;
	std::shared_ptr<State> StartNextState(const std::string & trigger);
	void SetActiveState(std::shared_ptr<State> & new_state);
private:
	std::vector<float4x4> InterpolatePoses(const std::vector<float4x4> & first_pose, const std::vector<float4x4> & second_pose) const;
	void GetClipTransform(float current_time, uint32_t skeleton_uuid, const std::shared_ptr<Clip> &clip, std::vector<math::float4x4> & pose) const;
public:
	std::shared_ptr<Clip> clip = nullptr;
	std::shared_ptr<StateMachine> state_machine;

private:
	std::shared_ptr<State> active_state;
	std::vector<std::shared_ptr<Clip>> fading_clips;
	std::shared_ptr<Transition> active_transition;
	float fade_time = 0;

	friend class PanelComponent;
};

#endif //_ANIMCONTROLLER_H_

