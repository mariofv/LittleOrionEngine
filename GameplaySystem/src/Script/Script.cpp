#include "Script.h"

Script* ScriptDLL()
{
	return new Script();
}

Script::Script()
{
}

void Script::AddReferences(GameObject *owner, Application *App)
{
	this->App = App;
	this->owner = owner;
}

Script::~Script()
{
}

