#ifndef _EDITORACTIONMULTIPLESCALE_H_
#define _EDITORACTIONMULTIPLESCALE_H_

#include "EditorAction.h"
#include <MathGeoLib.h>

class GameObject;

class EditorActionMultipleScale : public EditorAction
{
public:
	EditorActionMultipleScale(float3 &previous, float3 &current);
	~EditorActionMultipleScale() = default;

	void Undo();
	void Redo();

	bool HasParent(GameObject* go) const;
	bool BelongsToList(GameObject* go) const;

private:

	std::vector<uint64_t> game_objects_UUID;
	float3 previous_scale = float3::one;
	float3 current_scale = float3::one;
	float3 scale_factor = float3::one;

};
#endif _EDITORACTIONMULTIPLESCALE_H_