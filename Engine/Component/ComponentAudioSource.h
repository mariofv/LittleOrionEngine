#ifndef _COMPONENTAUDIOSOURCE_H_
#define _COMPONENTAUDIOSOURCE_H_

#include "Component.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"
#include "AK/SoundEngine/Common/AkTypes.h"

class GameObject;
class SoundBank;
class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource();
	ComponentAudioSource(GameObject* owner);
	~ComponentAudioSource() = default;

	void Init() override;
	void Update() override;
	void Delete() override;

	void SetSoundBank(uint32_t uuid);
	void PlayEvent(const std::string & event_to_play);

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;
	void Save(Config& config) const override;
	void Load(const Config& config) override;

private:
	AkSoundPosition sound_position;
	AkGameObjectID gameobject_source = 0;
	std::shared_ptr<SoundBank> soundbank;

	bool sound_3d = false;
	friend class PanelComponent;
};

#endif //_COMPONENTAUDIOSOURCE_H_