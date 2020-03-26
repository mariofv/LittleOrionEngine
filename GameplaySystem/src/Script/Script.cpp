#include "Script.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

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

void Script::Save(Config & config) const
{
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		GameObject* go = (*public_gameobjects[i]);

		if (go != nullptr)
		{
			std::string aux_name = "Name";
			aux_name.append(std::to_string(i));
			config.AddString(name_gameobjects[i], aux_name);
			config.AddUInt(go->UUID, name_gameobjects[i]);
		}
	}
}

void Script::Load(const Config & config)
{
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		std::string aux_name = "Name";
		aux_name.append(std::to_string(i));

		std::string name_go;
		config.GetString(aux_name, name_go, "Null");

		if (name_go != "Null")
		{
			go_uuids[i] = (config.GetUInt(name_go, 0));
		}

	}
}

void Script::Link()
{
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		if (go_uuids[i] != 0)
		{
			(*public_gameobjects[i]) = App->scene->GetGameObject(go_uuids[i]);
		}
	}
}

Script::~Script()
{
}

