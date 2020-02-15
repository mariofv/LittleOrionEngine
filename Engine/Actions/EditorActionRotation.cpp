#include "EditorActionRotation.h"



EditorActionRotation::EditorActionRotation(float3 &previous, float3 &current, GameObject* go)
{
	previous_rotation = previous;
	current_rotation = current;
	action_GO = go;
}


EditorActionRotation::~EditorActionRotation()
{
}


void EditorActionRotation::Undo()
{
	action_GO->transform.SetRotation(previous_rotation);
	return;
}

void EditorActionRotation::Redo()
{
	action_GO->transform.SetRotation(current_rotation);
	return;
}