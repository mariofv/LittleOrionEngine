#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleModelLoader.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMesh.h"
#include "GeometryRenderer.h"

#include <SDL/SDL.h>
#include "MathGeoLib.h"
#include <assimp/scene.h>
#include "imgui.h"
#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>
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

	APP_LOG_INIT("Creating Glew Renderer context");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

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

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openglCallbackFunction, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);

	SetVSync(VSYNC);
	SetDepthTest(true);

	geometry_renderer = new GeometryRenderer();

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

	delete geometry_renderer;
	for (auto& mesh : meshes)
	{
		delete mesh;
	}
	meshes.clear();
	return true;
}

void ModuleRender::Render() const
{
	BROFILER_CATEGORY("Render",Profiler::Color::Aqua);
	App->editor->Render();
	SDL_GL_SwapWindow(App->window->window);
	App->time->EndFrame();
}

void ModuleRender::RenderFrame(const ComponentCamera &camera)
{

	RenderGrid(camera);
	if (App->cameras->active_camera != nullptr) 
	{
		geometry_renderer->RenderHexahedron(camera, App->cameras->active_camera->GetFrustumVertices());

		for (auto &mesh : meshes)
		{
			if (mesh->IsEnabled() && App->cameras->active_camera->IsInsideFrustum(mesh->owner->aabb.bounding_box))
			{
				RenderMesh(*mesh, camera);
			}
		}
	}
	else 
	{
		for (auto &mesh : meshes)
		{
			RenderMesh(*mesh, camera);
		}
	}
}

void ModuleRender::RenderMesh(const ComponentMesh &mesh, const ComponentCamera &camera) const
{
	const GameObject& mesh_game_object = *mesh.owner;

	GLuint shader_program = App->program->texture_program;
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

	int mesh_material_index = mesh.material_index;
	const GLuint mesh_texture = mesh_game_object.GetMaterialTexture(mesh_material_index);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh_texture);
	glUniform1i(glGetUniformLocation(shader_program, "texture0"), 0);

	mesh.Render();

	glUseProgram(0);


	if (!mesh_game_object.aabb.IsEmpty())
	{
		geometry_renderer->RenderHexahedron(camera, mesh_game_object.aabb.GetVertices());
	}
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

void ModuleRender::RenderGrid(const ComponentCamera &camera) const
{
	glUseProgram(App->program->primitive_program);

	// CREATES MODEL MATRIX
	float4x4 model = float4x4::FromTRS(
		float3(0.0f, 0.0f, 0.0f),
		float3x3::identity,
		float3(1.0f, 1.0f, 1.0f)
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->primitive_program, "model"),
		1,
		GL_TRUE,
		&model[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->primitive_program, "view"),
		1,
		GL_TRUE,
		&camera.GetViewMatrix()[0][0]
	);
	glUniformMatrix4fv(
		glGetUniformLocation(App->program->primitive_program, "proj"),
		1,
		GL_TRUE,
		&camera.GetProjectionMatrix()[0][0]
	);

	glLineWidth(1.0f);
	float d = 200.0f;
	glBegin(GL_LINES);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}
	glEnd();

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	// red X
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
	glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);
	// green Y
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
	glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);
	// blue Z
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
	glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
	glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);
	glEnd();
	glLineWidth(1.0f);
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