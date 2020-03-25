#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleCamera.h"
#include "ModuleDebug.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleModelLoader.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "ModuleLight.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentLight.h"
#include "UI/DebugDraw.h"

#include <SDL/SDL.h>
#include "MathGeoLib.h"
#include <assimp/scene.h>
#include <algorithm>
#include "Brofiler/Brofiler.h"

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

	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

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
	return true;
}

void ModuleRender::Render() const
{
	BROFILER_CATEGORY("Global Render",Profiler::Color::Aqua);
	App->editor->Render();
	BROFILER_CATEGORY("Swap Window (VSYNC)", Profiler::Color::Aquamarine);
	SDL_GL_SwapWindow(App->window->window);
	App->time->EndFrame();
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

	GetMeshesToRender(&camera);
	for (auto &mesh : meshes_to_render)
	{
		BROFILER_CATEGORY("Render Mesh", Profiler::Color::Aquamarine);
		if (mesh->IsEnabled())
		{
			mesh->Render();
			glUseProgram(0);
		}
	}

	rendering_measure_timer->Stop();
	App->debug->rendering_time = rendering_measure_timer->Read();
}

void ModuleRender::GetMeshesToRender(const ComponentCamera *camera)
{
	meshes_to_render.clear();

	if (camera == App->cameras->scene_camera && !App->debug->culling_scene_mode)
	{
		meshes_to_render = meshes;
	}
	else
	{
		GetCullingMeshes(App->cameras->main_camera);
	}
}

