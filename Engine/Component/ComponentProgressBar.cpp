#include "ComponentProgressBar.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleUI.h"

ComponentProgressBar::ComponentProgressBar() : Component(ComponentType::UI_PROGRESS_BAR)
{
	
}

ComponentProgressBar::ComponentProgressBar(GameObject * owner) : Component(owner, ComponentType::UI_PROGRESS_BAR)
{
}

Component* ComponentProgressBar::Clone(bool original_prefab) const
{
	ComponentProgressBar * created_component;
	if (original_prefab)
	{
		created_component = new ComponentProgressBar();
	}
	else
	{
		created_component = App->ui->CreateComponentUI<ComponentProgressBar>();
	}
	*created_component = *this;
	return created_component;
};

void ComponentProgressBar::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentProgressBar*>(component_to_copy) = *this;
}


void ComponentProgressBar::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentProgressBar::SpecializedSave(Config& config) const
{
	config.AddFloat(percentage, "Percentage");
	config.AddUInt(bar_texture, "BarTexture");
	config.AddFloat3(bar_color, "BarColor");
}

void ComponentProgressBar::SpecializedLoad(const Config& config)
{
	percentage = config.GetFloat("Percentage", 0.0F);
	bar_texture = config.GetUInt("BarTexture", 0);
	config.GetFloat3("BarColor", bar_color, float3::one);
}