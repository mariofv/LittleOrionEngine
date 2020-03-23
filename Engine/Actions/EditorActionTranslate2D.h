#ifndef _EDITORACTIONTRANSLATE2D_H_
#define _EDITORACTIONTRANSLATE2D_H_

#include "EditorAction.h"
#include "Main/GameObject.h"
#include "MathGeoLib.h"

class EditorActionTranslate2D : public EditorAction
{
public:
	EditorActionTranslate2D(ComponentTransform2D* component, float2 &current, GameObject* go);
	~EditorActionTranslate2D() = default;

	void Undo();
	void Redo();



private:
	float2 previous_position = float2::zero;
	float2 current_position = float2::zero;
	uint64_t UUID_go;
};
#endif _EDITORACTIONTRANSLATE2D_H_
