#include "ComponentEventSystem.h"

#include "ComponentCanvas.h"
#include "ComponentButton.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleWindow.h"

#include <queue>


ComponentEventSystem::ComponentEventSystem() : Component(ComponentType::EVENT_SYSTEM)
{
	
}

ComponentEventSystem::ComponentEventSystem(GameObject* owner) : Component(owner, ComponentType::EVENT_SYSTEM)
{
	
}

ComponentEventSystem& ComponentEventSystem::operator=(const ComponentEventSystem& component_to_copy)
{
	Component::operator = (component_to_copy);
	return *this;
}

Component* ComponentEventSystem::Clone(GameObject* owner, bool original_prefab)
{
	ComponentEventSystem* created_component;
	if (original_prefab)
	{
		created_component = new ComponentEventSystem();
	}
	else
	{
		created_component = App->ui->CreateComponentEventSystem();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
	return created_component;
};

void ComponentEventSystem::CopyTo(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentEventSystem*>(component_to_copy) = *this;
}

void ComponentEventSystem::Delete()
{
	App->ui->RemoveComponentEventSystem(this);
}


void ComponentEventSystem::SpecializedSave(Config& config) const
{
}

void ComponentEventSystem::SpecializedLoad(const Config& config)
{
}

void ComponentEventSystem::Update()
{
	if (App->input->GetMouseButtonDown(MouseButton::Left) && App->ui->main_canvas != nullptr && App->ui->main_canvas->IsFocused())
	{
		MousePicking(App->input->GetMousePosition());
	}
}

void ComponentEventSystem::MousePicking(float2 mouse_position)
{
	ComponentCanvas* main_canvas = App->ui->main_canvas;
	if (main_canvas == nullptr)
	{
		return;
	}

	float2 canvas_size = main_canvas->GetCanvasScreenSize();
	float2 canvas_position = main_canvas->GetCanvasScreenPosition();

	float2 canvas_center_pos = canvas_position + canvas_size / 2;
	float2 canvas_mouse_position = mouse_position - canvas_center_pos;
	canvas_mouse_position.y = -canvas_mouse_position.y;
	
	ComponentButton* intersected_button = main_canvas->GetUIElementAtPosition(canvas_mouse_position);
	if (intersected_button != nullptr)
	{
		intersected_button->SetClicked(true);
	}
}