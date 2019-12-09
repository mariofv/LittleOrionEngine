#include "ModuleDebug.h"
#include "Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "OLQuadTree.h"
#include "OLQuadTreeNode.h"

#include <GL/glew.h>
#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

// Called before render is available
bool ModuleDebug::Init()
{
	APP_LOG_SECTION("************ Module Debug Init ************");

	geometry_renderer = new GeometryRenderer();
	
	APP_LOG_SUCCESS("Module Debug initialized correctly.")

	return true;
}

update_status ModuleDebug::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleDebug::PostUpdate()
{
	
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleDebug::CleanUp()
{
	APP_LOG_INFO("Destroying Debug");
	
	delete geometry_renderer;
	
	return true;
}

void ModuleDebug::Render(const ComponentCamera &camera) const
{
	if (show_grid)
	{
		RenderGrid(camera);
	}
	if (show_camera_frustum)
	{
		geometry_renderer->RenderHexahedron(camera, App->cameras->active_camera->GetFrustumVertices());
	}
	if (show_quadtree)
	{
		for (auto& ol_quadtree_node : App->renderer->ol_quadtree.flattened_tree) {
			geometry_renderer->RenderSquare(camera, ol_quadtree_node->GetVertices());
		}
	}
}

void ModuleDebug::RenderGrid(const ComponentCamera &camera) const
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


void ModuleDebug::ShowDebugWindow()
{
	if (ImGui::Begin(ICON_FA_BUG " Debug"))
	{
		ImGui::Checkbox("Grid", &show_grid);
		ImGui::Checkbox("Camera Frustum", &show_camera_frustum);
		ImGui::Checkbox("QuadTree", &show_quadtree);
	}
	ImGui::End();
}