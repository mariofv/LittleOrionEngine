#ifndef _EDITORACTIONMULTIPLEROTATE_H_
#define _EDITORACTIONMULTIPLEROTATE_H_

#include "EditorAction.h"
#include <MathGeoLib.h>

class GameObject;

class EditorActionMultipleRotate : public EditorAction
{
public:
	EditorActionMultipleRotate(float3 &previous, float3 &current);
	~EditorActionMultipleRotate() = default;

	void Undo();
	void Redo();

	bool HasParent(GameObject* go) const;
	bool BelongsToList(GameObject* go) const;

private:
	std::vector<uint64_t> game_objects_UUID;
	float3 previous_rotation = float3::zero;
	float3 current_rotation = float3::zero;
	float3 rotation_factor = float3::zero;

};
#endif _EDITORACTIONMULTIPLEROTATE_H_