#ifndef _COMPONENTCANVAS_H_
#define _COMPONENTCANVAS_H_

#include "ComponentUI.h"
#include "Component/ComponentCamera.h"
#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentCanvas : public ComponentUI
{
public:
	ComponentCanvas();
	ComponentCanvas(GameObject * owner);
	~ComponentCanvas();


	void Delete() override;
	void Render(float4x4*);

	void Save(Config& config) const override;
	void Load(const Config &config) override;
	
};

#endif //_COMPONENTCANVAS_H_

