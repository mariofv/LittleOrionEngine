#include "ComponentTransform2D.h"



ComponentTransform2D::ComponentTransform2D()
{
	position = float2::zero;
	size = float2(100, 100);
}


ComponentTransform2D::~ComponentTransform2D()
{}

void ComponentTransform2D::Delete()
{
	delete(&position);
	delete(&size);
}

Component* ComponentTransform2D::Clone(bool create_on_module = true) const
{
	ComponentTransform2D* clone = new ComponentTransform2D();
	
	clone->position.Set(position.x, position.y);
	clone->size.Set(size.x, size.y);

	return clone;
}

void ComponentTransform2D::Copy(Component * component_to_copy) const
{
	((ComponentTransform2D*)component_to_copy)->position.Set(position.x, position.y);
	((ComponentTransform2D*)component_to_copy)->size.Set(size.x, size.y);
}

void ComponentTransform2D::Save(Config & config) const
{
}

void ComponentTransform2D::Load(const Config & config)
{
}
