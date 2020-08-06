#ifndef _COMPONENTSPRITEMASK_H_
#define _COMPONENTSPRITEMASK_H_

#include "Component/Component.h"

#include <GL/glew.h>

class Texture;

class ComponentSpriteMask : public Component
{
public:
	ComponentSpriteMask();
	ComponentSpriteMask(GameObject * owner);
	~ComponentSpriteMask() = default;

	Component* Clone(GameObject* owner, bool original_prefab) override;
	void CopyTo(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

public:
	bool render_mask = false;
	bool inverted_mask = false;
};
#endif //_COMPONENTSPRITEMASK_H_
