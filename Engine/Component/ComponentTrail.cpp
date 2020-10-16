#include "ComponentTrail.h"

#include "Main/Application.h"
#include "Module/ModuleEffects.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"
#include "GL/glew.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

namespace { const float MAX_TRAIL_VERTICES = 5000; } //arbitrary number

ComponentTrail::ComponentTrail() : Component(nullptr, ComponentType::TRAIL)
{
	Init();
}

ComponentTrail::ComponentTrail(GameObject * owner) : Component(owner, ComponentType::TRAIL)
{
	Init();
}
ComponentTrail::~ComponentTrail()
{
	CleanUp();
}

void ComponentTrail::CleanUp()
{
	vertices.clear();
	if (trail_vbo != 0)
	{
		glDeleteBuffers(1, &trail_vbo);
		glDeleteVertexArrays(1, &trail_vao);
	}
}

void ComponentTrail::Init()
{
	//InitRenderer
	ChangeTexture(static_cast<uint32_t>(CoreResource::BILLBOARD_DEFAULT_TEXTURE));

	InitBuffers();

	ClearTrail();
}

void ComponentTrail::InitBuffers()
{
	glGenVertexArrays(1, &trail_vao);
	glGenBuffers(1, &trail_vbo);

	glBindVertexArray(trail_vao);

	glBindBuffer(GL_ARRAY_BUFFER, trail_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_TRAIL_VERTICES * 4 * 5, nullptr, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ComponentTrail::Update()
{

	if (!active)
	{
		return;
	}
	float3 gameobject_position = owner->transform.GetGlobalTranslation(); //current GO position
	bool on_transform_change = last_gameobject_position.Distance(gameobject_position) > 0.0f;

	if (on_transform_change)//always gets in this is wrong
	{
		TrailPoint next_point(gameobject_position, width, duration);
		test_points.push_back(next_point);	//create another Trail point and add it to the pool
		last_point = next_point; // So we're gonna calculate, on the next iteration how far we are from the last point created, and so on
		last_gameobject_position = gameobject_position;
	}
	if (test_points.size() > 0)
	{
		GetPerpendiculars();
		GetCatmull();
		GetUVs();
	}

	auto it = test_points.begin();
	while (it != test_points.end())
	{
		if (it->life >= 0) // If life is positive, all good
		{
			it->is_rendered = true;
			it->life -= App->time->real_time_delta_time; // Update time left
			++it;
		}
		else // But if not, we delete these points
		{
			it = test_points.erase(it);
		}
	}
	if (!blend_colors)
	{
		color_to_blend = color;
	}
}

void  ComponentTrail::GetPerpendiculars()
{
	TrailPoint* previous_point = nullptr;
	TrailPoint* current_point = nullptr;
	mesh_points.clear();
	for (int i = 0; i < test_points.size(); ++i)
	{
		current_point = &test_points[i];

		if (previous_point != nullptr)
		{
			mesh_points.push_back(std::make_pair(previous_point, current_point));
		}
		previous_point = current_point;
	}

	auto pair = mesh_points.begin();
	path_top.spline_points.clear();
	path_bottom.spline_points.clear();
	float3 top_left, bottom_left;
	while (pair != mesh_points.end())
	{
		if (pair->first->life > 0 && pair->second->life > 0)
		{
			//Get the vector that links every two points
			float3 vector_adjacent = (pair->second->position - pair->first->position).Normalized();//vector between each pair -> Normalized to get vector with magnitutde = 1 but same direction
			float3 perpendicular;
			perpendicular = vector_adjacent.Cross((App->cameras->main_camera->camera_frustum.pos - owner->transform.GetGlobalTranslation()).Normalized()) * width; //Front is currently local
			if (++pair == mesh_points.end())
			{
				--pair;
				top_left = pair->first->position + perpendicular;
				bottom_left = (pair->first->position - perpendicular);
			}
			else
			{
				top_left = pair->second->position + perpendicular;
				bottom_left = (pair->second->position - perpendicular);
			}
			//Spline points
			path_top.spline_points.emplace_back(top_left);// add points on the spline
			path_bottom.spline_points.emplace_back(bottom_left);// add points on the spline

			++pair;
		}
		else
		{
			pair = mesh_points.erase(pair);
		}
	}
}
void ComponentTrail::GetCatmull()
{
	if ((path_top.spline_points.size() < 5) || (path_bottom.spline_points.size() < 5))
	{
		return;
	}
	CalculateCatmull(path_top, spline_top);
	CalculateCatmull(path_bottom, spline_bottom);
}

void ComponentTrail::CalculateCatmull(Spline& const path_to_smoothen, std::vector<float3>& spline_points)
{
	float3 curve_point;
	spline_points.clear();
	for (unsigned int i = 0; i < path_to_smoothen.spline_points.size() - 2; i++)
	{
		float3 curve_begin, curve_end, smoothen_weight_point_left, smoothen_weight_point_right;
		curve_begin = path_to_smoothen.spline_points[i];
		curve_end = path_to_smoothen.spline_points[i + 1];
		//If we are are 1st point then p0 = reflection of p2;
		if (i == 0)
		{
			//Implementing r = v - 2 * (v . n) * n reflection formula
			float3 normal_begin = curve_begin.Cross(curve_end).Normalized(); //Normal of p1
			smoothen_weight_point_left = curve_end - 2 * (curve_end.Dot(normal_begin) * normal_begin);//refrection of p2 to p1 for getting p0

		}
		else
		{
			smoothen_weight_point_left = path_to_smoothen.spline_points[i - 1];//else P0 is previous point from where we are
		}
		//If we are at the last point then p3 = reflection of p2
		if (i == path_to_smoothen.spline_points.size() - 1)
		{
			float3 normal_end = curve_end.Cross(curve_begin).Normalized(); //Normal of p2
			smoothen_weight_point_right =  curve_begin - 2 * (curve_begin.Dot(normal_end) * normal_end);//refrection of p1 to p2 for getting p3
		}
		else
		{
			smoothen_weight_point_right = path_to_smoothen.spline_points[i + 2]; //else P3 is 2 points after the one we are now
		}
		//Calculate r intermediate points
		for (int r = 0; r < points_in_curve; r++)
		{
			float t = r / static_cast<float>(points_in_curve);
			curve_point = path_to_smoothen.GetSplinePoint(t, smoothen_weight_point_left, curve_begin, curve_end, smoothen_weight_point_right);
			spline_points.push_back(curve_point);
		}
	}
}

void ComponentTrail::GetUVs()
{
	float trail_segment_uv = 1.f / spline_top.size(); // to coordinate texture
	float trail_segment_uv_x = 1.f / (spline_top.size() / columns);
	float trail_segment_uv_y = rows;
	vertices.clear();
	for (int l = 0; l < spline_top.size(); l++)
	{
		switch (texture_mode)
		{
		case ComponentTrail::TextureMode::STRETCH:
			vertices.push_back({ spline_top[l], float2(trail_segment_uv * l , 1.0f) });//uv[++i]
			vertices.push_back({ spline_bottom[l], float2(trail_segment_uv * l, 0.0f) });//uv[++i]
			break;
		case ComponentTrail::TextureMode::TILE:
			vertices.push_back({ spline_top[l], float2(trail_segment_uv_x * l , 1.0f * trail_segment_uv_y) });//uv[++i]
			vertices.push_back({ spline_bottom[l], float2(trail_segment_uv_x * l, 0.0f) });//uv[++i]
			break;
		case ComponentTrail::TextureMode::REPEAT_PER_SEGMENT:
			vertices.push_back({ spline_top[l], float2(l , 1.0f) });//uv[++i]
			vertices.push_back({ spline_bottom[l], float2(l, 0.0f) });//uv[++i]
			break;
		}
	}
	spline_top.clear();
	spline_bottom.clear();
}
void ComponentTrail::Render()
{
	if (active && trail_texture)
	{
		GLuint shader_program = App->program->UseProgram("Trail");
		glUseProgram(shader_program);

		glBindVertexArray(trail_vao);

		//use glBufferMap to obtain a pointer to buffer data
		if (vertices.size() != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, trail_vbo);
			trail_renderer_vertices = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(Vertex) *  vertices.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);// 6 indices
			memcpy(trail_renderer_vertices, vertices.data(), vertices.size() * sizeof(Vertex));
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, trail_texture->opengl_texture);
		glUniform1i(glGetUniformLocation(shader_program, "tex"), 0);
		glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), owner->transform.GetGlobalModelMatrix().Transposed().ptr());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glUniform4fv(glGetUniformLocation(shader_program, "color"), 1, color.ptr());
		glUniform4fv(glGetUniformLocation(shader_program, "color_blend"), 1, color_to_blend.ptr());
		glUniform1f(glGetUniformLocation(shader_program, "emissive_intensity"), emissive_intensity);
		glUniform1f(glGetUniformLocation(shader_program, "percentage"), blend_percentage);
		glUniform1f(glGetUniformLocation(shader_program, "smooth_step"), smoothening_step);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
		glBindVertexArray(0);

		glUseProgram(0);
	}
}

