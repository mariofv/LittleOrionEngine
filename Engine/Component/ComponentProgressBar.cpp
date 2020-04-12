#include "ComponentProgressBar.h"
#include "Main/GameObject.h"

ComponentProgressBar::ComponentProgressBar() : ComponentUI(ComponentUI::UIType::PROGRESSBAR)
{
	
}

ComponentProgressBar::ComponentProgressBar(GameObject * owner) : ComponentUI(owner, ComponentUI::UIType::PROGRESSBAR)
{
	if (owner->transform_2d.is_new)
	{
		owner->transform_2d.SetSize(120, 10);
	}
}

ComponentProgressBar::~ComponentProgressBar()
{
	ComponentUI::~ComponentUI();
}

void ComponentProgressBar::Render(float4x4* projection)
{
	ComponentTransform2D* transform_2d = &owner->transform_2d;	
	// bar
	float4x4 bar_model;
	transform_2d->CalculateRectMatrix(transform_2d->rect.Width() * (percentage / 100), transform_2d->rect.Height(), bar_model);
	// background
	ComponentUI::Render(projection);
	ComponentUI::Render(projection, &bar_model, bar_texture, &bar_color);
}

void ComponentProgressBar::Delete()
{
	ComponentUI::Delete();
}

void ComponentProgressBar::Save(Config& config) const
{
	ComponentUI::Save(config);
	config.AddFloat(percentage, "Percentage");
	config.AddUInt(bar_texture, "BarTexture");
	config.AddFloat3(bar_color, "BarColor");
}

void ComponentProgressBar::Load(const Config& config)
{
	ComponentUI::Load(config);
	percentage = config.GetFloat("Percentage", 0.0F);
	bar_texture = config.GetUInt("BarTexture", 0);
	config.GetFloat3("BarColor", bar_color, float3::one);
}