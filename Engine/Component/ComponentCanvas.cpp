#include "ComponentCanvas.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleUI.h"

ComponentCanvas::ComponentCanvas() : ComponentUI(ComponentUI::UIType::CANVAS)
{
	
}

ComponentCanvas::ComponentCanvas(GameObject* owner) : ComponentUI(owner, ComponentUI::UIType::CANVAS)
{
	
}

ComponentCanvas::~ComponentCanvas()
{
	ComponentUI::~ComponentUI();
}

void ComponentCanvas::Render(float4x4* projection) 
{
	owner->transform_2d.SetSize(App->ui->window_width, App->ui->window_height);
	ComponentUI::Render(projection);
}

void ComponentCanvas::Delete()
{
	ComponentUI::Delete();
}

void ComponentCanvas::Save(Config& config) const
{
	ComponentUI::Save(config);
}

void ComponentCanvas::Load(const Config& config)
{
	ComponentUI::Load(config);
}

