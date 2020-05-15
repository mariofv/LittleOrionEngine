#include "ComponentButton.h"
#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/Gameobject.h"

#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"

ComponentButton::ComponentButton() : Component(ComponentType::UI_BUTTON)
{
}

ComponentButton::ComponentButton(GameObject * owner) : Component(owner, ComponentType::UI_BUTTON)
{
}

Component* ComponentButton::Clone(bool original_prefab) const
{
	ComponentButton * created_component;
	if (original_prefab)
	{
		created_component = new ComponentButton();
	}
	else
	{
		created_component = App->ui->CreateComponentUI<ComponentButton>();
	}
	*created_component = *this;
	return created_component;
};

void ComponentButton::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentButton*>(component_to_copy) = *this;
}


void ComponentButton::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentButton::SpecializedSave(Config& config) const
{
}

void ComponentButton::SpecializedLoad(const Config& config)
{
}

void ComponentButton::SetClicked(bool clicked)
{
	this->clicked = clicked;
}

bool ComponentButton::IsClicked() const
{
	return clicked;
}