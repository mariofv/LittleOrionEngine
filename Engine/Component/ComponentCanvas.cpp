#include "ComponentCanvas.h"

#include "Component/ComponentButton.h"
#include "Component/ComponentCanvasRenderer.h"

#include "EditorUI/Panel/PanelGame.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleWindow.h"

#include <queue>

#include "Component/ComponentImage.h"

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

void ComponentCanvas::Render(bool scene_mode)
{
	std::vector<ComponentCanvasRenderer*> components_to_render = GetComponentCanvasRendererToRender();

#if GAME
	canvas_screen_size.x = App->window->GetWidth();
	canvas_screen_size.y = App->window->GetHeight();

	canvas_screen_position = float2::zero;
	focused = App->window->IsFocused();
	
#else
	canvas_screen_size.x = App->editor->game_panel->game_window_content_area_width;
	canvas_screen_size.y = App->editor->game_panel->game_window_content_area_height;

	canvas_screen_position = App->editor->game_panel->game_window_content_area_pos;
	focused = App->editor->game_panel->IsFocused();
#endif

	float4x4 projection_view;
	if (scene_mode)
	{
		owner->transform_2d.SetTranslation(float3(canvas_screen_size / 2.f, 0.f));
		projection_view = App->cameras->scene_camera->GetProjectionMatrix() * App->cameras->scene_camera->GetViewMatrix();
	}
	else
	{
		owner->transform_2d.SetTranslation(float3::zero);
		projection_view = float4x4::D3DOrthoProjLH(-1, 200, canvas_screen_size.x, canvas_screen_size.y);
	}
	owner->transform_2d.SetWidth(canvas_screen_size.x);
	owner->transform_2d.SetHeight(canvas_screen_size.y);

	glDisable(GL_DEPTH_TEST);
	
	for (auto& canvas_renderer : components_to_render)
	{
		canvas_renderer->Render(&projection_view);
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
		if (component_canvas_renderer && component_canvas_renderer->active)
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

float2 ComponentCanvas::GetCanvasScreenPosition() const
{
	return canvas_screen_position;
}

float2 ComponentCanvas::GetCanvasScreenSize() const
{
	return canvas_screen_size;
}

bool ComponentCanvas::IsFocused() const
{
	return focused;
}

ComponentButton* ComponentCanvas::GetUIElementAtPosition(float2 mouse_position)
{
	GameObject* dummy = App->scene->CreateGameObject();
	dummy->name = "Dummy";
	
	ComponentImage* created_component = static_cast<ComponentImage*>(dummy->CreateComponent(Component::ComponentType::UI_IMAGE));
	created_component->SetTextureToRender(231082784);

	dummy->transform_2d.SetTranslation(float3(mouse_position, 0.f));
	owner->AddChild(dummy);

	return nullptr;
}