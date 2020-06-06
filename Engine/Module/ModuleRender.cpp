#include "ModuleRender.h"

#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleCamera.h"
#include "ModuleDebug.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleProgram.h"
#include "ModuleScene.h"
#include "ModuleSpacePartitioning.h"
#include "ModuleTime.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "ModuleLight.h"

#include "Component/ComponentBillboard.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentParticleSystem.h"
#include "Component/ComponentLight.h"

#include "EditorUI/DebugDraw.h"
#include "ModuleResourceManager.h"

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
		OPENGL_LOG_INIT(error_message); // Actually not an itialization entry, I use this type of entry because the yellow color
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

	APP_LOG_SUCCESS("Glew initialized correctly.")

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
	delete rendering_measure_timer;
	for (auto& mesh : meshes)
	{
		mesh->owner->RemoveComponent(mesh);
	}
	for (auto& particle : particle_systems)
	{
		particle->owner->RemoveComponent(particle);
	}
	return true;
}

void ModuleRender::Render() const
{
	BROFILER_CATEGORY("Global Render",Profiler::Color::Aqua);

#if GAME
	if (App->cameras->main_camera != nullptr) 
	{
		App->cameras->main_camera->RecordFrame(App->window->GetWidth(), App->window->GetHeight());
		App->cameras->main_camera->RecordDebugDraws();
	}
#endif

	App->editor->Render();

	BROFILER_CATEGORY("Swap Window (VSYNC)", Profiler::Color::Aquamarine);
	SDL_GL_SwapWindow(App->window->window);
}

