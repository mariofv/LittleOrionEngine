#ifndef _COMPONENTCANVAS_H_
#define _COMPONENTCANVAS_H_

#include "Component/Component.h"
#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentCanvas : public Component
{
public:
	ComponentCanvas();
	ComponentCanvas(GameObject * owner);
	~ComponentCanvas();


	void Delete() override;
	void Render() const;

	void Save(Config& config) const override;
	void Load(const Config &config) override;

	void ShowComponentWindow();

	void InitCanvas();

	GLuint vbo;
};

#endif //_COMPONENTCANVAS_H_

