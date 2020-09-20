#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

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
		BRIGHTNESS,

		DEPTH_NEAR,
		DEPTH_MID,
		DEPTH_FAR,
		DEPTH_FULL
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
	ENGINE_API void SetDrawMode(DrawMode draw_mode);
	ENGINE_API void SetAntialiasing(bool antialiasing);
	ENGINE_API void SetHDR(bool hdr);
	ENGINE_API void SetBloom(bool bloom);
	ENGINE_API void SetShadows(bool shadows_enabled);

private:
	void SetVSync(bool vsync);
	void SetDepthTest(bool gl_depth_test);
	void SetFaceCulling(bool gl_cull_face);
	void SetCulledFaces(GLenum culled_faces) const;
	void SetFrontFaces(GLenum front_faces) const;
	void SetHDRType(const HDRType type);
	std::string GetHDRType(const HDRType type) const;

	std::string GetDrawMode() const;

public:
	bool antialiasing = false;

	bool hdr = true;
	float exposure = 0.5f;

	bool shadows_enabled = false;
	bool cascade_debug = false;

	bool bloom = false;
	float emissive_exposure = 1.f;
	int amount_of_blur = 20;

	Viewport* scene_viewport = nullptr;
	Viewport* game_viewport = nullptr;

	std::vector<ComponentMeshRenderer*> mesh_renderers;

private:
	void* context = nullptr;

	bool vsync = false;
	bool gl_depth_test = false;
	bool gl_blend = true;
	bool gl_cull_face = false;
	int culled_faces = 0;
	int front_faces = 0;
	int filling_mode = 0;

	DrawMode draw_mode = DrawMode::SHADED;
	HDRType hdr_type = HDRType::FILMIC;

	friend class ModuleDebugDraw;
	friend class ModuleDebug;
	friend class ModuleSpacePartitioning;
	friend class PanelConfiguration;
	friend class PanelScene;
	friend class NavMesh; 
	friend class Viewport; 
};

#endif //_MODULERENDER_H_