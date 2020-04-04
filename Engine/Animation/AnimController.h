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

	void Play();
	void Stop();
	void Update();

	bool GetKeyframes(int& first_keyframe_index, int& second_keyframe_index, float& interpolationLambda);

	bool GetTransform(const std::string& channel_name, float3& position, Quat& rotation, const int first_keyframe_index,
		const int second_keyframe_index, const float interpolationLambda);

private:
	void UpdateChannelsGlobalTransformation();
public:
	std::shared_ptr<Animation> animation = nullptr;
	std::shared_ptr<StateMachine> state_machine;

	std::map<size_t, std::vector<uint32_t>> channel_hierarchy_cache;
	std::vector<float4x4> channel_global_transformation;

	bool playing = false;
private:
	std::shared_ptr<State> active_state;
	bool loop = false;

	int current_time = 0;
	int animation_time = 0;

	friend class PanelComponent;
};

#endif //_ANIMCONTROLLER_H_

