#ifndef _COMPONENTIMAGE_H_
#define _COMPONENTIMAGE_H_

#include "ComponentUI.h"

class ComponentImage : public ComponentUI
{
public:
	ComponentImage();
	ComponentImage(GameObject * owner);
	~ComponentImage() = default;

	void Delete();

	void UISpecializedSave(Config& config) const override;
	void UISpecializedLoad(const Config& config) override;

	void Render(float4x4* projection);
};
#endif