void ModuleRender::GetCullingMeshes(const ComponentCamera *camera)
{
	BROFILER_CATEGORY("Get culling meshes", Profiler::Color::Lavender);

	switch (App->debug->culling_mode)
	{
	case ModuleDebug::CullingMode::NONE:
		std::copy_if(
			meshes.begin(),
			meshes.end(),
			std::back_inserter(meshes_to_render),
			[camera](auto mesh)
			{ 
				return mesh->IsEnabled(); 
			}
		);
		break;

	case ModuleDebug::CullingMode::FRUSTUM_CULLING:
		if (camera != nullptr)
		{
			std::copy_if(
				meshes.begin(),
				meshes.end(),
				std::back_inserter(meshes_to_render),
				[camera](auto mesh)
			{
				return mesh->IsEnabled() && mesh->owner->IsVisible(*camera);
			}
			);
		}
		break;

	case ModuleDebug::CullingMode::QUADTREE_CULLING:
		if (camera != nullptr)
		{
			// First we get all non static objects inside frustum
			std::copy_if( 
				meshes.begin(),
				meshes.end(),
				std::back_inserter(meshes_to_render),
				[camera](auto mesh)
			{
				return mesh->IsEnabled() && mesh->owner->IsVisible(*camera) && !mesh->owner->IsStatic();
			}
			);

			// Then we add all static objects culled using the quadtree
			std::vector<GameObject*> rendered_objects;
			ol_quadtree.CollectIntersect(rendered_objects, *camera);

			for (auto &object : rendered_objects)
			{
				ComponentMesh *object_mesh = (ComponentMesh*)object->GetComponent(Component::ComponentType::MESH);
				meshes_to_render.push_back(object_mesh);
			}
		}
		break;

	case ModuleDebug::CullingMode::AABBTREE_CULLING:
		if (camera != nullptr)
		{
			// First we get all static objects inside frustum
			std::copy_if(
				meshes.begin(),
				meshes.end(),
				std::back_inserter(meshes_to_render),
				[camera](auto mesh)
			{
				return mesh->IsEnabled() && mesh->owner->IsVisible(*camera) && mesh->owner->IsStatic();
			}
			);

			// Then we add all dynamic objects culled using the aabbtree
			std::vector<GameObject*> rendered_objects;
			ol_abbtree->GetIntersection(rendered_objects, camera);

			for (auto &object : rendered_objects)
			{
				ComponentMesh *object_mesh = (ComponentMesh*)object->GetComponent(Component::ComponentType::MESH);
				meshes_to_render.push_back(object_mesh);
			}
		}
		break;
	case ModuleDebug::CullingMode::COMBINED_CULLING:
		if(camera != nullptr)
		{
			// We add all static objects culled using the quadtree
			std::vector<GameObject*> rendered_static_objects;
			ol_quadtree.CollectIntersect(rendered_static_objects, *camera);

			for (auto &object : rendered_static_objects)
			{
				ComponentMesh *object_mesh = (ComponentMesh*)object->GetComponent(Component::ComponentType::MESH);
				meshes_to_render.push_back(object_mesh);
			}

			// Then we add all dynamic objects culled using the aabbtree
			std::vector<GameObject*> rendered_dynamic_objects;
			ol_abbtree->GetIntersection(rendered_dynamic_objects, camera);

			for (auto &object : rendered_dynamic_objects)
			{
				ComponentMesh *object_mesh = (ComponentMesh*)object->GetComponent(Component::ComponentType::MESH);
				meshes_to_render.push_back(object_mesh);
			}

		}
		break;

	default:
		break;
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
	if (gl_blend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
	}
	else {
		glDisable(GL_BLEND);
	}
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

ComponentMesh* ModuleRender::CreateComponentMesh()
{
	ComponentMesh *created_mesh = new ComponentMesh();
	meshes.push_back(created_mesh);
	return created_mesh;
}

void ModuleRender::RemoveComponentMesh(ComponentMesh* mesh_to_remove)
{
	auto it = std::find(meshes.begin(), meshes.end(), mesh_to_remove);
	if (it != meshes.end())
	{
		delete *it;
		meshes.erase(it);
	}
}

void ModuleRender::GenerateQuadTree()
{
	AABB2D global_AABB;
	global_AABB.SetNegativeInfinity();

	for (auto & mesh : meshes)
	{
		float minX = std::fmin(mesh->owner->aabb.bounding_box2D.minPoint.x, global_AABB.minPoint.x);
		float minY = std::fmin(mesh->owner->aabb.bounding_box2D.minPoint.y, global_AABB.minPoint.y);

		float maxX = std::fmax(mesh->owner->aabb.bounding_box2D.maxPoint.x, global_AABB.maxPoint.x);
		float maxY = std::fmax(mesh->owner->aabb.bounding_box2D.maxPoint.y, global_AABB.maxPoint.y);
		global_AABB.maxPoint = float2(maxX,maxY);
		global_AABB.minPoint = float2(minX, minY);

	}

	ol_quadtree.Create(global_AABB);
	for (auto & mesh : meshes)
	{
		ol_quadtree.Insert(*mesh->owner);
	}
}

void ModuleRender::InsertAABBTree(GameObject * game_object)
{
	ComponentMesh* object_mesh = (ComponentMesh*)game_object->GetComponent(Component::ComponentType::MESH);
	if(object_mesh != nullptr)
		ol_abbtree->Insert(game_object);
}

void ModuleRender::RemoveAABBTree(GameObject * game_object)
{
	ComponentMesh* object_mesh = (ComponentMesh*)game_object->GetComponent(Component::ComponentType::MESH);
	if (object_mesh != nullptr)
		ol_abbtree->Remove(game_object);
}

void ModuleRender::UpdateAABBTree(GameObject* game_object)
{
	ComponentMesh* object_mesh = (ComponentMesh*)game_object->GetComponent(Component::ComponentType::MESH);
	if (object_mesh != nullptr)
		ol_abbtree->UpdateObject(game_object);
}

void ModuleRender::DeleteAABBTree()
{
	delete ol_abbtree;
}

void ModuleRender::CreateAABBTree()
{
	ol_abbtree = new OLAABBTree(INITIAL_SIZE_AABBTREE);
}

void ModuleRender::DrawAABBTree() const
{
	ol_abbtree->Draw();
}

GameObject* ModuleRender::GetRaycastIntertectedObject(const LineSegment & ray)
{
	GetCullingMeshes(App->cameras->scene_camera);
	std::vector<ComponentMesh*> intersected_meshes;
	for (auto & mesh : meshes_to_render)
	{
		if (mesh->owner->aabb.bounding_box.Intersects(ray))
		{
			intersected_meshes.push_back(mesh);
		}
	}

	std::vector<GameObject*> intersected;
	GameObject* selected = nullptr;
	float min_distance = INFINITY;
	for (auto & mesh : intersected_meshes)
	{
		LineSegment transformed_ray = ray;
		transformed_ray.Transform(mesh->owner->transform.GetGlobalModelMatrix().Inverted());
		std::vector<Triangle> triangles = mesh->mesh_to_render->GetTriangles();
		for (auto & triangle : triangles)
		{
			float distance;
			bool intersected = triangle.Intersects(transformed_ray, &distance);
			if (intersected && distance < min_distance)
			{
				selected = mesh->owner;
				min_distance = distance;
			}
		}
	}
	return selected;
}

