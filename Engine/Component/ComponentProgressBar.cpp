#include "ComponentProgressBar.h"
#include "Main/GameObject.h"

ComponentProgressBar::ComponentProgressBar() : ComponentUI(ComponentUI::UIType::PROGRESSBAR)
{

}

ComponentProgressBar::ComponentProgressBar(GameObject * owner) : ComponentUI(owner, ComponentUI::UIType::PROGRESSBAR)
{

}

void ComponentProgressBar::Render(float4x4* projection)
{
	ComponentTransform2D * transform_2d = &owner->transform_2d;	
	// bar
	float4x4 bar_model = float4x4(owner->transform_2d.scale_matrix);
	bar_model = float4x4::Scale(float3(transform_2d->rect.Width() * (percentage / 100), transform_2d->rect.Height(), 0), float3::zero);
	bar_model = owner->transform.GetGlobalModelMatrix() * bar_model;
	ComponentUI::Render(projection, &bar_model, bar_texture, &bar_color);
	// background
	ComponentUI::Render(projection);
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