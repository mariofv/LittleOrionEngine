#ifndef _EDITORACTIONROTATION2D_H_
#define _EDITORACTIONROTATION2D_H_

#include "EditorAction.h"
#include "Main/GameObject.h"
#include "MathGeoLib.h"

class EditorActionRotation2D : public EditorAction
{
public:
	EditorActionRotation2D(ComponentTransform2D* component, float current, GameObject* go);
	~EditorActionRotation2D() = default;

	void Undo();
	void Redo();

private:
	float previous_rotation = 0;
	float current_rotation = 0;
	uint64_t UUID_go;
};

#endif _EDITORACTIONROTATION2D_H_
