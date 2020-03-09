#ifndef _EDITORACTIONMODIFYLIGHT_H_
#define _EDITORACTIONMODIFYLIGHT_H_

#include "EditorAction.h"
#include "Component/ComponentLight.h"

class EditorActionModifyLight : public EditorAction
{
public:
	EditorActionModifyLight(ComponentLight* comp, float col[3], float intensity);
	~EditorActionModifyLight() = default;

	void Undo();
	void Redo();

private:
	float color[3];
	float intensity;
	uint64_t component_UUID;

};

#endif _EDITORACTIONMODIFYLIGHT_H_