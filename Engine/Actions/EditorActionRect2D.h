#ifndef _EDITORACTIONRECT2D_H_
#define _EDITORACTIONRECT2D_H_

#include "EditorAction.h"
#include "Main/GameObject.h"
#include "MathGeoLib.h"

class EditorActionRect2D : public EditorAction
{
public:
	EditorActionRect2D(ComponentTransform2D* component, Rect &current, GameObject* go);
	~EditorActionRect2D() = default;

	void Undo();
	void Redo();



private:
	math::Rect previous_rect = Rect(0, 0, 0, 0);
	math::Rect current_rect = Rect(0, 0, 0, 0);
	uint64_t UUID_go;
};
#endif _EDITORACTIONRECT2D_H_
