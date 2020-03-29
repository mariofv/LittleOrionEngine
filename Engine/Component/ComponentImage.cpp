#include "ComponentImage.h"
#include "Main/GameObject.h"

ComponentImage::ComponentImage() : ComponentUI(ComponentUI::UIType::IMAGE)
{

}

ComponentImage::ComponentImage(GameObject * owner) : ComponentUI(owner, ComponentUI::UIType::IMAGE)
{
	owner->transform_2d.SetSize(100, 100);
}

void ComponentImage::Render(float4x4* projection)
{
	ComponentUI::Render(projection);
}

void ComponentImage::Delete()
{
	ComponentUI::Delete();
}

void ComponentImage::Save(Config& config) const
{
	ComponentUI::Save(config);
}

void ComponentImage::Load(const Config& config)
{
	ComponentUI::Load(config);
}