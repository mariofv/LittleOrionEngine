#include "ComponentSpriteMask.h"

#include "Main/GameObject.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleUI.h"

#include "ResourceManagement/Resources/Texture.h"

ComponentSpriteMask::ComponentSpriteMask() : Component(ComponentType::UI_SPRITE_MASK)
{
}

ComponentSpriteMask::ComponentSpriteMask(GameObject * owner) : Component(owner, ComponentType::UI_SPRITE_MASK)
{
}

Component* ComponentSpriteMask::Clone(bool original_prefab) const
{
	ComponentSpriteMask * created_component;
	if (original_prefab)
	{
		created_component = new ComponentSpriteMask();
	}
	else
	{
		created_component = App->ui->CreateComponentUI<ComponentSpriteMask>();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));
	return created_component;
};

void ComponentSpriteMask::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentSpriteMask*>(component_to_copy) = *this;
}


void ComponentSpriteMask::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentSpriteMask::SpecializedSave(Config& config) const
{
	config.AddBool(render_mask, "RenderMask");
	config.AddBool(inverted_mask, "InvertedMask");
}

void ComponentSpriteMask::SpecializedLoad(const Config& config)
{
	render_mask = config.GetBool("RenderMask", false);
	inverted_mask = config.GetBool("InvertedMask", false);
}