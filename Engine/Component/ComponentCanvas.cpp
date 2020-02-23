#include "ComponentCanvas.h"



ComponentCanvas::ComponentCanvas() : Component(nullptr, ComponentType::CANVAS)
{
	
}

ComponentCanvas::ComponentCanvas(GameObject * owner) : Component(owner, ComponentType::CANVAS)
{
	
}

void ComponentCanvas::InitCanvas()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// args: left, right, bottom, top, near, far
	glOrtho(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f); // SDL Style 0,0 -> 640,480
	glMatrixMode(GL_MODELVIEW);
}


ComponentCanvas::~ComponentCanvas()
{
}

void ComponentCanvas::Save(Config& config) const
{
}

void ComponentCanvas::Load(const Config& config)
{

}
void ComponentCanvas::Render() const
{
}
void ComponentCanvas::Delete()
{
}
void ComponentCanvas::ShowComponentWindow()
{
}
