#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "Hierarchy.h"

class ModuleScene : public Module
{
public:
	ModuleScene() = default;
	~ModuleScene() = default;

	bool Init();
	bool CleanUp();
	void Render() const;

	GameObject* CreateGameObject();

	GameObject* GetRoot() const;

	void ShowSceneWindow();

public:
	Hierarchy hierarchy;
	bool scene_window_is_hovered = false;

private:
	GameObject *root = nullptr;
};

#endif // _MODULSESCENE_H