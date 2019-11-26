#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

class ModuleScene : public Module
{
public:
	ModuleScene() = default;
	~ModuleScene() = default;

	bool Init();
	bool CleanUp();

	void Render() const;

	GameObject* CreateGameObject();
	std::string GetNextGameObjectName();

	void ShowHierarchyWindow();
	void ShowSceneWindow();

public:
	bool is_hovered = false;

private:
	GameObject *root = nullptr;
	
	int num_game_objects = 0;
};

#endif // _MODULSESCENE_H