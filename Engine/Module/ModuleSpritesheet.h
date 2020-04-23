#ifndef _MODULESPRITESHEET_H_
#define _MODULESPRITESHEET_H_

#include "Module.h"
#include "Component/ComponentLight.h"

#include <vector>
#include <MathGeoLib.h>
#include <GL/glew.h>

#include "EditorUI/Helper/Billboard.h"

class ModuleSpritesheet : public Module
{
public:
	ModuleSpritesheet() = default;
	~ModuleSpritesheet();

	bool Init() override;
	bool CleanUp() override;

	void Render(const float3& position, GLuint program);


public:
	//std::vector<ComponentLight*> lights;

private:
	Billboard billboard;
	friend class ModuleEditor;
};

#endif // !_MODULESPRITESHEET_H_

