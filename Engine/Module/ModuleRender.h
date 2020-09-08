#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#define ENGINE_EXPORTS

#include "Module.h"
#include "Helper/Timer.h"
#include "Main/Globals.h"

#include <MathGeoLib/Geometry/LineSegment.h>
#include <GL/glew.h>
#include <list>
#include <vector>

class ComponentMeshRenderer;
class ComponentCamera;

class GameObject;

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

struct RaycastHit
{
	GameObject* game_object = nullptr;
	float hit_distance = 0.0f;
	float3 hit_point = float3::zero;
};

class ModuleRender : public Module
{
public:
	enum class DrawMode
	{
		SHADED,
		WIREFRAME, 
		BRIGHTNESS
	};
	enum class HDRType
	{
		REINHARD = 0,
		FILMIC = 1,
		EXPOSURE = 2,
		MAX_HDR_TYPE = 3
	};

	ModuleRender() = default;
	~ModuleRender() = default;

	bool Init();
	update_status PreUpdate();
	bool CleanUp();
	
	void Render() const;
	void RenderFrame(const ComponentCamera& camera);
	void RenderZBufferFrame(const ComponentCamera& camera);
	void GetMeshesToRender(const ComponentCamera* camera);


	ComponentMeshRenderer* CreateComponentMeshRenderer();
	void RemoveComponentMesh(ComponentMeshRenderer* mesh_to_remove);

	ENGINE_API int GetRenderedTris() const;
	ENGINE_API int GetRenderedVerts() const;

	void RenderQuad();

	ENGINE_API RaycastHit* GetRaycastIntersection(const LineSegment& ray, const ComponentCamera* cam);
	ENGINE_API void SetDrawMode(DrawMode draw_mode);

private:
	void SetVSync(bool vsync);
	void SetAlphaTest(bool gl_alpha_test);
	void SetDepthTest(bool gl_depth_test);
	void SetScissorTest(bool gl_scissor_test);
	void SetStencilTest(bool gl_stencil_test);
	void SetBlending(bool gl_blend);
	void SetFaceCulling(bool gl_cull_face);
	void SetCulledFaces(GLenum culled_faces) const;
	void SetFrontFaces(GLenum front_faces) const;
	void SetDithering(bool gl_dither);
	void SetMinMaxing(bool gl_minmax);
	void SetHDRType(const HDRType type);
	std::string GetHDRType(const HDRType type) const;
	void RenderPostProcessingEffects(const ComponentCamera &camera);
	std::string GetDrawMode() const;

	void SetListOfMeshesToRender(const ComponentCamera* camera);

public:
	bool anti_aliasing = false;
	bool toggle_ortho_frustum = false;
	bool toggle_directional_light_aabb = true;
	bool toggle_perspective_sub_frustums = false;
	bool bloom = false;
	bool threshold_brightness = false;
	float exposure = 1.0f;
	bool horizontal = true;
	bool hdr_active = false;
	float emisive_exposure = 1.0f;
	int amount_of_blur = 20;
private:
	void* context = nullptr;
	HDRType hdr_type = HDRType::FILMIC;

	bool vsync = false;
	bool gl_alpha_test = false;
	bool gl_depth_test = false;
	bool gl_scissor_test = false;
	bool gl_stencil_test = false;
	bool gl_blend = true;
	bool gl_cull_face = false;
	int culled_faces = 0;
	int front_faces = 0;
	int filling_mode = 0;
	bool gl_dither = false;
	bool gl_minmax = false;

	float quadVertices[20] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	DrawMode draw_mode = DrawMode::SHADED;

	std::vector<ComponentMeshRenderer*> meshes;
	std::vector<ComponentMeshRenderer*> meshes_to_render;

	typedef std::pair<float, ComponentMeshRenderer*> ipair;
	std::list <ipair> opaque_mesh_to_render, transparent_mesh_to_render;

	int num_rendered_tris = 0;
	int num_rendered_verts = 0;
	Timer * rendering_measure_timer = new Timer();

	friend class ModuleDebugDraw;
	friend class ModuleDebug;
	friend class ModuleSpacePartitioning;
	friend class PanelConfiguration;
	friend class PanelScene;
	friend class NavMesh; 
};

#endif //_MODULERENDER_H_