#include "EditorActionTranslate.h"



EditorActionTranslate::EditorActionTranslate(float3 &previous, float3 &current, GameObject* go)
{
	
	previous_position = previous;
	current_position = current;
	action_GO = go;
	
}


EditorActionTranslate::~EditorActionTranslate()
{
}


void EditorActionTranslate::Undo()
{
	return;
}

void EditorActionTranslate::Redo()
{
	return;
}