void ComponentTrail::ClearTrail()
{
	if (owner)
	{
		last_gameobject_position = owner->transform.GetGlobalTranslation(); //initial GO position
	}
	last_point = TrailPoint(last_gameobject_position, width, duration);
	test_points.clear();
}

void ComponentTrail::SetTrailTexture(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;
	ChangeTexture(texture_uuid);
}

void ComponentTrail::ChangeTexture(uint32_t texture_uuid)
{
	App->resources->loading_thread_communication.normal_loading_flag = true;
	if (texture_uuid != 0)
	{
		this->texture_uuid = texture_uuid;
		trail_texture = App->resources->Load<Texture>(texture_uuid);
	}
	App->resources->loading_thread_communication.normal_loading_flag = false;
}

Component* ComponentTrail::Clone(GameObject* owner, bool original_prefab)
{
	ComponentTrail* created_component;
	if (original_prefab)
	{
		created_component = new ComponentTrail();
	}
	else
	{
		created_component = App->effects->CreateComponentTrail(owner);
	}
	*created_component = *this;
	created_component->InitBuffers();
	CloneBase(static_cast<Component*>(created_component));

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
	return created_component;
};

void ComponentTrail::CopyTo(Component* component_to_copy) const
{
	*component_to_copy = *this;
	ComponentTrail* trail = static_cast<ComponentTrail*>(component_to_copy);
	trail->CleanUp();
	*trail = *this;
	trail->InitBuffers();
}

