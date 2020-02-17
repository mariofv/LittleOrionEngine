#include "EditorActionModifyLight.h"


EditorActionModifyLight::EditorActionModifyLight(ComponentLight* comp, float col[3], float intens)
{
	component = comp;
	color[0] = col[0];
	color[1] = col[1];
	color[2] = col[2];
	intensity = intens;
}


EditorActionModifyLight::~EditorActionModifyLight()
{
}

void EditorActionModifyLight::Undo()
{
	float x = component->light_color[0];
	float y = component->light_color[1];
	float z = component->light_color[2];

	float intens = component->light_intensity;

	component->light_color[0] = color[0];
	component->light_color[1] = color[1];
	component->light_color[2] = color[2];
	component->light_intensity = intensity;

	color[0] = x;
	color[1] = y;
	color[2] = z;
	intensity = intens;
}

void EditorActionModifyLight::Redo()
{
	Undo();
}