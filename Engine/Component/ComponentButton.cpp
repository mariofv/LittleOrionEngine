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
	if (owner->transform_2d.is_new)
	{
		owner->transform_2d.SetSize(170, 23);
	}
}

ComponentButton::~ComponentButton()
{
	ComponentUI::~ComponentUI();
}

void ComponentButton::Render(float4x4* projection)
{
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
	//Component* comp = owner->GetComponent(Component::ComponentType::UI);
	//ComponentTransform2D * transform_2d = &owner->transform_2d;
	//ComponentTransform2D * transform_2d_txt = nullptr;

	//if (comp != nullptr) {
	//	ComponentText* comp_txt = static_cast<ComponentText*>(comp);
	//	if (owner->children.size() > 0)
	//	{
	//		for (auto& child : owner->children)
	//		{
	//			if (child->name == "Text")
	//			{
	//				transform_2d_txt = &child->transform_2d;
	//				transform_2d->SetSize(owner->transform_2d.rect.Width() + transform_2d_txt->rect.Width(), owner->transform_2d.rect.Height() + transform_2d_txt->rect.Height());
	//			}
	//		}
	//	}
	//}

}