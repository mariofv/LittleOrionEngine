#include "ModuleDebugDraw.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentTrail.h"

#include "Component/ComponentLight.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentParticleSystem.h"

#include "EditorUI/Helper/Grid.h"
#include "EditorUI/Panel/PanelNavMesh.h"

#include "Log/EngineLog.h"
#include "Main/Application.h"
#include "Main/GameObject.h"

#include "ModuleAI.h"
#include "ModuleAnimation.h"
#include "ModuleCamera.h"
#include "ModuleDebug.h"
#include "ModuleEditor.h"
#include "ModuleProgram.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleSpacePartitioning.h"
#include "ModuleUI.h"

#include "SpacePartition/OLQuadTree.h"
#include "SpacePartition/OLOctTree.h"
#include "ResourceManagement/ResourcesDB/CoreResources.h"
#include "ResourceManagement/Resources/Skeleton.h"

#define DEBUG_DRAW_IMPLEMENTATION
#include "EditorUI/DebugDraw.h"     // Debug Draw API. Notice that we need the DEBUG_DRAW_IMPLEMENTATION macro here!

#include <array>
#include <assert.h>
#include <GL/glew.h>
#include <Brofiler/Brofiler.h>

class IDebugDrawOpenGLImplementation final : public dd::RenderInterface
{
public:

    //
    // dd::RenderInterface overrides:
    //
	void openGLDraw(const dd::DrawVertex* points, int count, bool depthEnabled, GLenum mode)
	{
		assert(points != nullptr);
		assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);
		glBindVertexArray(linePointVAO);
		GLuint shader_program = App->program->UseProgram("Linepoint");

		glUniformMatrix4fv(
			glGetUniformLocation(shader_program, "u_MvpMatrix"),
			1, GL_TRUE, reinterpret_cast<const float*>(&mvpMatrix)
		);

		bool already = glIsEnabled(GL_DEPTH_TEST);

