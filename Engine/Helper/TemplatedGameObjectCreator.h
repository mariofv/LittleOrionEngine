#ifndef _TEMPLATEDGAMEOBJECTCREATOR_H_
#define _TEMPLATEDGAMEOBJECTCREATOR_H_

#include <string>

enum class CoreResource;
class GameObject;

class TemplatedGameObjectCreator
{
public:

	TemplatedGameObjectCreator() = default;
	~TemplatedGameObjectCreator() = default;

	static GameObject* CreatePrimitive(CoreResource resource_type);

private:
	static std::string GetCoreResourceName(CoreResource resource_type);
};

#endif //_TEMPLATEDGAMEOBJECTCREATOR_H_

