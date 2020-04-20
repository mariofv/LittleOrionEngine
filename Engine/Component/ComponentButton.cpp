#include "ComponentButton.h"
#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/Gameobject.h"

#include "Module/ModuleScene.h"

ComponentButton::ComponentButton() : ComponentUI(ComponentType::UI_BUTTON)
{
}

ComponentButton::ComponentButton(GameObject * owner) : ComponentUI(owner, ComponentType::UI_BUTTON)
{
}

void ComponentButton::Render(float4x4* projection)
{
	/*
	owner->transform_2d.position.z = 1;
	
	ComponentUI* text = nullptr;
	for (const auto& child : owner->children)
	{
		text = static_cast<ComponentUI*>(child->GetComponent(ComponentType::UI_TEXT));
		if (text)
		{
			ComponentTransform2D* text_transform = &text->owner->transform_2d;
			owner->transform_2d.SetSize(text_transform->rect.Width(), text_transform->rect.Height());
			break;
		}
	}
	ComponentUI::Render(projection);
	*/
}

void ComponentButton::Delete()
{
	ComponentUI::Delete();
}


void ComponentButton::UISpecializedSave(Config& config) const
{
}

void ComponentButton::UISpecializedLoad(const Config& config)
{
}

