#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include <map>
#include <vector>

class StateMachine;
struct State;
class AnimController
{
public:
	AnimController() = default;
	~AnimController() = default;
	void Init();
	void SetActiveAnimation();

	void GetPose(float current_time, uint32_t skeleton_uuid, std::vector<float4x4> &pose) const;
	std::shared_ptr<State> GetNextState(const std::string & trigger) const;
	void SetActiveState(std::shared_ptr<State> & new_state);

public:
	std::shared_ptr<Animation> animation = nullptr;
	std::shared_ptr<StateMachine> state_machine;

	bool loop = false;
	int animation_time = 0;
private:
	std::shared_ptr<State> active_state;


	friend class PanelComponent;
};

#endif //_ANIMCONTROLLER_H_

