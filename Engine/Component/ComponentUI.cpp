#include "ComponentUI.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleUI.h"

ComponentUI::ComponentUI(ComponentType ui_type) : Component(nullptr, ui_type)
{
}

ComponentUI::ComponentUI(GameObject* owner, ComponentType ui_type) : Component(owner, ui_type)
{
}

ComponentUI::~ComponentUI()
{
}

void ComponentUI::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentUI::SpecializedSave(Config& config) const
{
	config.AddInt(layer, "Layer");
	UISpecializedSave(config);
}

void ComponentUI::SpecializedLoad(const Config& config)
{
	layer = config.GetInt("Layer", 0);
	UISpecializedLoad(config);
}

void ComponentUI::Enable()
{
	active = true;
	App->ui->SortComponentsUI();
}

void ComponentUI::Disable()
{
	active = false;
	App->ui->SortComponentsUI();
}