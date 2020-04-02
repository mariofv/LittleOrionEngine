#include "ComponentButton.h"
#include "ComponentText.h"

#include "Main/Application.h"
#include "Main/Gameobject.h"

#include "Module/ModuleScene.h"

ComponentButton::ComponentButton() : ComponentUI(ComponentUI::UIType::BUTTON)
{
}

ComponentButton::ComponentButton(GameObject * owner) : ComponentUI(owner, ComponentUI::UIType::BUTTON)
{
	
}

ComponentButton::~ComponentButton()
{
	ComponentUI::~ComponentUI();
}

void ComponentButton::Render(float4x4* projection)
{
	owner->transform_2d.position.z = 1;
	if (owner->transform_2d.is_new)
	{
		if (owner->children.size() > 0)
		{
			for (auto& child : owner->children)
			{
				if (child->name == "Text")
				{
					owner->transform_2d.SetSize(child->transform_2d.rect.Width(), child->transform_2d.rect.Height());
					child->transform_2d.SetPosition(owner->transform_2d.position.x, owner->transform_2d.position.y);
				}
			}
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

void ComponentButton::OnButtonChange()
{
	
}