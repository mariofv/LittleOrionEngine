#include "ModuleRender.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentLight.h"

#include "EditorUI/DebugDraw.h"

#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleCamera.h"
#include "ModuleDebug.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleEffects.h"
#include "ModuleProgram.h"
#include "ModuleResourceManager.h"
#include "ModuleSpacePartitioning.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "ModuleLight.h"
#include "Rendering/Viewport.h"

#include <algorithm>
#include <assimp/scene.h>
#include <MathGeoLib.h>
#include <SDL/SDL.h>
#include <Brofiler/Brofiler.h>

static void APIENTRY openglCallbackFunction(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
) {
	(void)source; (void)type; (void)id;
	(void)severity; (void)length; (void)userParam;

	char error_source[256];
	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             sprintf_s(error_source, "Source: API"); break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   sprintf_s(error_source, "Source: Window System"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sprintf_s(error_source, "Source: Shader Compiler"); break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     sprintf_s(error_source, "Source: Third Party"); break;
		case GL_DEBUG_SOURCE_APPLICATION:     sprintf_s(error_source, "Source: Application"); break;
		case GL_DEBUG_SOURCE_OTHER:           sprintf_s(error_source, "Source: Other"); break;
	}

	char error_type[256];
	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               sprintf_s(error_type, "Type: Error"); break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: sprintf_s(error_type, "Type: Deprecated Behaviour"); break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  sprintf_s(error_type, "Type: Undefined Behaviour"); break;
		case GL_DEBUG_TYPE_PORTABILITY:         sprintf_s(error_type, "Type: Portability"); break;
		case GL_DEBUG_TYPE_PERFORMANCE:         sprintf_s(error_type, "Type: Performance"); break;
		case GL_DEBUG_TYPE_MARKER:              sprintf_s(error_type, "Type: Marker"); break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          sprintf_s(error_type, "Type: Push Group"); break;
		case GL_DEBUG_TYPE_POP_GROUP:           sprintf_s(error_type, "Type: Pop Group"); break;
		case GL_DEBUG_TYPE_OTHER:               sprintf_s(error_type, "Type: Other"); break;
	}

	char error_message[4096];
	sprintf_s(error_message, "%s %s %s", error_source, error_type, message);
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:
		OPENGL_LOG_ERROR(error_message);
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		//OPENGL_LOG_INIT(error_message); // Actually not an itialization entry, I use this type of entry because the yellow color
		break;
	case GL_DEBUG_SEVERITY_LOW:
		//OPENGL_LOG_INFO(error_message); Too many messages in update
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		return;
	}
}

// Called before render is available
bool ModuleRender::Init()
{
	APP_LOG_SECTION("************ Module Render Init ************");
	// Init GLEW library
	GLenum err = glewInit();
	// … check for errors
	if (GLEW_OK != err)
	{
		APP_LOG_ERROR("Error initializing Glew");
		return false;

	}

	APP_LOG_INFO("Using Glew %s", glewGetString(GLEW_VERSION));
	APP_LOG_INFO("Vendor: %s", glGetString(GL_VENDOR));
	APP_LOG_INFO("Renderer: %s", glGetString(GL_RENDERER));
	APP_LOG_INFO("OpenGL version supported %s", glGetString(GL_VERSION));
	APP_LOG_INFO("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

#if OPENGL_MAJOR_VERSION >= 4

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openglCallbackFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);

#endif // OPENGL_MAJOR_VERSION >= 4

	SetVSync(VSYNC);
	SetDepthTest(true);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	APP_LOG_INFO("Glew initialized correctly.");

	int scene_viewport_options = (int)Viewport::ViewportOption::RENDER_UI | (int)Viewport::ViewportOption::SCENE_MODE | (int)Viewport::ViewportOption::BLIT_FRAMEBUFFER;
	scene_viewport = new Viewport(scene_viewport_options);

	int game_viewport_options = (int)Viewport::ViewportOption::RENDER_UI;
#if !GAME
	game_viewport_options |= (int)Viewport::ViewportOption::BLIT_FRAMEBUFFER;
#endif
	game_viewport = new Viewport(game_viewport_options);

	SetAntialiasing(antialiasing);
	SetHDR(hdr);
	SetBloom(bloom);

	return true;
}

