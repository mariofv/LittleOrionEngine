#include "ComponentImage.h"
#include "Main/GameObject.h"

ComponentImage::ComponentImage() : ComponentUI(ComponentType::UI_IMAGE)
{

}

ComponentImage::ComponentImage(GameObject * owner) : ComponentUI(owner, ComponentType::UI_IMAGE)
{
	if (owner->transform_2d.is_new)
	{
		owner->transform_2d.SetSize(100, 100);
	}
}

void ComponentImage::Render(float4x4* projection)
{
	ComponentUI::Render(projection);
}

void ComponentImage::Delete()
{
	ComponentUI::Delete();
}

void ComponentImage::UISpecializedSave(Config& config) const
{
}

void ComponentImage::UISpecializedLoad(const Config& config)
{
}