void ModuleRender::RenderFrame(const ComponentCamera &camera)
{
	BROFILER_CATEGORY("Render Frame", Profiler::Color::Azure);
  
	rendering_measure_timer->Start();
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

	static size_t projection_matrix_offset = App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET + sizeof(float4x4);
	glBufferSubData(GL_UNIFORM_BUFFER, projection_matrix_offset, sizeof(float4x4), camera.GetProjectionMatrix().Transposed().ptr());

	static size_t view_matrix_offset = App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET + 2 * sizeof(float4x4);
	glBufferSubData(GL_UNIFORM_BUFFER, view_matrix_offset, sizeof(float4x4), camera.GetViewMatrix().Transposed().ptr());

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	num_rendered_tris = 0;
	num_rendered_verts = 0;

	GetMeshesToRender(&camera);
	for (auto &mesh : opaque_mesh_to_render)
	{
		BROFILER_CATEGORY("Render Mesh", Profiler::Color::Aquamarine);
		if (mesh.second->mesh_uuid != 0 && mesh.second->IsEnabled())
		{
			mesh.second->Render();
			num_rendered_tris += mesh.second->mesh_to_render->GetNumTriangles();
			num_rendered_verts += mesh.second->mesh_to_render->GetNumVerts();
			glUseProgram(0);

		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	for (auto &mesh : transparent_mesh_to_render)
	{
		BROFILER_CATEGORY("Render Mesh", Profiler::Color::Aquamarine);
		if (mesh.second->mesh_uuid != 0 && mesh.second->IsEnabled())
		{
			mesh.second->Render();
			num_rendered_tris += mesh.second->mesh_to_render->GetNumTriangles();
			num_rendered_verts += mesh.second->mesh_to_render->GetNumVerts();
			glUseProgram(0);
			
		}
	}
	glDisable(GL_BLEND);
	
	for (auto &billboard : billboards)
	{
		billboard->Render(billboard->owner->transform.GetGlobalTranslation());
	}
	for (auto &particles : particle_systems)
	{
		particles->Render();
	}

	BROFILER_CATEGORY("Canvas", Profiler::Color::AliceBlue);
	App->ui->Render(&camera);

	rendering_measure_timer->Stop();
	App->debug->rendering_time = rendering_measure_timer->Read();
	
}

void ModuleRender::GetMeshesToRender(const ComponentCamera* camera)
{
	BROFILER_CATEGORY("Get meshes to render", Profiler::Color::Aquamarine);

	meshes_to_render.clear();
	if (camera == App->cameras->scene_camera && !App->debug->culling_scene_mode)
	{
		meshes_to_render = meshes;
	}
	else
	{
		App->space_partitioning->GetCullingMeshes(App->cameras->main_camera);
	}
	SetListOfMeshesToRender(camera);
}

void ModuleRender::SetListOfMeshesToRender(const ComponentCamera* camera)
{
	opaque_mesh_to_render.clear();
	transparent_mesh_to_render.clear();
	float3 camera_pos = camera->camera_frustum.pos;
	for (unsigned int i = 0; i < meshes_to_render.size(); i++)
	{
		if (meshes_to_render[i]->material_to_render->material_type == Material::MaterialType::MATERIAL_TRANSPARENT)
		{
			meshes_to_render[i]->owner->aabb.bounding_box;
			float3 center_bounding_box = (meshes_to_render[i]->owner->aabb.bounding_box.minPoint + meshes_to_render[i]->owner->aabb.bounding_box.maxPoint) / 2;
			float distance = center_bounding_box.Distance(camera_pos);
			transparent_mesh_to_render.push_back(std::make_pair(distance, meshes_to_render[i]));
			transparent_mesh_to_render.sort([](const ipair & a, const ipair & b) { return a.first > b.first; });
		}
		if (meshes_to_render[i]->material_to_render->material_type == Material::MaterialType::MATERIAL_OPAQUE)
		{
			meshes_to_render[i]->owner->aabb.bounding_box;
			float3 center_bounding_box = (meshes_to_render[i]->owner->aabb.bounding_box.minPoint + meshes_to_render[i]->owner->aabb.bounding_box.maxPoint) / 2;
			float distance = center_bounding_box.Distance(camera_pos);
			opaque_mesh_to_render.push_back(std::make_pair(distance, meshes_to_render[i]));
			opaque_mesh_to_render.sort([](const ipair & a, const ipair & b) { return a.first < b.first; });
		}
	}
}

void ModuleRender::SetVSync(bool vsync)
{
	this->vsync = vsync;
	vsync ? SDL_GL_SetSwapInterval(1) : SDL_GL_SetSwapInterval(0);
}

void ModuleRender::SetAlphaTest(bool gl_alpha_test)
{
	this->gl_alpha_test = gl_alpha_test;
	gl_alpha_test ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);
}

void ModuleRender::SetDepthTest(bool gl_depth_test)
{
	this->gl_depth_test = gl_depth_test;
	gl_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void ModuleRender::SetScissorTest(bool gl_scissor_test)
{
	this->gl_scissor_test = gl_scissor_test;
	gl_scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
}

void ModuleRender::SetStencilTest(bool gl_stencil_test)
{
	this->gl_stencil_test = gl_stencil_test;
	gl_stencil_test ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
}

void ModuleRender::SetBlending(bool gl_blend)
{
	this->gl_blend = gl_blend;

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

void ModuleRender::SetDithering(bool gl_dither)
{
	this->gl_dither = gl_dither;
	gl_dither ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
}

void ModuleRender::SetMinMaxing(bool gl_minmax)
{
	this->gl_minmax = gl_minmax;
	gl_minmax ? glEnable(GL_MINMAX) : glDisable(GL_MINMAX);
}

void ModuleRender::SetDrawMode(DrawMode draw_mode)
{
	this->draw_mode = draw_mode;
	switch (draw_mode)
	{
	case ModuleRender::DrawMode::SHADED:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case ModuleRender::DrawMode::WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	default:
		break;
	}
}

std::string ModuleRender::GetDrawMode() const
{
	switch (draw_mode)
	{
	case ModuleRender::DrawMode::SHADED:
		return "Shaded";
		break;
	case ModuleRender::DrawMode::WIREFRAME:
		return "Wireframe";
		break;
	default:
		return "Unknown";
		break;
	}
}

ComponentMeshRenderer* ModuleRender::CreateComponentMeshRenderer()
{
	ComponentMeshRenderer* created_mesh = new ComponentMeshRenderer();
	meshes.push_back(created_mesh);
	return created_mesh;
}

void ModuleRender::RemoveComponentMesh(ComponentMeshRenderer* mesh_to_remove)
{
	const auto it = std::find(meshes.begin(), meshes.end(), mesh_to_remove);
	if (it != meshes.end())
	{
		delete *it;
		meshes.erase(it);
	}
}

ComponentBillboard* ModuleRender::CreateComponentBillboard()
{
	ComponentBillboard* created_billboard = new ComponentBillboard();
	billboards.push_back(created_billboard);
	return created_billboard;
}

void ModuleRender::RemoveComponentBillboard(ComponentBillboard* billboard_to_remove)
{
	auto it = std::find(billboards.begin(), billboards.end(), billboard_to_remove);
	if (it != billboards.end())
	{
		delete *it;
		billboards.erase(it);
	}
}

ComponentParticleSystem* ModuleRender::CreateComponentParticleSystem()
{
	ComponentParticleSystem* created_particle_system = new ComponentParticleSystem();
	particle_systems.push_back(created_particle_system);
	return created_particle_system;
}

void ModuleRender::RemoveComponentParticleSystem(ComponentParticleSystem* particle_system_to_remove)
{
	auto it = std::find(particle_systems.begin(), particle_systems.end(), particle_system_to_remove);
	if (it != particle_systems.end())
	{
		delete *it;
		particle_systems.erase(it);
	}
}

RaycastHit* ModuleRender::GetRaycastIntersection(const LineSegment& ray, const ComponentCamera* cam)
{
	BROFILER_CATEGORY("Do Raycast", Profiler::Color::HotPink);
	App->space_partitioning->GetCullingMeshes(cam);
	std::vector<ComponentMeshRenderer*> intersected_meshes;
	for (const auto&  mesh : meshes_to_render)
	{
		if (mesh->owner->aabb.bounding_box.Intersects(ray))
		{
			//Allow non touchable meshes to be ignored from mouse picking in game mode
			if (cam != App->cameras->scene_camera && !mesh->is_raycastable) continue;

			intersected_meshes.push_back(mesh);
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

int ModuleRender::GetRenderedTris() const
{
	return num_rendered_tris;
}

int ModuleRender::GetRenderedVerts() const
{
	return num_rendered_verts;
}
