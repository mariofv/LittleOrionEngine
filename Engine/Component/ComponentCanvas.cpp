#include "ComponentCanvas.h"

#include "Component/ComponentButton.h"
#include "Component/ComponentCanvasRenderer.h"
#include "Component/ComponentSpriteMask.h"

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

Component* ComponentCanvas::Clone(GameObject* owner, bool original_prefab)
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
	CloneBase(static_cast<Component*>(created_component));	

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
	return created_component;
};

void ComponentCanvas::CopyTo(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentCanvas*>(component_to_copy) = *this;
}

void ComponentCanvas::Delete()
{
	App->ui->RemoveComponentCanvas(this);
}

void ComponentCanvas::Render(float width, float height, bool scene_mode)
{
	float2 last_canvas_screen_size = canvas_screen_size;

#if GAME
	canvas_screen_position = float2::zero;
	focused = App->window->IsFocused();
	
#else
	canvas_screen_position = App->editor->game_panel->game_window_content_area_pos;
	focused = App->editor->game_panel->IsFocused();
#endif

	canvas_screen_size.x = width;
	canvas_screen_size.y = height;

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

	RenderGameObject(owner, &projection_view);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void ComponentCanvas::SpecializedSave(Config& config) const
{
}

void ComponentCanvas::SpecializedLoad(const Config& config)
{
}

void ComponentCanvas::RenderGameObject(GameObject* game_object_to_render, float4x4* projection) const
{
	ComponentSpriteMask* sprite_mask = static_cast<ComponentSpriteMask*>(game_object_to_render->GetComponent(Component::ComponentType::UI_SPRITE_MASK));
	bool use_mask = sprite_mask != nullptr && sprite_mask->active;
	
	ComponentCanvasRenderer* component_canvas_renderer = static_cast<ComponentCanvasRenderer*>(game_object_to_render->GetComponent(Component::ComponentType::CANVAS_RENDERER));
	bool use_renderer = component_canvas_renderer != nullptr && component_canvas_renderer->active;

	if (use_renderer)
	{
		if (use_mask)
		{
			glEnable(GL_STENCIL_TEST);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.05f);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF); // each bit is written to the stencil buffer as is

			if (!sprite_mask->render_mask)
			{
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
				component_canvas_renderer->Render(projection);
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			}
			else
			{
				component_canvas_renderer->Render(projection);
			}


			if (sprite_mask->inverted_mask)
			{
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			}
			else
			{
				glStencilFunc(GL_EQUAL, 1, 0xFF);
			}
			glStencilMask(0x00); // disable writing to the stencil buffer
		}
		else
		{
			component_canvas_renderer->Render(projection);
		}
	}

	for (auto& child : game_object_to_render->children)
	{
		RenderGameObject(child, projection);
	}

	if (use_mask)
	{
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_STENCIL_TEST);
	}
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
			AABB2D button_global_rect = component_button->owner->transform_2d.GetGlobalRectAABB2D();

			float2 button_canvas_rect_min_point = button_global_rect.minPoint;
			float2 button_canvas_rect_max_point = button_global_rect.maxPoint;
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