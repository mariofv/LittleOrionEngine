#include "ComponentCanvasRenderer.h"

#include "ComponentImage.h"
#include "ComponentSpriteMask.h"
#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleUI.h"

ComponentCanvasRenderer::ComponentCanvasRenderer() : Component(ComponentType::CANVAS_RENDERER)
{

}

ComponentCanvasRenderer::ComponentCanvasRenderer(GameObject* owner) : Component(owner, ComponentType::CANVAS_RENDERER)
{

}

ComponentCanvasRenderer& ComponentCanvasRenderer::operator=(const ComponentCanvasRenderer& component_to_copy)
{
	Component::operator = (component_to_copy);
	return *this;
}

Component* ComponentCanvasRenderer::Clone(bool original_prefab) const
{
	ComponentCanvasRenderer* created_component;
	if (original_prefab)
	{
		created_component = new ComponentCanvasRenderer();
	}
	else
	{
		created_component = App->ui->CreateComponentCanvasRenderer();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));
	return created_component;
};

void ComponentCanvasRenderer::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentCanvasRenderer*>(component_to_copy) = *this;
}

void ComponentCanvasRenderer::Delete()
{
	App->ui->RemoveComponentCanvasRenderer(this);
}

void ComponentCanvasRenderer::Render(float4x4* projection)
{
	ComponentImage* component_image = static_cast<ComponentImage*>(owner->GetComponent(Component::ComponentType::UI_IMAGE));
	if (component_image != nullptr)
	{
		ComponentSpriteMask* sprite_mask = static_cast<ComponentSpriteMask*>(owner->GetComponent(Component::ComponentType::UI_SPRITE_MASK));
		if (sprite_mask != nullptr)
		{
			glEnable(GL_STENCIL_TEST);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.05);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF); // each bit is written to the stencil buffer as is

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			sprite_mask->Render(projection);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			if (sprite_mask->inverted_mask)
			{
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			}
			else
			{
				glStencilFunc(GL_EQUAL, 1, 0xFF);
			}
			glStencilMask(0x00); // disable writing to the stencil buffer
			component_image->Render(projection);

			glDisable(GL_ALPHA_TEST);
			glDisable(GL_STENCIL_TEST);


			if (sprite_mask->render_mask)
			{
				sprite_mask->Render(projection);
			}
		}
		else
		{
			component_image->Render(projection);
		}
	}

	Component* component_text = owner->GetComponent(Component::ComponentType::UI_TEXT);
	if (component_text != nullptr)
	{
		static_cast<ComponentText*>(component_text)->Render(projection);
	}
}

void ComponentCanvasRenderer::SpecializedSave(Config& config) const
{
}

void ComponentCanvasRenderer::SpecializedLoad(const Config& config)
{
}

