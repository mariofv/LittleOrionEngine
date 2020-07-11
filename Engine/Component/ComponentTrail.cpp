#include "ComponentTrail.h"
#include "Component/ComponentTrailRenderer.h"

#include "Main/Application.h"
#include "Module/ModuleEffects.h"
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
	App->effects->RemoveComponentTrail(this);
}

void ComponentTrail::Init()
{
	trail_renderer = new ComponentTrailRenderer(owner);
	gameobject_init_position = owner->transform.GetGlobalTranslation(); //initial GO position
	last_point = TrailPoint(gameobject_init_position, width, duration);
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

	std::vector<Vertex> vertices;
	unsigned int i = 0;
	mesh_index = 1 / (float)test_points.size(); // to coordinate texture
	for (auto pair = mesh_points.begin(); pair < mesh_points.end(); ++pair)
	{
		++i;
		if (pair->first->life > 0 && pair->second->life > 0)
		{
			//Get the vector that links every two points
			float3 vector_adjacent = (pair->second->position - pair->first->position).Normalized();//vector between each pair -> Normalized to get vector with magnitutde = 1 but same direction
			float3 perpendicular = vector_adjacent.Cross(owner->transform.GetFrontVector()) * width; //Front is currently local

			float3 top_left, top_right, bottom_left, bottom_right;
			
			top_left = pair->first->position + perpendicular;
			bottom_left = (pair->first->position - perpendicular);

			vertices.push_back({ top_left, float2(0.0f, 0.0f) });
			vertices.push_back({ bottom_left, float2(0.0f, 0.0f) });
			
			/*trail_renderer->tex_uv.x = mesh_index * i;
			trail_renderer->tex_uv.y = 0.0f;*/
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
		created_component = App->effects->CreateComponentTrail(owner);
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
	App->effects->RemoveComponentTrail(this);
}


void ComponentTrail::SpecializedSave(Config& config) const
{

}
void ComponentTrail::SpecializedLoad(const Config& config)
{

}