#include "ComponentCanvas.h"

#include "Component/ComponentCanvasRenderer.h"
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
	std::vector<ComponentCanvasRenderer*> components_to_render = GetComponentCanvasRendererToRender();

	float canvas_width, canvas_height;

#if GAME
	canvas_width = App->window->GetWidth();
	window_height = App->window->GetHeight();
#else
	canvas_width = App->editor->scene_panel->scene_window_content_area_width;
	canvas_height = App->editor->scene_panel->scene_window_content_area_height;
#endif

	float4x4 projection = float4x4::D3DOrthoProjLH(-1, 200, canvas_width, canvas_height);
	owner->transform_2d.SetWidth(canvas_width);
	owner->transform_2d.SetHeight(canvas_height);

	glDisable(GL_DEPTH_TEST);
	
	for (auto& canvas_renderer : components_to_render)
	{
		canvas_renderer->Render();
	}

	glEnable(GL_DEPTH_TEST);
}

void ComponentCanvas::SpecializedSave(Config& config) const
{
}

void ComponentCanvas::SpecializedLoad(const Config& config)
{
}

std::vector<ComponentCanvasRenderer*> ComponentCanvas::GetComponentCanvasRendererToRender() const
{
	std::vector<ComponentCanvasRenderer*> components_to_render;
	std::queue<GameObject*> pending_game_objects;
	pending_game_objects.push(owner);
	while (!pending_game_objects.empty())
	{
		GameObject* current_game_object = pending_game_objects.front();
		pending_game_objects.pop();

		Component* component_canvas_renderer = current_game_object->GetComponent(Component::ComponentType::CANVAS_RENDERER);
		if (component_canvas_renderer)
		{
			components_to_render.push_back(static_cast<ComponentCanvasRenderer*>(component_canvas_renderer));
		}
		for (auto& child : current_game_object->children)
		{
			pending_game_objects.push(child);
		}
	}

	return components_to_render;
}