		if (depthEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		// NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
		glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), points);

		// Issue the draw call:
		glDrawArrays(mode, 0, count);

		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		checkGLError(__FILE__, __LINE__);

		if (already)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}
    void drawPointList(const dd::DrawVertex* points, int count, bool depthEnabled) override
    {
		openGLDraw(points,count,depthEnabled,GL_POINTS);

    }

    void drawLineList(const dd::DrawVertex* lines, int count, bool depthEnabled) override
    {
		openGLDraw(lines, count, depthEnabled, GL_LINES);

    }

    void drawGlyphList(const dd::DrawVertex* glyphs, int count, dd::GlyphTextureHandle glyphTex) override
    {
        assert(glyphs != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

        glBindVertexArray(textVAO);
		GLuint shader_program = App->program->UseProgram("Text");

        // These doesn't have to be reset every draw call, I'm just being lazy ;)
        glUniform1i(
            glGetUniformLocation(shader_program, "u_glyphTexture"),
            0
        );
        glUniform2f(
            glGetUniformLocation(shader_program, "u_screenDimensions"),
            static_cast<GLfloat>(width),
            static_cast<GLfloat>(height)
        );

        if (glyphTex != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, handleToGL(glyphTex));
        }

        bool already_blend = glIsEnabled(GL_BLEND);

        if (!already_blend)
        {
            glEnable(GL_BLEND);
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        bool already = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), glyphs);

        glDrawArrays(GL_TRIANGLES, 0, count); // Issue the draw call

        if (!already_blend)
        {
            glDisable(GL_BLEND);
        }

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        checkGLError(__FILE__, __LINE__);

        if (already)
        {
            glEnable(GL_DEPTH_TEST);
        }
    }

    dd::GlyphTextureHandle createGlyphTexture(int width, int height, const void* pixels) override
    {
        assert(width > 0 && height > 0);
        assert(pixels != nullptr);

        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        checkGLError(__FILE__, __LINE__);

        return GLToHandle(textureId);
    }

    void destroyGlyphTexture(dd::GlyphTextureHandle glyphTex) override
    {
        if (glyphTex == nullptr)
        {
            return;
        }

        const GLuint textureId = handleToGL(glyphTex);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
    }

    // These two can also be implemented to perform GL render
    // state setup/cleanup, but we don't use them in this sample.
    //void beginDraw() override { }
    //void endDraw()   override { }

    //
    // Local methods:
    //

    IDebugDrawOpenGLImplementation()
        : mvpMatrix()
        , width(0)
        , height(0)
        , linePointVAO(0)
        , linePointVBO(0)
        , textVAO(0)
        , textVBO(0)
    {
        //std::printf("\n");
        //std::printf("GL_VENDOR    : %s\n",   glGetString(GL_VENDOR));
        //std::printf("GL_RENDERER  : %s\n",   glGetString(GL_RENDERER));
        //std::printf("GL_VERSION   : %s\n",   glGetString(GL_VERSION));
        //std::printf("GLSL_VERSION : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        //std::printf("IDebugDrawOpenGLImplementation initializing ...\n");

        // This has to be enabled since the point drawing shader will use gl_PointSize.
        glEnable(GL_PROGRAM_POINT_SIZE);

        setupVertexBuffers();

        //std::printf("IDebugDrawOpenGLImplementation ready!\n\n");
    }

    ~IDebugDrawOpenGLImplementation()
    {
        glDeleteVertexArrays(1, &linePointVAO);
        glDeleteBuffers(1, &linePointVBO);

        glDeleteVertexArrays(1, &textVAO);
        glDeleteBuffers(1, &textVBO);
    }

    void setupVertexBuffers()
    {
        //std::printf("> IDebugDrawOpenGLImplementation::setupVertexBuffers()\n");

        //
        // Lines/points vertex buffer:
        //
        {
            glGenVertexArrays(1, &linePointVAO);
            glGenBuffers(1, &linePointVBO);
            checkGLError(__FILE__, __LINE__);

            glBindVertexArray(linePointVAO);
            glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);

            // RenderInterface will never be called with a batch larger than
            // DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the same amount here.
            glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
            checkGLError(__FILE__, __LINE__);

            // Set the vertex format expected by 3D points and lines:
            std::size_t offset = 0;

            glEnableVertexAttribArray(0); // in_Position (vec3)
            glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 3,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 3;

            glEnableVertexAttribArray(1); // in_ColorPointSize (vec4)
            glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));

            checkGLError(__FILE__, __LINE__);

            // VAOs can be a pain in the neck if left enabled...
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //
        // Text rendering vertex buffer:
        //
        {
            glGenVertexArrays(1, &textVAO);
            glGenBuffers(1, &textVBO);
            checkGLError(__FILE__, __LINE__);

            glBindVertexArray(textVAO);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);

            // NOTE: A more optimized implementation might consider combining
            // both the lines/points and text buffers to save some memory!
            glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
            checkGLError(__FILE__, __LINE__);

            // Set the vertex format expected by the 2D text:
            std::size_t offset = 0;

            glEnableVertexAttribArray(0); // in_Position (vec2)
            glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 2;

            glEnableVertexAttribArray(1); // in_TexCoords (vec2)
            glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 2;

            glEnableVertexAttribArray(2); // in_Color (vec4)
            glVertexAttribPointer(
                /* index     = */ 2,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));

            checkGLError(__FILE__, __LINE__);

            // Ditto.
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    static GLuint handleToGL(dd::GlyphTextureHandle handle)
    {
        const std::size_t temp = reinterpret_cast<std::size_t>(handle);
        return static_cast<GLuint>(temp);
    }

    static dd::GlyphTextureHandle GLToHandle(const GLuint id)
    {
        const std::size_t temp = static_cast<std::size_t>(id);
        return reinterpret_cast<dd::GlyphTextureHandle>(temp);
    }

    static void checkGLError(const char* file, int line)
    {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            //errorF("%s(%d) : GL_CORE_ERROR=0x%X - %s", file, line, err, errorToString(err));
        }
    }

    // The "model-view-projection" matrix for the scene.
    // In this demo, it consists of the camera's view and projection matrices only.
    math::float4x4 mvpMatrix;
    unsigned width, height;

