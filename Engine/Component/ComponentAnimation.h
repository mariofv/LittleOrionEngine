#ifndef _COMPONENTANIMATION_H_
#define _COMPONENTANIMATION_H_

#include "Component.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include <GL/glew.h>
#include <memory>

class Animation;
class AnimController;
class GameObject;
class StateMachine;

class ComponentAnimation :	public Component
{
public:
	ComponentAnimation();
	ComponentAnimation(GameObject* owner);
	~ComponentAnimation() = default;

	void Init();

	//Copy and move
	ComponentAnimation(const ComponentAnimation& component_to_copy) = default;
	ComponentAnimation(ComponentAnimation&& component_to_move) = default;

	ComponentAnimation & operator=(const ComponentAnimation& component_to_copy);
	ComponentAnimation & operator=(ComponentAnimation&& component_to_move) = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Disable() override;

	void SetStateMachine(uint32_t state_machine_uuid);

	//API
	ENGINE_API void Play();
	ENGINE_API void Stop();
	ENGINE_API void ActiveAnimation(const std::string & trigger);
	ENGINE_API bool IsOnState(const std::string & trigger);
	ENGINE_API float GetCurrentClipPercentatge() const;

	void Update() override;
	void UpdateMeshes();
	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

private:
	void GetChildrenMeshes(GameObject * current_mesh);
	void GenerateJointChannelMaps();

private:
	std::vector<ComponentMeshRenderer*> skinned_meshes;
	AnimController* animation_controller = nullptr;
	std::vector<float4x4> pose;
	bool playing = false;

	friend class PanelComponent;
	
};

#endif //_COMPONENTANIMATION_H_

