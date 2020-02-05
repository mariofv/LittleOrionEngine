#ifndef _EDITORACTIONSCALE_H_
#define _EDITORACTIONSCALE_H_

#include "EditorAction.h"
#include "GameObject.h"
#include "MathGeoLib.h"

class EditorActionScale : public EditorAction 
{
public:
	EditorActionScale(float3 &previous, float3 &current, GameObject* go);
	~EditorActionScale();

	void Undo();

	void Redo();


private:
	float3 previous_scale = float3::zero;
	float3 current_scale = float3::zero;
	GameObject* action_GO;

};

#endif _EDITORACTIONSCALE_H_
