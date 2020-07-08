#include "ComponentTrail.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleDebugDraw.h"

#include "Component/ComponentTrailRenderer.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleDebugDraw.h"
#include "GL/glew.h"

#include <stdio.h> 
#include <string.h> 
#include <list>
#include <algorithm>

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
	std::queue<TrailPoint> empty;
	std::swap(trail_points, empty);
}

void ComponentTrail::Init()
{
	trail_renderer = new ComponentTrailRenderer(owner);
	trail_points.emplace(float3{ 0.0f, 0.0f, 0.0f }, width, duration*10000);//initialize 1st trail_point in queue
	gameobject_init_position = owner->transform.GetGlobalTranslation(); //initial GO position
	
	last_point = TrailPoint(gameobject_init_position, width, duration);
	//last_point_added = gameobject_init_position;
}

void ComponentTrail::UpdateTrail()
{
	float3 gameobject_position = owner->transform.GetGlobalTranslation(); //current GO position

	if (gameobject_init_position.x != gameobject_position.x || gameobject_init_position.y != gameobject_position.y || gameobject_init_position.z != gameobject_position.z)//if user moves Trail GO
		on_transform_change = true;
	
	if (on_transform_change)//always gets in this is wrong ! TODO create an event here/ merge with event system
	{
		head_point = TrailPoint(gameobject_position, width, duration); //If there is movement, update the head point on the GO's position
		head_point.life = duration; // If the GO moves, we reset the life of the head point
		
		if (head_point.position.Distance(last_point.position) >= min_distance) //If the head point is at a greater distance than the distance we specified...
		{
			TrailPoint next_point(head_point.position, width, duration);
			test_points.push_back(next_point);	//create another Trail point and add it to the pool		
			last_point = next_point; // So we're gonna calculate, on the next iteration how far we are from the last point created, and so on
		}
	}

	GetPerpendiculars();


	for (auto it = test_points.begin(); it < test_points.end(); ++it)
	{
		if (it->life >= 0) // If life is positive, all good
		{
			it->is_rendered = true;
			it->life -= App->time->real_time_delta_time; // Update time left
		}

		else // But if not, we delete these points
		{
			it = test_points.erase(it);
		}
	}
}

void  ComponentTrail::GetPerpendiculars()
{
	//											p + width		 p			p - width
	// Calculate the points defined by the width	·  --------- · ---------  ·
	//												<-- width --->
	unsigned int destroy_point_vertice = 0;
	TrailPoint* previous_point = nullptr;
	TrailPoint* current_point = nullptr;
	mesh_points.clear();
	float mesh_index = 0.0f;
	for (int i = 0; i < test_points.size(); ++i)
	{
		current_point = &test_points[i];
		if (previous_point != nullptr)
		{
			mesh_points.push_back(std::make_pair(previous_point, current_point));
		}
		previous_point = current_point;

	}

	std::vector<float> vertices;
	int max_size = 20 * sizeof(float);
	int size = 0;
	float* vetrices_to_render = new float[max_size];
	float* cursor = vetrices_to_render;
	unsigned int bytes1 =  3 * sizeof(float);
	unsigned int bytes2 =  2;
	int i = 0;
	for (auto pair = mesh_points.begin(); pair < mesh_points.end(); ++pair)
	{
		++i; //for uvs if needed

		if (pair->first->life > 0 && pair->second->life > 0)
		{
			//Get the vector that links every two points
			float3 vector_adjacent = (pair->second->position - pair->first->position).Normalized();//vector between each pair -> Normalized to get vector with magnitutde = 1 but same direction
			float3 perpendicular = vector_adjacent.Cross(owner->transform.GetFrontVector()) * width; //Front is currently local

			float3 top_left, top_right, bottom_left, bottom_right;
			
			top_left = pair->first->position + perpendicular;
			top_right = (pair->second->position + perpendicular);
			bottom_right = (pair->second->position - perpendicular);
			bottom_left = (pair->first->position - perpendicular);


			vertices.push_back(top_left.x);
			vertices.push_back(top_left.y);
			vertices.push_back(0.0f);
			vertices.push_back(bottom_left.x);
			vertices.push_back(bottom_left.y);
			vertices.push_back(0.0f);

			//App->debug_draw->RenderLine(top_left, bottom_left); // Draw the perpendicular vector
			//App->debug_draw->RenderLine(top_right, bottom_right); // Draw the perpendicular vector
			//App->debug_draw->RenderLine(top_left, top_right); // Draw the perpendicular vector
			//App->debug_draw->RenderLine(bottom_left, bottom_right); // Draw the perpendicular vectormesh_points
			//App->debug_draw->RenderLine(top_left, bottom_right); // Draw the perpendicular vectormesh_points
			
		}
		else
		{
			mesh_points.erase(pair);
		}
	}
	trail_renderer->rendered_vertices = vertices.size();
	trail_renderer->Render(vertices);
	trail_renderer->owner = owner;
}

void  ComponentTrail::RespawnTrailPoint(TrailPoint& point)
{
	point.time_left -= App->time->real_time_delta_time; //reduce life of 1st point
	
	//size
	point.width = width;
	trail_renderer->width = point.width;
	
	//if point's life is enough push it further
	if (point.time_left > 0)
	{
		trail_points.push(point);
		point.is_rendered = true;
	}

	
	//trail_points.pop();//Older trail points should progressively disappear 
}

void ComponentTrail::Render()
{
	glDisable(GL_CULL_FACE);
	unsigned int destroy_point_vertice = 0;
	if (active)
	{
											

		UpdateTrail(); // Do this last 

		/*for (unsigned int i = 0; i < total_points; ++i)
		{
			TrailPoint& point = trail_points.front();
			float3 outline_left, outline_right;

			
			if (point.life > 0.0f)
			{
				UpdateTrail();

				trail_vertices = trail_points.size();// create vertices for each trail point

				if (point.is_rendered)
				{
					float width = point.width; //Distance between each pair of vertices generated for a given point.
					outline_left = point.position + point.position_perpendicular_point * width;
					outline_right = point.position - point.position_perpendicular_point * width;
					trail_renderer->Render(owner->transform.GetGlobalTranslation() + outline_left);//Move perpendicular vector over points and multiply by desired width
					trail_renderer->Render(owner->transform.GetGlobalTranslation() + outline_left);
					trail_renderer->Render(owner->transform.GetGlobalTranslation() + point.position);
					App->debug_draw->RenderPoint(outline_left);
					App->debug_draw->RenderPoint(outline_right);
				}
			}
			else
			{
				++destroy_point_vertice;
			}
		}
		trail_renderer->rendered_vertices = trail_vertices;
			
		}*/
		
	}
	glEnable(GL_CULL_FACE);
}
void ComponentTrail::SetTrailTexture(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;
	trail_renderer->ChangeTexture(texture_uuid);
}


ComponentTrail& ComponentTrail::operator=(const ComponentTrail& component_to_copy)
{
	Component::operator = (component_to_copy);
	return *this;
}

Component* ComponentTrail::Clone(bool original_prefab) const
{
	ComponentTrail* created_component;
	if (original_prefab)
	{
		created_component = new ComponentTrail();
	}
	else
	{
		created_component = App->renderer->CreateComponentTrail(owner);
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));
	return created_component;
};

void ComponentTrail::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentTrail*>(component_to_copy) = *this;
}

void ComponentTrail::Delete()
{
	App->renderer->RemoveComponentTrail(this);
}


void ComponentTrail::SpecializedSave(Config& config) const
{

}
void ComponentTrail::SpecializedLoad(const Config& config)
{

}