private:
    GLuint linePointVAO;
    GLuint linePointVBO;

    GLuint textVAO;
    GLuint textVBO;
}; // class IDebugDrawOpenGLImplementation


IDebugDrawOpenGLImplementation* ModuleDebugDraw::dd_interface_implementation = 0;

// Called before render is available
bool ModuleDebugDraw::Init()
{
	APP_LOG_SECTION("************ Module Debug Draw Init ************");

	dd_interface_implementation = new IDebugDrawOpenGLImplementation();
    dd::initialize(dd_interface_implementation);


	grid = new Grid();

    APP_LOG_INFO("Module Debug Draw initialized correctly.")

	return true;
}


void ModuleDebugDraw::RenderTangentsAndBitangents() const
{
	BROFILER_CATEGORY("Render Tangent, Bitangent and Normal Vectors", Profiler::Color::Lavender);

	for (auto& mesh_renderer : App->renderer->mesh_renderers)
	{
		if (!mesh_renderer->active || mesh_renderer->mesh_to_render == nullptr)
		{
			continue;
		}

		size_t i = 0;
		while (i < 30 && i < mesh_renderer->mesh_to_render->vertices.size())
		{
			Mesh::Vertex current_vertex = mesh_renderer->mesh_to_render->vertices[i];

			float4 normal = float4(current_vertex.normals, 0.0F);
			float4 tangent = float4(current_vertex.tangent, 0.0F);
			float4 bitangent = float4(current_vertex.bitangent, 0.0F);
			float4 position = float4 (current_vertex.position, 1.0F);

			float4x4 axis_object_space = float4x4(tangent, bitangent, normal, position);
			float4x4 axis_transform = mesh_renderer->owner->transform.GetGlobalModelMatrix() * axis_object_space;
			dd::axisTriad(axis_transform, 0.1F, 1.0F);

			++i;
		}	
	}
}

void ModuleDebugDraw::RenderRectTransform(const GameObject* rect_owner) const
{
	BROFILER_CATEGORY("Render Rect Transform", Profiler::Color::Lavender);

	if (!App->debug->show_transform_2d || rect_owner->GetTransformType() == Component::ComponentType::TRANSFORM)
	{
		return;
	}

	float4x4 selected_game_object_global_2d_model_matrix = rect_owner->transform_2d.GetSizedGlobalModelMatrix();
	std::array<float3, 4> rect_points = 
	{
		(selected_game_object_global_2d_model_matrix * float4(-0.5f, -0.5f, 0.f, 1.f)).xyz(),
		(selected_game_object_global_2d_model_matrix * float4(-0.5f, 0.5f, 0.f, 1.f)).xyz(),
		(selected_game_object_global_2d_model_matrix * float4(0.5f, 0.5f, 0.f, 1.f)).xyz(),
		(selected_game_object_global_2d_model_matrix * float4(0.5f, -0.5f, 0.f, 1.f)).xyz()
	};

	dd::line(rect_points[1], rect_points[2], float3::one);
	dd::line(rect_points[2], rect_points[3], float3::one);
	dd::line(rect_points[3], rect_points[0], float3::one);
	dd::line(rect_points[0], rect_points[1], float3::one);
}

void ModuleDebugDraw::RenderLine(const float3& a, const float3& b, const float3& color) const
{
	dd::line(a, b, color);
}

void ModuleDebugDraw::RenderSphere(float3& position, float3& color, float radius) const
{
	dd::sphere(position, color, radius);
}

void ModuleDebugDraw::RenderCameraFrustum() const
{
	BROFILER_CATEGORY("Render Selected GameObject Camera Frustum", Profiler::Color::Lavender);

	Component * selected_camera_component = App->editor->selected_game_object->GetComponent(Component::ComponentType::CAMERA);
	if (selected_camera_component != nullptr) {
		ComponentCamera* selected_camera = static_cast<ComponentCamera*>(selected_camera_component);

		if (selected_camera->camera_frustum.type == FrustumType::PerspectiveFrustum)
		{
			RenderPerspectiveFrustum(selected_camera->GetInverseClipMatrix());
		}
		else if (selected_camera->camera_frustum.type == FrustumType::OrthographicFrustum)
		{
			float3 points[8];
			selected_camera->camera_frustum.GetCornerPoints(points);
			RenderOrtographicFrustum(points);
		}
	}	
}

