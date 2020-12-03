#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "Module.h"
#include "Helper/Timer.h"
#include "Main/Globals.h"

#include <MathGeoLib/MathGeoLib.h>
#include <GL/glew.h>
#include <list>
#include <vector>

class ComponentMeshRenderer;
class ComponentCamera;

class GameObject;
class Viewport;

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

	ComponentMeshRenderer* CreateComponentMeshRenderer();
	void RemoveComponentMesh(ComponentMeshRenderer* mesh_to_remove);

	ENGINE_API int GetRenderedTris() const;
	ENGINE_API int GetRenderedVerts() const;

	ENGINE_API RaycastHit* GetRaycastIntersection(const LineSegment& ray, const ComponentCamera* camera);
	ENGINE_API bool MeshesIntersectsWithRay(const LineSegment& ray, const std::vector<ComponentMeshRenderer*>& meshes, int& index_intersection) const;
	ENGINE_API void SetDrawMode(DrawMode draw_mode);
	ENGINE_API void SetAntialiasing(bool antialiasing);
	ENGINE_API void SetVSync(bool vsync);
	ENGINE_API void SetHDR(bool hdr);
	ENGINE_API void SetHDRType(const HDRType type);
	ENGINE_API void SetBloom(bool bloom);
	ENGINE_API void SetFog(bool fog_enabled);
	ENGINE_API void SetFogDensity(float fog_density);
	ENGINE_API void SetFogColor(const float4&  fog_color);
	ENGINE_API void SetShadows(bool shadows_enabled);

private:
	void SetDepthTest(bool gl_depth_test);
	void SetFaceCulling(bool gl_cull_face);
	void SetCulledFaces(GLenum culled_faces) const;
	void SetFrontFaces(GLenum front_faces) const;
	std::string GetHDRType(const HDRType type) const;

	std::string GetDrawMode() const;

public:
	bool vsync = false;
	bool antialiasing = true;

	bool hdr = true;
	HDRType hdr_type = HDRType::FILMIC;
	float exposure = 1.f;

	bool shadows_enabled = false;
	bool depth_map_debug = false;
	int depth_map_debug_index = 0;
	bool cascade_mapping = false;
	bool cascade_debug = false;

	bool bloom = false;
	int amount_of_blur = 20;

	bool fog_enabled = false;
	float4 fog_color = float4::zero;
	float fog_density = 1.0f;

	Viewport* scene_viewport = nullptr;
	Viewport* game_viewport = nullptr;

	std::vector<ComponentMeshRenderer*> mesh_renderers;

private:
	void* context = nullptr;

	bool gl_depth_test = false;
	bool gl_blend = true;
	bool gl_cull_face = false;
	int culled_faces = 0;
	int front_faces = 0;
	int filling_mode = 0;

	DrawMode draw_mode = DrawMode::SHADED;

	friend class ModuleDebugDraw;
	friend class ModuleDebug;
	friend class ModuleSpacePartitioning;
	friend class PanelConfiguration;
	friend class PanelScene;
	friend class NavMesh; 
	friend class Viewport; 
};

#endif //_MODULERENDER_H_