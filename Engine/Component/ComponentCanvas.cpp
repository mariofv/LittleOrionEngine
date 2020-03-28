#include "ComponentCanvas.h"
#include "./Module/ModuleCamera.h"
#include "./Module/ModuleDebugDraw.h"
#include "./Module/ModuleProgram.h"
#include "Main/Application.h"
#include "EditorUI/DebugDraw.h" 

ComponentCanvas::ComponentCanvas() : Component(nullptr, ComponentType::UI)
{
	InitCanvas();
}

ComponentCanvas::ComponentCanvas(GameObject * owner) : Component(owner, ComponentType::UI)
{
	InitCanvas();
}

void ComponentCanvas::InitCanvas()
{
    
}


ComponentCanvas::~ComponentCanvas()
{
	//glDeleteFramebuffers(1, &fbo);
}

void ComponentCanvas::Save(Config& config) const
{
}

void ComponentCanvas::Load(const Config& config)
{

}
void ComponentCanvas::Render(const ComponentCamera& cam) const
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, cam.camera_frustum.orthographicWidth, cam.camera_frustum.orthographicHeight, 0.0f, 1.0f, -1.0f);
	glDisable(GL_DEPTH_TEST);

	//TODO

	glEnable(GL_DEPTH_TEST);
}
void ComponentCanvas::Delete()
{
}
void ComponentCanvas::ShowComponentWindow()
{
}
