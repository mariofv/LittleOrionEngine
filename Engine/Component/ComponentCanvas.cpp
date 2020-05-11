#include "ComponentCanvas.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleWindow.h"

#include <queue>


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

void ComponentCanvas::Render() 
{
	float window_width, window_height;

#if GAME
	window_width = App->window->GetWidth();
	window_height = App->window->GetHeight();
#else
	window_width = App->editor->scene_panel->scene_window_content_area_width;
	window_height = App->editor->scene_panel->scene_window_content_area_height;
#endif

	float4x4 projection = float4x4::D3DOrthoProjLH(-1, 200, window_width, window_height);

	glDisable(GL_DEPTH_TEST);
	
	std::vector<ComponentUI*> components_to_render;
	std::queue<GameObject*> pending_game_objects;
	pending_game_objects.push(owner);
	while (!pending_game_objects.empty())
	{
		GameObject* current_game_object = pending_game_objects.front();
		//if (current_game_object->GetC(Component::ComponentType::CANVAS_RENDERER))

	}
	
	glEnable(GL_DEPTH_TEST);
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

