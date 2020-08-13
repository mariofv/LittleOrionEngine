#include "ComponentCanvasRenderer.h"

#include "ComponentImage.h"
#include "ComponentVideoPlayer.h"
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

Component* ComponentCanvasRenderer::Clone(GameObject* owner, bool original_prefab)
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

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
	return created_component;
};

void ComponentCanvasRenderer::CopyTo(Component* component_to_copy) const
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
		component_image->Render(projection);
	}

	Component* component_text = owner->GetComponent(Component::ComponentType::UI_TEXT);
	if (component_text != nullptr)
	{
		static_cast<ComponentText*>(component_text)->Render(projection);
	}

	Component* component_video_player = owner->GetComponent(Component::ComponentType::VIDEO_PLAYER);
	if (component_video_player != nullptr)
	{
		static_cast<ComponentVideoPlayer*>(component_video_player)->Render(projection);
	}
}

void ComponentCanvasRenderer::SpecializedSave(Config& config) const
{
}

void ComponentCanvasRenderer::SpecializedLoad(const Config& config)
{
}

