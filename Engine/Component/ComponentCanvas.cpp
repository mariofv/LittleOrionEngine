#include "ComponentCanvas.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleUI.h"

ComponentCanvas::ComponentCanvas() : ComponentUI(ComponentType::CANVAS)
{
	
}

ComponentCanvas::ComponentCanvas(GameObject* owner) : ComponentUI(owner, ComponentType::CANVAS)
{
	
}

void ComponentCanvas::Render(float4x4* projection) 
{
	/*
	owner->transform_2d.SetSize(App->ui->window_width, App->ui->window_height);
	ComponentUI::Render(projection);
	*/
}

void ComponentCanvas::Delete()
{
	ComponentUI::Delete();
}

void ComponentCanvas::UISpecializedSave(Config& config) const
{
}

void ComponentCanvas::UISpecializedLoad(const Config& config)
{
}

