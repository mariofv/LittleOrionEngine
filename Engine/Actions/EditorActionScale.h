#ifndef _EDITORACTIONSCALE_H_
#define _EDITORACTIONSCALE_H_

#include "EditorAction.h"
#include <MathGeoLib.h>

class GameObject;

class EditorActionScale : public EditorAction 
{
public:
	EditorActionScale(const float3 &previous, const float3 &current, GameObject* go);
	~EditorActionScale() = default;

	void Undo();
	void Redo();


private:
	float3 previous_scale = float3::zero;
	float3 current_scale = float3::zero;
	uint64_t UUID_go;

};

#endif _EDITORACTIONSCALE_H_
