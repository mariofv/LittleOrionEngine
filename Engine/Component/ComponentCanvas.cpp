#include "ComponentCanvas.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleUI.h"

ComponentCanvas::ComponentCanvas() : Component(ComponentType::CANVAS)
{
	
}

ComponentCanvas::ComponentCanvas(GameObject* owner) : Component(owner, ComponentType::CANVAS)
{
	
}

ComponentCanvas& ComponentCanvas::operator=(const ComponentCanvas& component_to_copy)
{
	Component::operator = (component_to_copy);
	return *this;
}

Component* ComponentCanvas::Clone(bool original_prefab) const
{
	ComponentCanvas* created_component;
	if (original_prefab)
	{
		created_component = new ComponentCanvas();
	}
	else
	{
		created_component = App->ui->CreateComponentCanvas();
	}
	*created_component = *this;
	return created_component;
};

void ComponentCanvas::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentCanvas*>(component_to_copy) = *this;
}

void ComponentCanvas::Delete()
{
	App->ui->RemoveComponentCanvas(this);
}

void ComponentCanvas::Render(float4x4* projection) 
{
	/*
	owner->transform_2d.SetSize(App->ui->window_width, App->ui->window_height);
	ComponentUI::Render(projection);
	*/
}

void ComponentCanvas::SpecializedSave(Config& config) const
{
}

void ComponentCanvas::SpecializedLoad(const Config& config)
{
}

