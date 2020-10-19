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
class Quad;

class Viewport
{
public:
	enum class ViewportOption
	{
		SCENE_MODE = 1 << 0,
		BLIT_FRAMEBUFFER = 1 << 1,
		RENDER_UI = 1 << 2,
	};

	enum class ViewportOutput
	{
		COLOR,
		BRIGHTNESS
	};

	Viewport(int options);
	~Viewport();

	void Render(ComponentCamera* camera);

	void SetSize(float width, float height);
	void SetAntialiasing(bool antialiasing);
	void SetHDR(bool hdr);
	void SetBloom(bool bloom);
	void SetFog(bool fog);

	void SetOutput(ViewportOutput output);

private:
	void BindCameraFrustumMatrices(const Frustum& camera_frustum) const;
	void BindDepthMaps(GLuint program) const;

	void LightCameraPass() const;
	void MeshRenderPass() const;
	void EffectsRenderPass() const;
	void UIRenderPass() const;
	void PostProcessPass();
	void BlitPass() const;
	void DebugPass() const;
	void DebugDrawPass() const;
	void EditorDrawPass() const;

	void SelectDisplayedTexture();
	bool IsOptionSet(ViewportOption option) const;

	void SkyboxPass() const;
	void DepthMapPass(LightFrustum* light_frustum, FrameBuffer* depth_fbo, bool render_debug_depth_map) const;
	void BloomPass();
	void HDRPass() const;

public:
	GLuint displayed_texture = 0;
	mutable GLuint depth_map_texture = 0;
	ViewportOutput viewport_output = ViewportOutput::COLOR;
	Quad* scene_quad = nullptr;

	std::vector<FrameBuffer*> framebuffers;
	FrameBuffer* scene_fbo = nullptr;
	FrameBuffer* postprocess_fbo = nullptr;

	FrameBuffer* ping_fbo = nullptr;
	FrameBuffer* pong_fbo = nullptr;
	FrameBuffer* ping_pong_fbo = nullptr;

	FrameBuffer* source_fbo = nullptr;
	FrameBuffer* blit_fbo = nullptr;

	FrameBuffer* debug_depth_map_fbo = nullptr;
	FrameBuffer* depth_full_fbo = nullptr;
	FrameBuffer* depth_near_fbo = nullptr;
	FrameBuffer* depth_mid_fbo = nullptr;
	FrameBuffer* depth_far_fbo = nullptr;

	bool shadows_pass = false;
	bool effects_pass = true;
	bool effects_draw_all = false;
	bool debug_pass = true;
	bool debug_draw_pass = true;

	mutable int num_rendered_triangles = 0;
	mutable int num_rendered_vertices = 0;

private:
	ComponentCamera* camera = nullptr;
	std::vector<ComponentMeshRenderer*> culled_mesh_renderers;

	float width = 0;
	float height = 0;

	int viewport_options = 0;
	bool antialiasing = false;
	bool hdr = false;
	bool bloom = false;
	bool fog = false;
};

#endif //_VIEWPORT_H_