#include "ComponentTrail.h"

#include "Main/Application.h"
#include "Module/ModuleEffects.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"
#include "GL/glew.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

namespace { const float MAX_TRAIL_VERTICES = 1000; } //arbitrary number 

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
		glDeleteBuffers(1, &trail_vao);
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
	bool on_transform_change = last_gameobject_position.Distance(gameobject_position) >= min_distance;

	if (on_transform_change)//always gets in this is wrong ! TODO create an event here/ merge with event system
	{
		TrailPoint next_point(gameobject_position, width, duration);
		test_points.push_back(next_point);	//create another Trail point and add it to the pool		
		last_point = next_point; // So we're gonna calculate, on the next iteration how far we are from the last point created, and so on
		last_gameobject_position = gameobject_position;
	}

	GetPerpendiculars();

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
			test_points.erase(it);
		}
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

	unsigned int j = 0;
	vertices.clear();
	float trail_segment_uv = 1.f / test_points.size(); // to coordinate texture
	auto pair = mesh_points.begin();
	while (pair != mesh_points.end())
	{
		if (pair->first->life > 0 && pair->second->life > 0)
		{
			//Get the vector that links every two points
			float3 vector_adjacent = (pair->second->position - pair->first->position).Normalized();//vector between each pair -> Normalized to get vector with magnitutde = 1 but same direction
			float3 perpendicular = vector_adjacent.Cross(owner->transform.GetFrontVector()) * width; //Front is currently local

			float3 top_left, bottom_left;
			++j;
			//TODO - Commented code is for rendering the last point of path
			/*if (++pair == mesh_points.end())
			{
				--pair;
				top_left = pair->second->position + perpendicular;
				bottom_left = (pair->second->position - perpendicular);
			}
			else
			{
				top_left = pair->first->position + perpendicular;
				bottom_left = (pair->first->position - perpendicular);
			}*/
			top_left = pair->first->position + perpendicular;
			bottom_left = (pair->first->position - perpendicular);
			vertices.push_back({ top_left, float2(trail_segment_uv * j, 1.0f) }); //uv[i]
			vertices.push_back({ bottom_left, float2(trail_segment_uv * j, 0.0f) });//uv[++i]
			++pair;
		}
		else
		{
			mesh_points.erase(pair);
		}
	}
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

		glUniform1f(glGetUniformLocation(shader_program, "emissive_intensity"), emissive_intensity);

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
	config.AddFloat(min_distance, "Min_Distance");
	config.AddUInt(texture_uuid, "TextureUUID");
	config.AddColor(color, "Color");
	config.AddBool(emissive, "Emissive");
	config.AddFloat(emissive_intensity, "Emissive Intensity");
}
void ComponentTrail::SpecializedLoad(const Config& config)
{
	width = config.GetFloat("Width", 1.0f);
	duration = config.GetFloat("Duration", 1000.0f);
	min_distance = config.GetFloat("Min_Distance", 1.0f);
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	texture_uuid = config.GetUInt("TextureUUID", 0);
	ChangeTexture(texture_uuid);
	config.GetColor("Color", color, float4(1.0f, 1.0f, 1.0f, 1.0f));
	emissive = config.GetBool("Emissive", false);
	emissive_intensity = config.GetFloat("Emissive Intensity", 1.f);
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