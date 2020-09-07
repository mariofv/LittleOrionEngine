#ifndef _COMPONENTVIDEOPLAYER_H_
#define _COMPONENTVIDEOPLAYER_H_

#include "Component/Component.h"
#include <AK/SoundEngine/Common/AkTypes.h>

#include <GL/glew.h>
class Video;
class SoundBank;
class Quad;
class ComponentVideoPlayer : public Component
{
public:
	ComponentVideoPlayer();
	ComponentVideoPlayer(GameObject * owner);
	~ComponentVideoPlayer();

	Component* Clone(GameObject* owner, bool original_prefab) override;
	void CopyTo(Component* component_to_copy) const override;

	void InitResource(uint32_t uuid, ResourceType resource);

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	void LoadResource(uint32_t uuid, ResourceType resource) override;

	void ReassignResource() override;

	void SetVideoToRender(uint32_t video_uuid);
	void SetVideoToRenderFromInspector(uint32_t video_uuid);
	void SetSoundBank(uint32_t uuid);

	ENGINE_API void PlayVideo();
	ENGINE_API void StopVideo();
	ENGINE_API bool IsFinish();

	void Init() override;

	void Render(float4x4* projection);

private:
	void RenderTexture(math::float4x4 * projection, GLuint texture);

public:
	uint32_t video_uuid = 0;
	std::shared_ptr<Video> video_to_render;
	std::shared_ptr<SoundBank> soundbank;
	std::string sound_event = {};

	bool preserve_aspect_ratio = false;


private:
	Quad* quad = nullptr;
	GLuint program = 0;
	float4 color = float4::one;
	float texture_aspect_ratio = 0.f;
	bool playing_video = false;

	AkGameObjectID gameobject_source = 0;
	AkPlayingID playing_id = 0;
};
#endif //_COMPONENTVIDEOPLAYER_H_

