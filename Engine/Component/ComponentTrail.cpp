#include "ComponentTrail.h"

#include "Main/Application.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleProgram.h"


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
}

void ComponentTrail::Init()
{
	trail_points.reserve(total_points + 2);

	billboard = new ComponentBillboard(owner);
	billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::CROSSED);

	for (unsigned int i = 0; i < total_points; ++i)
	{
		trail_points.emplace_back(TrailPoint());
		trail_points[i].position = { 0.0f, 0.0f, 0.0f };
		trail_points[i].width = 10.0F;
		trail_points[i].life = 0.5f;
		trail_points[i].time_passed = 0.0f;
	}
}

void ComponentTrail::UpdateTrail()
{

}

void ComponentTrail::Render()
{
	glDisable(GL_CULL_FACE);
	if (active)
	{
		time_counter += App->time->real_time_delta_time;

		for (unsigned int i = 0; i < total_points; ++i)
		{
			TrailPoint& p = trail_points[i];
			float3 outline_left, outline_right;

			if (p.life > 0.0f)
			{
				UpdateTrail();
				billboard->Render(p.position);
			
				trail_vertices = trail_points.size();
				if (p.is_rendered)
				{
					float width = p.width;
					outline_left = p.position + p.position_adjacent_point * width;
					outline_right = p.position - p.position_adjacent_point * width;
				}
				
			}
		}
	}
	glEnable(GL_CULL_FACE);
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