void ComponentTrail::Delete()
{
	App->effects->RemoveComponentTrail(this);
}


void ComponentTrail::SpecializedSave(Config& config) const
{
	config.AddFloat(width, "Width");
	config.AddFloat(duration, "Duration");
	config.AddUInt(texture_uuid, "TextureUUID");
	config.AddColor(color, "Color");
	config.AddFloat(emissive_intensity, "Emissive Intensity");
	config.AddUInt(points_in_curve, "Curve Points");
	config.AddInt(static_cast<int>(texture_mode), "Texture Mode");
	config.AddUInt(rows, "Rows");
	config.AddUInt(columns, "Columns");
	config.AddBool(blend_colors, "Color Blend");
	config.AddColor(color_to_blend, "Color To Blend");
	config.AddFloat(blend_percentage, "Portion Color 2");
	config.AddFloat(smoothening_step, "Smoothening Step");
}
void ComponentTrail::SpecializedLoad(const Config& config)
{
	width = config.GetFloat("Width", 1.0f);
	duration = config.GetFloat("Duration", 1000.0f);
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	texture_uuid = config.GetUInt("TextureUUID", 0);
	ChangeTexture(texture_uuid);
	config.GetColor("Color", color, float4(1.0f, 1.0f, 1.0f, 1.0f));
	emissive_intensity = config.GetFloat("Emissive Intensity", 1.f);
	points_in_curve = config.GetUInt("Curve Points", 5);
	texture_mode = static_cast<TextureMode>(config.GetInt("Texture Mode", static_cast<int>(TextureMode::STRETCH)));
	rows = config.GetUInt("Rows", 1);
	columns = config.GetUInt("Columns", 1);
	blend_colors = config.GetBool("Color Blend", false);
	config.GetColor("Color To Blend", color_to_blend, float4(1.0f, 1.0f, 1.0f, 1.0f));
	blend_percentage = config.GetFloat("Portion Color 2", 0.5f);
	smoothening_step = config.GetFloat("Smoothening Step", 0.3f);
}

void ComponentTrail::Disable()
{
	active = false;
}

void ComponentTrail::Enable()
{
	active = true;
	ClearTrail();
}
