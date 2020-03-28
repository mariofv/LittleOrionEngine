#include "ComponentCanvas.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleUI.h"

ComponentCanvas::ComponentCanvas() : ComponentUI()
{
	
}

ComponentCanvas::ComponentCanvas(GameObject * owner) : ComponentUI(owner)
{
	
}


void ComponentCanvas::Render(float4x4* projection) 
{
	ComponentUI::Render(projection);
	owner->transform_2d.position.x = -App->ui->window_width / 2;
	owner->transform_2d.position.y = App->ui->window_height / 2;
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

