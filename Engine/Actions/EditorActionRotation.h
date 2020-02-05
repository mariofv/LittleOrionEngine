#ifndef _EDITORACTIONROTATION_H_
#define _EDITORACTIONROTATION_H_

#include "EditorAction.h"
#include "GameObject.h"
#include "MathGeoLib.h"

class EditorActionRotation : public EditorAction
{
public:
	EditorActionRotation(float3 &previous, float3 &current, GameObject* go);
	~EditorActionRotation();

	void Undo();
	void Redo();

private:
	float3 previous_rotation = float3::zero;
	float3 current_rotation = float3::zero;
	GameObject* action_GO;
};

#endif _EDITORACTIONROTATION_H_
