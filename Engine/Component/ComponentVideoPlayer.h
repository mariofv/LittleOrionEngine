#ifndef _COMPONENTVIDEOPLAYER_H_
#define _COMPONENTVIDEOPLAYER_H_

#include "Component/Component.h"

#include "Helper/Quad.h"

class Video;
class ComponentVideoPlayer : public Component
{
public:
	ComponentVideoPlayer();
	ComponentVideoPlayer(GameObject * owner);
	~ComponentVideoPlayer() = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	void LoadResource(uint32_t uuid, ResourceType resource) override;

	void ReassignResource() override;

	void SetVideoToRender(uint32_t video_uuid);
	void SetVideoToRenderFromInspector(uint32_t video_uuid);

	ENGINE_API void PlayVideo();
	ENGINE_API void StopVideo();

	void Render(float4x4* projection);

private:
	void RenderTexture(math::float4x4 * projection, GLuint texture);

public:
	uint32_t video_uuid = 0;
	std::shared_ptr<Video> video_to_render;

	bool preserve_aspect_ratio = false;

	float4 color = float4::one;

private:
	GLuint program = 0;
	Quad quad;
	float texture_aspect_ratio = 0.f;
	bool playing_video = false;
};
#endif //_COMPONENTVIDEOPLAYER_H_

