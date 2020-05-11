#include "ComponentCanvasRenderer.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleWindow.h"

#include <queue>


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

void ComponentCanvasRenderer::Render()
{
	
}

void ComponentCanvasRenderer::SpecializedSave(Config& config) const
{
}

void ComponentCanvasRenderer::SpecializedLoad(const Config& config)
{
}

