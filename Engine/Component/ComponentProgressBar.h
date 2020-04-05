#ifndef _COMPONENTPROGRESSBAR_H_
#define _COMPONENTPROGRESSBAR_H_

#include "ComponentUI.h"

class ComponentProgressBar : public ComponentUI
{
public:
	ComponentProgressBar();
	ComponentProgressBar(GameObject * owner);
	~ComponentProgressBar();

	void Delete();

	void Save(Config& config) const;
	void Load(const Config& config);
	void Render(float4x4* projection);

public:
	float percentage = 20.0F;
	unsigned int bar_texture = 2;
	float3 bar_color = float3::unitX;
};
#endif