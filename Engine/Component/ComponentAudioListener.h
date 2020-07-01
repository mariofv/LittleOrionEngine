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

private:
	//AkSoundPosition sound_position;
	AkTransform listener_transform;
	AkGameObjectID gameobject_listener = 0;
};

#endif //_COMPONENTAUDIOLISTENER_H_