update_status ModuleRender::PreUpdate()
{
	// CLEAR WINDOW COLOR AND DEPTH BUFFER
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	APP_LOG_INFO("Destroying renderer");
	for (auto& mesh : mesh_renderers)
	{
		mesh->owner->RemoveComponent(mesh);
	}

	delete scene_viewport;
	delete game_viewport;

	return true;
}

void ModuleRender::Render() const
{
	BROFILER_CATEGORY("Module Render Render",Profiler::Color::Aqua);

#if GAME
	if (App->cameras->main_camera != nullptr)
	{
		game_viewport->SetSize(App->window->GetWidth(), App->window->GetHeight());
		game_viewport->Render(App->cameras->main_camera);
	}
#endif

	App->editor->Render();

	BROFILER_CATEGORY("Swap Window (VSYNC)", Profiler::Color::Aquamarine);
	SDL_GL_SwapWindow(App->window->window);
}

void ModuleRender::SetVSync(bool vsync)
{
	this->vsync = vsync;
	vsync ? SDL_GL_SetSwapInterval(1) : SDL_GL_SetSwapInterval(0);
}

void ModuleRender::SetDepthTest(bool gl_depth_test)
{
	this->gl_depth_test = gl_depth_test;
	gl_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void ModuleRender::SetFaceCulling(bool gl_cull_face)
{
	this->gl_cull_face = gl_cull_face;
	gl_cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void ModuleRender::SetCulledFaces(GLenum culled_faces) const
{
	glCullFace(culled_faces);
}

void ModuleRender::SetFrontFaces(GLenum front_faces) const
{
	glFrontFace(front_faces);
}

void ModuleRender::SetDrawMode(DrawMode draw_mode)
{
	this->draw_mode = draw_mode;
	switch (draw_mode)
	{
	case ModuleRender::DrawMode::SHADED:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		scene_viewport->SetOutput(Viewport::ViewportOutput::COLOR);
		break;

	case ModuleRender::DrawMode::WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		scene_viewport->SetOutput(Viewport::ViewportOutput::COLOR);
		break;

	case ModuleRender::DrawMode::BRIGHTNESS:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		scene_viewport->SetOutput(Viewport::ViewportOutput::BRIGHTNESS);
		break;

	default:
		break;
	}
}

void ModuleRender::SetAntialiasing(bool antialiasing)
{
	this->antialiasing = antialiasing;
#if !GAME
	scene_viewport->SetAntialiasing(antialiasing);
#endif
	game_viewport->SetAntialiasing(antialiasing);
}

void ModuleRender::SetHDR(bool hdr)
{
	this->hdr = hdr;
#if !GAME
	scene_viewport->SetHDR(hdr);
#endif
	game_viewport->SetHDR(hdr);
}

void ModuleRender::SetBloom(bool bloom)
{
	this->bloom = bloom;
#if !GAME
	scene_viewport->SetBloom(bloom);
#endif
	game_viewport->SetBloom(bloom);
}

void ModuleRender::SetFog(bool fog_enabled)
{
	this->fog_enabled = fog_enabled;
	game_viewport->SetFog(fog_enabled);
}

void ModuleRender::SetFogDensity(float fog_density)
{
	this->fog_density = fog_density;
}

void ModuleRender::SetFogColor(const float4& fog_color)
{
	this->fog_color = fog_color;
}

void ModuleRender::SetShadows(bool shadows_enabled)
{
	this->shadows_enabled = shadows_enabled;
#if !GAME
	scene_viewport->shadows_pass = shadows_enabled;
#endif
	game_viewport->shadows_pass = shadows_enabled;
}

std::string ModuleRender::GetDrawMode() const
{
	switch (draw_mode)
	{
	case ModuleRender::DrawMode::SHADED:
		return "Shaded";

	case ModuleRender::DrawMode::WIREFRAME:
		return "Wireframe";

	case ModuleRender::DrawMode::BRIGHTNESS:
		return "Brightness";

	default:
		return "Unknown";
	}
}

ComponentMeshRenderer* ModuleRender::CreateComponentMeshRenderer()
{
	ComponentMeshRenderer* created_mesh = new ComponentMeshRenderer();
	mesh_renderers.push_back(created_mesh);
	return created_mesh;
}

void ModuleRender::RemoveComponentMesh(ComponentMeshRenderer* mesh_to_remove)
{
	const auto it = std::find(mesh_renderers.begin(), mesh_renderers.end(), mesh_to_remove);
	if (it != mesh_renderers.end())
	{
		delete *it;
		mesh_renderers.erase(it);
	}
}

RaycastHit* ModuleRender::GetRaycastIntersection(const LineSegment& ray, const ComponentCamera* camera)
{
	if (camera != App->cameras->scene_camera)
	{
		return nullptr;
	}

	BROFILER_CATEGORY("Do Raycast", Profiler::Color::HotPink);
	std::vector<ComponentMeshRenderer*> culled_mesh_renderers = App->space_partitioning->GetCullingMeshes(camera, mesh_renderers);
	std::vector<ComponentMeshRenderer*> intersected_meshes;
	for (const auto& mesh_renderer : culled_mesh_renderers)
	{
		if (mesh_renderer->owner->aabb.bounding_box.Intersects(ray))
		{
			//Allow non touchable meshes to be ignored from mouse picking in game mode
			if (!mesh_renderer->IsPropertySet(ComponentMeshRenderer::MeshProperties::RAYCASTABLE))
			{
				continue;
			}

			intersected_meshes.push_back(mesh_renderer);
		}
	}

	BROFILER_CATEGORY("Intersect", Profiler::Color::HotPink);
	std::vector<GameObject*> intersected;
	GameObject* selected = nullptr;
	float min_distance = INFINITY;

	RaycastHit* result = new RaycastHit();

	for (const auto&  mesh : intersected_meshes)
	{
		LineSegment transformed_ray = ray;
		transformed_ray.Transform(mesh->owner->transform.GetGlobalModelMatrix().Inverted());
		BROFILER_CATEGORY("Triangles", Profiler::Color::HotPink);
		std::vector<Mesh::Vertex> &vertices = mesh->mesh_to_render->vertices;
		std::vector<uint32_t> &indices = mesh->mesh_to_render->indices;
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			float3 first_point = vertices[indices[i]].position;
			float3 second_point = vertices[indices[i + 1]].position;
			float3 third_point = vertices[indices[i + 2]].position;
			Triangle triangle(first_point, second_point, third_point);

			float distance;
			float3 intersected_point;
			bool intersected = triangle.Intersects(transformed_ray, &distance, &intersected_point);
			if (intersected && distance < min_distance)
			{
				selected = mesh->owner;
				min_distance = distance;

				result->game_object = mesh->owner;
				result->hit_distance = distance;
				result->hit_point = intersected_point;
			}
		}
	}
	return result;
}

ENGINE_API bool ModuleRender::MeshesIntersectsWithRay(const LineSegment& ray, const std::vector<ComponentMeshRenderer*>& meshes, int& index_intersection) const
{
	float min_distance = INFINITY;
	
	index_intersection = 0;
	for (const ComponentMeshRenderer* mesh : meshes)
	{
		LineSegment transformed_ray = ray;
		transformed_ray.Transform(mesh->owner->transform.GetGlobalModelMatrix().Inverted());
		BROFILER_CATEGORY("Triangles", Profiler::Color::HotPink);
		std::vector<Mesh::Vertex> &vertices = mesh->mesh_to_render->vertices;
		std::vector<uint32_t> &indices = mesh->mesh_to_render->indices;
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			float3 first_point = vertices[indices[i]].position;
			float3 second_point = vertices[indices[i + 1]].position;
			float3 third_point = vertices[indices[i + 2]].position;
			Triangle triangle(first_point, second_point, third_point);

			float distance;
			float3 intersected_point;
			bool intersected = triangle.Intersects(transformed_ray, &distance, &intersected_point);
			if (intersected)
			{
				return true;
			}

		}

		++index_intersection;
	}

	index_intersection = -1;

	return false;
}

int ModuleRender::GetRenderedTris() const
{
	return game_viewport->num_rendered_triangles;
}

int ModuleRender::GetRenderedVerts() const
{
	return game_viewport->num_rendered_vertices;
}

void ModuleRender::SetHDRType(const HDRType type)
{
	switch (type)
	{
		case HDRType::REINHARD:
			hdr_type = HDRType::REINHARD;
		break;
		case HDRType::FILMIC:
			hdr_type = HDRType::FILMIC;
		break;
		case HDRType::EXPOSURE:
			hdr_type = HDRType::EXPOSURE;
		break;
	}
}
std::string ModuleRender::GetHDRType(const HDRType type) const
{
	switch (type)
	{
		case HDRType::REINHARD:
			return "Reinhard";
		case HDRType::FILMIC:
			return "Filmic";
		case HDRType::EXPOSURE:
			return "Exposure";
	}
}
