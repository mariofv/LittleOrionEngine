#include "EditorActionScale.h"



EditorActionScale::EditorActionScale(float3 &previous, float3 &current, GameObject* go)
{
	previous_scale = previous;
	current_scale = current;
	action_GO = go;
}


EditorActionScale::~EditorActionScale()
{
}


void EditorActionScale::Undo()
{
	action_GO->transform.SetScale(previous_scale);
	return;
}

void EditorActionScale::Redo()
{
	action_GO->transform.SetScale(current_scale);
	return;
}