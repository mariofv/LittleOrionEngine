#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include <GL/glew.h>
#include <MathGeoLib.h>
#include <vector>

class ComponentCamera;
class ComponentMeshRenderer;
class FrameBuffer;
class MultiSampledFrameBuffer;

class Viewport
{
public:
	enum class ViewportOption
	{
		SCENE_MODE = 1 << 0,
		BLIT_FRAMEBUFFER = 1 << 1
	};

	Viewport(int options);
	~Viewport();

	void Render(ComponentCamera* camera);

	void SetSize(float width, float height);
	void SetAntialiasing(bool antialiasing);

private:
	void BindCameraMatrices() const;
	Frustum InitLightFrustum(const float3& position, const float3& up, const float3& front, float vertical_fov, float horizontal_fov, float near_distance, float far_distance) const;

	void LightCameraPass() const;
	void MeshRenderPass() const;
	void EffectsRenderPass() const;
	void UIRenderPass() const;
	void PostProcessPass() const;
	void DebugPass() const;
	void DebugDrawPass() const;
	void EditorDrawPass() const;

	void SelectLastDisplayedTexture();
	bool IsOptionSet(ViewportOption option) const;

public:
	GLuint last_displayed_texture = 0;

	FrameBuffer* main_fbo = nullptr;
	FrameBuffer* blit_fbo = nullptr;

	FrameBuffer* regular_fbo = nullptr;
	MultiSampledFrameBuffer* multisampled_fbo = nullptr;

	bool shadows_pass = true;
	bool effects_pass = true;
	bool debug_pass = true;
	bool debug_draw_pass = true;


private:
	ComponentCamera* camera = nullptr;
	std::vector<ComponentMeshRenderer*> culled_mesh_renderers;

	float width = 0;
	float height = 0;

	int viewport_options = 0;
	bool antialiasing = true;
};

#endif //_VIEWPORT_H_

/* 

CURRENT RENDERING PIPELINE
/------------------------/
MESHES -> EFFECTS -> UI -> POST_PROCESS -> DEBUG_DRAWS -> EDITOR DRAWS

*/

