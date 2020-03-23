#ifndef _EDITORACTIONSCALE2D_H_
#define _EDITORACTIONSCALE2D_H_

#include "EditorAction.h"
#include "Main/GameObject.h"
#include "MathGeoLib.h"

class EditorActionScale2D : public EditorAction 
{
public:
	EditorActionScale2D(ComponentTransform2D* component, float2 &current, GameObject* go);
	~EditorActionScale2D() = default;

	void Undo();
	void Redo();


private:
	float2 previous_scale = float2::zero;
	float2 current_scale = float2::zero;
	uint64_t UUID_go;

};

#endif _EDITORACTIONSCALE2D_H_
