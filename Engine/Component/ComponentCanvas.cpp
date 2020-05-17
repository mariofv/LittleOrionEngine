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

	float2 last_canvas_screen_size = canvas_screen_size;

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

	if (last_canvas_screen_size.x != canvas_screen_size.x)
	{
		owner->transform_2d.SetWidth(canvas_screen_size.x);
	}
	if (last_canvas_screen_size.y != canvas_screen_size.y)
	{
		owner->transform_2d.SetHeight(canvas_screen_size.y);
	}

	glDisable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	for (auto& canvas_renderer : components_to_render)
	{
		canvas_renderer->Render(&projection_view);
	}
	glDisable(GL_BLEND);

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
	std::queue<GameObject*> pending_game_objects;
	pending_game_objects.push(owner);
	while (!pending_game_objects.empty())
	{
		GameObject* current_game_object = pending_game_objects.front();
		pending_game_objects.pop();

		Component* component_button = current_game_object->GetComponent(Component::ComponentType::UI_BUTTON);
		if (component_button && component_button->active)
		{
			float2 canvas_translation = App->ui->main_canvas->owner->transform_2d.GetGlobalTranslation().xy();
			AABB2D button_global_rect = component_button->owner->transform_2d.GetGlobalRectAABB2D();

			float2 button_canvas_rect_min_point = button_global_rect.minPoint - canvas_translation;
			float2 button_canvas_rect_max_point = button_global_rect.maxPoint - canvas_translation;
			AABB2D button_canvas_rect = AABB2D(button_canvas_rect_min_point, button_canvas_rect_max_point);

			if (button_canvas_rect.Contains(AABB2D(mouse_position, mouse_position)))
			{
				return static_cast<ComponentButton*>(component_button);
			}
		}

		for (auto& child : current_game_object->children)
		{
			pending_game_objects.push(child);
		}
	}

	return nullptr;
}