#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#include "Module.h"
#include "Main/Globals.h"
#include "SpacePartition/OLQuadTree.h"
#include "SpacePartition/OLAABBTree.h"
#include "Helper/Timer.h"

#include <GL/glew.h>

const unsigned INITIAL_SIZE_AABBTREE = 10;

class ComponentMesh;
class ComponentCamera;

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	enum class DrawMode
	{
		SHADED,
		WIREFRAME
	};

	ModuleRender() = default;
	~ModuleRender() = default;

	bool Init();
	update_status PreUpdate();
	bool CleanUp();
	
	void Render() const;
	void RenderFrame(const ComponentCamera &camera);

	ComponentMesh* CreateComponentMesh();
	void RemoveComponentMesh(ComponentMesh* mesh_to_remove);
	void GenerateQuadTree();
	void InsertAABBTree(GameObject* game_object);
	void RemoveAABBTree(GameObject * game_object);
	void UpdateAABBTree(GameObject* game_object);
	void DeleteAABBTree();
	void CreateAABBTree();
	void DrawAABBTree() const;

	GameObject* GetRaycastIntertectedObject(const LineSegment & ray);

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

	void SetDrawMode(DrawMode draw_mode);
	std::string GetDrawMode() const;

	void GetMeshesToRender(const ComponentCamera *camera);
	void GetCullingMeshes(const ComponentCamera *camera);

public:
	bool anti_aliasing = false;

private:
	void* context = nullptr;

	OLQuadTree ol_quadtree;
	OLAABBTree* ol_abbtree = new OLAABBTree(INITIAL_SIZE_AABBTREE);


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

	DrawMode draw_mode = DrawMode::SHADED;

	std::vector<ComponentMesh*> meshes;
	std::vector<ComponentMesh*> meshes_to_render;
	Timer * rendering_measure_timer = new Timer();

	friend class ModuleDebugDraw;
	friend class PanelConfiguration;
	friend class PanelDebug;
	friend class PanelScene;
};

#endif //_MODULERENDER_H_