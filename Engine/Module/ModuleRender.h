#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#include "Module.h"
#include "Globals.h"
#include "OLQuadTree.h"
#include "Timer.h"

#include <GL/glew.h>

class ComponentMesh;
class ComponentCamera;

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
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

	GameObject* GetRaycastIntertectedObject(const LineSegment & ray);

	void ShowRenderOptions();

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
	void SetWireframing(bool gl_wireframe);

	void GetMeshesToRender(const ComponentCamera *camera);
	void GetCullingMeshes(const ComponentCamera *camera);

private:
	void* context = nullptr;

	OLQuadTree ol_quadtree;

	bool vsync = false;
	bool gl_alpha_test = false;
	bool gl_depth_test = false;
	bool gl_scissor_test = false;
	bool gl_stencil_test = false;
	bool gl_blend = false;
	bool gl_cull_face = false;
	int culled_faces = 0;
	int front_faces = 0;
	int filling_mode = 0;
	bool gl_dither = false;
	bool gl_minmax = false;
	bool gl_wireframe = false;

	std::vector<ComponentMesh*> meshes;
	std::vector<ComponentMesh*> meshes_to_render;
	Timer * rendering_measure_timer = new Timer();

	friend class ModuleDebug;
	friend class ModuleEditor;
};

#endif //_MODULERENDER_H_