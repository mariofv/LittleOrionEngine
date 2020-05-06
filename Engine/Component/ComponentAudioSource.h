#ifndef _COMPONENTAUDIOSOURCE_H_
#define _COMPONENTAUDIOSOURCE_H_

#include "Component.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

class GameObject;

class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource();
	ComponentAudioSource(GameObject* owner);
	~ComponentAudioSource() = default;

	void Init();
	void Update() override;
	void Delete() override;
	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;
	void Save(Config& config) const override;
	void Load(const Config& config) override;

private:
	friend class PanelComponent;
};

#endif //_COMPONENTAUDIOSOURCE_H_