#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleDebug.h"
#include "ModuleDebugDraw.h"
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
#include "imgui.h"
#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>
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
		// OPENGL_LOG_INFO(error_message); Too many messages in update
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

	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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

update_status ModuleRender::PostUpdate()
{

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
	meshes.clear();
	meshes_to_render.clear();
	return true;
}

void ModuleRender::Render() const
{
	BROFILER_CATEGORY("Render",Profiler::Color::Aqua);
	App->ui->Render();
	SDL_GL_SwapWindow(App->window->window);
	App->time->EndFrame();
}

void ModuleRender::RenderFrame(const ComponentCamera &camera)
{
	glStencilMask(0x00); // make sure we don't update the stencil buffer while drawing debug shapes
	
	if (App->debug->show_grid)
	{
		dd::xzSquareGrid(-100.0f, 100.0f, 0.0f, 1.0f, math::float3(0.65f, 0.65f, 0.65f));
		dd::axisTriad(math::float4x4::identity, 0.125f, 1.25f, 0, false);
	}
	if (App->debug->show_camera_frustum && App->cameras->active_camera != nullptr)
	{
		dd::frustum(App->cameras->active_camera->GetInverseClipMatrix(), float3::one);
	}
	if (App->debug->show_quadtree)
	{
		for (auto& ol_quadtree_node : App->renderer->ol_quadtree.flattened_tree)
		{
			float3 quadtree_node_min = float3(ol_quadtree_node->box.minPoint.x, 0, ol_quadtree_node->box.minPoint.y);
			float3 quadtree_node_max = float3(ol_quadtree_node->box.maxPoint.x, 0, ol_quadtree_node->box.maxPoint.y);
			dd::aabb(quadtree_node_min, quadtree_node_max, float3::one);
		}
	}

	dd::sphere(intersection_point, float3(1, 0, 0), 0.75f);

	rendering_measure_timer->Start();
	GetCullingMeshes(App->cameras->active_camera);
	for (auto &mesh : meshes_to_render)
	{
		RenderMesh(*mesh, camera);
	}
	rendering_measure_timer->Stop();
	App->debug->rendering_time = rendering_measure_timer->Read();

	App->debug_draw->Render(camera);
}

void ModuleRender::GetCullingMeshes(const ComponentCamera *camera)
{
	meshes_to_render.clear();

	std::copy_if(meshes.begin(),
		meshes.end(),
		std::back_inserter(meshes_to_render), [camera](auto mesh)
	{

		if (App->debug->frustum_culling && mesh->owner->IsVisible(*camera))
		{
			return true;
		}
		else if(App->debug->frustum_culling && !mesh->owner->IsVisible(*camera))
		{
			return false;
		}
		if (App->debug->quadtree_culling &&  mesh->IsEnabled() && !mesh->owner->IsStatic())
		{
			return true;
		}
		return !App->debug->quadtree_culling  && mesh->IsEnabled();
	});

	if (App->debug->quadtree_culling)
	{
		std::vector<GameObject*> rendered_objects;
		ol_quadtree.CollectIntersect(rendered_objects, *camera);

		for (auto &object : rendered_objects)
		{
			ComponentMesh *object_mesh = (ComponentMesh*)object->GetComponent(Component::ComponentType::MESH);
			meshes_to_render.push_back(object_mesh);
		}
	}
}

void ModuleRender::RenderMesh(const ComponentMesh &mesh, const ComponentCamera &camera) const
{
	const GameObject& mesh_game_object = *mesh.owner;

	if (App->debug->show_bounding_boxes && !mesh_game_object.aabb.IsEmpty())
	{
		dd::aabb(mesh_game_object.aabb.bounding_box.minPoint, mesh_game_object.aabb.bounding_box.maxPoint, float3::one);
	}

	GLuint shader_program = mesh.shader_program == 0 ? App->program->texture_program : mesh.shader_program;
	glUseProgram(shader_program);

	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "model"),
		1,
		GL_TRUE,
		&mesh_game_object.transform.GetGlobalModelMatrix()[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "view"),
		1,
		GL_TRUE,
		&camera.GetViewMatrix()[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(shader_program, "proj"),
		1,
		GL_TRUE,
		&camera.GetProjectionMatrix()[0][0]
	);

	if (App->scene->hierarchy.selected_game_object == &mesh_game_object)
	{
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}

	App->lights->RenderLight(shader_program);
	mesh_game_object.RenderMaterialTexture(shader_program);
	mesh.Render();

	if (App->scene->hierarchy.selected_game_object == &mesh_game_object)
	{
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		GLuint outline_shader_program = -1;// App->program->outline_program;
		glUseProgram(outline_shader_program);

		glUniformMatrix4fv(
			glGetUniformLocation(outline_shader_program, "model"),
			1,
			GL_TRUE,
			&mesh_game_object.transform.GetGlobalModelMatrix()[0][0]
		);
		glUniformMatrix4fv(
			glGetUniformLocation(outline_shader_program, "view"),
			1,
			GL_TRUE,
			&camera.GetViewMatrix()[0][0]
		);
		glUniformMatrix4fv(
			glGetUniformLocation(outline_shader_program, "proj"),
			1,
			GL_TRUE,
			&camera.GetProjectionMatrix()[0][0]
		);
		mesh.Render();
		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
	}

	glUseProgram(0);
}

