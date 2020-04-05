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
	void ActiveAnimation(const std::string & trigger);

	bool GetTransform(float current_time,const std::string& channel_name, float3& position, Quat& rotation);
	std::shared_ptr<Animation> GetCurrentAnimation() const;
private:
	std::vector<float4x4> GetPose(float current_time, const std::vector<size_t> & joint_channels_map);
public:
	std::shared_ptr<Animation> animation = nullptr;
	std::shared_ptr<StateMachine> state_machine;

	std::map<size_t, std::vector<uint32_t>> channel_hierarchy_cache;

	bool loop = false;
	int animation_time = 0;
private:
	std::shared_ptr<State> active_state;


	friend class PanelComponent;
};

#endif //_ANIMCONTROLLER_H_