void ModuleDebugDraw::RenderParticleSystem() const 
{
	BROFILER_CATEGORY("Render Selected GameObject Particle System Gizmo", Profiler::Color::Lavender);

	Component* particle_system = App->editor->selected_game_object->GetComponent(Component::ComponentType::PARTICLE_SYSTEM);
	if (particle_system != nullptr)
	{
		ComponentParticleSystem* selected_particle_system = static_cast<ComponentParticleSystem*>(particle_system);
		float gizmo_radius = 2.5F;
		switch (selected_particle_system->type_of_particle_system)
		{
			case ComponentParticleSystem::TypeOfParticleSystem::SPHERE:
				dd::point_light(
					App->editor->selected_game_object->transform.GetGlobalTranslation(), 
					float3(1.f, 1.f, 0.f),
					selected_particle_system->particles_life_time*selected_particle_system->velocity_particles_start
				);
			break;
			case ComponentParticleSystem::TypeOfParticleSystem::BOX:
			{

				float min_x = static_cast<float>(selected_particle_system->min_range_random_x);
				float max_x = static_cast<float>(selected_particle_system->max_range_random_x);
				float min_z = static_cast<float>(selected_particle_system->min_range_random_z);
				float max_z = static_cast<float>(selected_particle_system->max_range_random_z);
				float height = selected_particle_system->particles_life_time*selected_particle_system->velocity_particles_start *100.0f;
				float3 box_points[8] = {
					float3(min_x,0.0f,min_z) / 100,
					float3(min_x, 0.0f, max_z) / 100,
					float3(max_x, 0.0f, max_z) / 100,
					float3(max_x, 0.0f, min_z) / 100,

					float3(min_x,height,min_z) / 100,
					float3(min_x, height, max_z) / 100,
					float3(max_x, height, max_z) / 100,
					float3(max_x, height, min_z) / 100
				};

				for (unsigned int i = 0; i < 8; ++i)
				{
					box_points[i] = App->editor->selected_game_object->transform.GetGlobalTranslation() + (App->editor->selected_game_object->transform.GetGlobalRotation() *box_points[i]);
				}
				dd::box(box_points, ddVec3(1.f, 1.f, 0.f));
			
			break;
			}
			case ComponentParticleSystem::TypeOfParticleSystem::CONE:
			
				dd::cone(
					App->editor->selected_game_object->transform.GetGlobalTranslation(), 
					App->editor->selected_game_object->transform.GetGlobalRotation()*float3::unitY * 
					selected_particle_system->particles_life_time*selected_particle_system->velocity_particles_start,
					float3(1.f, 1.f, 0.f), 
					selected_particle_system->outer_radius, 
					selected_particle_system->inner_radius
				);
				
			break;
		}
	}
}

void ModuleDebugDraw::RenderLightGizmo() const	
{	
	BROFILER_CATEGORY("Render Selected GameObject Light Gizmo", Profiler::Color::Lavender);

	Component* selected_light_component = App->editor->selected_game_object->GetComponent(Component::ComponentType::LIGHT);	
	if (selected_light_component != nullptr)
	{	
		ComponentLight* selected_light = static_cast<ComponentLight*>(selected_light_component);	
		ComponentTransform* selected_light_transform = &selected_light->owner->transform;
		float gizmo_radius = 2.5F;	
		switch (selected_light->light_type)	
		{	
		case ComponentLight::LightType::DIRECTIONAL_LIGHT:	
			dd::directional_light(selected_light_transform->GetGlobalTranslation(), selected_light_transform->GetRotation().ToFloat4x4(), float3(1.f, 1.f, 0.f), 5.f, gizmo_radius);	
			break;	
		case ComponentLight::LightType::SPOT_LIGHT:	
			dd::spot_light(	
				selected_light_transform->GetGlobalTranslation(), 	
				selected_light_transform->GetRotation().ToFloat4x4(),	
				float3(1.f, 1.f, 0.f),	
				selected_light->spot_light_parameters.range / 10.f,	
				tan(DegToRad(selected_light->spot_light_parameters.spot_angle/2.f)) * selected_light->spot_light_parameters.range / 10.f
			); 	
			break;	
		case ComponentLight::LightType::POINT_LIGHT:	
			dd::point_light(selected_light_transform->GetGlobalTranslation(), float3(1.f, 1.f, 0.f), selected_light->point_light_parameters.range / 10.f);
			break;	
		default:	
			break;	
		}	
	}	
}	

