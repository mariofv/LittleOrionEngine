#ifndef _COMPONENTAUDIOLISTENER_H_
#define _COMPONENTAUDIOLISTENER_H_

#include "Component.h"

#include <AK/SoundEngine/Common/AkTypes.h>
#include <unordered_map>

class GameObject;
class ComponentAudioListener :
	public Component
{
public:
	ComponentAudioListener();
	ComponentAudioListener(GameObject* owner);
	~ComponentAudioListener() = default;

	void Init() override;
	void Update() override;
	void Delete() override;

	Component* Clone(GameObject* owner, bool original_prefab) override;
	void CopyTo(Component* component_to_copy) const override;

	void Disable();

	void Enable();

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	AkGameObjectID GetAkID();

private:
	//AkSoundPosition sound_position;
	AkTransform listener_transform;
	AkGameObjectID gameobject_listener = 0;
};

#endif //_COMPONENTAUDIOLISTENER_H_

