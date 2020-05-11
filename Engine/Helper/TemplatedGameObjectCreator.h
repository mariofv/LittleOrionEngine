#ifndef _TEMPLATEDGAMEOBJECTCREATOR_H_
#define _TEMPLATEDGAMEOBJECTCREATOR_H_

#include "Component/Component.h"

#include <string>

enum class CoreResource;
class GameObject;

class TemplatedGameObjectCreator
{
public:

	TemplatedGameObjectCreator() = default;
	~TemplatedGameObjectCreator() = default;

	static GameObject* CreatePrimitive(CoreResource resource_type);
	static GameObject* CreateUIElement(Component::ComponentType ui_type);

private:
	static std::string GetCoreResourceName(CoreResource resource_type);
};

#endif //_TEMPLATEDGAMEOBJECTCREATOR_H_