void ModuleDebugDraw::RenderBones(GameObject* game_object) const
{
	ComponentMeshRenderer* mesh_renderer = static_cast<ComponentMeshRenderer*>(game_object->GetComponent(Component::ComponentType::MESH_RENDERER));
	if (mesh_renderer != nullptr)
	{
		std::shared_ptr<Skeleton> mesh_skeleton = mesh_renderer->skeleton;
		if (mesh_skeleton != nullptr)
		{
			float3 color(1.0f, 0.0f, 0.0f);
			for (auto& joint : mesh_skeleton->skeleton)
			{
				if (joint.parent_index != -1)
				{
					dd::line(
						(game_object->transform.GetGlobalModelMatrix() * mesh_skeleton->skeleton[joint.parent_index].transform_global.Inverted() * float4(0.f, 0.f, 0.f, 1.f)).xyz(),
						(game_object->transform.GetGlobalModelMatrix() * joint.transform_global.Inverted() * float4(0.f,0.f,0.f,1.f)).xyz(),
						color
					);
				}

				if (color.x == 1.f)
					color = float3(0.f, 1.f, 0.f);
				else if (color.y == 1.f)
					color = float3(0.f, 0.f, 1.f);
				else if (color.z == 1.f)
					color = float3(1.f, 0.f, 0.f);
			}
		}
	}
}

void ModuleDebugDraw::RenderOutlineInternal(ComponentMeshRenderer* mesh_renderer, const float4& color) const
{
	BROFILER_CATEGORY("Render Outline", Profiler::Color::Lavender);

	if (mesh_renderer != nullptr && mesh_renderer->IsEnabled())
	{
		BROFILER_CATEGORY("Render Outline Write Stencil", Profiler::Color::Lavender);

		if (!mesh_renderer->mesh_to_render)
		{
			return;
		}

		GameObject* mesh_renderer_owner = mesh_renderer->owner;

		glDrawBuffer(0);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		glStencilMask(0xFF);

		GLuint mesh_renderer_program = mesh_renderer->BindDepthShaderProgram();
		mesh_renderer->BindMeshUniforms(mesh_renderer_program);
		mesh_renderer->RenderModel();
		glUseProgram(0);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		BROFILER_CATEGORY("Render Outline Read Stencil", Profiler::Color::Lavender);

		GLuint outline_shader_program = App->program->UseProgram("Outline");
		float4x4 new_transformation_matrix;
		if (mesh_renderer_owner->parent != nullptr)
		{
			new_transformation_matrix = mesh_renderer_owner->parent->transform.GetGlobalModelMatrix() * mesh_renderer_owner->transform.GetModelMatrix() * float4x4::Scale(float3(1.01f));

			ComponentTransform object_transform_copy = mesh_renderer_owner->transform;
			float3 object_scale = object_transform_copy.GetScale();
			object_transform_copy.SetScale(object_scale*1.01f);
			object_transform_copy.GenerateGlobalModelMatrix();
		}
		else
		{
			new_transformation_matrix = mesh_renderer_owner->transform.GetGlobalModelMatrix() * float4x4::Scale(float3(1.01f));
		}


		ModuleRender::DrawMode last_draw_mode = App->renderer->draw_mode;
		App->renderer->SetDrawMode(ModuleRender::DrawMode::WIREFRAME);
		glLineWidth(15.f);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		mesh_renderer->BindMeshUniforms(outline_shader_program);
		glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), mesh_renderer_owner->transform.GetGlobalModelMatrix().Transposed().ptr());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glUniform4fv(glGetUniformLocation(outline_shader_program, "base_color"), 1, color.ptr());

		mesh_renderer->RenderModel();

		glLineWidth(1.f);
		App->renderer->SetDrawMode(last_draw_mode);

		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);

		glUseProgram(0);

	}
	
}

