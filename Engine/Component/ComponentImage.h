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

	void Save(Config& config) const;
	void Load(const Config& config);
	void Render(float4x4*);
	float3 color = float3(0.0f, 1.0f, 0.0f);
};
#endif
