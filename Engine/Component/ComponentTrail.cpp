#include "ComponentTrail.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleDebugDraw.h"

#include "Component/ComponentTrailRenderer.h"
#include "Module/ModuleResourceManager.h"
#include "GL/glew.h"

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
	gameobject_init_position = owner->transform.GetGlobalTranslation();//initial GO position
}

void ComponentTrail::UpdateTrail()
{
	float3 gameobject_position = owner->transform.GetGlobalTranslation();//current GO position
	if (gameobject_init_position.x != gameobject_position.x || gameobject_init_position.y != gameobject_position.y || gameobject_init_position.z != gameobject_position.z)//if user moves Trail GO
	{
		on_transform_change = true;
	}

	total_points = trail_points.size();
	for (unsigned int i = 0; i < total_points; ++i)
	{
		if (on_transform_change)
		{
			//update front of trail
			TrailPoint& point = trail_points.front();
			RespawnTrailPoint(point);
			
		}
	}
	//update back of trail
	TrailPoint& last_point = trail_points.back();
	float distance_previous_point = last_point.position.Distance(gameobject_position);
	
	if (distance_previous_point > min_distance)// A new point is generated when distance from current tracking position and last generated point is at least this distance.
	{
		if (trail_points.size() == 0)//to define path we need k + 2 points
		{
			TrailPoint new_point(gameobject_position, width, duration*10000);
			trail_points.push(new_point);
		}
		else
		{
			TrailPoint new_point(static_cast<ComponentTransform>(owner->transform), trail_points.back().position, width, duration*10000);
			new_point.is_rendered = true;
		}
	}
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
		for (unsigned int i = 0; i < total_points; ++i)
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
				}
			}
			else
			{
				++destroy_point_vertice;
			}
		}
		trail_renderer->rendered_vertices = trail_vertices;
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