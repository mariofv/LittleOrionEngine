#ifndef _COMPONENTCANVAS_H_
#define _COMPONENTCANVAS_H_

#include "Component/Component.h"
#include "Component/ComponentCamera.h"
#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentCanvas : public Component
{
public:
	ComponentCanvas();
	ComponentCanvas(GameObject * owner);
	~ComponentCanvas();


	void Delete() override;
	void Render(const ComponentCamera& cam) const;

	void Save(Config& config) const override;
	void Load(const Config &config) override;

	void ShowComponentWindow();

	void InitCanvas();
	
	unsigned fbo = 0;
	unsigned rbo = 0;
	unsigned fb_depth = 0;
	unsigned fb_tex = 0;
	unsigned fb_width = 0;
	unsigned fb_height = 0;
	ComponentCamera* camera;

	unsigned vao;
	unsigned vbo;
	unsigned ebo;

private:
	friend class PanelComponent;
};

#endif //_COMPONENTCANVAS_H_