void ModuleRender::SetVSync(const bool vsync)
{
	this->vsync = vsync;
	vsync ? SDL_GL_SetSwapInterval(1) : SDL_GL_SetSwapInterval(0);
}

void ModuleRender::SetAlphaTest(const bool gl_alpha_test)
{
	this->gl_alpha_test = gl_alpha_test;
	gl_alpha_test ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);
}

void ModuleRender::SetDepthTest(const bool gl_depth_test)
{
	this->gl_depth_test = gl_depth_test;
	gl_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void ModuleRender::SetScissorTest(const bool gl_scissor_test)
{
	this->gl_scissor_test = gl_scissor_test;
	gl_scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
}

void ModuleRender::SetStencilTest(const bool gl_stencil_test)
{
	this->gl_stencil_test = gl_stencil_test;
	gl_stencil_test ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
}

void ModuleRender::SetBlending(const bool gl_blend)
{
	this->gl_blend = gl_blend;
	gl_blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void ModuleRender::SetFaceCulling(const bool gl_cull_face)
{
	this->gl_cull_face = gl_cull_face;
	gl_cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void ModuleRender::SetCulledFaces(const GLenum culled_faces) const
{
	glCullFace(culled_faces);
}

void ModuleRender::SetFrontFaces(const GLenum front_faces) const
{
	glFrontFace(front_faces);
}

void ModuleRender::SetDithering(const bool gl_dither)
{
	this->gl_dither = gl_dither;
	gl_dither ? glEnable(GL_DITHER) : glDisable(GL_DITHER);
}

void ModuleRender::SetMinMaxing(const bool gl_minmax)
{
	this->gl_minmax = gl_minmax;
	gl_minmax ? glEnable(GL_MINMAX) : glDisable(GL_MINMAX);
}

void ModuleRender::SetWireframing(const bool gl_wireframe)
{
	this->gl_wireframe = gl_wireframe;
	gl_wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
void ModuleRender::ShowRenderOptions()
{
	if (ImGui::CollapsingHeader(ICON_FA_CLONE " Renderer"))
	{
		if (ImGui::Checkbox("VSync", &vsync))
		{
			SetVSync(vsync);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Wireframe", &gl_wireframe))
		{
			SetWireframing(gl_wireframe);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Depth test", &gl_depth_test))
		{
			SetDepthTest(gl_depth_test);
		}
		ImGui::Separator();
		if (ImGui::Checkbox("Face culling", &gl_cull_face))
		{
			SetFaceCulling(gl_cull_face);
		}
		if (ImGui::Combo("Culled faces", &culled_faces, "Back\0Front\0Front and back\0"))
		{
			switch (culled_faces)
			{
			case 0:
				SetCulledFaces(GL_BACK);
				break;
			case 1:
				SetCulledFaces(GL_FRONT);
				break;
			case 2:
				SetCulledFaces(GL_FRONT_AND_BACK);
				break;
			}
		}
		if (ImGui::Combo("Front faces", &front_faces, "Counterclockwise\0Clockwise\0"))
		{
			switch (front_faces)
			{
			case 0:
				SetFrontFaces(GL_CCW);
				break;
			case 1:
				SetFrontFaces(GL_CW);
				break;
			}
		}
		if (ImGui::Combo("Filling mode", &filling_mode, "Fill\0Lines\0Vertices"))
		{
			if (filling_mode == 0) {
				glPolygonMode(GL_FRONT, GL_FILL);
			}
			if (filling_mode == 1) {
				glPolygonMode(GL_FRONT, GL_LINE);
			}
			if (filling_mode == 2) {
				glPolygonMode(GL_FRONT, GL_POINT);
			}
		}
		ImGui::Separator();
		HelpMarker("This settings have no visual impact, WIP.");
		ImGui::SameLine();
		if (ImGui::TreeNode("Non-functional settings"))
		{
			if (ImGui::Checkbox("Alpha test", &gl_alpha_test))
			{
				SetAlphaTest(gl_alpha_test);
			}
			if (ImGui::Checkbox("Scissor test", &gl_scissor_test))
			{
				SetScissorTest(gl_scissor_test);
			}
			if (ImGui::Checkbox("Stencil test", &gl_stencil_test))
			{
				SetStencilTest(gl_stencil_test);
			}
			if (ImGui::Checkbox("Blending", &gl_blend))
			{
				SetBlending(gl_blend);
			}
			if (ImGui::Checkbox("Dithering", &gl_dither))
			{
				SetDithering(gl_dither);
			}
			if (ImGui::Checkbox("Min Maxing", &gl_minmax))
			{
				SetMinMaxing(gl_minmax);
			}
			ImGui::TreePop();
		}
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

GameObject* ModuleRender::GetRaycastIntertectedObject(LineSegment & ray)
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
		ray.Transform(mesh->owner->transform.GetGlobalModelMatrix().Inverted());

		for (auto& triangle : mesh->mesh_to_render->triangles)
		{
			float distance;
			float3 aux_intersection_point;
			bool intersected = triangle.Intersects(ray, &distance, &aux_intersection_point);
			
			if (intersected && distance < min_distance)
			{
				selected = mesh->owner;
				min_distance = distance;
				float4 aux_asidjai = float4(aux_intersection_point, 1);
				aux_asidjai = mesh->owner->transform.GetGlobalModelMatrix() * aux_asidjai;
				intersection_point = float3(aux_asidjai.x, aux_asidjai.y, aux_asidjai.z);
			}
		}
	}
	return selected;
}