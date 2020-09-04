#ifndef _EDITORACTIONMULTIPLETRANSLATE_H_
#define _EDITORACTIONMULTIPLETRANSLATE_H_

#include "EditorAction.h"
#include <MathGeoLib.h>

class GameObject;

class EditorActionMultipleTranslate : public EditorAction
{
public:
	EditorActionMultipleTranslate(const float3 &previous, const float3 &current);
	~EditorActionMultipleTranslate() = default;

	void Undo();
	void Redo();


private:

	std::vector<uint64_t> game_objects_UUID;
	float3 previous_position = float3::zero;
	float3 current_position = float3::zero;
	float3 translation_vector = float3::zero;
	
};
#endif _EDITORACTIONMULTIPLETRANSLATE_H_