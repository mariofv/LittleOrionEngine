#ifndef _COMPONENTANIMATION_H_
#define _COMPONENTANIMATION_H_

#define ENGINE_EXPORTS

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
	~ComponentAnimation();

	void Init() override;

	//Copy and move
	ComponentAnimation(const ComponentAnimation& component_to_copy) = default;
	ComponentAnimation(ComponentAnimation&& component_to_move) = default;

	ComponentAnimation & operator=(const ComponentAnimation& component_to_copy);
	ComponentAnimation & operator=(ComponentAnimation&& component_to_move) = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Disable() override;

	void SetStateMachine(uint32_t state_machine_uuid);
	//to find it from NodeEditor
	AnimController* GetAnimController();
	//API
	ENGINE_API void Play();
	ENGINE_API void Stop();
	ENGINE_API void ActiveAnimation(const std::string & trigger);
	ENGINE_API bool IsOnState(const std::string & trigger);
	ENGINE_API float GetCurrentClipPercentatge() const;
	ENGINE_API int GetTotalAnimationTime() const;
	ENGINE_API void SetAnimationSpeed(float speed) const;

	void Update() override;
	void UpdateMeshes();
	void Delete() override;

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	
	bool playing = false;
private:
	void GetChildrenMeshes(GameObject * current_mesh);
	void GenerateJointChannelMaps();

private:
	std::vector<ComponentMeshRenderer*> skinned_meshes;
	AnimController* animation_controller = nullptr;
	std::vector<float4x4> pose;
	
	friend class PanelComponent;
	
};

#endif //_COMPONENTANIMATION_H_

