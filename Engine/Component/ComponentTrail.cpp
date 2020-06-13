#include "ComponentTrail.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"

#include "Component/ComponentBillboard.h"
#include "Module/ModuleResourceManager.h"
#include "GL/glew.h"

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
	std::queue<TrailPoint> empty;
	std::swap(trail_points, empty);
}

void ComponentTrail::Init()
{
	billboard = new ComponentBillboard(owner);
	billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::AXIAL);

	trail_points.emplace(float3{ 0.0f, 0.0f, 0.0f }, width, duration);//initialize 1st trail_point in queue
}

void ComponentTrail::UpdateTrail()
{
	float3 gameobject_position = owner->transform.GetGlobalTranslation();//current GO position
	for (unsigned int i = 0; i < total_points; ++i)
	{
		//update front of trail
		TrailPoint& p = trail_points.front();
		p.time_left -= App->time->real_time_delta_time; // reduce life of 1st point
		
		//size
		p.width = width; 
		billboard->width = p.width; 

		//if point's life is enough push it further
		if (p.time_left > 0)
		{
			trail_points.push(p);
		}
	}

	//update back of trail
	TrailPoint& p_last = trail_points.back();
	float distance_previous_point = p_last.position.Distance(gameobject_position);
	if (distance_previous_point > min_distance)// A new point is generated when distance from current tracking position and last generated point is at least this distance.
	{
		if (trail_points.size() == 0)
		{
			TrailPoint new_point(gameobject_position, width, duration);
			trail_points.push(new_point);
		}
		else
		{
			TrailPoint new_point(gameobject_position, trail_points.back().position, width, duration);
		}
	}
	//trail_points.pop(); //Older trail points should progressively disappear 
}

void ComponentTrail::Render()
{
	glDisable(GL_CULL_FACE);
	unsigned int destroy_point_vertice = 0;
	if (active)
	{
		time_counter += App->time->real_time_delta_time;
		for (unsigned int i = 0; i < total_points; ++i)
		{
			TrailPoint& p = trail_points.front();
			float3 outline_left, outline_right;

			if (p.life > 0.0f)
			{
				UpdateTrail();
			
				trail_vertices = trail_points.size();// create vertices for each trail point

				billboard->Render(owner->transform.GetGlobalTranslation() + p.position);
				if (p.is_rendered)
				{
					float width = p.width; //Distance between each pair of vertices generated for a given point.
					outline_left = p.position + p.position_adjacent_point * width;
					outline_right = p.position - p.position_adjacent_point * width;
				}
			}
			else
			{
				++destroy_point_vertice;
			}
		}
	}
	glEnable(GL_CULL_FACE);
}
void ComponentTrail::SetTrailTexture(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;
	billboard->ChangeTexture(texture_uuid);
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
		created_component = App->renderer->CreateComponentTrail();
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