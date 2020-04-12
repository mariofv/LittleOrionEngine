#include "ComponentButton.h"
#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/Gameobject.h"

#include "Module/ModuleScene.h"
#include "Brofiler/Brofiler.h"
ComponentButton::ComponentButton() : ComponentUI(ComponentUI::UIType::BUTTON)
{
}

ComponentButton::ComponentButton(GameObject * owner) : ComponentUI(owner, ComponentUI::UIType::BUTTON)
{
	if (owner->transform_2d.is_new)
	{
		owner->transform_2d.SetSize(170, 23);
		owner->transform_2d.SetPosition(&float3(0, 0, 1));
	}
}

ComponentButton::~ComponentButton()
{
	ComponentUI::~ComponentUI();
}

void ComponentButton::Render(float4x4* projection)
{
	BROFILER_CATEGORY("UI: Button Render", Profiler::Color::Bisque);
	owner->transform_2d.position.z = 1;
	
	ComponentUI* text = nullptr;
	for (const auto& child : owner->children)
	{
		text = static_cast<ComponentUI*>(child->GetComponentUI(ComponentUI::UIType::TEXT));
		if (text)
		{
			ComponentTransform2D* text_transform = &text->owner->transform_2d;
			owner->transform_2d.SetSize(text_transform->rect.Width(), text_transform->rect.Height());
			break;
		}
	}
	ComponentUI::Render(projection);
}

void ComponentButton::Delete()
{
	ComponentUI::Delete();
}


void ComponentButton::Save(Config& config) const
{
	ComponentUI::Save(config);
}

void ComponentButton::Load(const Config& config)
{
	ComponentUI::Load(config);
}

