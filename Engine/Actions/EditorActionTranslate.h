#ifndef _EDITORACTIONTRANSLATE_H_
#define _EDITORACTIONTRANSLATE_H_

#include "EditorAction.h"
#include "GameObject.h"
#include "MathGeoLib.h"

class EditorActionTranslate : public EditorAction
{
public:
	EditorActionTranslate();
	~EditorActionTranslate();

	void Undo();

	void Redo();


private:
	float3 previos_position = float3::zero;
	float3 current_position = float3::zero;
	GameObject* action_GO;
};
#endif _EDITORACTIONTRANSLATE_H_
