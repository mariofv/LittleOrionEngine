#ifndef _EDITORACTIONMODIFYCAMERA_H_
#define _EDITORACTIONMODIFYCAMERA_H_

#include "EditorAction.h"
#include "Component/ComponentCamera.h"

class EditorActionModifyCamera : public EditorAction
{
public:
	EditorActionModifyCamera(ComponentCamera* component);
	~EditorActionModifyCamera() = default;

	void Undo();
	void Redo();


private:
	float mov_speed;
	float fov;
	float aspect_ratio;
	float near_plane;
	float far_plane;
	float ortographic_size;
	int depth;
	uint64_t component_UUID;

};

#endif _EDITORACTIONMODIFYCAMERA_H_