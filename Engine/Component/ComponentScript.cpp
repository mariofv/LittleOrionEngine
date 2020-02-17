#include "ComponentScript.h"

ComponentScript::ComponentScript() : Component(nullptr, ComponentType::SCRIPT)
{
}
ComponentScript::ComponentScript(GameObject * owner) : Component(owner, ComponentType::SCRIPT)
{
}

ComponentScript::~ComponentScript()
{
}

void ComponentScript::SetAppPointer(Application *App)
{
	this->App = App;
}