void ModuleDebugDraw::RenderBoundingBoxes(const float3& color) const
{
	BROFILER_CATEGORY("Render Bounding Boxes", Profiler::Color::Lavender);

	for (auto& mesh_renderer : App->renderer->mesh_renderers)
	{
		GameObject* mesh_game_object = mesh_renderer->owner;
		if (!mesh_game_object->aabb.IsEmpty())
		{
			dd::aabb(mesh_game_object->aabb.bounding_box.minPoint, mesh_game_object->aabb.bounding_box.maxPoint, color);
		}
	}
}

void ModuleDebugDraw::RenderGlobalBoundingBoxes(const float3& color) const
{
	BROFILER_CATEGORY("Render Global Bounding Boxes", Profiler::Color::Beige);

	for (auto& object : App->scene->game_objects_ownership)
	{
		dd::aabb(object->aabb.global_bounding_box.minPoint, object->aabb.global_bounding_box.maxPoint, color);
	}
}

void ModuleDebugDraw::RenderBillboards() const
{
	BROFILER_CATEGORY("Render Billboards", Profiler::Color::Brown);

	for (auto& object : App->scene->game_objects_ownership)
	{
		Component * light_component = object->GetComponent(Component::ComponentType::LIGHT);
		if (light_component != nullptr) {
		}

		Component * camera_component = object->GetComponent(Component::ComponentType::CAMERA);
		if (camera_component != nullptr) {
		}

	}
}

void ModuleDebugDraw::RenderPathfinding() const
{
	BROFILER_CATEGORY("Render PathFindings", Profiler::Color::SaddleBrown);
	//First check if starting and ending point are null and render
	if(App->artificial_intelligence->start_initialized)
	{
		dd::point(App->artificial_intelligence->start_position, float3(0, 255, 0), 20.0f);
	}

	if (App->artificial_intelligence->end_initialized)
	{
		dd::point(App->artificial_intelligence->end_position, float3(0, 255, 255), 20.0f);
	}

	for(const auto& point : App->artificial_intelligence->debug_path)
	{
		dd::point(point, float3(0, 0, 255), 10.0f);
	}
}

void ModuleDebugDraw::RenderSelectedGameObjectsOutline()
{
	for (auto selected_game_object : App->editor->selected_game_objects)
	{
		ComponentMeshRenderer* selected_object_mesh = static_cast<ComponentMeshRenderer*>(selected_game_object->GetComponent(Component::ComponentType::MESH_RENDERER));
		RenderOutline(selected_object_mesh, float4(1.0, 0.4, 0.0, 1.0));
	}
}

void ModuleDebugDraw::RenderGrid() const
{
	BROFILER_CATEGORY("Render Grid", Profiler::Color::RosyBrown);
	float scene_camera_height = App->cameras->scene_camera->owner->transform.GetGlobalTranslation().y;
	grid->ScaleOnDistance(scene_camera_height);
	grid->Render();
}

ENGINE_API void ModuleDebugDraw::RenderSingleAABB(AABB& aabb, const float3& color) const
{
	dd::aabb(aabb.minPoint, aabb.maxPoint, color);
}

void ModuleDebugDraw::RenderNavMesh(ComponentCamera & cam) const
{
	App->artificial_intelligence->RenderNavMesh(cam);
}

void ModuleDebugDraw::RenderQuadTree(const float3& color) const
{
	BROFILER_CATEGORY("Render QuadTree", Profiler::Color::Brown);
	for (auto& ol_quadtree_node : App->space_partitioning->ol_quadtree->flattened_tree)
	{
		float3 quadtree_node_min = float3(ol_quadtree_node->box.minPoint.x, 0, ol_quadtree_node->box.minPoint.y);
		float3 quadtree_node_max = float3(ol_quadtree_node->box.maxPoint.x, 0, ol_quadtree_node->box.maxPoint.y);
		dd::aabb(quadtree_node_min, quadtree_node_max, color);
	}
}

