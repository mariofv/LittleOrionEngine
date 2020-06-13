#ifndef _EDITORACTIONTRANSLATE_H_
#define _EDITORACTIONTRANSLATE_H_

#include "EditorAction.h"
#include <MathGeoLib.h>

class GameObject;

class EditorActionTranslate : public EditorAction
{
public:
	EditorActionTranslate(float3 &previous, float3 &current, GameObject* go);
	~EditorActionTranslate() = default;

	void Undo();
	void Redo();



private:
	float3 previous_position = float3::zero;
	float3 current_position = float3::zero;
	uint64_t UUID_go;
};
#endif _EDITORACTIONTRANSLATE_H_
