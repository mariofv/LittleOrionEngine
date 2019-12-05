#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#include "Module.h"
#include "Globals.h"
#include "GeometryRenderer.h"
#include "OLQuadTree.h"

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
	update_status PostUpdate();
	bool CleanUp();
	
	void Render() const;
	void RenderFrame(const ComponentCamera &camera);
	void RenderMesh(const ComponentMesh &mesh, const ComponentCamera &camera) const;
	void RenderGrid(const ComponentCamera &camera) const;

	ComponentMesh* CreateComponentMesh();
	void GenerateQuadTree();

	void ShowRenderOptions();

private:
	void SetVSync(const bool vsync);
	void SetAlphaTest(const bool gl_alpha_test);
	void SetDepthTest(const bool gl_depth_test);
	void SetScissorTest(const bool gl_scissor_test);
	void SetStencilTest(const bool gl_stencil_test);
	void SetBlending(const bool gl_blend);
	void SetFaceCulling(const bool gl_cull_face);
	void SetCulledFaces(const GLenum culled_faces) const;
	void SetFrontFaces(const GLenum front_faces) const;
	void SetDithering(const bool gl_dither);
	void SetMinMaxing(const bool gl_minmax);
	void SetWireframing(const bool gl_wireframe);

public:
	GeometryRenderer *geometry_renderer = nullptr;

private:
	void* context = nullptr;

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
	OLQuadTree ol_quadtree;
};

#endif //_MODULERENDER_H_