void ModuleDebugDraw::RenderOcTree(const float3& color) const
{
	BROFILER_CATEGORY("Render OcTree", Profiler::Color::Brown);
	for (auto& ol_octtree_node : App->space_partitioning->ol_octtree->flattened_tree)
	{
		float3 octtree_node_min = float3(ol_octtree_node->box.minPoint.x, ol_octtree_node->box.minPoint.y, ol_octtree_node->box.minPoint.z);
		float3 octtree_node_max = float3(ol_octtree_node->box.maxPoint.x, ol_octtree_node->box.maxPoint.y, ol_octtree_node->box.maxPoint.z);
		dd::aabb(octtree_node_min, octtree_node_max, color);
	}
}

void ModuleDebugDraw::RenderAABBTree(const float3& color) const
{
	BROFILER_CATEGORY("Render ABBTree", Profiler::Color::LightGoldenRodYellow);
	//TODO: Change this
	App->space_partitioning->DrawAABBTree();
}

void ModuleDebugDraw::RenderPhysics() const
{
	BROFILER_CATEGORY("Render Physcis", Profiler::Color::PaleGoldenRod);
	App->physics->world->debugDrawWorld();
}

void ModuleDebugDraw::RenderSelectedGameObjectHelpers() const
{
	if (App->editor->selected_game_object != nullptr)
	{
		BROFILER_CATEGORY("Render Selected GameObject DebugDraws", Profiler::Color::Lavender);

		RenderCameraFrustum();
		RenderLightGizmo();
		RenderRectTransform(App->editor->selected_game_object);
		RenderBones(App->editor->selected_game_object);
		RenderParticleSystem();
	}
}

void ModuleDebugDraw::RenderPoint(const float3& point, float size, const float3& color) const
{
	dd::point(point, color, size);
}

void ModuleDebugDraw::RenderCircle(const float3& center, float radius, const float3& normal, const float3 & color) const
{
	dd::circle(center, normal, color, radius, 20.f);
}

void ModuleDebugDraw::RenderSphere(const float3& center, float radius, const float3& color) const
{
	dd::sphere(center, color, radius);
}

ENGINE_API void ModuleDebugDraw::RenderBox(const float3 points[8], const float3& color)
{
	dd::box(points, color);
}

void ModuleDebugDraw::RenderAxis(const float4x4& axis_space, float size, float length, const float3 & color)
{
	return dd::axisTriad(axis_space, size, length);
}

void ModuleDebugDraw::RenderPerspectiveFrustum(const float4x4& inverse_clip_matrix, const float3& color) const
{
	dd::frustum(inverse_clip_matrix, color);
}

void ModuleDebugDraw::RenderOrtographicFrustum(const float3 points[8], const float3& color) const
{
	dd::box(points, color);
}

void ModuleDebugDraw::RenderOutline(ComponentMeshRenderer* mesh_renderer, const float4& color)
{
	if (mesh_renderer != nullptr)
	{
		outline_entities.push({ mesh_renderer, color });
	}
}

void ModuleDebugDraw::Render(float width, float height, const float4x4& projection_view_matrix)
{
	BROFILER_CATEGORY("Flush Debug Draw", Profiler::Color::Lavender);

	while (!outline_entities.empty())
	{
		OutlineEntity current_outline_entity = outline_entities.front();
		outline_entities.pop();
		RenderOutlineInternal(current_outline_entity.mesh_renderer, current_outline_entity.color);
	}

	dd_interface_implementation->width = static_cast<unsigned int>(width);
	dd_interface_implementation->height = static_cast<unsigned int>(height);
	dd_interface_implementation->mvpMatrix = projection_view_matrix;
	dd::flush();
}

// Called before quitting
bool ModuleDebugDraw::CleanUp()
{
	APP_LOG_INFO("Destroying Module Debug Draw");

    dd::shutdown();

    delete dd_interface_implementation;
    dd_interface_implementation = 0;

	delete grid;

	return true;
}
