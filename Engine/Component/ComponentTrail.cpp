#include "ComponentTrail.h"

#include "Main/Application.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleProgram.h"

#include "Component/ComponentBillboard.h"
#include "Module/ModuleResourceManager.h"
#include "GL/glew.h"

namespace { const int total_points = 2; }
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

}

void ComponentTrail::Update()
{

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

void ComponentTrail::Render()
{

}
void ComponentTrail::SpecializedSave(Config& config) const
{

}
void ComponentTrail::SpecializedLoad(const Config& config)
{

}