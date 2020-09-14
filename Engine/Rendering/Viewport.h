#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include <GL/glew.h>
#include <MathGeoLib.h>
#include <vector>

class ComponentCamera;
class ComponentMeshRenderer;
class DepthFrameBuffer;
class FrameBuffer;
class LightFrustum;
class MultiSampledFrameBuffer;

class Viewport
{
public:
	enum class ViewportOption
	{
		SCENE_MODE = 1 << 0,
		BLIT_FRAMEBUFFER = 1 << 1
	};

	enum class ViewportOutput
	{
		COLOR,
		DEPTH_NEAR,
		DEPTH_MID,
		DEPTH_FAR
	};

	Viewport(int options);
	~Viewport();

	void Render(ComponentCamera* camera);

	void SetSize(float width, float height);
	void SetAntialiasing(bool antialiasing);

	void SetOutput(ViewportOutput output);

private:
	void BindCameraFrustumMatrices(const Frustum& camera_to_bind) const;

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
	ViewportOutput viewport_output = ViewportOutput::COLOR;

	FrameBuffer* main_fbo = nullptr;

	std::vector<FrameBuffer*> framebuffers;
	FrameBuffer* blit_fbo = nullptr;
	FrameBuffer* regular_fbo = nullptr;
	MultiSampledFrameBuffer* multisampled_fbo = nullptr;

	FrameBuffer* depth_near_fbo = nullptr;
	FrameBuffer* depth_mid_fbo = nullptr;
	FrameBuffer* depth_far_fbo = nullptr;

	bool shadows_pass = true;
	bool effects_pass = true;
	bool debug_pass = true;
	bool debug_draw_pass = true;

private:
	ComponentCamera* camera = nullptr;
	std::vector<ComponentMeshRenderer*> culled_mesh_renderers;

	float width = 0;
	float height = 0;

	LightFrustum* near_frustum = nullptr;
	LightFrustum* mid_frustum = nullptr;
	LightFrustum* far_frustum = nullptr;

	int viewport_options = 0;
	bool antialiasing = true;
};

#endif //_VIEWPORT_H_

/* 

CURRENT RENDERING PIPELINE
/------------------------/
MESHES -> EFFECTS -> UI -> POST_PROCESS -> DEBUG_DRAWS -> EDITOR DRAWS

*/

